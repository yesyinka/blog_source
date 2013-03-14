#include "layer1.h"
#include "layer2.h"
#include <time.h>

extern int sw;
extern int qid;
extern int msg_sender;
extern int msg_receiver;
extern int msg_data;
extern int msg_service;
extern int queues[MAXCHILDS + 1];

/*
 * Queue initialization.
 * This function creates an IPC key and build a message queue.
 */
int init_queue(int num)
{
  key_t key;
  int qid;
  int i;
    
  key = build_key((char) num);
  qid = create_queue(key);

  return qid;
}

/*
 * Queue deletion.
 * This function removes a queue.
 */
void close_queue(int qid)
{
  remove_queue(qid);
}

/*
 * Connect message (child).
 * This function sends a message to the switch notifying that the user has been activated.
 */
void user_send_connect(int sender, int sw)
{
  mymsgbuf_t message;
  
  set_type(&message, TYPE_SERV);
  set_service(&message, SERV_CONNECT);
  set_sender(&message, sender);
  set_receiver(&message, 999);
  set_data(&message, 999);
  send_message(sw, &message);
}

/*
 * Qid message (child).
 * This function sends a message to the switch notifying the id of the child's queue.
 */
void user_send_qid(int sender, int qid, int sw)
{
  mymsgbuf_t message;
  
  set_type(&message, TYPE_SERV);
  set_service(&message, SERV_QID);
  set_sender(&message, sender);
  set_data(&message, qid);
  set_receiver(&message, 999);
  send_message(sw, &message);
}

/*
 * Message (child).
 * This function sends a message to another user.
 */
void user_send_msg(int sender, int receiver, char *text, int sw)
{
  mymsgbuf_t message;

  set_type(&message, TYPE_CONN);
  set_sender(&message, sender);
  set_receiver(&message, receiver);
  set_text(&message, text);
  set_service(&message, 999);
  send_message(sw, &message);
}

/*
 * Time message (child).
 * This function sends a message to the switch containing
 * the current time.
 */
void user_send_time(int sender, int sw)
{
  mymsgbuf_t message;
  
  set_type(&message, TYPE_SERV);
  set_sender(&message, sender);
  set_service(&message, SERV_TIME);
  set_data(&message, (int) time(NULL));
  set_receiver(&message, 999);
  send_message(sw, &message);
}

/*
 * Disconnect Message (child).
 * This function sends a message to the switch notifying that the user has been deactivated.
 */
void user_send_disconnect(int sender, int pid, int sw)
{
  mymsgbuf_t message;

  set_type(&message, TYPE_SERV);
  set_sender(&message, sender);
  set_service(&message, SERV_DISCONNECT);
  set_data(&message, pid);
  set_receiver(&message, 999);
  send_message(sw, &message);
}

/*
 * Read message (child).
 * This function reads a message with the specified type
 * from the child queue.
 */
int user_get_msg(int type, mymsgbuf_t *in)
{
  switch(type){
  case TYPE_CONN:
    if(receive_message(qid, TYPE_CONN, in) != 0){
      return 1;
    }
    else return 0;
    break;
  case TYPE_SERV:
    if(receive_message(qid, TYPE_SERV, in) != 0){
      return 1;
    }
    else return 0;
    break;
  }
}

/*
 * Read message (switch).
 * This function reads a message with the specified type
 * from the switch queue.
 */
int switch_get_msg(int type, mymsgbuf_t *in)
{
  switch(type){
  case TYPE_CONN:
    if(receive_message(sw, TYPE_CONN, in) != 0){
      return 1;
    }
    else return 0;
    break;
  case TYPE_SERV:
    if(receive_message(sw, TYPE_SERV, in) != 0){
      return 1;
    }
    else return 0;
    break;
  }
}

/*
 * Send a message (switch).
 * This function sends a message to a user.
 */
void switch_send_msg(int sender, int data, int user)
{
  mymsgbuf_t message;

  set_type(&message, TYPE_CONN);
  set_sender(&message, sender);
  set_data(&message, data);
  set_service(&message, 999);
  set_receiver(&message, 999);
  send_message(user, &message);
}

/*
 * Send termination signal (switch).
 * Through this function you can tell a user it must begin
 * the termination procedure.
 */
void switch_send_term(int sender, int qid)
{
  mymsgbuf_t message;
  
  set_type(&message, TYPE_SERV);
  set_service(&message, SERV_TERM);
  set_sender(&message, 999);
  set_receiver(&message, 999);
  set_data(&message, 999);
  send_message(qid, &message);
}

/* Time service request message (switch).
 * This function send a message to a user requesting a timing operation.
 */  
void switch_send_time(int qid)
{
  mymsgbuf_t message;
  
  set_type(&message, TYPE_SERV);
  set_service(&message, SERV_TIME);
  set_sender(&message, 999);
  set_receiver(&message, 999);
  set_data(&message, 999);
  send_message(qid, &message);
}


