---
layout: post
title: "Digging up Django class-based views - 2"
date: 2013-11-10 15:53
comments: true
categories: [python, django]
---

## Abstract

_This post refers to Django 1.5. Please be warned that some of the matters discussed here, some solutions or the given code can be outdated by more recent Django versions_

In the first installment of this short series I introduced the theory behind Django class-based views and the reason why in this context classes are more powerful than pure functions. I also introduced one of the generic views Django provides out-of-the-box, which is `ListView`.
In this second post I want to talk about the second most used generic view, `DetailView`, and about custom querysets and arguments. Last, I'm going to introduce unspecialized generic views that allow you to build more complex Web pages. To fully understand `DetailView`, however, you need to grasp two essential concepts, namely **querysets** and **view parameters**. So I'm sorry for the learn-by-doing readers but this time too I'm going to start with some pure programming topics.

<!--more-->

## QuerySets or the art of extracting information

One of the most important parts of Django is the ORM (Object Relational Mapper), which allows you to access the underlying database just like a collection of Python objects. As you know, Django provides you tools to simplify the construction of DB queries; they are **managers** (the `.objects` attribute of any models, for example) and **query methods** (`get()`, `filter()`, and friends). Pay attention because things here are slightly more magical than you can think at a first glance.

When you use one of the methods of a manager you get as a result a `QuerySet`, which most of the time is used as a list, but is more than this. You can find [here](https://docs.djangoproject.com/en/1.5/topics/db/queries/) and [here](https://docs.djangoproject.com/en/1.5/ref/models/querysets/) the official documentation about queries and QuerySets, a very recommended reading.

What I want to stress here is that QuesySets are not evaluated until you perform an action that access the content like slicing or iterating on it. This means that we can build QuerySets, pass them to functions, store them, and even build them programmatically or metaprogramming them without the DB being hitted. If you think at QuerySets as recipes you are not far from the truth: they are objects that store how you want to retrieve the data of your interest. Actually retriving them is another part of the game. This separation between the definition of something and its execution is called **lazy evaluation**.

Let me give you a very trivial example to show why the lazy evaluation of QuerySets is important.

``` python
[...]

def get_oldest_three(queryset):
	return queryset.order_by['id'][0:2]

old_books = get_oldest_three(Book.objects.all())
old_hardcover_books = \
    get_oldest_three(Book.objects.filter('type=Book.HARDCOVER'))
```

As you can see the `get_oldest_three()` method is just filtering an incoming QuerySet (which can be of any type); it simply orders the objects and gets the first three inserted in the DB. The important thing here is that we are using QuerySets like pure 'algorithms', or descriptions of a procedure. When creating the `old_books` variable we are just telling the `get_oldest_three()` method "Hey, this is the way I exctract the data I'm interested in. May you please refine it and return the actual data?"

Being such flexible objects, QuerySets are an important part of generic views, so keep them warm for the upcoming banquet.

## Being easygoing: parametric views

URLs are the API of our Web site or service. This can be more or less evident for the user that browses through the pages, but from the programmer's point of view URLs are the entry points of a computer system. As such, they are not very different from the API of a library: here, static pages are just like constants, or functions that always return that same value (such as a configuration parameter), while dynamic pages are like functions that process incoming data (parameters) and return a result.

So Web URLs can accept parameters, and our underlying view shall do the same. You basically have two methods to convey parameters from the user's browser to your server using HTTP. The first method is named [query string](http://en.wikipedia.org/wiki/Query_string) and lists parameters directly in the URL through a universal syntax. The second method is storing parameters in the HTTP request body, which is what POST requests do. We will dicuss this method in a later post about forms.

The first method has one big drawback: most of the time URLs are long (and sometimes *too* long), and difficult to use as a real API. To soften this effect the concept of [clean URL](http://en.wikipedia.org/wiki/Clean_URL) arose, and this is the way Django follows natively (though, if you want, you can also stick to the query string method).

Now, [you know](https://docs.djangoproject.com/en/1.5/topics/http/urls/) that you can collect parameters contained in the URL parsing it with a regular expression; what we need to discover is how class-based views receive and process them.

In the previous post we already discussed the `as_view()` method that instances the class and returns the result of `dispatch()` ([views/generic/base.py#L46](https://github.com/django/django/blob/stable/1.5.x/django/views/generic/base.py#L46)).

``` python
    @classonlymethod
    def as_view(cls, **initkwargs):
        """
        Main entry point for a request-response process.
        """
        # sanitize keyword arguments
        for key in initkwargs:
            if key in cls.http_method_names:
                raise TypeError("You tried to pass in the %s method name as a "
                                "keyword argument to %s(). Don't do that."
                                % (key, cls.__name__))
            if not hasattr(cls, key):
                raise TypeError("%s() received an invalid keyword %r. as_view "
                                "only accepts arguments that are already "
                                "attributes of the class." % (cls.__name__, key))

        def view(request, *args, **kwargs):
            self = cls(**initkwargs)
            if hasattr(self, 'get') and not hasattr(self, 'head'):
                self.head = self.get
            self.request = request
            self.args = args
            self.kwargs = kwargs
            return self.dispatch(request, *args, **kwargs)

        # take name and docstring from class
        update_wrapper(view, cls, updated=())

        # and possible attributes set by decorators
        # like csrf_exempt from dispatch
        update_wrapper(view, cls.dispatch, assigned=())
        return view
```

Now look at what the `view()` wrapper function actually does with the instanced class ([views/generic/base.py#L65](https://github.com/django/django/blob/stable/1.5.x/django/views/generic/base.py#L65)); not surprisingly it takes the `request`, `args` and `kwargs` passed by the URLconf passes and converts them into as many class attributes with the same names. This means that *everywhere in our CBVs* we can access the original call parameters simply reading `self.request`, `self.args` and `self.kwargs`.

## Details

Just after listing things, one of the most useful things a Web site does is giving details about objects. Obviously any e-commerce site is made for the most part by pages that list products and show product details, but also a blog is made of one or more pages with a list of posts and a page for each of them. So building a detail view of the content of our database is worth learning.

To help us in this task Django provides `DetailView`, which indeed deals, as the name suggests, with the details of what we get from the DB. While `ListView`'s basic behaviour is to extract the list of all objects with a given model, `DetailView` extracts a single object. How does it know what object shall be extracted?

When `dispatch()` is called on an incoming HTTP request the only thing it does is to look at the `method` attribute, which for `HttpRequest` objects contains the name of the HTTP verb used (e.g. `'GET'`); then `dispatch()` looks for a method of the class with the lowercase name of the verb (e.g. `'GET'` --> `get()`) ([views/generic/base.py#L78](https://github.com/django/django/blob/stable/1.5.x/django/views/generic/base.py#L78)). This handler is then called with the same parameters of `dispatch()`, namely the `request` itself, `*args` and `**kwargs`.

`DetailView` has no body and inherits everything from two objects, namely `SingleObjectTemplateResponseMixin` and `BaseDetailView`; this latter implements the `get()` method ([generic/detail.py#L107](https://github.com/django/django/blob/stable/1.5.x/django/views/generic/detail.py#L107)).

``` python
def get(self, request, *args, **kwargs):
    self.object = self.get_object()
    context = self.get_context_data(object=self.object)
    return self.render_to_response(context)
```

As you can see this method extracts the single object it shall represent calling `self.get_object()`, then calls `self.get_context_data()` (that we met in the previous post) and last the familiar `self.render_to_response()` that is the class equivalent of the well know Django function. The method `self.get_object()` is provided by `SingleObjectMixin` ([generic/detail.py#L10](https://github.com/django/django/blob/stable/1.5.x/django/views/generic/detail.py#L10)): the most important parts of its code, for the sake of our present topic are

``` python
def get_object(self, queryset=None):
    if queryset is None:
        queryset = self.get_queryset()

    pk = self.kwargs.get(self.pk_url_kwarg, None)
    if pk is not None:
        queryset = queryset.filter(pk=pk)
    
    obj = queryset.get()
    
    return obj
```

**Warning**: I removed many lines from the previous function to improve readability; please check the original source code for the complete implementation.

The code shows where `DetailView` gets the queryset from; the `get_queryset()` method is provided by `SingleObjectMixin` itself and basically returns `self.queryset` if present, otherwise returns all objects of the given model (acting just like `ListView` does). This `queryset` is then refined by a `filter()` and last by a `get()`. Here `get()` is not used directly (I think) to manage the different error cases and raise the correct exceptions.

The parameter `pk` used in `filter()` comes directly from `self.kwargs`, so it is taken directly from the URL. Since this is a core concept of views in general I want to look at this part carefully.

Our `DetailView` is called by an URLconf that provides a regular expression to parse the URL, for example `url(r'^(?P<pk>\d+)/$',`. This regex extracts a parameter and gives it the name `pk`, so `kwargs` of the view will contain `pk` as key and the actual number in the URL as value. For example the URL `123/` will result in `{'pk': 123}`. The default behaviour of `DetailView` is to look for a `pk` key and use it to perform the filtering of the queryset, since `self.pk_url_kwarg` is `'pk'`.

So if we want to change the name of the parameter we can simply define the `pk_url_kwarg` of our class and provide a regex that extract the primary key with the new name. For example `url(r'^(?P<key>\d+)/$',` extracts it with the name `key`, so we will define `pk_url_kwarg = 'key'` in our class.

So from this quick exploration we learned that `DetailView`:

* provides a context whit the `object` key initialized to a single object
* **must** be configured with a `model` class attribute, to know what objects to exctract
* **can** be configured with a `queryset` class attribute, to refine the set of objects where the single object is extracted from
* **must** be called from a URL that includes a regexp that extracts the primary key of the searched object as `pk`
* **can** be configured to use a different name for the primary key through the `pk_url_kwarg` class attribute

The basic use of `DetailView` is thus exemplified by the following code.

``` python
class BookDetail(DetailView):
    model = Book
    
urlpatterns = patterns('',
    url(r'^(?P<pk>\d+)/$',
        BookDetail.as_view(),
        name='detail'),
    )
```

The view extracts a single object with the `Book` model; the regex is configured with the standard `pk` name.

As shown for 
