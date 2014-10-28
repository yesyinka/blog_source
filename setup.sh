#!/bin/bash

# This script sets up the blog development environment

virtualenv venv
source venv/bin/activate

pip install fabric pelican markdown

git clone https://github.com/lgiordani/pelican-plugins.git pelican/pelican-plugins
git clone https://github.com/lgiordani/pelican-themes.git pelican/pelican-themes
git clone https://github.com/lgiordani/lgiordani.github.com.git

echo
echo "Remember to issue source venv/bin/activate"
