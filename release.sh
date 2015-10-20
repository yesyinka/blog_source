#!/bin/bash

version=$(~/Devel/bumpversion/tbump --allow-dirty --dry-run --list date | grep new_version | sed -r s,"^.*=",,)
#version=$(date +%Y%m%d%H%M)

git flow release start ${version}
echo "${version}" > version.txt
cat .bumpversion.cfg | sed -r s,"current_version = .*","current_version = ${version}", > new_bumpversion.cfg
mv new_bumpversion.cfg .bumpversion.cfg
git add version.txt .bumpversion.cfg
git ci -m "Version bumped"
git flow release finish -m "Version ${version}" -p ${version}

