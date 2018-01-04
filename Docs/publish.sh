
#!/bin/sh

DIR=$(dirname "$0")

cd $DIR/..

if [[ $(git status -s) ]]
then
    echo "The working directory is dirty. Please commit any pending changes."
    exit 1;
fi

echo "Checking out gh-pages branch into public"
git worktree add -B gh-pages Generated/html origin/gh-pages

echo "Generating HTML"
doxygen Doxyfile

echo "Updating gh-pages branch"
cd public && git add --all && git commit -m "Publishing to gh-pages (publish.sh)"
