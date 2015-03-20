#!/bin/bash

# This script sets up the IPython Notebook development environment

sudo apt-get install -y python3-dev g++

virtualenv -p python3 venv3
source venv3/bin/activate

pip install ipython pyzmq jinja2 pygments tornado jsonschema
