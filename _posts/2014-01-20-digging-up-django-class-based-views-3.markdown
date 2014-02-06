---
layout: post
title: "Digging up Django class-based views - 3"
date: 2014-01-20 16:13:41 +0100
comments: true
categories: [python, django]
---

_This post refers to Django 1.5. Please be warned that some of the matters discussed here, some solutions or the given code can be outdated by more recent Django versions_

In the first two issues of this short series we wandered around the basic concepts of class-based views in Django, and started understanding and using two of the basic generic views Django makes available to you: `ListView` and `DetailView`. Both are views that read some data from the database and show them on a renderized template.

This third issue wants to introduce the reader to the class-based version of Django forms. This post is not meant to be a full introduction to the Django form library; rather, I want to show how class-based generic views implement the CUD part of the CRUD operations (Create, Read, Update, Delete), the Read parte being implemented by generic views, and how the class nature can be leveraged to achieve some common behaviours.

## A very basic example

To start working with CBF (class-based forms) let's suppose we are working with a `StickyNote` class which represents a simple text note with a date.

``` python
class StickyNote(models.Model):
    timestamp = models.DateTimeField()
    text = models.TextField(blank=True, null=True)
```

One of the first things we usually want to do is to build a form that allows the user to create a new entry in the database, in this case a new sticky note. The functional version of such a form would be the following:

```python
def note_add(request):
    form = StickyNoteForm(request.POST or None)
    if form.is_valid():
        new_note = form.save()
        new_note.save()
    return render_to_response('note_add.html')
    
urlpatterns = patterns('',
    url(r'^note_add/$', 'note_add'),
)
```

which is not too complex to grasp. Note that I left aside some imports for clarity's sake; the `StickNoteForm` class is built using a [model form](https://docs.djangoproject.com/en/1.5/topics/forms/modelforms/). Since you [already know](https://docs.djangoproject.com/en/1.5/topics/forms/) how functional form views work, let's compare it with the same view expressed with a class:

``` python
class NoteAdd(CreateView):
    model = StickyNote
```

It is no surprise that most of the code went away, thanks to inheritance. As happened in the first two posts with standard views, the class mechanism provides us with a bunch of code that lives somewhere in the class hierarchy and works behind the scenes. Our mission is now to uncover that code to figure out how exactly CBFs work and how we can change them to perform what we need.

To make the post easier to follow please always remember the aim of a CBF: as for the more general concept of class-based view, a class based form's job is to process incoming HTTP requests and return a HTTP response. Form views do this in a slightly different way than the standard ones, mostly due to the different nature of POST requests compared with GET ones. Let us take a look at this concept before moving on.

## HTTP requests: GET and POST

_Please note that this is a broad subject and that the present section wants only to be a very quick review of the main points related to Django CBFs_

HTTP requests come in different forms, depending on the **method** they carry. Those methods are called **HTTP verb**s and the two most used ones are **GET** and **POST**. The GET method tells the server that the client wants to retrieve a resource (the one connected with the relative URL) and shall have no side effects (such as changing the resource). The POST method is used to send some data to the server, the given URL being the _resource_ that shall handle the data.

As you can see, the definition of POST is very broad: the server accepts the incoming data and is allowed to perform any type of action with it, such as creating a new entity, editing or deleting one or more of them, and so on.

Keep in mind that forms are not the same thing as POST request. As a matter of fact they are connected just incidentally: a form is a way to collect data from a user browsing an HTML page, while POST is the way that data is transmitted to the server. You do not need to have a form to make a POST request, you just need some data to send. HTML forms are just a useful way to send POST requests, but not the only one.

## Form views

Why are form views different from standard views? The answer can be found looking at the flow of a typical data submission on a Web site:

1. The user browses a web page (GET)
2. The server answers the GET request with a page containing a form
3. The user fills the form and submits it (POST)
4. The server receives the data and precesses it

as you can see the procedure involves a double interaction with the server: the first request GETs the page, the second POSTs the data. So you need to build a view that answers the GET request and a view that answers the POST one.

Since most of the time the URL we use to POST data is the same URL we used to GET the page we need to build a view that accepts both methods. This is the main reason of the pattern you are accustomed to use with functional form views in Django. [The offical Django documentation](https://docs.djangoproject.com/en/1.5/topics/forms/) on the subject uses this snippet of code

``` python
def contact(request):
    if request.method == 'POST': # If the form has been submitted...
        form = ContactForm(request.POST) # A form bound to the POST data
        if form.is_valid(): # All validation rules pass
            # Process the data in form.cleaned_data
            # ...
        return HttpResponseRedirect('/thanks/') # Redirect after POST
    else:
        form = ContactForm() # An unbound form

    return render(request, 'contact.html', {
        'form': form,
    })
```

As you can see the first conditional path deals with the data submission (POST), while the else part deals with the usual case of a GET request.

Now it is time to dig into the class-based forms Django provides us to understand how they deal with this double interaction.

Following the first example with notes we find the `CreateView` class in [views/generic/edit.py#202](https://github.com/django/django/blob/stable/1.5.x/django/views/generic/edit.py#L202). It is an almost empty class that inherits from `SingleObjectTemplateResponseMixin` and from `BaseCreateView`. The first class deals with the template selected to render the response and we can leave it aside for the moment. The second class, on the other hand, can be found a couple of lines above, at [views/generic/edit.py#L187](https://github.com/django/django/blob/stable/1.5.x/django/views/generic/edit.py#L187), and implements two methods which names are self explaining, `get()` and `post()`.

We already met the `get()` method in the [past article](/blog/2013/12/11/digging-up-django-class-based-views-2) when we talked about the `dispatch()` method of the `View` class.




http://www.peachybits.com/2011/09/django-1-3-form-api-modelform-example/

Class-based generic forms
Create, Update, Delete
HTTP POST and post()
Pre-filled forms

