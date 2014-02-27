---
layout: post
title: "OOP concepts in Python"
date: 2014-02-26 10:45:36 +0100
comments: true
categories: [python]
---

## Abstract

Object-oriented programming (OOP) has been the leading programming paradigm for several decades now, starting from the initial attempts back in the 60s to some of the most important languages which are used nowadays. Being a set of programming concepts and design methodologies, OOP can never be said to be "correctly" or "fully" implemented by a language: indeed there are as many implementations as languages.

So one of the most interesting aspects of a OOP language is to understand how it implements those concepts. In this post I am going to try and start analyzing the OOP implementation of the Python language. Due to the richness of the topic, however, I consider this attempt just like a set of thoughts for Python beginners trying to find their way into this beautiful (and sometimes peculiar) language.

## The basic ideas behind OOP

OOP is such a wide topic that it is very hard to find a set of concepts universally addressed as its "basic concepts". Since this is a post focused on Python, however, I list here the OOP concepts that this language implements and takes advantage of, using the language specific nomenclature:

* Objects
* Classes and instances
* Interfaces and types
* Encapsulation (methods and attributes)
* Inheritance
* Polymorphism

(
* Metaprogramming and meta-classes
* Lazy binding
* Dynamic typing
* Message passing between objects
* Abstract base classes
)

Please note that many of those concepts overlap and sometimes is difficult to talk about one of them without referring to another one.

## Why "objects"?

Computer science deals with data and procedures to manipulate them. Everything, from the earliest Fortran programs to the latest mobile apps is about data and its manipulation. 

So if data are the ingredients and procedures are the recipes, it seems (and can be) reasonable to keep them separate. Let's do some functional programming in Python

``` python
# This is some data
data = (13, 63, 5, 378, 58, 40)

# This is a procedure that computes the average
def avg(d):
    return sum(d)/len(d)
    
print avg(data)
```

As you can see the procedure is quite good and general: the procedure is run on a sequence of data, and it returns the average of the sequence items. So far, so good: computing the average of some numbers leaves the numbers untouched and creates new data.

The observation of the everyday world, however, shows that _complex data mutate_: an electrical device is on or off, a door is open or closed, the content of a bookshelf in your room changes as you buy new books. This can still be managed keeping data and procedures separate, for example

``` python
# These are two numbered doors, initially closed
door1 = [1, 'closed']
door2 = [2, 'closed']

# This procedure opens a door
def open(door):
    door[1] = 'open'
    
open(door1)
print door1
```

What happens when I introduce a "lockable door" data type, which can be opened only when it is not locked? Let's see

``` python
# These are two standard doors, initially closed
door1 = [1, 'closed']
door2 = [2, 'closed']

# This is a lockable door, initially closed and unlocked
ldoor1 = [1, 'closed', 'unlocked']

# This procedure opens a standard door
def open(door):
    door[1] = 'open'

# This procedure opens a lockable door
def lopen(door):
    if door[2] == 'unlocked`:
        door[1] = `open`

open(door1)
print door1

lopen(ldoor1)
print ldoor1

