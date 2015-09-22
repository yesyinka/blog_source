Title: Clojure sequential data types for Python programmers
Date: 2015-09-22 19:00:00 +0100
Category: Programming/Projects
Tags: functional programming, Python, Python2, Python3, Clojure
Authors: Leonardo Giordani
Slug: clojure-sequential-data-types-for-python-programmers
Summary: 

I have been working with Python for more than fifteen years and I developed very big systems with this language, learning to know and love both its power and its weaknesses, admiring its gorgeous object-oriented system and exploring some of its darkest corners.

Despite being so commited to this language (both for personal choices and for business reasons) I never stopped learning new languages, trying to get a clear picture of new (or old) paradigms, to explore new solutions to old problems and in general to get my mind open.

Unfortunately introducing a new language in your business is always very risky, expensive and many times not a decision of yours only. You are writing software that other may have to maintain, so either you evangelize your team and persuade other to join you or you will be forced to stay with the languages shared by them. This is not bad per se, but sometimes is a big obstacle for the adoption of a new language.

One of the languages I fell in love (but never used in production) is LISP. I strongly believe that everyone calls theirselves a programmer shall be somehow familiar with LISP, as well as with Assembly language. The reasons are many and probably do not fit well with the title of this article so I will save them for another, more theoretical post, and move on.

Clojure is a LISP implementation that runs on the JVM, thus being available on a wide number of platforms. As any other LISP its syntax is a step lower in the abstraction tree: while other languages provide their syntax and then a conversion to an AST (Abstract Syntax Tree), LISPs provide only their AST syntax. This has the great advantage of homoiconicity, something that they share with Assembly, but comes at the cost of a slightly more difficult syntax, at least for the novice.

In this post I want to review the sequential data types provided by Clojure, trying to introduce them to programmers accustomed to Python datatypes. Anyway, while the nomenclature will be specific to Python, I think the discussion can easily extended to other dynamic languages such as Ruby.

