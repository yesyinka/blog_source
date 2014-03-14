---
layout: post
title: "OOP concepts in Python 2.x - Part 2"
date: 2014-03-10 18:56:56 +0100
comments: false
categories: [python]
---

## Abstract

This post continues the analysis of the Python OOP implementation started with [this post](/blog/2014/03/05/oop-concepts-in-python-2-dot-x-part-1), which I recommend reading before taking on this new one.

This second post discusses the following OOP features in Python:

* Polymorphism
* Interfaces and types
* Metaclasses

_This post refers to the internals of Python 2.x - please note that Python 3.x changes (improves!) some of the features shown here. As soon as I feel comfortable with my Python 3 knowledge, I will post an update._

<!--more-->

## Escape from Polymorphism

The term _polymorphism_, in the OOP lingo, refers to the ability of an object to adapt the code to the type of the data it deals with. Polymorphism has two major applications in a OOP language. The first is that an object may provide different implementations of one of its methods depending on the type of the input parameters. The second is that code written for a given type of data may be used on data with a derived type, i.e. methods understand the class hierarchy of a type.

In Python polymorphism is one of the key concept, and we can say that it is a built-in feature. Let us deal with this concept step by step.

First of all, you know that in Python the type of a variable is not explicitly declared. Beware that this does not mean that Python variables are _untyped_. On the contrary, everything in Python has a type, it just happens that the type is implicitly assigned. If you remeber the last paragraph of the previous post, I stated that in Python variables are just pointers (using a C-like nomenclature), in other words they just tell the language _where_ in memory a variable has been stored. What is stored at that address is not a business of the variable.

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

So, thanks to the reference system, Python type system is both _strong_ and _dynamic_. The definition of those two concepts is not universal, so if you are interested in those concepts be ready to dive into a broad matter. However, in Python, the meaning of those two words is the following:

* type system is _strong_ because everything has a well-defined type, that you can check with the `type()` built-in
* type system is _dynamic_ since the type of a variable is not explicitly declared, but changes with the content

Onward! We just scratched the surface of the whole thing.

To explore the subject a little more, try to define the simplest function in Python (apart from an empty function)

``` python
def echo(a):
    return a
```

Pretty straightforward, isn't it? Well if you come from a static compiled language such as C or C++ you should be at least puzzled. What is `a`? I mean: what type of data does it contain? And how can Python know what it is returning if there is no type specification?

Again, if you recall the references stuff everything becomes clear: that function accepts a reference and returns a reference. In other words we just defined a sort of universal function, that does the same thing regardless of the input.

Well, this is exactly the problem that polymorphism wants to solve. We want to describe an action regardless of the type of objects, and this is what we do when we talk among humans. When you explain how to move an object by pushing it, you may explain it using a box, but you expect the person you are addressing to be able to repeat the action even if you need to move a pen, or a book, or a bottle.

There are two main strategies you can apply to get code that performs the same operation regardless of the input types.

The first strategy is to cover all cases, and this is a typical approach of procedural languages. If you need to sum two numbers that can be integers, float or complex, you just need to write three `sum()` functions, one bound to the integer type, the second bound to the float type and the third bound to the complex type, and to have some language feature that takes charge of choosing the correct implementation depending on the input type. This logic can be implemented by a compiler (if the language is statically typed) or by a runtime environment (if the language is dynamically typed) and is the approach chosen by C++. The disadvantage of this solution is that it requires the programmer to forecast all the possible situations: what if I need to sum an integer with a float? What if I need to sum two lists? (Please note that C++ is not so poorly designed, and the operator overloading technique allows to manage such cases, but the base polymorphism strategy of that language is the one exposed here).

The second strategy, the one implemented by Python, is to simply require the input objects to solve the problem for you. In other words you _ask the data itself to perform the operation_, reversing the problem. Instead of writing a bunch on functions that sum all the possibile types in every possible combination you just write one function that requires the input data to sum, trusting that they know how to do it. Does it sound complex? It is not.

Let's look at the Python implementation of the `+` operator. When we write `c = a + b`, Python actually executes `c = a.__add__(b)`. As you can see the sum operation is delegated to the first input variable. So if we write

``` python
def sum(a, b):
    return a + b
```

there is no need to specify the type of the two input variables. The object `a` (the object contained in the variable `a`) shall be able to sum with the object `b`. This is a very beautiful and simple implementation of the polymorphism concept. Python functions are polymorphic simply because they accept everything and trust the input data to be able to do perform some actions.

Let us consider another simple example before moving on. The built-in `len()` function returns the length of the input object. For example

``` python
>>> l = [1, 2, 3]
>>> len(l)
3
>>> s = "Just a sentence"
>>> len(s)
15
```

As you can see it is perfectly polymorphic: you can feed both a list or a string to it and it just computes its length. Does it work with any type? let's check

``` python
>>> d = {'a': 1, 'b': 2}
>>> len(d)
2
>>> i = 5
>>> len(i)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: object of type 'int' has no len()
```

