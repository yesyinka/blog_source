#!/bin/bash

source venv/bin/activate
cd pelican
pelican content -s publishconf.py
cd ..
rm -fR deploy/*
cp -R pelican/output/* deploy/
cd deploy
mkdir download
cp -R code download/
cp -R categories blog/

echo "Move to the deploy directory, check, commit and push"
