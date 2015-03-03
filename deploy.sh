#!/bin/bash

source venv/bin/activate
cd pelican
pelican content -s publishconf.py
cd ..
rm -fR deploy/*
cp -R pelican/output/* deploy/
cd deploy
mkdir downloads
cp -R code downloads/
cp -R categories blog/

echo "Move to the deploy directory, check, commit and push"
