/*
 * File: WimpMsg
 * Author: Copyright © 2003, Antony Sidwell
 *
 * Abstract: A simpler interface to sending Wimp messages.
 *
 * This file is part of DeskLib, the general-purpose freeware C library for
 * RISC OS.  Please refer to the accompanying documentation for conditions
 * of use.
 */

#ifndef __dl_wimpmsg_h
#define __dl_wimpmsg_h

#ifdef __cplusplus
extern "C" {
#endif

/* Overview */
/*
  This header declares a few functions to make sending Wimp messages
  a little easier.

  In particular, the functions construct the message header for you,
  and where applicable they return the reference of the sent message
  for you to check against any replies you receive.
*/


int WimpMsg_Send(event_type type, message_action action, void *message_body,
                 size_t datalen, message_destinee dest, icon_handle icon);
/*
  Sends a wimp message to the given task (or window, or window/icon
  combination).

  Pass the type of broadcast (typically event_SEND or event_SENDWANTACK)
  and the message code (e.g. message_DATASAVE), along with a pointer to
  the "data" part of the message, and the length of this data block.

  The final message block is then constructed for you, and sent out.
  The block should be no longer than 236 bytes, and the size need not be
  a multiple of 4 (this is taken care of internally).

  It returns the reference of the sent message (or 0 if the message is
  not successfully sent).  This can then be checked against any replies
  you receive to check they are responses to this message.
*/


#define WimpMsg_SendTask(type, action, message_body, datalen, task) \
  WimpMsg_Send(type, action, message_body, datalen, task, 0)
/*
  Sends a message to the task with the given task handle, in the same
  way as WimpMsg_Send.
*/

#define WimpMsg_Broadcast(type, action, message_body, datalen) \
  WimpMsg_Send(type, action, message_body, datalen, 0, 0)
/*
  Broadcasts a message in the same way as WimpMsg_Send, for any task
  to receive.
*/


int WimpMsg_Reply(message_block *previous, event_type type,
                  message_action action, void *message_body, size_t datalen);
/*
  This replies to the 'previous' message (as received as a Wimp Event) with
  the given message.  It replies directly to the sender of that message.

  It returns the reference of the sent message (or 0 if the message is
  not successfully sent).  This can then be checked against any replies
  you receive to check they are responses to this message.
*/


void WimpMsg_Ack(message_block *previous);
/*
  This acknowledges the received message, preventing it from being passed
  on to other tasks, or from being returned to the sender if it was a
  UserMessage_Recorded.
*/


task_handle WimpMsg_GetTaskHandle(window_handle window, icon_handle icon);
/*
  Returns the task handle of the task which owns the given window/icon
  combination (useful for finding the owner of an iconbar icon).
*/


#ifdef __cplusplus
}
#endif


#endif
