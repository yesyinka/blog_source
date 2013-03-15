#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/ipc.h>
#include <linux/msg.h>
#include <time.h>
#include <signal.h>
#include <wait.h>
#include "layer1.h"
#include "layer2.h"

#define MINCHILDS 2
#define MAXCHILDS 15

pid_t pid;
int i;

int users_number;
int service_probability;
int text_message_probability;

int status;
int deadproc; /* A counter of the already terminated user processes */
int qid;
int sw; /* Qid of the switch */
int dest; /* Destination of the message */
int olddest; /* Destination of the previous message */

int queues[MAXCHILDS + 1]; /* Queue identifiers - 0 is the qid of the switch */

int msg_sender;
int msg_receiver;
char msg_text[160];
int msg_service;
int msg_service_data;

int t;
int timing[MAXCHILDS + 1][2];

messagebuf_t msg, in;

FILE *fp;

int init_rand(unsigned int seed){
  srandom(seed);
}

int random_number(int max)
{
  double r,x;
  r = (double) random();
  x = r * (double) max / RAND_MAX;
  return((int) x);
}

void usage(char *argv[])
{
  printf("Telephone switch simulator v1 - A concurrent processing demonstration environment\n");
  printf("%s <number of users> <service probability> <text message probability>\n", argv[0]);
  printf("     <number of users> - Number of users alive in the system (%d - %d)\n", MINCHILDS, MAXCHILDS);
  printf("     <service probability> - The probability that the switch requires a service from the user (0-100)\n");
  printf("     <text message probability> - The probability the a user sends a message to another user (0-100)\n\n");
}

