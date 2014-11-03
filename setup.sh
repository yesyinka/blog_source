#!/bin/bash

# This script sets up the blog development environment

if [[ ! -z $1 ]]
    then
    method=$1;
else
    method='https'
fi

# repo <method> <user> <name>
function repo () {
    if [[ $1 -eq 'https' ]]
        then
        echo https://github.com/${2}/${3}.git
    elif [[ $1 -eq 'ssh' ]]
        then
        echo git@github.com:${2}/${3}.git
    else
        echo "Method '${1}' not supported"
        exit 1
    fi
}

# get_repo <method> <name>
function get_repo () {
    git clone $(repo ${1} lgiordani ${2}) pelican/${2}
    cd pelican/${2}
    git remote add upstream $(repo ${1} getpelican ${2})
    cd ..
}

#virtualenv venv
#source venv/bin/activate

#pip install fabric pelican markdown

$(get_repo ${method} pelican-plugins)
# git clone $(repo ${method} lgiordani pelican-plugins)
# cd pelican/pelican-plugins
# git remote add upstream $(repo ${method} getpelican pelican-plugins)
# cd ..

$(get_repo ${method} pelican-themes)
# git clone https://github.com/lgiordani/pelican-themes.git pelican/pelican-themes
# cd pelican/pelican-themes
# git remote add upstream https://github.com/getpelican/pelican-themes.git
# cd ..

git clone https://github.com/lgiordani/lgiordani.github.com.git deploy

echo
echo "Remember to issue source venv/bin/activate"
