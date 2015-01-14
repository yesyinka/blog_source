Title: "Getting attributes in Python"
Date: 2015-01-12 17:00:00 +0100
Category: Programming
Tags: Python, Python3, OOP
Authors: Leonardo Giordani
Slug: getting-attributes-in-python
Summary:

Python is a language that tries to push the Object-oriented paradigm to its maximum. This means that its object model is very powerful compared to that of other languages, but also that the behaviour of Python code may result surprising to new programmers. Moreover, some very powerful Python features (like introspection) may be pushed too far and result in code with unexpected failure points or difficult to maintain/understand.

In this post I want to review some of the method that Python provides to access object attributes, trying to provide a comprensive overview of the matter to everyone starts programming in this beautiful language.

## What are attributes

Since the nomenclature may vary from language to language, in Python we name "attribute" everything is contained inside an object. In Python there is no real distinction between plain data and functions, being both objects, so what I will say about attributes is perfectly valid even for methods.

As a working example, in this post I will use the following class. It represents a book with a title and an author. It provides a `get_entry()` method which returns a string representation of the book.

``` python
class Book(object):
    def __init__(self, title, author):
        self.title = title
        self.author = author

    def get_entry(self):
        return "{0} by {1}".format(self.title, self.author)

```

## Basic attribute access

In Python you may call an attribute of an object using the widely accepted dotted-syntax

``` pycon
>>> b = Book(title="Pawn of Prophecy", author="David Eddings")
>>> b.title
'Pawn of Prophecy'
```

As already mentioned, this mechanism works with methods too.

``` pycon
>>> b.get_entry
<bound method Book.get_entry of <__main__.Book object at 0xb703952c>>
```

For a in-depth explanation of the difference between functions and bound methods read [this post](/blog/2014/08/20/python-3-oop-part-2-classes-and-members).

When an object does not contain the attribute we are looking for, Python raises an `AttributeError` exception

``` pycon
>>> b.publisher
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
AttributeError: 'Book' object has no attribute 'publisher'
```

Remember also that Python objects provide a wide range of automatically created attributes such as `__class__` or `__doc__`. These attributes can be read exactly like standard attributes, since in Python the double underscore prefix and suffix is just a gentleman agreement between programmers.

``` pycon
>>> b.__class__
<class '__main__.Book'>
```

## Softcoding attribute access

When you write a basic attribute access like I did in the previous section you are _hardcoding_ the attribute name. The attribute you are looking for is part of the source code itself, and shall be known at coding time.

What happens if you want to access an attribute whose name is contained in a variable as a string? This usually happens when writing debuggers or inspection tools that let the user interactively specify the attributes they want to see.

To perform this "indirect" access Python provides the `getattr()` builtin function, which accepts an object and the name of an attribute.

``` pycon
>>> getattr(b, 'title')
'Pawn of Prophecy'
>>> getattr(b, 'get_entry')
<bound method Book.get_entry of <__main__.Book object at 0xb703952c>>
```

This type of attribute access may be useful even when accessing a lot of attributes, allowing to write simple for loops or list comprehensions

``` pycon
>>> for attr in ['title', 'author']:
...  getattr(b, attr)
... 
'Pawn of Prophecy'
'David Eddings'
```

As always when leveraging the Python magic tricks pay attention that such shortcuts generally make the code more difficult to debug.

## Avoiding to fail

When you try to access an attribute that does not exist, either with the dotted syntax or with `getattr()`, Python gives you a last chance before raising the `AttributeError` exception. It calls the `__getattr__()` special method passing it the name of the attribute: in the previous example, when accessing `b.publisher`, Python calls `b.__getattr__('publisher')`. 

Since the `Book` class or its ancestors do not define the `__getattr__()` method the attribute access fails and Python raises the exception.

Let us try to define the method, just to show that it actually works

``` python
class Book(object):
    def __init__(self, title, author):
        self.title = title
        self.author = author

    def get_entry(self):
        return "{0} by {1}".format(self.title, self.author)

    def __getattr__(self, attr):
        return None
```

_WARNING_: this is just an example to show how `__getattr__()` works. The code presented here shall not be considered a good example of Python programming.

``` pycon
>>> b = Book(title="Pawn of Prophecy", author="David Eddings")
>>> b.title
'Pawn of Prophecy'
>>> b.publisher
>>> b.somename
```

As you can see the `publisher` and `somename` attributes are correctly accessed even if they do not actually exist inside the object.

So what is `__getattr__()` good for? A good example may be the explicit delegation mechanism involved in a composition between objects. You may find more details on this topic in [this post](/blog/2014/08/20/python-3-oop-part-3-delegation-composition-and-inheritance). 

## The deepest secret

Every Python object contains a very special method called `__getattribute__()` which should never be called explicitly or overridden. This method implements the attribute resolution inside the object, provides the attribute lookup through the inheritance hierarchy, calls `__getattr__()` and if needed raises the `AttributeError` exception.

Due to the very complex nature of this method, and the uttermost importance of its role in making Python objects run, you shall never, never try to override it. If you find yourself in a situation where dealing explicitly with `__getattribute__()` is needed, you may be pretty sure that you did something wrong.


- dotted
- properties
- getattr
- __getattr__
- __getattribute__