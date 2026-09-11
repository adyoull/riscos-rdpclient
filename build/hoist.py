#!/usr/bin/env python3
"""Hoist C99 mid-block declarations to C89 (block-top), semantics-preserving.
Char-accurate brace tracking (ignores braces in strings/chars/comments)."""
import re, sys

TYPES = set(open("/home/claude/typeset.txt").read().split())
TYPEKW = set("unsigned signed short long int char float double void const volatile struct union enum".split())
STORAGE = set("register static auto extern".split())
skipped = []

def brace_events(line, st):
    """Yield '{'/'}' outside strings/chars/comments. st carries state across lines:
    st = {'bc':bool block-comment, 'str':char-or-None}. Returns list of events."""
    ev = []
    i = 0; n = len(line)
    while i < n:
        c = line[i]
        if st['bc']:
            if c == '*' and i+1 < n and line[i+1] == '/':
                st['bc'] = False; i += 2; continue
            i += 1; continue
        if st['str']:
            if c == '\\': i += 2; continue
            if c == st['str']: st['str'] = None
            i += 1; continue
        if c == '/' and i+1 < n and line[i+1] == '*':
            st['bc'] = True; i += 2; continue
        if c == '/' and i+1 < n and line[i+1] == '/':
            break  # line comment
        if c == '"' or c == "'":
            st['str'] = c; i += 1; continue
        if c == '{': ev.append('{')
        elif c == '}': ev.append('}')
        i += 1
    return ev

def strip_comment(line):
    out = ''; i = 0; n = len(line); s = None; bc = False
    while i < n:
        c = line[i]
        if bc:
            if c=='*' and i+1<n and line[i+1]=='/': bc=False; i+=2; continue
            i+=1; continue
        if s:
            out+=c
            if c=='\\' and i+1<n: out+=line[i+1]; i+=2; continue
            if c==s: s=None
            i+=1; continue
        if c=='/' and i+1<n and line[i+1]=='*': bc=True; i+=2; continue
        if c=='/' and i+1<n and line[i+1]=='/': break
        if c=='"' or c=="'": s=c
        out+=c; i+=1
    return out

def split_top_commas(s):
    parts, depth, cur = [], 0, ""
    for ch in s:
        if ch in "([{": depth += 1
        elif ch in ")]}": depth -= 1
        if ch == "," and depth == 0: parts.append(cur); cur = ""
        else: cur += ch
    if cur.strip(): parts.append(cur)
    return parts

def starts_decl(core):
    """True if `core` begins a declaration: an optional storage-class keyword
    (register/static/auto/extern) then a type name."""
    m = re.match(r'\s*([A-Za-z_]\w*)', core)
    if not m: return False
    w = m.group(1)
    if w in STORAGE:
        m2 = re.match(r'\s*[A-Za-z_]\w*\s+([A-Za-z_]\w*)', core)
        return bool(m2 and (m2.group(1) in TYPES or m2.group(1) in TYPEKW))
    return w in TYPES

def declname(lhs):
    """Extract the identifier from a declarator like '*p', 'buf[10]', '**q'."""
    x = lhs.strip().lstrip('*& ').strip()
    x = re.sub(r'\[.*$', '', x).strip()      # drop array dims
    return x

def parse_decl(body):
    toks = body.split()
    # strip leading storage-class specifiers (register/static/auto/extern),
    # keep them for the hoisted declaration. static-with-initialiser is kept
    # whole (init must stay compile-time), handled below via 'is_static'.
    storage = []
    while toks and toks[0] in STORAGE:
        storage.append(toks.pop(0))
    is_static = "static" in storage or "extern" in storage
    # handle glued pointer type: 'char*' -> 'char' '*...'
    if toks and toks[0] not in TYPES and toks[0].rstrip('*') in TYPES and toks[0].endswith('*'):
        base = toks[0].rstrip('*'); stars = toks[0][len(base):]
        if len(toks) >= 2:
            toks = [base, stars + toks[1]] + toks[2:]
        else:
            toks = [base]
    if not toks or toks[0] not in TYPES: return None
    i = 0; consumed = False
    while i < len(toks):
        t = toks[i]
        if t in TYPEKW:
            i += 1
            if t in ("struct","union","enum") and i < len(toks): i += 1
            consumed = True; continue
        if not consumed and t in TYPES: i += 1; consumed = True; continue
        break
    if i == 0: return None
    type_prefix = " ".join(toks[:i])
    rest = " ".join(toks[i:]).strip()
    if not rest: return None
    hoist_parts, assigns = [], []
    for d in split_top_commas(rest):
        d = d.strip()
        if not d: return None
        if "=" in d:
            lhs, rhs = d.split("=", 1); lhs = lhs.strip(); rhs = rhs.strip()
            if "(" in lhs: return None          # function-pointer declarator: too complex
            if not re.match(r'^[A-Za-z_]\w*$', declname(lhs)): return None  # not a plain declarator
            if is_static or "[" in lhs or rhs.startswith("{"):
                hoist_parts.append(d)           # static/array/aggregate init: keep whole
            else:
                hoist_parts.append(lhs)         # scalar/pointer: hoist bare, assign in place
                assigns.append("%s = %s;" % (lhs.replace("*","").strip(), rhs))
        else:
            if "(" in d: return None            # function pointer / prototype: skip
            if not re.match(r'^[A-Za-z_]\w*$', declname(d)): return None    # not a plain declarator
            hoist_parts.append(d)
    prefix = " ".join(storage + [type_prefix]) if storage else type_prefix
    return "%s %s;" % (prefix, ", ".join(hoist_parts)), assigns