```

Everything works, no surprises in this code. However, as you can see, I had to find a different name for the procedure that opens a locked door, since its implementation differs from the procedure that opens a standard door. But, wait... I'm _opening_ a door, the action is the same, and it just changes the status of the door itself. So why shall I remember that a locked door shall be opened with `lopen()` instead of `open()` if the verb is the same?

Chances are that this separation between data and procedures doesn't perfectly fit some situations. The key problem is that the "open" action is not actually _using_ the door; rather it is _changing its state_. So, just like the volume control buttons of your phone, which are _on_ your phone, the "open" procedure should stick to the "door" data.

This is exactly what leads to the concept of _object_: an object, in the OOP context, is a structure holding data _and_ procedures operating on them.

## What is "type"?

When you talk about data you immediately need to introduce the concept of _type_. This concept can have two meanings that are worth being mentioned in computer science: the _behavioural_ and the _structural_ one.

The behavioural meaning represents the fact that you know what something is by describing how it acts. This is the foundation of the so-called "duck typing" (here "typing" means "to give a type" and not "to type on a keyboard"): if it acts like a duck, it _is_ a duck.

The structural meaning identifies the type of something by looking at its internal structure. So two things that act in the same way but are internally different are of different type.

Both points of view can be valid, and different languages may implement and emphasize one meaning of type or the other, and even both.

## Python classes and instances

Objects in Python may be build describing their structure through a _class_. A class is the programming representation of a generic object, such as "a book", "a car", "a door": when I talk about "a door" everyone can understand what I'm saying, without the need of referring to a specific door in the room.

In Python the type of an object is represented by the class used to build the object: that is, in Python the word type has the same meaning of the word class (more on this later).

For example, one of the built-in classes of Python is `int`, which represents an integer number

``` python
>>> a = 6
>>> print a
6
>>> print type(a)
<type 'int'>
>>> print a.__class__
<type 'int'>
```

As you can see the built-in function `type()` returns the content of the _magic attribute_ `__class__` (magic means that its value is managed by Python itself offstage). The type of the variable `a`, or its class, is `int`.

Once you have a class you can _instantiate_ it to get a concrete object of that type (_instance_), i.e. built according to the structure of that class. The Python syntax to instantiate a class is the same of a function call

``` python
>>> b = int()
>>> type(b)
<type 'int'>
```

When you create an instance you can pass some values, according to the class definition, to _initialize_ it.

``` python
>>> b = int()
>>> print b
0
>>> b = int(7)
>>> print b
7
```

In this example the `int` class creates an integer with value 0 when called without arguments, otherwise it uses the given argument to initialize the newly created object.

Let us write a class that represents a door to match the functional examples done in the first section

``` python
class Door(object):
    def __init__(self, number, status):
        self.number = number
        self.status = status
        
    def open(self):
        self.status = 'open'
        
    def close(self):
        self.status = 'closed'
