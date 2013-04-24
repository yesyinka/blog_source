---
layout: post
title: "Concurrent programming - 6"
date: 2013-04-23 08:29
comments: true
categories: [operating systems, concurrent programming]
---

## Abstract

It's time to build something bsed on the concepts introduced in the past articles, to demonstrate a practical use of IPC structures. In this article I am going to show and comment the code of a very simple communication simulator written in C. The code leverages IPC queues to allow multiple processes to talk each other while running concurrently.

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
 int receiver;
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

Here `sender` and `receiver` are numbers that identify the switch and the users (0 is the switch, then users are numbered increasingly when they connect to the switch); `text` is the content of a message a user sends to another user, and is 160 characters long to mimic SMS behaviour. Last `service` is the identifier of some system operation, like a request the switch sends to the users; `service_data` carries the optional data the service needs to communicate. Actual services are implemented in layer 2, I am going to describe them later.

Layer 1 exposes many functions: some are simple set and get functions to deal with the message structure, while five simplify the access to IPC structures; `build_key()` makes an IPC key from a given character, `create_queue()` and `remove_queue()` manage IPC queues and last `send_message()` and `receive_message()` give a simple way to route messages.

These functions are a straightforward implementation of what was presented in the previous articles so I do not comment the code line by line. I just point out that very simple error management code has been introduced. C language does not allow to use exceptions, so errors have to be managed by functions or returned through the `return` statement. A solid error management in C is outside the scope of this article, so here you will find the bare minimum.

The error management of `receive_message()` needs a little explanation, however: `msgrcv()` fails even when there are no messages of the given type, producing a `ENOMSG` error. This is the reason why that case has been ignored through the `if(errno == ENOMSG)` construct.


