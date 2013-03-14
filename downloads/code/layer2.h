/* Define the type of messages we can handle */

#define MAXCHILDS 7
#define MAX_SLEEP 6

#define TYPE_SERV   1
#define TYPE_CONN   2

/* Define the service types */

#define SERV_TERM 1
#define SERV_TIME 2

#define SERV_USERDEF1 2
#define SERV_USERDEF2 3

#define SERV_CONNECT 4
#define SERV_DISCONNECT 5
#define SERV_QID   6


int init_queue();
void close_queue(int qid);
void user_send_connect(int sender, int sw);
void user_send_qid(int sender, int qid, int sw);
void user_send_msg(int sender, int receiver, char *text, int sw);
void user_send_time(int sender, int sw);
void user_send_disconnect(int sender, int pid, int sw);
int user_get_msg(int type, mymsgbuf_t *in);

int switch_get_msg(int type, mymsgbuf_t *in);
void switch_send_msg(int sender, int data, int user);
void switch_send_term(int sender, int qid);
void switch_send_time(int qid);
