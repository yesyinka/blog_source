---
layout: post
title: "Digging up Django class-based views - 2"
date: 2013-11-10 15:53
comments: true
categories: [python, django]
---

* DetailView
* Queryset for ListView and DetailView
* Passing arguments to CBVs
* View

The second class-based generic view Django provides us is DetailView, which deals, as the name suggests, with the details of what we get from the DB. While ListView's basic behaviour was to extract the list of all objects with a given model, DetailView extracts only one object. Before we look into the internal behaviour of DetailView we have to deal with two new topics: view queryset and view arguments. The last topic we will introduce in this new installment is going to be the View class and the creation of customized class-based views.

Queryset

If you use Django you already know what a Queryset is, otherwise please look at the documentation to introduce yourself to this powerful mechanism. Class-based generic views are intimately related to a queryset since their aim is to provide objects extracted from the DB and this implies building a queryset. As you know, however, a queryset can be built only if it has been defined, i.e. if the SQL query can be built. As you can already guess, CBGVs hide a pre-constructed queryset and provide their result through it. The specific point where a CBV builds the queryset is the get_queryset() method; for ListView it is defined by one of its ancestors, namely MultipleObjectMixin (list.py). The default queryset for ListView is (list.py:36) `self.model._default_manager.all()`, which exctracts every object with that model. Pay attention that it uses `_default_manager` and this allows a certain degree of customization from the model class itself. if you loo at the code some line above, however, you can see that MultipleObjectMixin first checks the queryset attribute. This is the same pattern we met for the model attribute, with the difference that here the class can provide a default behaviour (providing a default model to manage makes no sense).

??? cosa fa qui con queryset.all()?

When we define a CBV like

```
from myapp.models import Element
from django.views import ListView ??? Check

class ElementList(ListView):
      model = Element
```

what Django does internally is to execute `Element.objects.all()`, if `objects` is the default manager. This means that our view lists all objects in the Element table, and this is perfectly fine when we need complete listings. Sometimes, however, we would like be able to show a subset of objects, and here the queryset attribute comes to the rescue. Say for example that we want to show all Element objects with a `weight` greater than 100 (don't bother with measure units for the moment being).

```
...
class BigElementList(ListView):
      model = Element
      queryset = Element.objects.filter(weight__gt=100)
```

