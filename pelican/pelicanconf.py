#!/usr/bin/env python
# -*- coding: utf-8 -*- #
from __future__ import unicode_literals

AUTHOR = u'Leonardo Giordani'
SITENAME = u'The Digital Cat'
SITEURL = ''

PATH = 'content'

TIMEZONE = 'Europe/Paris'

DEFAULT_LANG = u'en'

# Feed generation is usually not desired when developing
FEED_ALL_ATOM = None
CATEGORY_FEED_ATOM = None
TRANSLATION_FEED_ATOM = None

# Blogroll
#LINKS = (('Pelican', 'http://getpelican.com/'),
#         ('Python.org', 'http://python.org/'),)

# Social widget
# SOCIAL = (
# 		('Twitter', 'https://twitter.com/tw_lgiordani'),
# 		('Google+', 'http://plus.google.com/+LeonardoGiordani?rel=author'),
# 		('GitHub', 'https://github.com/lgiordani'),
# 	)

DEFAULT_PAGINATION = 10
PAGINATION_PATTERNS = (
	(1, '{base_name}/', '{base_name}/index.html'),
	(2, '{base_name}/page/{number}/', '{base_name}/page/{number}/index.html'),
	)

# Uncomment following line if you want document-relative URLs when developing
#RELATIVE_URLS = True
STATIC_PATHS = ['images', 'code', 'extra/CNAME']
EXTRA_PATH_METADATA = {'extra/CNAME': {'path': 'CNAME'},}

THEME = "pelican-themes/pelican-bootstrap3"
CC_LICENSE = 'CC-BY-SA'
#SHOW_ARTICLE_AUTHOR = True
BOOTSTRAP_THEME = 'spacelab'
PYGMENTS_STYLE = 'monokai'
SITELOGO = 'images/TheDigitalCat_favicon_32.png'
SITELOGO_SIZE = 24
FAVICON = 'images/TheDigitalCat_favicon_32.png'
DISPLAY_ARTICLE_INFO_ON_INDEX = True
DISPLAY_TAGS_INLINE = True
DISPLAY_RECENT_POSTS_ON_SIDEBAR = True
#GITHUB_USER = 'lgiordani'
TWITTER_USERNAME = 'tw_lgiordani'
TWITTER_CARDS = True
OPEN_GRAPH_IMAGE = 'images/TheDigitalCat_logo_200.jpg'

ARTICLE_URL = 'blog/{date:%Y}/{date:%m}/{date:%d}/{slug}/'
ARTICLE_SAVE_AS = ARTICLE_URL + 'index.html'

CATEGORY_URL = 'category/{slug}/'
CATEGORY_SAVE_AS = CATEGORY_URL + 'index.html'

TAG_URL = 'blog/categories/{slug}/'
TAG_SAVE_AS = TAG_URL + 'index.html'

ARCHIVES_SAVE_AS = 'archives/index.html'

PLUGIN_PATHS = ["pelican-plugins"]
PLUGINS = ['related_posts', 'sitemap']

GOOGLE_ANALYTICS_UNIVERSAL = 'UA-38715090-1'

SITEMAP = {
    'format': 'xml',
}
