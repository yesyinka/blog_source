Title: Functional Python: decorators
Date: 2015-04-21 16:00:00 +0100
Category: Programming/Projects
Tags: decorators, functional programming, OOP, Python
Authors: Leonardo Giordani
Slug: functional-python-decorators
Summary: An insight into the power of Python decorators, their syntax and some usage examples

This post is the result of a lot of personal research on Python decorators, meta- and functional programming. I want however to thank Bruce Eckel and the people behind the open source book "Python 3 Patterns, Recipes and Idioms" for a lot of precious information on the subject. See the Resources section at the end of the post to check their work.

## Is Python functional?

Well, no. Python is a strong object-oriented programming language and is not really going to mix OOP and functional like, for example, Scala (which is a very good language, by the way).

However, Python provides _some_ features taken from functional programming. Generators and iterators are one of them, and Python is not the only non pure functional programming language to have them in their toolbox.

Perhaps the most distinguishing feature of functional languages is that functions are first-class citizens (or first-class objects). This means that functions can be passed as an argument to other functions or can be returned by them. Functions, in functional languages, are just one of the data types available (even if this is a very rough simplification).

Python has three important features that allows it to provide a functional behaviour: references, function objects and callables.

### References

Python variables share a common nature: they are all references. This means that variables are not typed per se, being pure memory addresses, and that functions do not declare the incoming data type for arguments (leaving aside gradual typing). Python polymorphism is based on delegation, and incoming function arguments are expected to provide a given behaviour, not a given structure.

Python functions are thus ready to accept every type of data that can be referenced, and functions can.

## Functions are objects

Since Python pushes the object-oriented paradigm to its maximum, it makes a point of always following the tenet _everything is an object_. So Python functions are objects as you can see from this simple example

``` pycon
>>> def f():
...  pass
... 
>>> type(f)
<class 'function'>
>>> type(type(f))
<class 'type'>
>>> type(f).__bases__
(<class 'object'>,)
>>>
```

Given that, Python does nothing special to treat functions like first-class citizens, it simply recognizes that they are objects just like any other thing.

### Callables

While Python has the well-defined `function` class seen in the above example, it relies more on the presence of the `__call__` method. That is, in Python any object can act as a function, provided that it has this method, which is invoked when the object is "called".

This will be crucial for the discussion about decorators, so be sure that you remember that we are usually more interested in _callable objects_ and not only in functions, which, obviously, are a particular type of callable objects (or simply _callables_).

This can also be shown with some simple code

``` pycon
>>> def f():
...  pass
... 
>>> f.__call__
<method-wrapper '__call__' of function object at 0xb6709fa4>
```

## Metaprogramming

While this is not a post on languages theory, it is worth spending a couple of words about metaprogramming. Usually "programming" can be seen as the task of applying transformations to data. Data and functions can be put together by an object-oriented approach, but they still are two different things. But you soon realize as you may run some code to change data, you may also run some code to change the code itself.

In low level languages this can be very simple, since at machine level everything is a sequence of bytes, and changing data or code does not make any difference. One of the most simple examples that I recall from my x86 Assembly years is the very simple self obfuscation code found is some computer viruses. The code was encrypted with a [XOR cipher](http://en.wikipedia.org/wiki/XOR_cipher) and the first thing the code itself did upon execution was to decrypt its own code and then run it. The purpose of such tricks was (and is) to obfuscate the code such that it would be difficult for an antivirus to find the virus code and remove it. This is a very primitive form of metaprogramming, since it recognizes that for Assembly language there is no real distinction between code and data.

In higher lever languages such as Python achieving metaprogramming is no more a matter of changing byte values. It requires the language to treat its own structures as data. Every time we are trying to alter the behaviour of a language part we are actually metaprogramming. The first example that usually comes to mind are metaclasses (probably due to the "meta" word in their name), which are actually a way to change the default behaviour of the class creation process. Classes (part of the language) are created by another part of the language (metaclasses).

## Decorators

Metaclasses are often perceived as a very tricky and dangerous thing to play with, and indeed they are seldom required in Python, with the most notable exception (no pun intended) being the Abstract Base Classes provided by the `collections` module.

Decorators, on the other side, are a feature loved by many experienced programmers and after their introduction the community has developed a big set of very interesting use cases.

I think that the first approach to decorators is often difficult for beginners because the functional version of decorators are indeed a bit complex to understand. Luckily, Python allows us to write decorators using classes too, which make the whole thing really easy to understand and write, I think, with the only (important) caveat about decorator arguments.

So I will now review Python decorators starting from their rationale, then looking at class-based decorators without arguments, class-based decorators with arguments, and finally moving to function-based decorators.

## Rationale

What are decorators, and why should you learn how to define and use them? Well, decorators are a way to change the behaviour of a function or a class, so they are actually a way of metaprogramming, but they make it a lot more accessible than metaclasses do. Decorators are, in my opinion, a very _natural_ way of altering functions and classes.

Moreover, with the addition of some syntactic sugar, they are a very compact way to both make changes and signal that those changes have been made.

The best syntactic form of a decorator is the following

``` python
@dec
def func(*args, **kwds):
    pass
```

where `dec` is the name of the decorator and the function `func` is said to be _decorated_ by it. As you can see it is very quick to identify that the function has a special label attached, thus being altered in its behaviour.

This form, however, is just a simplification of the more generic form

``` python
def func(*args, **kwds):
    pass
func = dec(func)
```

## Class-based decorators without arguments



