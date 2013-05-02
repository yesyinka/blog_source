---
layout: post
title: "Concurrent programming - 6"
date: 2013-04-23 08:29
comments: true
categories: [operating systems, concurrent programming]
series: pippo
---

## Abstract

Issue 5 of this series ended with a small program where two processes exchanged ten numbers through a message queue, thus being a synchronized producer-consumer couple. This time I am going to show and comment the code of a very simple communication simulator written in C. The code leverages IPC queues to allow multiple processes to talk each other while running concurrently.

## The simulator

The program simulates a messaging switch, where multiple users (child processes) connect to a central switch (parent process) and send text messages (SMS-like) to other users through it. The switch receives messages and route them if the recipient is reachable (process is running). Moreover, the switch can perform a timing operation on a user, checking how much time the user needs to answer a message, or select it for termination. Last, the switch counts how many times a user sends a message to an unreachable user and when a threshold is reached terminates it. Switch and user decisions are taken extracting pseudo-random numbers and comparing them with thresholds given on the command line. Remember that the simulation wants to demonstrate a concrete use of message queues, and does not claim to be a complete communication system between processes.

## 10,000 feet overview

I splitted the simulator in 3 parts, namely two stacked layers of function and the main application code. The first layer implements the message structure, provides functions to interact with it (getters and setters), exports basic functions to manage queues and to send and receive messages. The second layer exports functions that implement the protocol, i.e. the actions users and switch can perform. Last the main function contains the actual logic of the whole simulation. Due to the instructional purpose of the simulation some of the solution implemented are far from optimal or even correct (e.g. random numbers management); I suggest you to try to evolve the simulation, adding new services or messages between users.

## Debugging nightmares

Be ready to spend some time debugging your multiprocessing applications! If you ever debugged some code you know that the most friendly bugs are by far those which are reproducible: you only need to add some debugging output or step through the code with the debugger and the bug is spotted. Multiprocessing and network application are at the very opposite corner: most of the time the bugs in that sort of applications are very difficult to reproduce, when not impossible. The concurrent execution of multiple processes, indeed, makes every execution unique, since the actual execution sequence is random (random means that it is the result of so many factors that the result is unpredictable and thus, irreproducible). Network programs suffers from the same problems (concurrent execution), worsened by network lags, that add an additional factor of randomness.

## Layer 1

#### Message structure

The structure of the message is

``` c
typedef struct
{
 int sender;
 int recipient;
 char text[160];
 int service;
 int service_data;
} message_t;

typedef struct
{
 long mtype;
 message_t mtext;
} messagebuf_t;
```

Here `sender` and `recipient` are numbers that identify the switch and the users (0 is the switch, then users are numbered increasingly when they connect to the switch); `text` is the content of a message a user sends to another user, and is 160 characters long to mimic SMS behaviour. Last `service` is the identifier of some system operation, like a request the switch sends to the users; `service_data` carries the optional data the service needs to communicate. Actual services are implemented in layer 2, I am going to describe them later.

#### Other functions

Layer 1 exposes many functions: some are simple set and get functions to deal with the message structure, while five simplify the access to IPC structures; `build_key()` makes an IPC key from a given character, `create_queue()` and `remove_queue()` manage IPC queues and last `send_message()` and `receive_message()` give a simple way to route messages.

These functions are a straightforward implementation of what was presented in the previous articles so I do not comment the code line by line. I just point out that very simple error management code has been introduced. C language does not allow to use exceptions, so errors have to be managed by functions or returned through the `return` statement. A solid error management in C is outside the scope of this article, so here you will find the bare minimum.

The error management of `receive_message()` needs a little explanation, however: `msgrcv()` fails even when there are no messages of the given type, producing a `ENOMSG` error. This is the reason why that case has been ignored through the `if(errno == ENOMSG)` construct.

## Layer 2

#### The protocol

Layer2 implements the actual communication protocol between the switch and the users. Users send text messages to other users and those messages are routed by the switch. Users and switch can also send service messages; those are messages encompassing information used to manage the system. 

