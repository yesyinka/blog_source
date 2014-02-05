---
layout: post
title: "Digging up Django class-based views - 3"
date: 2014-01-20 16:13:41 +0100
comments: true
categories: [python, django]
---

_This post refers to Django 1.5. Please be warned that some of the matters discussed here, some solutions or the given code can be outdated by more recent Django versions_

In the first two issues of this short series we wandered around the basic concepts of class-based views in Django, and started understanding and using two of the basic generic views Django makes available to you: `ListView` and `DetailView`. Both are views that read some data from the database and show them on a renderized template.

This third issue wants to introduce the reader to the class-based version of Django forms. This post is not meant to be a full introduction to the Django form library; rather, I want to show how class-based generic views implement the CRUD (Create, Read, Update, Delete) operations and how the class nature can be leveraged to achieve some common behaviours.


## A very basic example

To start working with CBF (class-based forms) let's suppose we are working with a `StickyNote` class which represents a simple text note with a date.

``` python
class StickyNote(models.Model):
    timestamp = models.DateTimeField()
    text = models.TextField(blank=True, null=True)
```

One of the first things we usually want to do is to build a form that allows the user to create a new entry in the database, in this case a new sticky note. The functional version of such a form wouold be the following:

```python
def create_note(request):
    form = StickyNoteForm(request.POST or None)
    if form.is_valid():
        new_note = form.save()
        new_note.save()
    return render_to_response(...)
```


http://www.peachybits.com/2011/09/django-1-3-form-api-modelform-example/
