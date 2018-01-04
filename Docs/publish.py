#!/usr/bin/env python3

import subprocess
import os

HTML_DIR = 'Generated/html'

def publish():
    cmd = subprocess.Popen(['git', 'status', '-s'], stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    result, err = cmd.communicate()
    output = result.decode('utf-8')
    if output:
        print("The working directory is dirty. Please commit any pending changes.")
        return

    output_dir = os.path.join(os.getcwd(), HTML_DIR)

    print("Checking out gh-pages branch into public")
    subprocess.call(['git', 'worktree', 'add', '-B', 'gh-pages', output_dir, 'origin/gh-pages'])

    print('Generating HTML')
    subprocess.call(['doxygen', 'Doxyfile'])

    print('Updating gh-pages branch')
    subprocess.call(['git', 'add', '--all', '&&', 'git', 'commit', '-m',
                     '"Publishing to gh-pages (publish.sh)"'], cwd=output_dir)



if __name__ == '__main__':
    publish()

# if [[ $(git status -s) ]]
# then
#     echo "The working directory is dirty. Please commit any pending changes."
#     exit 1;
# fi

# echo "Checking out gh-pages branch into public"
# git worktree add -B gh-pages Generated/html origin/gh-pages

# echo "Generating HTML"
# doxygen Doxyfile

# echo "Updating gh-pages branch"
# cd Generated/html && git add --all && git commit -m "Publishing to gh-pages (publish.sh)"
