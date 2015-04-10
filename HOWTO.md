source venv/bin/activate
cd pelican
git flow feature start <nomefeature>

Make new file in content/ with the following content

Title: "<title of the post>"
Date: $(date +"%Y-%m-%d %H:00:00 +0100")
Category: Programming/Projects
Tags: <comma separated tags>
Authors: Leonardo Giordani
Slug: <slug>
Summary: <summary>

write the post

add, commit

git flow feature finish

git flow release start $(date +"%Y%m%d%H%M")

version >> version.txt

git flow release finish

git push

./deploy.sh


http://goo.gl/ To shorten URL

Take picture

Twitter
http://djangolinks.com/
Google+
Webmasters Tools
pythondigest.ru
http://pycoders.com
