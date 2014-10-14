---
layout: post
title: "Decorators and metaclasses"
date: 2014-10-14 09:43:08 +0200
comments: true
categories: [python3, python, oop]
---

## AAA

Python offers a rich and complex implementation of object-oriented concepts. Among them, the most importan points I want to highlight are:

1. Everything in Python is an object, also functions and classes.
2. The use of references and delegation makes the language polymorphic by design.
3. The use of __getattr__() may blend the separation between the two delegation techniques, inheritance and composition.
4. Metaclasses provide an extended implementation of the constructor concept, giving the programmer a way to customize how the class itself is created.

Let me quickly review a couple of possibilities that these language features open.

#### Sofcoding to the maximum

Sofcoding is the programming technique in which a value is passed to the program at runtime from outside the program itself, as opposed to hardcoding, that embeds values directly into the code.

The choice between hard- and softcoding is one of the big issues of software design as both provide advantages and drawback, so it is a very wide topic, outside the purpose of this post.

Speaking about softcoding advantages, however, one of the possibilities given by the Python feature that "everything is an object" is that of passing classes and functions as arguments to other classes and functions. In other words, we may build a function that expects another function as argument and use this latter inside to code of the former.

A quick example comes from the `list` built-in class, and specifically from its `sort()` method. This latter accepts a function as its only argument, which is used to compare the elements in the list, not differently from what happens in C with the `qsort()` function of the standard library.

Thanks to the references mechanism, functions always accept "generic" objects, and being functions just callable objects you may also give that function a class, provided that it contains a `__call__()` method.

Not only, we may also pass a class to another class that will later instantiate it, to easily implement the Abstract Factory design pattern.

#### Parametrized inheritance

A class may be composed with a class passed as argument at instantiation time, but the use of `__getattr__()` allows the external class to delegate some or all of its tasks to the contained one, just like the two were part of an inheritance tree. A quick example:

``` python
class SoftDelegation:
    def __init__(self, _class, *args, **kwds):
        self.delegate = _class(*args, **kwds)

    def __getattr__(self, attr):
        return getattr(self.delegate, attr)
```

The `SoftDelegation` class is customizable at runtime, since it accepts a class that is instanced and stored as `self.delegate`. At the same time, the `__getattr__()` function redirects all attribute lookup to this latter, making the `SoftDelegation` class actually "inherit" from it.

Please note that this is not completely true, as some special methods may bypass `__getattr__()`. As an example, issuing a `for` loop on an object that delegates to a `list` doesn't work, because the `__iter__()` method is not retrieved through the standard machinery. In most cases, however, such a technique provides you a good approximation of "parametrized inheritance".

## Metaclasses and decorators: a match made in space

Metaclasses are a complex topic, and most of the times even advanced programmers do not see a wide range of practical uses for them. Chances are that this is the part of Python (or other languages that support metaclasses, like Smalltalk and Ruby) that fits the least the "standard" object-oriented patterns or solutions found in C++ and Java, just to mention two big players.

Indeed metaclases usually come in play when programming advanced libraries or frameworks, where a lot of automation must be provided. For example, Django Forms system heavily relies on metaclasses to provide all its magic.

We also have to note, however, that we usually call "magic" or "tricks" all those techniques we are not familiar to, and as a result in Python many things are called this way.

Time to bring some spice into your programming! Let's practice some Python wizardry and exploit the power of the language!

In this post I want to show you an interesting joint use of decorators and metaclasses. I will show you how to use decorators to mark methods so that they can be automatically used by the class when performing a given operation.

More in detail, I will implement a class that can be called on a string to "process" it, and show you how to implement different "filters" through simple decorated methods. What I want to obtain is something like this:

``` python
class StringProcessor:
    [...]

class MyStringProcessor(StringProcessor):
    @stringfilter
    def capitalize(self, str):
        [...]

    @stringfilter
    def remove_double_spaces(self, str):
        [...]

msp = MyStringProcessor()
"A test string" == msp("a test  string")
```

The module defines a `StringProcessor` class that I can inherit and customize adding methods that have a standard signature `(self, str)` and are decorated with `@stringfilter`. This class can later be instatiated and the instance used to directly process a string and return the result. Internally the class automatically executes all the decorated methods in succession. I also would like the class to obey the order I defined the filters: first defined, first executed.

## Metaclasses in action

How can metaclasses help to reach this target?

Simply put, metaclasses are classes that are instantiated to get classes. That means that whenever I use a class, for example to instantiate it, first Python _builds_ that class using the metaclass and the class definition we wrote. For example, you know that you can find the class members in the `__dict__` attribute: this attribute is created by the standard metaclass, which is `type`.

Given that, a metaclass is a good starting point for us to insert some code to identify a subset of functions inside the definition of the class. In other words, we want the output of the metaclass (that is, the class) be built exactly as happens in the standard case, but with an addition: a separate list of all the methods decorated with `@stringfilter`.

You know that a class has a _namespace_, that is a dictionary of what was defined inside the class. So, when a standard metaclass is used to create a class, the class body is parsed and a standard `dict()` object is used to collect the namespace.

We are however interested in preserving the order of definition and a Python dictionary is an unordered structure, so we take advantage of the `__prepare__` hook introduced in the class creation process with Python 3. This function, if present in the metaclass, is used to preprocess the class and to return the structure used to host the namespace. So, following the example found in the official documentation, we start defining a metaclass like

``` python

```

This way, when the class will be created, an `OrderedDict` will be used to host the namespace, allowing us to keep the definition order. Please note that the signature `__prepare__(metacls, name, bases, **kwds)` and the `@classmethod` decorator are enforced by the language.

The second function we want to define in our metaclass is `__new__`. Just like happens for the instantiation of classes, this method is invoked by Python to get a new instance of the metaclass, and is run before `__init__`. Its signature has to be `__new__(cls, name, bases, namespace, **kwds)` and the result shall an instance of the metaclass. As for its normal class counterpart (after all a metaclass is a class), `__new__()` usually wraps the same method of the parent class, `type` in this case, adding its own customizations.

The customization we need is the creation of a list of methods that are marked in some way (the decorated filters). Say for simplicity's sake that the decorated methods have an attribute `_filter`.

The full metaclass is then

``` python

```

Now we have to find a way to mark all filter methods with a `_filter` attribute.

## Dectorators in action

Decorators are, as the name suggests, the best way to augment functions or methods. Remember that a decorator is basically a callable that accepts another callable, processes it, and returns it.

As for metaclasses, decorators are a very powerful and expressive way to implement advanced behaviours in our code. In this case we may easily use them to add an attribute to our decorated methods, one of the most basic tasks for a decorator.

I decided to implement the `@stringfilter` decorator as a function. The reason is that for decorators without arguments the behaviour of a decorator class is different from that of a decorator with arguments and and explanation of that matter would be overkill now. In a later section of this post you will find all the gory details.

The decorator is very simple:

``` python

```

