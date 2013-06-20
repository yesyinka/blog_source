---
layout: post
title: "The Lord of the Rings: an Erlang epic"
date: 2013-06-20 17:49
comments: true
categories: [erlang, concurrent programming]
---

## Abstract

One of the first really challenging problems an Erlang novice must face is the classical process ring, which can be found around the Internet and most notably in "Erlang Programming" by Cesarini and Thompson (page 115).

Its formulation may vary, but the core of it requires the programmer to design and implement a closed ring of processes, to make them pass a given number of messages each other and then terminate gracefully.

I try here to give an in-depth analysis of the matter, to point out some of the most interesting issues of this exercise. I strongly suggest the Erlang novice to try and solve the exercise before looking at the solutions proposed here.
<!--more-->

## Linking processes

The aim of the exercise is to build a chain of processes that exchange messages. This latter specification is important since we are required to connect our processes with the only purpose of sending messages and we know that, in Erlang, a process may send a message to another process simply by knowing its pid.

Actual linking between processes, in Erlang, has the aim of making them exchange _exit signals_; links are thus a mean to control the way the system collapses (or not) when a process crashes. The processes in the ring will thus be linked to ensure that when one of them exits all other processes will be terminated, but this has nothing to do with the ring itself.

The point is to let a process know how to send a message to the next one, forming a chain. This chain, then, is closed, i.e. the last process sends messages to the first one.

There are two main strategies that can be leveraged to get the ring build and behave in a correct way: the **master process** approach and the **recursive** one. For each of the two, I am going to present and analyze the following steps: a ring of processes with **no message** exchange, a ring with **a single message** travelling through it, a ring with **multiple messages** and a ring that exposes a **functional interface** to send messages.

## Debugging

Debugging concurrent applications is everything but simple. For this exercise, it is however enough to get simple information about which process is forwarding which message. To allow a simple removal of debug prints a very small macro has been included in the file `debug.hrl`, activated by the definition of `debug`.

``` erlang
-ifdef(debug).
-define(DEBUG(Format, Args),
        io:format("~p: " ++ Format, [self()] ++ Args)).
-else.
-define(DEBUG(Format, Args), true).
-endif.
```

This converts the line

``` erlang
?DEBUG("A message~n", []).
```

to

``` bash
<0.44.0>: A message
```

where `<0.44.0>` is the pid of the message calling `?DEBUG` (only if `-define(debug, true).` is in the file).


## The “master process” solution

A good way to face the problem of building the ring is to spawn a _master process_, which will then spawn all other processes. This solution keeps the creation phase simple since everything happens between the master process and the process that is currently being spawned.

A small caveat: the creation of the ring must be performed **backwards**. When the master process spawns another process the only other process it knows is the process spawned in the previous loop. This means that the master process spawns a process that connects backwards to it, then spawns a process that connects backward to the second one, and so on.

#### Building the ring

{% include_code lang:erlang erlang-rings/ring_master_no_messages.erl %}

Note that the first spawn cannot directly call `create/2` passing `self()` since at that moment `self()` is the pid of the calling process (e.g. the Erlang shell) and not the pid of the first process of the ring, which is what we want. So we have to bypass this by spawning a process which executes `create/1`, which in turn calls `create/2` with the pid the process extracted with `self()`.

This first program spawns a set of processes that sit down and do nothing. Since they are all linked together (through the link with the master process), you can terminate the whole set by sending an exit signal to one of them. You can get the list of pid and names through the shell function `i()`. Otherwise, you can get and use the pid returned by the function `start/1`.

``` erlang
1> i().
[...]
2> exit(pid(0,44,0), boom).
```

#### Sending a single message

Now I am going to develop further the solution by making a single message travel the whole ring.

The code undergoes the following modifications:

* function `start()` must accept the message and pass it to `create()`.
* function `create()` must accept the message
* when the ring is ready the master process has to send the message to the second process; then it terminates
* each node just forwards the incoming message and terminates

{% include_code lang:erlang erlang-rings/ring_master_single_message.erl %}

