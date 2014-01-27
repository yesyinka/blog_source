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


Class-based generic forms
Create, Update, Delete
HTTP POST and post()
Pre-filled forms
