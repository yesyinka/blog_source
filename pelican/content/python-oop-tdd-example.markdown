Title: A simple example of Python OOP development (with TDD)
Date: 2015-05-08 16:00:00 +0100
Category: Programming
Tags: OOP, Python, Python3, TDD
Authors: Leonardo Giordani
Slug: python-oop-tdd-example
Summary: A simple implementation of binary numbers in Python to exemplify OOP and TDD

If you are eager to learn some Python and do not know how to start, this post may give you some hints. I will develop a very simple Python package from scratch, exemplifying some Object-oriented Programming (OOP) techniques and concepts, and using a Test-Driven Development (TDD) approach.

The package will provide some classes to deal with binary numbers (see the Rationale section), but remember that it is just a toy project. Nothing in this package has been designed with performance in mind: it wants to be as clearer as possible.

## Rationale

Binary numbers are rather easy to understand, even if becoming familiar with them requires some time. I expect you to have knowledge of the binary numeral system. If you need to review them just take a look at the [Wikipedia entry](http://en.wikipedia.org/wiki/Binary_number) or one of the countless resources on Internet.

The package we are going to write will provide a class that represents binary numbers (`Binary`) and a class that represents binary numbers with a given bit size (`SizeBinary`). They shall provide basic binary operations like logical (and, or, xor), arithmetic (addition, subtraction, multiplication, division), shifts and indexing.

A quick example of what the package can do:

``` pycon
>>> b = Binary('0101110001')
>>> hex(b)
'0x171'
>>> int(b)
369
>>> b[0]
'1'
>>> b[9]
'0'
>>> b.SHR()
'010111000'
```

## Python and bases

Binary system is just a _representation_ of numbers with base 2, just like hexadecimal (base 16) and decimal (base 10). Python can already natively deal with different bases, even if internally numbers are always stored as decimal integers. Let us check it

``` pycon
>>> a = 5
>>> a
5
>>> a = 0x5
>>> a
5
>>> a = 0b101
>>> a
5
>>> hex(0b101)
'0x5'
>>> bin(5)
'0b101'
```

As you can see Python understands some common bases out of the box, using the `0x` prefix for hexadecimal numbers and the `0b` for binary ones (and `0o`) for octals). However the number is always printed in its base-10 form (`5` in this case). This means however that a binary number cannot be indexed, since integers does not provide support for this operation

``` pycon
>>> 0b101[0]
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: 'int' object is not subscriptable
```

You can also use a different base when converting things to integers, through the `base` attribute

``` pycon
>>> a = int('101', base=2)
>>> a
5
>>> a = int('10', base=5)
>>> a
5
```

## Test-driven development

Simple tasks are the best way to try and use new development methodologies, so this is a good occasion to start working with the so-called test-driven approach. Test-driven Development (TDD) basically means that the first thing you do when developing is to write some _tests_, that is programs that use what you are going to develop. The purpose of those programs is to test that your final product complies with a given behaviour so they provide 

* **Documentation** for your API: they are examples of use of your package.
* **Regression** checks: when you change the code to develop new features the package shall not break the behaviour of the previous versions.
* **TODO list**: until all tests run successfully you have something still waiting to be implemented.

## Development environment

Let us set up a simple environment for the upcoming development. First of all create a Python virtual environment

``` bash
~$ virtualenv -p python3 venv
```

then activate the virtualenv and install py.test, which is what we will use to write tests.

``` bash
~$ source venv/bin/activate
(venv)~$ pip install pytest
```

Then create a directory for the package, the `__init__.py` file and a directory for tests

``` bash
(venv)~$ mkdir binary
(venv)~$ cd binary
(venv)~/binary$ touch __init__.py
(venv)~/binary$ mkdir tests
```

Finally, let us check that is correctly working. Py.test does not find any test so it should exit without errors.

``` bash
(venv)~/binary$ py.test
===================== test session starts =====================
platform linux -- Python 3.4.3 -- py-1.4.27 -- pytest-2.7.0
rootdir: /home/leo/Devel/binary, inifile: 
collected 0 items 

=======================  in 0.00 seconds ======================
```

## Writing some tests


