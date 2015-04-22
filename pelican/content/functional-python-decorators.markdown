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

But what actually ARE the changes that you may want do to functions or classes? Let us stick for the moment to a very simple task: adding attributes. As you will see later, this is by no means a meaningless task, since there are many practical use cases that make use of it. Let us test how we can add attributes to functions in plain Python

``` pycon
>>> def func():
...  pass
... 
>>> func.attr = "a custom function attribute"
>>> func.attr
'a custom function attribute'
```

and to classes

``` pycon
>>> class SomeClass:
...  pass
... 
>>> SomeClass.attr = "a custom class attribute"
>>> SomeClass.attr
'a custom class attribute'
>>> s = SomeClass()
>>> s.attr
'a custom class attribute'
```

As you can see adding attributes to a class correctly results in a class attribute, which is thus shared by any instance of that class.

## Class-based decorators without arguments

As already explained, Python allows you to call any object (as you do with functions) as long as it provides the `__call__()` method. So to write a class-based decorator you just need to create an object that defines such a method.

When used as a decorator, a class is instantiated at decoration time, that is when the function is defined, and called when the function is called.

``` python
class CustomAttr:
    def __init__(self, obj):
        self.attr = "a custom function attribute"
        self.obj = obj

    def __call__(self):
        self.obj()
```

As you can see there is already a lot of things that shall be clarified. First of all the class, when used as a decorator, is initialized with the object that is going to be decorated, here called `obj` (most of the time it is just called `f` for function, but that is only a special case).

While the `__init__()` method is called at decoration time, the `__call__()` method of the decorator is called instead of the same method of the decorated object. In this case (decorator without arguments), the `__call__()` method of the decorator does not receive any argument. In this example we just "redirect" the call to the original function, which was stored during the initialization step.

So you see that in this case we have two different moments in which we may alter the behaviour of the decorated objects. The first is at its definition and the second is when it is actually called.

The decorator can be applied with the simple syntax shown in a previous section

``` python
@CustomAttr
def func():
    pass
```

When Python parses the file and defines the function `func` the code it executes under the hood is

``` python
def func():
    pass

func = CustomAttr(func)
```

according to the definition of decorator, and this is why the class shall accept the decorated object as a parameter in its `__init__()` method.

Note that in this case the `func` object we obtain after the decoration is no more a function but a `CustomAttr` object

``` pycon
>>> func
<__main__.CustomAttr object at 0xb6f5ea8c>
```

and this is why in the `__init__()` method I attached the `attr` attribute to the class instance `self` and not to `obj`, so that now this works

``` pycon
>>> func.attr
'a custom function attribute'
```

This replacement is also the reason why you shall also redefine `__call__()`. When you write `func()` you are not executing the function but calling the instance of `CustomAttr`.

## Class-based decorators with arguments

This case is the most natural step beyond the previous one. Once you started metaprogramming, you want to do it with style, and the first thing to do is to add parametrization. Adding parameters to decorators has the only purpose of generalizing the metaprogramming code, just like you write parametrized functions instead of hardcoding values.

There is a big caveat here. Class-based decorators with arguments behave in a slightly different way than their counterpart without arguments. Specifically, the `__call__()` method is run during the decoration and not during the call.

Let us first review the syntax

``` python
class CustomAttrArg:
    def __init__(self, value):
        self.value = value

    def __call__(self, obj):
        obj.attr = "a custom function attribute with value {}".format(self.value)
        return obj

@CustomAttrArg(1)
def func():
    pass
```

Now the `__init__()` method shall accept some arguments, with the standard rules of Python functions for named and default arguments. The `__call__()` method receives the decorated object, which in the previous case was passed to the `__init__()` method.

The biggest change, however is that `__call__()` is not run when you call the decorated object, but immediately after `__init__()` during the decoration phase. This results in the following difference: while in the previous case the decorated object was no more itself, but an instance of the decorator, now the decorated objects becomes the return value of the `__call__()` method.

But remember that, when you call the decorated object, now you are calling what you get from `__call__()` so be sure of returning something meaningful.

In the above example I stored one single argument in `__init__()`, and this argument is passed to the decorator when applying it to the function. Then, in `__call__`, I set the attribute of the decorated object, using the stored argument, and return the object itself. This is important, since I have to return a callable object.

This means that, if you have to do something complex with the decorated object, you may just define a local function that makes use of it and return this function. Let us see a very simple example

``` python
class CustomAttrArg:
    def __init__(self, value):
        self.value = value

    def __call__(self, obj):
        def wrap():
            # Here you can do complex stuff
            obj()
            # Here you can do complex stuff
        return wrap

@CustomAttrArg(1)
def func():
    pass
```

Here the returned object is no more the decorated one, but a new function `wrap()` defined locally. It is interesting to show how Python identifies it

``` pycon
>>> @CustomAttrArg(1)
... def func():
...     pass
... 
>>> func
<function CustomAttrArg.__call__.<locals>.wrap at 0xb70185cc>
```

This pattern enables you to do every sort of things with the decorated object. Not only to change it (adding attributes, for example), but also pre- or post- filtering its results. You may start to understand that what we called metaprogramming may be very useful for everyday tasks, and not only for some obscure wizardry.

## Decorators and prototypes

If you write a class-based decorator with arguments, you are in charge of returning a callable object of choice. There is no assumption on the returned object, even if the usual case is that the returned object has the same prototype as the decorated one.

This means that if the decorated object accepts zero arguments (like in my example), you usually return a callable that accepts zero arguments. This is however by no means enforced by the language, 
