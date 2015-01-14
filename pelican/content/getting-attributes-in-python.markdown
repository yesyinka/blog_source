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

Since the nomenclature may vary from language to language, in Python we name "attribute" everithing is contained inside an object. In Python there is no real distinction between plain data and functions, being both objects, so what I will say about attributes is perfectly valid even for methods.

## Basic attribute access

In Python you may call an attribute of an object using the widespread dotted-syntax

``` python
class Book(object):
    def __init__(self, title, author):
        self.title = title
        self.author = author

    def get_entry(self):
        return "{0} by {1}".format(self.title, self.author)


- dotted
- getattr
- __getattr__
- __getattribute__