int main(int argc, char *argv[])
{

  /* Command line argument parsing */
  if(argc != 4){
    usage(argv);
    exit(0);
  }

  users_number = strtol(argv[1], NULL, 10);  
  service_probability = strtol(argv[2], NULL, 10);
  text_message_probability = strtol(argv[3], NULL, 10);
  

  if((users_number < MINCHILDS) || (users_number > MAXCHILDS)){
    usage(argv);
    exit(1);
  }

  if((service_probability < 0) || (service_probability > 100)){
    usage(argv);
    exit(0);
  }

  if((text_message_probability < 0) || (text_message_probability > 100)){
    usage(argv);
    exit(0);
  }

  printf("Number of users: %d\n", users_number);
  printf("Probability of a service request: %d%%\n", service_probability);
  printf("Probability of a text message: %d%%\n", text_message_probability);
  printf("\n");

  /* Initialize the random number generator */
  init_rand(time(NULL));

  /* Queues initialization */
  sw = init_queue(0);
  
  /* All queues are "uninitialized" (set equal to switch queue) */
  for(i = 0; i <= users_number; i++){
    queues[i] = sw;
  }

  /* Create users */
  for(i = 1; i <= users_number; i++){
    pid = fork();

    if (pid == 0){
      init_rand(time(NULL));

      /* Initialize queue  */
      qid = init_queue(i);
      
      /* Let the switch know we are alive */
      user_send_connect(i, sw);
      
      /* Let the switch know how to reach us */
      user_send_qid(i, qid, sw);
      
      /* Enter the main loop */
      while(1){
	sleep(rand()%MAX_SLEEP);

	/* Send a message */
	if(random_number(100) < text_message_probability){
	  dest = random_number(users_number + 1);

	  /* Do not send a message to the switch, to yourself and to the previous recipient */
	  while((dest == 0) || (dest == i) || (dest == olddest)){
	    dest = random_number(users_number + 1);
	  }
	  olddest = dest;

	  printf("%d -- U %d -- Message to user %d\n", (int) time(NULL), i, dest);
	  user_send_text_message(i, dest, "A message from me", sw);
	}
	
	/* Check the incoming box for simple messages */
	if(receive_message(qid, TYPE_TEXT, &in)){
	  msg_sender = get_sender(&in);
	  get_text(&in, msg_text);
	  printf("%d -- U %d -- Message from user %d: %s\n", (int) time(NULL), i, msg_sender, msg_text);
	}
	
	/* Check if the switch requested a service */
	if(receive_message(qid, TYPE_SERVICE, &in)){
	  msg_service = get_service(&in);

	  switch(msg_service){

	  case SERVICE_TERMINATE:
	    /* Send an acknowledgement to the switch */
	    user_send_disconnect(i, getpid(), sw);
	    
	    /* Read the last messages we have in the queue */
	    while(receive_message(qid, TYPE_TEXT, &in)){
	      msg_sender = get_sender(&in);
	      get_text(&in, msg_text);
	      printf("%d -- U %d -- Message from user %d: %s\n", (int) time(NULL), i, msg_sender, msg_text);
	    }
	    
	    /* Remove the queue */
	    close_queue(qid);	 
	    printf("%d -- U %d -- Termination\n", (int) time(NULL), i);
	    exit(0);
	    break;

	  case SERVICE_TIME:
	    user_send_time(i, sw);
	    printf("%d -- U %d -- Timing\n", (int) time(NULL), i);
	    break;
	  }
	}
      }
    }
  }
  
  /* Switch (parent process) */ 
  while(1){
    /* Check if some user has connected */
    if(receive_message(sw, TYPE_TEXT, &in)){

      msg_receiver = get_receiver(&in);
      msg_sender = get_sender(&in);
      get_text(&in, msg_text);
      
      /* If the destination is connected */
      if(queues[msg_receiver] != sw){
	/* Send the message (forward it) */
	switch_send_text_message(msg_sender, msg_text, queues[msg_receiver]);
	
	printf("%d -- S -- Sender: %d -- Destination: %d -- Text: %s\n", (int) time(NULL), msg_sender, msg_receiver, msg_text);
      }
      else{
	printf("%d -- S -- Unreachable destination (Sender: %d - Destination: %d -- Text: %s)\n", (int) time(NULL), msg_sender, msg_receiver, msg_text);
      }
      
      /* Randomly request a service to the sender of the last message */
      if((random_number(100)  < service_probability) && (queues[msg_sender] != sw)){
	switch(random_number(2)){

	case 0:
	  /* The user must terminate */
	  printf("%d -- S -- User %d chosen for termination\n", (int) time(NULL), msg_sender);
	  switch_send_term(i, queues[msg_sender]);
	  break;

	case 1:
	  /* Check if we are already timing that user */
	  if(!timing[msg_sender][0]){
	    timing[msg_sender][0] = 1;
	    timing[msg_sender][1] = (int) time(NULL);
	    printf("%d -- S -- User %d chosen for timing...\n", timing[msg_sender][1], msg_sender);
	    switch_send_time(queues[msg_sender]);
	  }
	  break;
	}
      }
    }
    else{
      if(deadproc == users_number){
	/* All childs have been terminated, just wait for the last to complete its jobs */
	waitpid(pid, &status, 0);

	/* Remove the switch queue */
	remove_queue(sw);

	/* Terminate the program */
	exit(0);
      }
    }

    /* Check if some user is answering to service messages */
    if(receive_message(sw, TYPE_SERVICE, &in)){
      msg_service = get_service(&in);
      msg_sender = get_sender(&in);

      switch(msg_service){
      case SERVICE_CONNECT:
	/* A new user has connected */
	printf("%d -- S -- User %d connected\n", (int) time(NULL), msg_sender);
	break;

      case SERVICE_DISCONNECT:
	/* The user is terminating */
	printf("%d -- S -- User %d disconnected\n", (int) time(NULL), msg_sender);

	/* Remove its queue from the list */
	queues[msg_sender] = sw;

	deadproc++;
	break;

      case SERVICE_QID:
	/* The user is sending us its queue id */
	msg_service_data = get_service_data(&in);
	printf("%d -- S -- Got queue id of user %d: %d\n", (int) time(NULL), msg_sender, msg_service_data);
	queues[msg_sender] = msg_service_data;
	break;

      case SERVICE_TIME:
	msg_service_data = get_service_data(&in);

	printf("%d -- S -- Timing of user %d: (%d - %d)\n", (int) time(NULL), msg_sender, msg_service_data, timing[msg_sender][1]);

	/* Timing informations */
	timing[msg_sender][1] = msg_service_data - timing[msg_sender][1];

	printf("%d -- S -- Timing of user %d: %d seconds\n", (int) time(NULL), msg_sender, timing[msg_sender][1]);
	/* The user is no more blocked by a timing operation */
	timing[msg_sender][0] = 0;
	break;
      }
    }
  }
}