def transform(path):
    lines = open(path, encoding="latin-1").read().split("\n")
    stack = [dict(content=[], hoist=[], seen=False)]
    st = {'bc': False, 'str': None}
    def close():
        if len(stack) == 1: return
        fr = stack.pop()
        stack[-1]["content"].extend(fr["hoist"] + fr["content"])
    n = len(lines)
    i = 0
    while i < n:
        raw = lines[i]
        # snapshot comment/string state, compute events
        pre_bc, pre_str = st['bc'], st['str']
        ev = brace_events(raw, st)
        lead = 0
        for e in ev:
            if e == '}': lead += 1
            else: break
        trail = 0
        for e in reversed(ev):
            if e == '{': trail += 1
            else: break
        # code view (no comment) for classification
        cs = strip_comment(raw).strip() if not pre_bc else ""
        # 1) leading closes
        for _ in range(lead): close()
        core = cs

        # --- multi-line declaration join ---------------------------------
        # A declaration whose initialiser wraps across physical lines won't
        # end with ';' on the first line. If this line starts a declaration
        # (no braces, no leading/trailing close) but is unterminated, gather
        # following brace-free, comment-clean lines until the statement ends
        # with ';' at balanced parens. joined = full code; jraws = the physical
        # lines consumed; jend = index just past them.
        joined = None; jraws = [raw]; jend = i + 1
        if (len(stack) >= 2 and lead == 0 and trail == 0 and not pre_bc
                and core and not core.startswith("#")
                and "{" not in core and "}" not in core
                and starts_decl(core) and not core.endswith(";")):
            peek = dict(st); parts = [core]; raws = [raw]; j = i + 1; good = False
            while j < n and j - i <= 12:
                if peek["bc"]: break
                nraw = lines[j]
                nev = brace_events(nraw, peek)
                if nev: break                      # brace on continuation: bail
                ncode = strip_comment(nraw).strip()
                parts.append(ncode); raws.append(nraw); j += 1
                jc = " ".join(p for p in parts if p).strip()
                if jc.endswith(";"):
                    if jc.count("(") == jc.count(")"): good = True
                    break
            if good:
                jc = " ".join(p for p in parts if p).strip()
                joined = jc; jraws = raws; jend = j

        code_for_class = joined if joined is not None else core
        is_decl = (len(stack) >= 2 and lead == 0 and trail == 0
                   and code_for_class.endswith(";") and starts_decl(code_for_class))
        handled = False
        if is_decl:
            if stack[-1]["seen"]:
                res = parse_decl(code_for_class[:-1])
                if res is not None:
                    hoist, assigns = res
                    indent = raw[:len(raw)-len(raw.lstrip())]
                    stack[-1]["hoist"].append(indent + hoist)
                    for a in assigns: stack[-1]["content"].append(indent + a)
                    handled = True
                else:
                    skipped.append((path, code_for_class[:70]))
            # decl before any statement -> leave in place (legal)
        if not handled:
            for r in jraws: stack[-1]["content"].append(r)
            if not pre_bc:
                code_only = code_for_class
                if code_only and not code_only.startswith("#") \
                   and code_only not in ("{","}","};") \
                   and not re.match(r'^[A-Za-z_]\w*\s*:$', code_only) \
                   and not (is_decl):
                    stack[-1]["seen"] = True
        # advance brace state over any extra physical lines we consumed
        for cr in jraws[1:]:
            brace_events(cr, st)
        # 3) trailing opens
        if trail > 0:
            stack[-1]["seen"] = True  # control opener is a statement in this block
            for _ in range(trail):
                stack.append(dict(content=[], hoist=[], seen=False))
        i = jend
    while len(stack) > 1: close()
    open(path, "w", encoding="latin-1").write("\n".join(stack[0]["content"]))

if __name__ == "__main__":
    for p in sys.argv[1:]:
        try: transform(p)
        except Exception as e: print("ERROR", p, e)
    for s in skipped: print("SKIP", s[0], "::", s[1])
    print("skipped total:", len(skipped))