Every user, when spawned, initializes a queue, connects to the switch and communicates the queue id. Since the queues can contain old unread messages it is necessary to empty them before the use. Each user enters a loop where it sleeps a while, receives service messages, sends messages and last receives text messages. At the same time the switch collects queue ids, routes text messages from the sender to the recipient and sometimes sends a service request to a user.

The services the switch can request from users are to terminate and to test the answering time. Both can be requested on a probability base but the first is also forced when the user sends too many messages to unreachable recipients.

The answering time test is performed in a simple way: the switch requests the service and records the time of the request. The user answers with a message that contains the time of the receiving and the switch computes the difference between the two. Pay attention that both the timing service and the random number extraction used to request services on a probability base are not exact; the code works but is good only for a demonstration system.

I defined some useful constants in the header file: `MAX_SLEEP` is the maximum number of seconds a user waits before performing an action, while `TYPE_SERVICE` and `TYPE_TEXT` identify the type of the message. The defines which name starts with `SERVICE_` list all the possible services: `SERVICE_TERMINATE` forces a user to quit; `SERVICE_TIME` makes it perform a timing operation; `SERVICE_CONNECT` and `SERVICE_DISCONNECT` tell the switch a new user has connected or disconnected; `SERVICE_QID` bears the identifier of the user queue to the switch; `SERVICE_UNREACHABLE_DESTINATION` communicates a user that the recipient of a message is no more online.

#### Queues

Two functions are dedicated to queues, `init_queue()` and `close_queue()`. The first builds a IPC key from a given number (previously converting it to a char) and runs the create_queue() system call. Because of the char conversion the range of integers it can accept is 0-255 and for simplicity's sake there is no check in the whole code that a queue key has not yet been assigned. For this example I am simply leveraging that different numbers return different key and, thus, queues. The second function closes the queue running the `remove_queue()` system call. The system call are not used directly to allow the introduction of more complex checks on the assigned queues.

#### User functions

Users have five functions that implement their part of the protocol. Two functions communicate to the switch that the user connected or disconnected, namely `user_send_connect()` and `user_send_disconnect()`; both carry the `sender` id and the switch id `sw`, which however in the main program is always 0. The `user_send_qid()` function communicates to the switch the queue id of the user; `user_send_text_message()` sends a string of text to another user and `user_send_time()` answers the timing service.

#### Switch functions

The switch can execute `switch_send_text_message()` to deliver a text message sent by a user; `switch_send_terminate()` to ask a user to terminate; `switch_send_time()` to ask a user to perform a timing service.

#### Code analysis

Now I will briefly review the whole application code to better explain the different parts of the system. Please remember that this is a demonstration system so many choices have been made for simplicity's sake.

``` c
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

#define MINCHILDS 1
#define MAXCHILDS 15

#define MAXFAILS 10

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
```

After the list of includes you will find three defines that rule the number of child processes the system can spawn and the maximum number of messages a user can send to unreachable recipients before the switch asks it to terminate. The `dest` and `olddest` variables are used to avoid a user to send a message to the recipient of the previous one.


``` c
int queues[MAXCHILDS + 1]; /* Queue identifiers - 0 is the qid of the switch */

int msg_sender;
int msg_recipient;
char msg_text[160];
int msg_service;
int msg_service_data;

int t;
int timing[MAXCHILDS + 1][2];

int unreachable_destinations[MAXCHILDS + 1];

char *padding = "                                                                      ";
char text[160];

messagebuf_t msg, in;
```

Many of this variables are just helpers that simplify the code; `queues` holds the queue identifier of each process. Remember that in C you have to declare variables at the beginning of the program, while in C++ you can declare them anywhere. This means that child processes, that are a copy of the parent, carry in memory some variables such as `queues` and `timings` that are used only by the switch. This is both a waste of resources and a dangerous situation, so remember that in general statically allocated variables are not a good choice for concurrent programs. The `padding` variable is a quick and dirty way to create two columns, the left one filled by swtich messages and the right one by user ones.

``` c
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
  printf("\n");
  printf("     <number of users> - Number of users alive in the system (%d - %d)\n", MINCHILDS, MAXCHILDS);
  printf("     <service probability> - The probability that the switch requires a service from the user (0-100)\n");
  printf("     <text message probability> - The probability the a user sends a message to another user (0-100)\n\n");
}
```



``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

``` c
```

