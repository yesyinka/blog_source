---
layout: post
title: "OOP concepts in Python 2.x - Part 3"
date: 2014-06-11 14:41:11 +0200
comments: false
categories: [python, oop]
---

## Abstract

Welcome to the third installment of this little series of posts about Python OOP implementation. The [first](/blog/2014/03/05/oop-concepts-in-python-2-dot-x-part-1) and [second](/blog/2014/03/10/oop-concepts-in-python-2-dot-x-part-2) issues introduced the most important concepts at the basis of Python as an object-oriented language.

This post will continue the discussion about metaclasses, introducing Abstract Base Classes.

_This post refers to the internals of Python 2.x - please note that Python 3.x changes (improves!) some of the features shown here. As soon as I feel comfortable with my Python 3 knowledge, I will post an update._

## The Inpection Club

As you know, Python leverages polymorphism at its maximum by dealing only with generic references to objects. This makes OOP not an addition to the language but part of its structure from the ground up. Moreover, Python pushes the EAFP appoach, which tries to avoid direct inspection of objects as much as possible.

It is however very interesting to read what Guido van Rossumas says in [PEP 3119](http://legacy.python.org/dev/peps/pep-3119/): _Invocation means interacting with an object by invoking its methods. Usually this is combined with polymorphism, so that invoking a given method may run different code depending on the type of an object. Inspection means the ability for external code (outside of the object's methods) to examine the type or properties of that object, and make decisions on how to treat that object based on that information. [...] In classical OOP theory, invocation is the preferred usage pattern, and inspection is actively discouraged, being considered a relic of an earlier, procedural programming style. However, in practice this view is simply too dogmatic and inflexible, and leads to a kind of design rigidity that is very much at odds with the dynamic nature of a language like Python._

The author of Python recognizes that forcing the use of a pure polymorphic approach leads sometimes to solutions that are too complex or even incorrect. In this section I want to show some of the problems that can arise from a pure polymorphic approach and introduce Abstract base Classes, which aim to solve them. I strongly suggest to read [PEP 3119](http://legacy.python.org/dev/peps/pep-3119/) (as for any other PEP) since it contains a deeper and better explanation of the whole matter. Indeed I think that this PEP is so well written that any further explanation is hardly needed. I am however used to write explanations to check how much I understood about the topic, so I am going to try it this time too.

The EAFP coding style requires you to trust the incoming objects to provide the attributes and methods you need, and to manage the possible exceptions, if you know how to do it. Sometimes, however, you need to test if the incoming object matches a complex behaviour. For example, you could be interested in testing if the object _acts_ like a list, but you quickly realize that the amount of methods a `list` provides is very big and this could lead to odd EAFP code like

``` python
try:
    obj.append
    obj.count
    obj.extend
    obj.index
    obj.insert
    [...]
except AttributeError:
    [...]
```

where the methods of the `list` type are accessed (not called) just to force the object to raise the `AttributeError` exception if they are not present. This code, however, is not only ugly but also wrong. If you recall the "Enter the Composition" section of the [first post](/blog/2014/03/05/oop-concepts-in-python-2-dot-x-part-1), you know that in Python you can always customize the `__getattr__()` method, which is called whenever the requested attribute is not found in the object. So I could write a class that passes the test but actually does not act like a list

``` python
class FakeList(object):
    def fakemethod(self):
        pass
    
    def __getattr__(self, name):
        if name in ['append', 'count', 'extend', 'index`, 'insert', ...]:
            return self.fakemethod
```

This is obviously just an example, and no one will ever write such a class, but this demonstrates that just accessing methods does not guarantee that a class _acts_ like the one we are expecting.

There are many examples that could be done leveraging the highly dynamic nature of Python and its rich object model. I would summarize them by saying that sometimes you'd better to check the type of the incoming object.

In Python you can obtain the type of an object using the `type()` built-in function, but to check it you'd better use `isinstance()`, which returns a boolean value. Let us see an example before moving on

``` python
>>> isinstance([], list)
True
>>> isinstance(1, int)
True
>>> class Door(object):
...  pass
... 
>>> d = Door()
>>> isinstance(d, Door)
True
>>> class EnhancedDoor(Door):
...  pass
... 
>>> ed = EnhancedDoor()
>>> isinstance(ed, EnhancedDoor)
True
>>> isinstance(ed, Door)
True
```

As you can see the function can also walk the class hierarchy, so the check is not so trivial like the one you would obtain by directly using `type()`.

The `isinstance()` function, however, does not completely solve the problem. If we write a class that actually _acts_ like a `list` but does not inherit from it, `isinstance()` does not recognize the fact that the two may be considered the same type. The following code returns `False` regardless the content of the `MyList` class

``` python
>>> class MyList(object):
...  pass
... 
>>> ml = MyList()
>>> isinstance(ml, list)
False
```

since `isinstance()` does not check the content of the class.

The problem, thus, may be summed up with the following question: what is the best way to test that an object exposes a given interface? Here, the word _interface_ is used without any reference to other programming solutions, which however address the same problem.

The solution proposed through PEP 3119 is, in my opinion, very simple and elegant, and it perfectly fits the nature of Python, where things are usually agreed rather being enforced. Not only, the solution follows the spirit of polymorphism, where information is provided by the object itself and not extracted by the calling code.

From Python 2.6 `isinstance()` and `issubclass` may be overloaded in the metaclass using `__isinstancecheck__()` and `__subclasscheck__()`. This means that any class (to be more precise the class's metaclass) may define a custom method that is invoked by the `isinstance()` and `issubclass()` built-ins.

before diving into the dailts of this solution let us first discuss the philosophy behind it. The original problem was to find a way to check if an object acts like a given type, so we need a way to store this information. Following the Python polymorphism principles the object is in charge of providing this information, so the basic solution is to give any object an attribute that lists the interfaces the object exposes.

This is the basic idea behind Abstract Base Classes: classes now have a way to tell you what interfaces they expose, or what types they implement.

The implementation of this idea, as already stated, involves two methods, `__isinstancecheck__()` and `__subclasscheck__()`. Methods here are a far better choice than attributes since they allow the programmer to use rich constructs such as conditional structures and may be easily overridden to redefine part of their behaviour.













## Movie Trivia

Section titles of this issue come from the following movies: _The Breakfast Club_,
