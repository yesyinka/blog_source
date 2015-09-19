Title: Python 3 generators and coroutines
Date: 2015-09-19 10:00:00 +0100
Category: Programming
Tags: concurrent programming, functional programming, generators, OOP, Python, Python2, Python3
Authors: Leonardo Giordani
Slug: python-3-generators-and-coroutines
Summary: An updated edition of the series of posts on Python generators

## Introduction

Two years have passed since I wrote the series of three posts "Python generators - from iterators to cooperative multitasking". Now Python 3 is the official Python version in use and Python 2 is on the run. No, just joking, Python 2 is to be maintained with bugfixes until 2020, but new features will be added to Python 3 only, so I thought it was time to update the information provided in that series.

Since my purpose is to provide updated, consistent and clear information I don't like to copy and paste my essays, refurbishing them with a new title and call them a new post. The old series contains valuable information, and I believe it is still worth reading. Some properties of generators are however missing, most notably the `send()` method of generators and the new `yield from` syntax.

Thus, I will just give a brief review of the generator concept, for the sake of consistency, but I strongly suggest to read the [original series](/blog/2013/03/25/python-generators-from-iterators-to-cooperative-multitasking) of three posts.

