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

pid_t pid;
int i;

int childs;
int death_prob;
int send_prob;

int status;
int deadproc; /* Contatore per i processi utenti terminati */
int qid;
int sw; /*qid del sw*/
int dest; /* Contiene un identificatore numerico (da 1 a numfigli) che rappresenta la destinazione del messaggio */
int olddest; /* Contiene un identificatore numerico che rappresenta la destinazione del messaggio precedente */

int queues[MAXCHILDS + 1]; /* Contiene gli identificatori delle code della centralina (msgqid[0]) e degli utenti */
int queues_num;

int msg_sender; /* Contiene il mittente di un messaggio IPC */
int msg_receiver; /* Contiene il destinatario di un messaggio IPC */
int msg_data; /* Contiene il dato spedito in un messaggio IPC */
int msg_service;

int t; /* Tempo in secondi */   
int timing[MAXCHILDS+1][2];

mymsgbuf_t msg, in;

FILE *fp;

void usage(void)
{
  printf("IPCdemo v1.1 - Un ambiente dimostrativo delle comunicazioni tra processi concorrenti\n");
  printf("\nSINTASSI: ipcdemo <figli> <prob. morte> <prob. spedizione>\n");
  printf("          <figli> - Numero di processi figli da generare (2 - 15)\n");
  printf("          <prob. morte> - Probabilità che ad un processo mittente venga richiesto un servizio, inclusa la terminazione (0-100)\n");
  printf("          <prob. spedizione> - Probabilità che un processo spedisca un messaggio (0-100)\n\n");
}

int main(int argc, char *argv[])
{

  /* Command line argument parsing */
  if(argc != 4){
    usage();
    exit(0);
  }

  childs = strtol(argv[1], NULL, 10);  
  death_prob = strtol(argv[2], NULL, 10);
  send_prob = strtol(argv[3], NULL, 10);
  

  if((childs < 3) || (childs > MAXCHILDS)){
    usage();
    exit(0);
  }

  if((death_prob < 1) || (death_prob > 100)){
    usage();
    exit(0);
  }

  if((send_prob < 1) || (send_prob > 100)){
    usage();
    exit(0);
  }

  init_rand(time(NULL));

  /* Queues initialization */
  queues_num = childs;
  qid = init_queue(0);
  
  /* All queues are "uninitialized" (set equal to switch queue) */
  for(i = 0; i <= childs; i++){
    queues[i] = qid;
  }
  sw = qid;

  /* Create users (childs) */
  for(i = 1;i <= childs; i++){
    pid = fork();
    if (pid == 0){
      init_rand(time(NULL));

      /* Initialize queue  */
      qid = init_queue(i);
      
      /* Let the switch know we are alive */
      child_send_birth(i, sw);
      
      /* Let the switch know how to reach us */
      child_send_qid(i, qid, sw);
      
      /* Enter the main loop */
      while(1){
	sleep(rand()%MAX_SLEEP);
	/* Send a message if the random numbers are good */
	if(myrand(100) < send_prob){
	  dest = 0;

	  /* Do not send messages to the switch, to you, */
	  /* and to the same receiver of the previous message */
	  while((dest == 0) || (dest == i) || (dest == olddest)){
	    dest = myrand(childs + 1);
	  }
	  olddest = dest;

	  printf("%d -- U %d -- Message to user %d\n", (int) time(NULL), i, dest);
	  child_send_msg(i, dest, 0, sw);
	}
	
	/* Check the incoming box for simple messages */
	if(child_get_msg(TYPE_CONN, &in)){
	  msg_sender = get_sender(&in);
	  msg_data = get_data(&in);
	  printf("%d -- U %d -- Message from user %d: %d\n", (int) time(NULL), i, msg_sender, msg_data);
	}
	
	/* Check if the switch requested a service */
	if(child_get_msg(TYPE_SERV, &in)){
	  msg_service = get_service(&in);

	  switch(msg_service){
	  case SERV_TERM:
	    /* Sorry, we have to terminate */
	    /* Send an acknowledgement to the switch */
	    child_send_death(i, getpid(), sw);
	    
	    /* Read the last messages we have in the queue */
	    while(child_get_msg(TYPE_CONN, &in)){
	      msg_sender = get_sender(&in);
	      msg_data = get_data(&in);
	      printf("%d -- U %d -- Message from user %d: %d\n", (int) time(NULL), i, msg_sender, msg_data);
	    }
	    
	    /* Remove the queue */
	    close_queue(qid);	 
	    printf("%d -- U %d -- Termination\n", (int) time(NULL), i);
	    exit(0);
	    break;
	  case SERV_TIME:
	    /* We have to time our work */
	    child_send_time(i, sw);
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
    if(switch_get_msg(TYPE_CONN, &in)){

      msg_receiver = get_receiver(&in);
      msg_sender = get_sender(&in);
      msg_data = get_data(&in);
      
      /* If the destination is alive */
      if(queues[msg_receiver] != sw){
	
	/* Send a messge to the destination (follow-up the received message) */
	switch_send_msg(msg_sender, msg_data, queues[msg_receiver]);
	
	printf("%d -- S -- Sender: %d -- Destination: %d\n", (int) time(NULL), msg_sender, msg_receiver);
      }
      else{
	/* The destination is not alive */
	printf("%d -- S -- Unreachable destination (Sender: %d - Destination: %d)\n", (int) time(NULL), msg_sender, msg_receiver);
      }
      
      /* Randomly request a service to the sender of the last message */
      if((myrand(100)  < death_prob) && (queues[msg_sender] != sw)){
	switch(myrand(2))
	  {
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
      if(deadproc == childs){
	/* All childs have been terminated, just wait for the last to complete its last jobs */
	waitpid(pid, &status, 0);

	/* Remove the switch queue */
	remove_queue(sw);

	/* Terminate the program */
	exit(0);
      }
    }
    
    /* Check if some user is answering to service messages */
    if(switch_get_msg(TYPE_SERV, &in)){
      msg_service = get_service(&in);
      msg_sender = get_sender(&in);

      switch(msg_service)
	{
	case SERV_BIRTH:
	  /* A new user has connected */
	  printf("%d -- S -- Activation of user %d\n", (int) time(NULL), msg_sender);
	  break;

	case SERV_DEATH:
	  /* The user is terminating */
	  printf("%d -- S -- User %d is terminating\n", (int) time(NULL), msg_sender);

	  /* Remove its queue from the list */
	  queues[msg_sender] = sw;

	  /* Remember how many users are dead */
	  deadproc++;
	  break;

	case SERV_QID:
	  /* The user is sending us its queue id */
	  msg_data = get_data(&in);
	  printf("%d -- S -- Got queue id of user %d: %d\n", (int) time(NULL), msg_sender, msg_data);
	  queues[msg_sender] = msg_data;
	  break;

	case SERV_TIME:
	  msg_data = get_data(&in);

	  /* Timing informations */
	  timing[msg_sender][1] = msg_data - timing[msg_sender][1];

	  printf("%d -- S -- Timing of user %d: %d seconds\n", (int) time(NULL), msg_sender, timing[msg_sender][1]);
	  /* The user is no more under time control */
	  timing[msg_sender][0] = 0;
	  break;
	}
    }
  }
}

