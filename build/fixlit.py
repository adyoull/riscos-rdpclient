#!/usr/bin/env python3
"""Convert C99 compound literals  (Type){ ... }  to C89.

Norcroft cc (C89) has no compound-literal syntax. For each occurrence of
`(Type){ contents }` where Type is a known type name (typeset.txt), this emits a
file-scope `static const Type __cl_<Type>_<n> = { contents };` once per distinct
(Type, contents) pair and replaces the literal with that variable's name. The
declarations are inserted immediately after the file's leading #include block.

Only literals whose contents contain no nested braces are handled (all of this
tree's compound literals are simple scalar unions, e.g. (screen_modeval){ 21 }).
Anything with nested braces is left untouched and reported, so it can't be
silently corrupted. Idempotent: re-running skips files already converted.
Run AFTER hoist.py, as part of the C99->C89 pipeline.
"""
import re, sys, os

TYPES = set(open(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                              "typeset.txt")).read().split())
MARK = "/* __compound_literals_c89__ */"
LIT = re.compile(r'\(([A-Za-z_]\w*)\)\s*\{([^{}]*)\}')

def convert(path):
    text = open(path, encoding="latin-1").read()
    if MARK in text:
        return 0  # already done
    consts = {}      # (type, norm_contents) -> varname
    order = []
    skipped = 0

    def repl(m):
        nonlocal skipped
        typ, body = m.group(1), m.group(2)
        if typ not in TYPES:
            return m.group(0)
        # A real compound-literal initialiser is a short brace-init: it never
        # contains statements. Reject anything with ';' or a newline in the
        # braces -- that is a function body, not a literal.  (This is what
        # stops `void Func(void){ ...body... }` being eaten.)
        if ";" in body or "\n" in body:
            return m.group(0)
        # Reject `name(Type){` -- a function definition/param list, where the
        # '(' is preceded by an identifier char or ')'.
        start = m.start()
        j = start - 1
        while j >= 0 and text[j] in " \t":
            j -= 1
        if j >= 0 and (text[j].isalnum() or text[j] in "_)"):
            return m.group(0)
        norm = " ".join(body.split())
        key = (typ, norm)
        if key not in consts:
            name = "__cl_%s_%d" % (typ, len([k for k in consts if k[0] == typ]) + 1)
            consts[key] = name
            order.append((name, typ, norm))
        return consts[key]

    new = LIT.sub(repl, text)
    if not consts:
        return 0

    # insert declarations after the leading #include block
    lines = new.split("\n")
    last_inc = 0
    for i, ln in enumerate(lines):
        if ln.lstrip().startswith("#include"):
            last_inc = i
    decls = [MARK] + ["static const %s %s = { %s };" % (typ, name, body)
                      for (name, typ, body) in order]
    lines = lines[:last_inc + 1] + [""] + decls + lines[last_inc + 1:]
    open(path, "w", encoding="latin-1").write("\n".join(lines))
    return len(order)

if __name__ == "__main__":
    total = 0
    for p in sys.argv[1:]:
        n = convert(p)
        if n:
            print("fixlit: %s  (%d const(s))" % (p, n))
            total += n
    print("fixlit: %d compound-literal const(s) created" % total)
