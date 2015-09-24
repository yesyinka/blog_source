Title: Coroutines and objects
Date: 2015-09-19 10:00:00 +0100
Category: Programming
Tags: concurrent programming, functional programming, generators, OOP, Python, Python2, Python3
Authors: Leonardo Giordani
Slug: python-3-generators-and-coroutines
Summary: An updated edition of the series of posts on Python generators

## Introduction

Coroutines seem to live a second youth during the last years, and for some of the youngest programmers they are a completely new concept. Their invention dates however back to the 50s, and later formalized in the paper "Design of a separable transition-diagram compiler" (1963) by Melvin Conway.

So coroutines are something that roots into the mists of ancient computer science history, born together with LISP (1960) and the integrated circuit (1958). This tells us that the implementation of such a concept does not need anything more complex than a programmable machine.

Object-oriented programming is not a much younger concept. Its first appearances can be found in the LISP community, even if its formal definition comes to life with Simula 67 and brought to success by Smalltalk. Despite the strong 










Two years have passed since I wrote the series of three posts "Python generators - from iterators to cooperative multitasking". Now Python 3 is the official Python version in use and Python 2 is on the run. No, just joking, Python 2 is to be maintained with bugfixes until 2020, but new features will be added to Python 3 only, so I thought it was time to update the information provided in that series.

Since my purpose is to provide updated, consistent and clear information I don't like to copy and paste my essays, refurbishing them with a new title and call them a new post. The old series contains valuable information, and I believe it is still worth reading. Some properties of generators are however missing, most notably the `send()` method of generators and the new `yield from` syntax.

Thus, I will just give a brief review of the generator concept, for the sake of consistency, but I strongly suggest to read the [original series](/blog/2013/03/25/python-generators-from-iterators-to-cooperative-multitasking) of three posts.

## About coroutines and objects

Let me run through a brief review of the differences between coroutines and objects.






, being both ways to encapsulate code.

An object is basically a way to encapsulate code and data, that is to tightly glue data with the functions that modify it. This is just one of the three main pillars of object-oriented programming, but is the one that gives objects a reason to exist, while delegation and polymorphism are behavioural concepts.

An object is thus a set of data and code, but this can also be easily represented with a state machine, that is a status (the data part) with a series of actions that can change it (the code part).

Considering this view of objects you can easily recognize that coroutines are, after all, the same thing. They are functions that can interrupt their execution holding the internal state, and with the `send()` method it is also easy to alter this state when resuming execution.

Obviously coroutines and objects are not the same thing if you consider a more specific definition of object taking into play the class and instance concepts or the implicit delegation mechanism provided by inheritance. In Python however, being everything an object, this difference between coroutines and pure objects is somehow blurred. A generator is an object, even if its internal interface is simplified and hidden. 

