#!/bin/bash

rm -fR venv
rm -fR pelican/pelican-plugins
#rm -fR pelican/pelican-themes
rm -fR pelican/pelican-bootstrap3
rm -fR pelican/output
rm -fR pelican/cache
rm -fR deploy

find . -iname "*.pyc" -delete