Ouch! Seems that the `len()` function is smart enough to deal with dictionaries, but not with integers. Well, after all, the lenght of an integer is not defined.

Indeed this is exactly the point of Python polymorphism: _the integer type does not define a lenght operation_. While you blame the `len()` function, the `int` type is at fault. The `len()` function just calls the `__len__()` method of the input object, as you can see from this code

``` python
>>> l.__len__()
3
>>> s.__len__()
15
>>> d.__len__()
2
>>> i.__len__()
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
AttributeError: 'int' object has no attribute '__len__'
```

but the `'int' object` does not define any `__len__()` method.

So, to sum up what we discovered until here, I would say that _Python polymorphism is based on delegation_. In the following sections we will talk about the [EAFP](http://docs.python.org/2/glossary.html#term-eafp) Python principle, and you will see that the delegation principle is somehow ubiquitous in this language.

## Polymorphism II

Another real-life concept that polymorphism wants to bring into a programming language is the ability to walk the class hierarchy, that is _to run code on specialized types_. This is a complex sentence to say something we are used to do every day, and an example will clarify the matter.

You know how to open a door, it is something you learned in your early years. Under an OOP point of view you are an object (sorry, no humiliation intended) which is capable of interacting with a wood rectangle rotating on hinges. When you can open a door, however, you can also open a window, which, after all, is a specialized type of wood-rectangle-with-hinges, hopefully with some glass in it too. You are also able to open the car door, which is also a specialized type (this one is a mix between a standard door and a window). This shows that, once you know how to interact with the most generic type (basic door) you can also interact with specialized types (window, car door) as soon as they act like the ancestor type (e.g. as soon as they rotate on hinges).

This directly translates into OOP languages: polymorphism requires that _code written for a given type may also be run on derived types_. For example a list (a generic list object, not a Python one) that can contain "numbers" shall be able to accept integers because they _are_ numbers. The list could specify an ordering operation which requires the numbers to be able to compare each other. So, as soon as integers specify a way to compare each other they can be inserted into the list and ordered.

Statically compiled languages shall provide specific language features to implement this part of the polymorphism concept. In C++, for example, the language needs to introduce the concept of pointer compatibility between parent and child classes.

In Python there is no need to provide special language features to implement subtype polymorphism. As we already discovered Python functions accept any variable without checking the type and rely on the variable itself to provide the correct methods. But you already know that a subtype must provide the methods of the parent type, either redefining them or through impleicit delegation, so as you can see Python implements subtype polymorphism from the very beginning.

I think this is one of the most important things to understand when working with this language. Python is not really interested in the actual type of the variables you are working with. It is interested in how those variables act, that is it just wants the variable _to provide the right methods_. So, if you come from statically typed languages, you need to make a special effort to think about _acting like_ instead of _being_. This is what we called "duck typing".

Time to do an example.

``` python
example
```

Explanation.

## List can contain anything

If you recall the list object example I made before, you can now understand why Python lists can accept any type of object. The problem is not what type of object the list accepts, but what shall the object do to be hosted in a list. For example, Python could require the objects to be able to compare each other

## EAFP

EAFP is a Python acronym that stands for _easier to ask for forgiveness than permission_. This coding style is highly pushed in the Python community because it completely relies on the duck typing concept, thus fitting well with the language philosophy. 

## Abstract Base Classes






## Notes

This raises a problem: where is the code that implements the actual opeation on that type? After all summing integers is different from summing lists, and joining two files is not the same as joining two databases. 

The dirty secret is that Python simply trusts the objects to provide suitable methods to answer the requests made in a given code.






Python implements the polymorphism techniques by converting every operation on an object in a method call, thus asking the object to answer a request.







At this point it is convenient to talk about two different aspects of polymorphism: _operator overloading_ and _method


In a statically typed language like C++, polymorphism can be achieved defining as many function as types, all with the same name. The language is in charge of choosing the right implementation depending on the types of the variables you are using at a given time. In Python there is no need to do this: polymorphism is a built-in feature, thanks to the untyped variables.

Beware: the Python way has many advantages and many shortcomings, like any solution. I believe that the advantages are enough to overcome the disadvantages, but do not expect them to be the heal-all for your programming issues.

Ok, we definitely understood that Python deals with references and not with actual values. A question, however, arises: if Python does not know in advance the type of the variables, how can it perform the correct action? For example, let us take a very simple unary operator, `len()`, which returns the length of an object. We know that calling `len(a)` returns the length of the object referenced by `a`, regardless of the type of that object. But there shall be a way for Python to select the correct 






Do you see the problem? "The length of an object" is a very loose definition. For a list, the length is simply the number of elements contained in it. For a dictionary it is defined as the number of key/value couples in it. And for a file? The number of characters? The number of lines? And for an integer?

This shows the essence of polymorphism: we use the same word for different actions, and we expect the objects to act accordingly. So polymorphism is an OOP feature, since it shifts the 




So how does Python know how to perform actions on the variables? After all, you can talk about, for example, _addition_ bu







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