Beware that a subtle mechanism conceals here: in Erlang, you can always send a message to a pid, even if it the relative process does not exist; in this latter case the message is simply discarded. This feature allows us to make the first process send the message and terminate without having the last process crash by sending a message to it. Remember that you cannot do this with registered processes alias, only with pids.

#### Sending multiple messages

The exercise requests that a message can travel more than once through the whole ring. Adding this possibility is pretty straightforward: it simply requires to add a parameter to `start()` and `create()` and to change the loop clauses.

Function `loop/1` now becomes `loop/2` and has the following clauses:

* when the number of remaining travels is 1 the process forwards the message and then terminates.
* when the number of remaining travels is more than 1 the process loops again with a decremented value of travels.

{% include_code lang:erlang erlang-rings/ring_master_multiple_messages.erl %}

As before, we are here relying on the possibility of sending a message to a non existent process.

#### Exposing a functional interface

An Erlang best practice is to expose a functional interface to the user that hides the underlying message passing. We are going to convert our ring program to expose the following functions:

* `start/1` - starts a ring with the given number of processes
* `stop/0` - terminates all processes in the ring
* `send_message/1` - sends a message that travels once through the ring
* `send_message/2` - sends a message that travels the given number of times through the ring

To expose a functional interface you need to register one or more processes, to get a global access point for your functions, so the first change to the code is that the spawned process is registered. Note that `Message` and `NumberProcesses` are no more passed to the `create()` function.

``` erlang
register(ring_master_functional, spawn(?MODULE, create, [NumberProcesses])),
```

This is not enough. We have to be sure that the whole ring is ready before giving control back to the user issuing `start()`. Until now the message was sent by the master process just after the creation of the last ring process, so there was no need to synchronize the return of the start function with the spawned processes. Now we need it, so just after registering the master process we wait for a ready message coming from the ring. To allow the ring to send the message to the initial caller we have to pass `self()` to `create()`. Pay attention that `self()` passed to the spawned process is the pid of the external process (e.g. the Erlang shell) while `self()` passed to `create()` is the pid of the master ring process.

{% include_code lang:erlang erlang-rings/ring_master_functional.erl %}

The exposed functions are very simple. As you can see they use two different messages: `{command, stop}` and `{command, message, [Message, Times]}`. One of the advantages of exposing a functional API is that you are free to format your messages according to the current status of the software and change the format if it does no more suit the application needs.

The `loop()` function has been splitted in two different functions now: `loop/1` rules the behaviour of the standard ring process, while `loop_master/1` rules the behaviour of the master process.

The standard process has to react to a stop command, forwarding it and terminating, and to a message command, simply forwarding it. The master process has to check an incoming message to decide if it shall be injected again in the ring.

## A recursive solution

The simplest way to make a process know the pid of another process is to let the first spawn the second, and this gives us a hint about another way the process ring can be built. Like many things in Erlang, this can be solved recursively by saying:

In a process do:

* spawn another process and store its pid, call recursively on the spawned process
* if you are the last process just connect to the first

This solution has the advantage of being very straightforward for an Erlang programmer since it implements the standard recursion pattern. However, it forces the programmer to deal most of the time with the last node, which is a little counterintuitive.

The following programs are the recursive version of the four presented in the previous section. Once grasped the two main differences, building the ring forwards instead of backwards and dealing with the last node instead of the first, the two solutions present pretty much the same evolution steps.

The recursive ring construction.

{% include_code lang:erlang erlang-rings/ring_recursion_no_messages.erl %}

The recursive ring with a single message travelling.

{% include_code lang:erlang erlang-rings/ring_recursion_single_message.erl %}

The recursive ring with support for multiple message travels.

{% include_code lang:erlang erlang-rings/ring_recursion_multiple_messages.erl %}

The recursive ring exposing the functional API.

{% include_code lang:erlang erlang-rings/ring_recursion_functional.erl %}

## Conclusions

The process ring is an exercise that can be solved in many ways (I just presented the two more straightforward ones) but makes the programmer face problems that may later rise in real-world applications. For this reason, it is an invaluable sandbox where the Erlang programmer can try different approaches to solve both the concurrency and the topology problems.

Keep in touch for other Erlang articles on [this page](/blog/categories/erlang/).


