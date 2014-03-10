---
layout: post
title: "OOP concepts in Python 2.x - Part 2"
date: 2014-03-10 18:56:56 +0100
comments: false
categories: [python]
---

## Abstract

This post continues the analysis of the Python OOP implementation started with [this post](), which I recommend reading before taking on this new one.

This second post discusses the following OOP features in Python:

* Polymorphism
* Interfaces and types
* Metaclasses

_This post refers to the internals of Python 2.x - please note that Python 3.x changes (improves!) some of the features shown here. As soon as I feel comfortable with my Python 3 knowledge, I will post an update._

<!--more-->

## Escape from Polymorphism

The term _polymorphism_, in the OOP lingo, refers to the ability of an object to adapt the code to the type of the data it deals with. Polymorphism has two major applications in a OOP language. The first is that an object may provide different implementations of one of its methods depending on the type of the input parameters. The second is that code written for a given type of data may be used on data with a derived type, i.e. methods understand the class hierarchy of a type.

In Python polymorphism is one of the key concept, and we can say that it is a built-in feature. Let us deal with this concept step by step.

First of all, you know that in Python the type of a variable is not explicitely declared. Beware that this does not mean that Python variables are _untyped_. On the contrary, everything in Python has a type, it just happens that the type is implicitly assigned. If you remeber the last paragraph of the previous post, I stated that in Python variables are just pointers (using a C-like nomenclature), in other words they just tell the language _where_ in memory a variable has been stored. What is at that address is not a business of the variable.

``` python
>>> a = 5
>>> a
5
>>> type(a)
<type 'int'>
>>> hex(id(a))
'0x89812b0'
>>> a = "five"
>>> a
'five'
>>> type(a)
<type 'str'>
>>> hex(id(a))
'0xb74bb280L'
```

This little example shows a lot about the Python typing system. The variable `a` is not statically declared, after all it can contain only one type of data: a memory address. When we assign the number 5 to it, Python stores in `a` the _address_ of the number 5 (`0x89812b0` in my case, but your result will be different). The `type()` built-in function is smart enough to understand that we are not asking about the type of `a` (which is always a reference), but about the type of the content. When you store another value in `a`, the string `"five"`, Python shamelessly replaces the previous content of the variable with the new address.

So, thanks to the reference system, Python type system is both _strong_ and _dynamic_. The definition of those two concepts is not universal, so if you are interested in those concepts be ready to dive into a brad matter. However, in Python, the meaning of those two words is the following:

* type system is _strong_ because everything has a well-defined type, that you can check with the `type()` built-in
* type system is _dynamic_ since the type of a variable is not explicitly declared, but changes with the content

Onward! We just scratched the surface.

To explore the subject a little more, try to define the simplest function in Python (apart from an empty function)

``` python
def echo(a):
    return a
```

Pretty straightforward, isn't it? Well if you come from a static compiled language such as C or C++ you should be at leat puzzled. What is `a`? I mean: what type of data does it contain? And how can Pyton know what it is returning if there is no type specification?

Again, if you recall the references stuff, everything becomes clear: that function accepts a reference and returns a reference. In other words we just defined a sort of universal function, that does that same thing regardless of the input.

This is exactly the problem that polymorphism wants to solve. We want to describe an action regardless of the type of objects, and this is what we do when we talk among humans. When you explain how to move an object by pushing it, you may explain it using a box, but you expect the person you are addressing to be able to repeat the action even if you need to move a pen, or a book, or a bottle.

In a statically typed language like C++ polymorphism can be achieved defining as many function as types, all with the same name. The language is in charge of choosing the right implementation depending on the types of the variables you are using at a given time. In Python there is no need to do this: polymorphism is a built-in feature, thanks to the untyped variables.

Beware: the Python way has many advantages and many shortcomings, like any solution. I believe that the advantages are enough to overcome the disadvantages, but do not expect them to be the heal-all for your programming issues.







What happens when we define a function in Python, for example a simple sum of two elements?

``` python
def sum(a, b):
    return a + b
```



This question is important, since the _sum_ is an operation that changes from type to type. Summing integers means to perform the sum as we know from the basic mathematics, but what is the meaning of summing... strings, for example? Or dictionaries? Or databases? And so on. With that function we told 


In Python variable types are not declared, and this also extends to function parameters. This means that there is no embedded mechanism to check whether the variables passed to a function are of a given type or not. As a consequence you must write methods trusting that the input parameters are of the given type.


Python implements polymorphism in a very simple way: everything is an object (remember?) and variable types are not declared. When you write a method in Python you just name the input parameters, without specifying the type of each parameter

## Interfaces and types

## Metaclasses

## Movie Trivia

If you are wondering why section titles are so weird, chances are that you missed some good movies to watch while you are not coding =). The movies are: _Back to the Future_, _What About Bob?_, _Wargames_, _The Empire Strikes Back_, _Raiders of the Lost Ark_, _Revenge of the Nerds_, _When Harry Met Sally_, _Apocalypse Now_, _Enter the Dragon_.

## Sources

Some sources for the content of this post. Thank you authors!

* [The official Python documentation](http://docs.python.org/2/tutorial/classes.html) and the Python source code.
* [Python Types and Objects - by Shalabh Chaturvedi](http://www.cafepy.com/article/python_types_and_objects/)
* [Python’s super() considered super! - by Raymond Hettinger](http://rhettinger.wordpress.com/2011/05/26/super-considered-super/)
* [Design Patterns in Python - by Alex Martelli](http://www.aleax.it/gdd_pydp.pdf)
* Many [Stackoverflow](http://stackoverflow.com/questions/tagged/python) questions and answers

## Updates

2014-03-08: "When methods met classes" section had a typo: the sentence "The class method is bound to the class itself, while the instance method is bound to the class of the instance." is "while the instance method is bound to instance of the class.". Thanks [Mohcin Shah](https://plus.google.com/u/0/116309787480950087834/posts) for spotting it!

2014-03-10: Fixed link to Alex Martelli's presentation.

## Feedback

Feel free to use [the blog Google+ page](https://plus.google.com/u/0/b/110554719587236016835/110554719587236016835/posts) to comment the post. The [GitHub issues](https://github.com/lgiordani/lgiordani.github.com/issues) page is the best place to submit corrections.

