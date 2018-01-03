#!/usr/bin/env python3

import os
import argparse
import subprocess

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-b', '--build-type', choices=['Debug', 'Release'], required=True)
    parser.add_argument('--build-examples', action='store_true')
    parser.add_argument('--build-tests', action='store_true')

    print('JobRocket: Configuring build files')

    args = parser.parse_args()
    cmd = ['cmake', '../..', '-DCMAKE_BUILD_TYPE={0}'.format(args.build_type)]
    if args.build_examples:
        cmd.append('-DBUILD_EXAMPLES=ON')
    else:
        cmd.append('-DBUILD_EXAMPLES=OFF')

    if args.build_tests:
        cmd.append('-DBUILD_TESTS=ON')
    else:
        cmd.append('-DBUILD_TESTS=OFF')

    build_dir = os.path.join(os.getcwd(), 'Build')
    build_type_dir = os.path.join(build_dir, args.build_type)
    deps_dir = os.path.join(os.getcwd(), 'Deps')

    if not os.path.exists(deps_dir):
        os.mkdir(deps_dir)
        subprocess.call(['git', 'submodule', 'update', '--init'], cwd=deps_dir)

    if not os.path.exists(build_dir):
        os.mkdir(build_dir)
    if not os.path.exists(build_type_dir):
        os.mkdir(build_type_dir)

    subprocess.call(args=cmd, cwd=build_type_dir)
    subprocess.call(['cmake', '--build', build_type_dir], cwd=build_type_dir)
