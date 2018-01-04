#!/usr/bin/env python3

import subprocess
import os

HTML_DIR = 'Generated/html'

def print_status(status):
    GREEN = '\033[32m'
    WHITE = '\033[0m'
    print('{0}{1}{2}'.format(GREEN, status, WHITE))

def publish():
    print_status("Jobrocket: Generating docs")

    cmd = subprocess.Popen(['git', 'status', '-s'], stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    result, err = cmd.communicate()
    output = result.decode('utf-8')

    print(output)
    if output:
        print_status("The working directory is dirty. Please commit any pending changes.")
        return

    script_dir = os.path.dirname(os.path.realpath(__file__))
    output_dir = os.path.join(script_dir, HTML_DIR)

    print_status("Checking out gh-pages branch into public")
    subprocess.call(['git', 'worktree', 'add', '-B', 'gh-pages', HTML_DIR, 'origin/gh-pages'],
                    cwd=script_dir)

    print_status('Generating HTML')
    subprocess.call(['doxygen', 'Doxyfile'], cwd=script_dir)

    print_status('Updating gh-pages branch')
    subprocess.call(['git', 'add', '--all'], cwd=output_dir)
    subprocess.call(['git', 'commit', '-m', '"docs/API: Update API reference docs and HTML"'],
                    cwd=output_dir)
    subprocess.call(['git', 'push', 'origin', 'gh-pages'])

if __name__ == '__main__':
    publish()