```

The `class` keyword defines a new class named `Door`; everything indented under `class` is part of the class. The functions you write inside the object are called _methods_ and don't differ at all from standard functions; the name changes only to highlight the fact that those functions now are part of an object.

Methods of a class must accept as first argument a special value called `self` (the name is a convention but please never break it).

The class can be given a special method called `__init__` which is run when the class is instantiated, receiving the arguments passed when calling the class; the general name of such a method, in the OOP context, is _constructor_, even if the `__init__` method is not the only part of this mechanism in Python (more on this later)

The `self.number` and `self.status` variables are called _attributes_ of the object. In Python, methods and attributes are both _members_ of the object and are accessible with the dotted syntax; the difference between attributes and methods is that the latter can be called (the Python exact nomemclature is _callable_).

As you can see the `__init__` method creates and initializes the attributes, since they are not declared elsewhere.

Now the class can be used to create a concrete object

``` python
>>> door1 = Door(1, 'closed')
>>> type(door1)
<class '__main__.Door'>
>>> print door1.number
1
>>> print door1.status
closed
```

Now `door1` is an instance of the `Door` class; `type()` returns the class as `__main__.Door` since the class was defined directly in the interactive shell, that is in the current main module. To call a method of an object, that is to run one of its internal functions, you just access it as an attribute with the dotted syntax and call it like a standard function.

``` python
>>> door1.open()
>>> print door1.number
1
>>> print door1.status
open
```

In this case the `open()` method of the `door1` instance has been called. No arguments have been passed to the `open()` method, but if you review the class declaration, you see that it was declared to accept an argument (`self`). When you call a method of an instance, Python automatically passes the instance itself to the method as the first argument.

You can create as many instances as needed and they are completely unrelated each other. That is the changes you make on one instance do not reflect on another instance of the same class.

#### Recap

Objects are described by a _class_, which can generate one or more _instances_, unrelated each other. A class contains _methods_, which are functions, and they accept at least one argument called `self`, which is the actual instance on which the method has been called. A special method, `__init__` deals with the initialization of the object, that is, sets the initial value of the _attributes_.

## Python classes strike again

The Python implementation of classes has some peculiarities. The bare thruth is that in Python the class of an object is an object itself. You can check this by issuing `type()` on the class itself

``` python
>>> type(door1)
<class '__main__.Door'>
>>> print type(Door)
<type 'type'>
```

This shows that the `Door` class is an object, an instance of the `type` class.

This concept is not so difficult to grasp as it can seem at first sight: in the real world we deal with concepts using them like _things_: for example we can talk about the concept of "door", telling people how a door looks like and how it works. So in our everyday experience the _type_ of an object is an object itself. In Python this can be expressed by saying that _everything is an object_. More on this later.

If the class is an instance it is a concrete object and is stored somewhere in memory. Let us leverage the _inspection_ capabilities of Python and its `id()` function to check the status of our objects. The `id()` built-in function returns the memory position of an object.

First of all let's check that the two objects are stored at different addresses

``` python
>>> hex(id(door1))
'0x7fa4c818bad0'
>>> hex(id(door2))
'0x7fa4c818b890'
```

**Please note that your values are very likely to be different from the ones I got.**
This confirms that the two instances are separate and unrelated.

However if we use `id()` on the class of the two instances we discover that the class is _exactly_ the same

``` python
>>> hex(id(door1.__class__))
'0x766800'
>>> hex(id(door2.__class__))
'0x766800'
```

Well this is very important. In Python, a class is not just the schema used to build an object. Rather, the class is a shared living object, which code is accessed at run time.

As we already tested, however, attributes are not stored in the class but in every instance, due to the fact that `__init__` works on `self` when creating them. Classes can be given _class attributes_, that is attributes that are stored in the class object, thus being shared among the class instances

``` python
class Door(object):
    color = 'brown'

    def __init__(self, number, status):
        self.number = number
        self.status = status
        
    def open(self):
        self.status = 'open'
        
    def close(self):
        self.status = 'closed'
```

The `color` attribute here is not created using `self`, and any change of its value reflects on all instances

``` python
>>> door1 = Door(1, 'closed')
>>> door2 = Door(2, 'closed')
>>> Door.color
'brown'
>>> door1.color
'brown'
>>> door2.color
'brown'
>>> Door.color = 'white'
>>> Door.color
'white'
>>> door1.color
'white'
>>> door2.color
'white'
>>> hex(id(Door.color))
'0xb74c5420L'
>>> hex(id(door1.color))
'0xb74c5420L'
>>> hex(id(door2.color))
'0xb74c5420L'
```

What happens to methods? Are they shared or not? Let us test it a bit

``` python
>>> Door.__init__
<unbound method Door.__init__>
>>> door1.__init__
<bound method Door.__init__ of <__main__.Door object at 0x869a28c>>
>>> type(Door.__init__)
<type 'instancemethod'>
>>> type(door1.__init__)
<type 'instancemethod'>
>>> hex(id(Door.__init__))
'0x854e964'
>>> hex(id(door1.__init__))
'0x854e964'
>>> hex(id(door2.__init__))
'0x854e964'
>>> 
```

Well this puts some new stuff into play. First of all you see that the method can be _unbound_ or _bound_, then its type is `instancemethod`. Last, you see that despite of being bound and unbound, the method is always the same (its address is the same for the class and for the two instances).




#### Inheritance ???

Our experience shows us also that it makes sense to talk about _concrete instances_ of concepts. When I talk about "my cat" I am referring to a concrete instance of the "cat" concept, which is a _subtype_ of "animal". So there is a very important difference between types and instances, despite being them both objects. While types can be _specialized_, instances cannot.
