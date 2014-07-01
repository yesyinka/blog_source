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

## The Inspection Club

As you know, Python leverages polymorphism at its maximum by dealing only with generic references to objects. This makes OOP not an addition to the language but part of its structure from the ground up. Moreover, Python pushes the EAFP appoach, which tries to avoid direct inspection of objects as much as possible.

It is however very interesting to read what Guido van Rossum says in [PEP 3119](http://legacy.python.org/dev/peps/pep-3119/): _Invocation means interacting with an object by invoking its methods. Usually this is combined with polymorphism, so that invoking a given method may run different code depending on the type of an object. Inspection means the ability for external code (outside of the object's methods) to examine the type or properties of that object, and make decisions on how to treat that object based on that information. [...] In classical OOP theory, invocation is the preferred usage pattern, and inspection is actively discouraged, being considered a relic of an earlier, procedural programming style. However, in practice this view is simply too dogmatic and inflexible, and leads to a kind of design rigidity that is very much at odds with the dynamic nature of a language like Python._

The author of Python recognizes that forcing the use of a pure polymorphic approach leads sometimes to solutions that are too complex or even incorrect. In this section I want to show some of the problems that can arise from a pure polymorphic approach and introduce Abstract Base Classes, which aim to solve them. I strongly suggest to read [PEP 3119](http://legacy.python.org/dev/peps/pep-3119/) (as for any other PEP) since it contains a deeper and better explanation of the whole matter. Indeed I think that this PEP is so well written that any further explanation is hardly needed. I am however used to write explanations to check how much I understood about the topic, so I am going to try it this time too.

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

since `isinstance()` does not check the content of the class or its behaviour, it just consider the class and its ancestors.

The problem, thus, may be summed up with the following question: what is the best way to test that an object exposes a given interface? Here, the word _interface_ is used for its natural meaning, without any reference to other programming solutions, which however address the same problem.

A better way to address the problem could be to write inside an attribute of the object the list of interfaces it promises to implement, and to agree that any time we want to test the behaviour of an object we simply have to check the content of this attribute. This is exactly the path followed by Python, and it is very important to understand that the whole system is just about a promised behaviour.

The solution proposed through PEP 3119 is, in my opinion, very simple and elegant, and it perfectly fits the nature of Python, where things are usually agreed rather than being enforced. Not only, the solution follows the spirit of polymorphism, where information is provided by the object itself and not extracted by the calling code.

In the next sections I am going to try and describe this solution in its main building blocks. The matter is complex so my explanation will lack some details (or even be slightly incorrect). Please refer to the forementioned PEP 3119 for a complete description.

#### The metaclasses way

As already described, Python provides two built-ins to inspect objects and classes, which are `isinstance()` and `issubclass()` and it would be desirable that a solution to the inspection problem allows the programmer to go on with using those two functions.

This means that we need to find a way to inject the "behaviour promise" into both classes and instances. This is the reason why metaclasses come in play. If you recall what we said about them in the second issue of this series, metaclasses are the classes used to build classes, which means that they are the preferred way to change the structure of a class, and, in consequence, of its instances.

Another way to do the same job would be to leverage the inheritance mechanism, injecting the behaviour through a dedicated parent class. This solution has many downsides, which I'm am not going to detail. It is enough to say that affecting the class hierarchy may lead to complex situations or subtle bugs. Metaclasses may provide here a different entry point for the introduction of a "virtual base class" (as PEP 3119 specifies, this is not the same concept as in C++).

#### Overriding methods

As said, `isinstance()` and `issubclass()` are buil-in functions, not object methods, so we cannot simply override them providing a different implementation in a given class. So the first part of the solution is to change the behaviour of those two functions to first check if the class or the instance contains a special method, which is `__instancecheck__()` for `isinstance()` and `__subclasscheck__()` for `issubclass()`. So both built-ins try to run the respective special method, reverting to the standard algorithm if it is not present.

A note about naming. Methods may accept the object they belong to as the first argument, so the two special methods shall have the form

``` python
def __instancecheck__(cls, inst):
   [...]
   
def __subclasscheck__(cls, sub):
   [...]
```

where `cls` is the class where they are injected, that is the one representing the promised behaviour. The two built-ins, however, have a reversed argument order, where the behaviour comes after the tested object: when you write `isinstance([], list)` you want to check if the `[]` instance has the `list` behaviour. This is the reason behind the name choice: just calling the methods `__isinstance__()` and `__issubclass__()` and passing arguments in a reversed order would have been confusing.

#### The ABC Support Framework

The proposed solution is thus called Abstract Base Classes, as it provides a way to attach to a concrete class a virtual class with the only purpose of signaling a promised behaviour to anyone inspecting it with `isinstance()` or `issubclass()`.

To help programmers implement Abstract Base Classes, the standard library has been given an `abc` module, thet contains the `ABCMeta` class (and other facilities). This class is the one that implements `__instancecheck__()` and `__subclasscheck__()` and shall be used as a metaclass to augment a standard class. This latter will then be able to register other classes as implementation of its behaviour.

Sounds complex? An example may clarify the whole matter. The one from the official documentation is rather simple:

``` python
from abc import ABCMeta

class MyABC:
    __metaclass__ = ABCMeta

MyABC.register(tuple)

assert issubclass(tuple, MyABC)
assert isinstance((), MyABC)
```

Here, the `MyABC` class is provided the `ABCMeta` metaclass. This puts the two `__isinstancecheck__()` and `__subclasscheck__()` methods inside `MyABC` so that you can use them like

``` python
d = {'a': 1}

MyABC.__isinstancecheck__(d)
```

that returns `True` if the dictionary `d` is an instance of the Abstract Base Class `MyABC`. In other words if the dictionary `d` implements the behaviour promised by the `MyABC` class.

After the definition of `MyABC` we need a way to signal that a given class is an instance of the Abstract Base Class and this happens through the `register()` method, provided by the `ABCMeta` metaclass. Calling `MyABC.register(tuple)` we record inside `MyABC` the fact that the `tuple` class shall be identified as a subclass of `MyABC` itself. This is analogous to saying that `tuple` inherits from `MyABC` but not wuite the same. As already said registering a class in an Abstract Base Class with `register()` does not affect the class hierarchy.

## Collections

In addition to the `abc` module, the standard library now provides a `collections` module that, besides some interesting container datatypes like `namedtuple` and `OrderedDict`, supplies a remarkable number of ABCs that represent container behaviours. An example is `collections.Sized` that pledges that the registered class will contain the `__len__()` method, enabling the code to pass it to the `len()` builtin. Let us exemplify that:

``` python
>>> class Snake(object):
...   def __init__(self, meters):
...     self.len = meters
...   def __len__(self):
...     return self.len
... 
>>> 
>>> s = Snake(5)
>>> len(s)
5
>>> 
>>> import collections
>>> collections.Sized.register(Snake)
>>> 
>>> issubclass(Snake, collections.Sized)
True
```

If not stressed enough, ABCs assure that a given behaviour will be implemented but there is no actual check of this. For example:

``` python
>>> class FakeSnake(object):
...   def __init__(self, meters):
...     pass
... 
>>> 
>>> collections.Sized.register(FakeSnake)
>>> issubclass(FakeSnake, collections.Sized)
True
>>> f = FakeSnake(6)
>>> len(f)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: object of type 'FakeSnake' has no len()
```

Remember that ABCs are classes just like any other standard Python class, which means that they can be inherited.

TODO: Abstract methods?????????????????????? Decorator and friends...



## Slots

## classattribute

## method-wrapper vs method, slot wrapper vs slot, dict_proxy

## Callable objects

In the first post, when I discussed for the first time the concept of instantiation, I stated that an instance of a class may be obtained by calling the class itself just like a function. Let me recall you the example

``` python
>>> b = int()
>>> type(b)
<type 'int'>
```

Later, speaking about methods and attributes, I defined _callable_ something that can be called with that syntax. Thus, we can now say that classes are callables, just like methods. What makes an object callable, anyway? It turns out that, as many other things in Python, the solution is pretty straightforward: an object is callable if it contains the `__call__()` method. Simple, isn't it?

When we execute a syntax like

``` python
>>> some_object()
```

Python executes under the hood the following code

``` python
>>> some_object.__call__()
```

where any parameter passed to the class is obviously passed to `__call__()`.

What `__call__` does is to run the constructor mechanism as depicted in the second post, executing `__new__()` and `__init()__` to get a new instance and initialize it. The `__call__()` method is usually defined by `type`, that is the metaclass of the class under exam.

The above definition of callable object is very powerful, since it allows to flatten the difference between class and function. In OOP many times the two are presented as two completely separated concepts, but in Python it is usually more convenient to talk about callables. Here, Python shows its polymorphic nature at its maximum: if I expect a function and what is given to me is something that acts like a function everything is fine.

Since `__call__` is a method we can redefine it in any class, let us try and see what happens

``` python
class CallMe(object):
    def __call__(self, *args, **kwds):
        return 1
```

This is something new, in that we defined a method with the same name is the method which is contained in the metaclass. 

************************************

That means that we may redefine `__call__`  in a class

in a class to return any value?
No, it does not work.

>>> class Pippo(object):
...  def __call__(cls, *args, **kwds):
...   return 1
... 
>>> p = Pippo()
>>> p
<__main__.Pippo object at 0xb700030c>
>>> Pippo.__call__
<unbound method Pippo.__call__>
>>> Pippo()
<__main__.Pippo object at 0xb700038c>
>>> 
>>> 

Do not return something different, anyway, it is bad.

For example, the `sort()` method of a list wants a callable. Can we pass a class? An instance?

## function 

>>> type(prova)
<type 'function'>
>>> dir(function)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
NameError: name 'function' is not defined
>>> dir(prova)   
['__call__', '__class__', '__closure__', '__code__', '__defaults__', '__delattr__', '__dict__', '__doc__', '__format__', '__get__', '__getattribute__', '__globals__', '__hash__', '__init__', '__module__', '__name__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', 'func_closure', 'func_code', 'func_defaults', 'func_dict', 'func_doc', 'func_globals', 'func_name']
>>> prova.__dict__
{}
>>> function
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
NameError: name 'function' is not defined
>>> prova.__class__
<type 'function'>
>>> prova.__class__.__dict__
dict_proxy({'func_closure': <member 'func_closure' of 'function' objects>, '__module__': <member '__module__' of 'function' objects>, '__getattribute__': <slot wrapper '__getattribute__' of 'function' objects>, '__dict__': <attribute '__dict__' of 'function' objects>, '__code__': <attribute '__code__' of 'function' objects>, 'func_code': <attribute 'func_code' of 'function' objects>, '__setattr__': <slot wrapper '__setattr__' of 'function' objects>, '__new__': <built-in method __new__ of type object at 0x83362c0>, '__closure__': <member '__closure__' of 'function' objects>, '__call__': <slot wrapper '__call__' of 'function' objects>, '__get__': <slot wrapper '__get__' of 'function' objects>, '__doc__': <member '__doc__' of 'function' objects>, 'func_dict': <attribute 'func_dict' of 'function' objects>, 'func_name': <attribute 'func_name' of 'function' objects>, '__name__': <attribute '__name__' of 'function' objects>, 'func_globals': <member 'func_globals' of 'function' objects>, '__defaults__': <attribute '__defaults__' of 'function' objects>, '__globals__': <member '__globals__' of 'function' objects>, '__delattr__': <slot wrapper '__delattr__' of 'function' objects>, 'func_defaults': <attribute 'func_defaults' of 'function' objects>, '__repr__': <slot wrapper '__repr__' of 'function' objects>, 'func_doc': <member 'func_doc' of 'function' objects>})

## dir(afunction) __closure__

## Method and slot wrapper
Door.__call__
type.__call___

## first class objects

## Previous articles

* [OOP Concepts in Python 2.x - Part 1](/blog/2014/03/05/oop-concepts-in-python-2-dot-x-part-1)
* [OOP Concepts in Python 2.x - Part 2](/blog/2014/03/10/oop-concepts-in-python-2-dot-x-part-2)










## Movie Trivia

Section titles of this issue come from the following movies: _The Breakfast Club_,
