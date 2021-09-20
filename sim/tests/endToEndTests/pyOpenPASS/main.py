#!/usr/bin/env python3

################################################################################
# Copyright (c) 2020, 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import logging
import sys
import argparse
import os.path
import os
import json
import glob
import config

import pytest
from tests.default_tests import *

class SLAVEINFO:
    exe = 'OpenPassSlave' + ('.exe' if os.name == 'nt' else '')
    log_file = 'OpenPassSlave.log'
    configs = 'configs'
    results = 'results'


class CONFIGINFO:
    common = 'common'


class Slave:
    base_path = None
    exe = None
    log_file = None
    configs = None
    results = None

    def __init__(self, base_path, log_file=None, configs=None, results=None):
        self.base_path = base_path
        exe = os.path.join(base_path, SLAVEINFO.exe)
        if not os.path.isfile(exe):
            terminate_program(f'Cannot find slave ({exe})')
        self.exe = exe

        if log_file is None:
            self.log_file = os.path.join(base_path, SLAVEINFO.log_file)
        else:
            self.log_file = log_file

        if results is None:
            self.results = os.path.join(base_path, SLAVEINFO.results)
        else:
            self.results = results

        if configs is None:
            self.configs = os.path.join(base_path, SLAVEINFO.configs)
        else:
            self.configs = configs

    def execute(self):
        os.chdir(self.base_path)
        command = f'{self.exe} --logFile {self.log_file} --configs {self.configs} --results {self.results}'
        logging.info(f'Executing {command}')
        return os.system(command)

    def copy_artifacts(self, destination):
        from shutil import copyfile
        copydir(self.configs, destination)
        copydir(self.results, destination)
        if os.path.exists(self.log_file):
            _, tail = os.path.split(self.log_file)
            copyfile(self.log_file, os.path.join(destination, tail))

    def copy(self):
        return Slave(self.base_path, self.log_file, self.configs, self.results)


    def log_empty(self):
        if os.path.exists(self.log_file):
            return os.path.getsize(self.log_file) == 0
        return True

    def get_log(self):
        if not self.log_empty():
            with open(self.log_file, 'r') as l:
                return l.read().strip()
        else:
            return ''


def terminate_program(message):
    logging.error(message)
    sys.exit(message)


def parse_arguments():
    ap = argparse.ArgumentParser()
    ap.add_argument('-s', '--slave',
                    help=r'path to the openPASS slave (e.g. /openPASS/bin/)', 
                    required=True)
    ap.add_argument('-m', '--mutual',
                    default=None,
                    help=r'path to mutual configurations')
    ap.add_argument('-r', '--resources',
                    help=r'path to the resources (e.g. /openPASS/bin/examples/configs)', 
                    required=True)
    ap.add_argument('-c', '--config',
                    help=r'config file (e.g. end_to_end.json)', 
                    required=True)
    ap.add_argument('--scope',
                    help=r'scope (e.g. generic)', 
                    required=True)
    ap.add_argument('-d', '--debug',
                    help=r'activates csv output of some pandas dataframes to /tmp or C:\\temp',
                    action='store_true',
                    default=False)

    return ap.parse_args()


def load_test_config(config):
    if not os.path.isfile(config):
        terminate_program(f'Cannot find config ({config})')
    with open(config) as c:
        return json.load(c)


def get_scope(config, scope):
    if 'scopes' not in config.keys():
        terminate_program(f'Missing section scopes')
    if scope not in config['scopes'].keys():
        terminate_program(f'Invalid scope {scope}')
    return config['scopes'][scope]


def get_all_files(base_path, sub_path):
    common_path = os.path.join(base_path, sub_path)
    if not os.path.isdir(common_path):
        terminate_program(f'Cannot find {common_path}')
    return [f for f in glob.glob(f'{common_path}/**/*.*', recursive=True)]


def copydir(source, dest):
    """Copy a directory structure overwriting existing files"""
    import shutil
    for root, dirs, files in os.walk(source):
        if not os.path.isdir(root):
            os.makedirs(root)

        for file in files:
            rel_path = root.replace(source, '').lstrip(os.sep)
            dest_path = os.path.join(dest, rel_path)

            if not os.path.isdir(dest_path):
                os.makedirs(dest_path)

            shutil.copyfile(os.path.join(root, file),
                            os.path.join(dest_path, file))


class ConfigUnderTest:
    files = []

    def __init__(self, default_path, base_path, name):
        self.name = name
        self.default_path = default_path
        self.base_path = base_path

    def exists(self):
        return os.path.exists(str(self))

    def copy_to(self, destination):
        from shutil import rmtree

        rmtree(destination, ignore_errors=True)
        os.makedirs(destination)
        if self.default_path:
            copydir(self.default_path, destination)
        copydir(os.path.join(self.base_path, self.name), destination)

    def __str__(self):
        return str(os.path.join(self.base_path, self.name))


def main():
    args = parse_arguments()

    config.DEBUG.PRINT_CSV = args.debug

    slave = Slave(args.slave)

    the_config = load_test_config(args.config)
    scope = get_scope(the_config, args.scope)
    configs_under_test = [ConfigUnderTest(
        args.mutual, args.resources, c) for c in scope["configurations"]]

    config.MetaInfo.slave = slave
    config.MetaInfo.scope = args.scope
    config.MetaInfo.base_config = scope["baseConfig"]
    config.MetaInfo.configs_under_test = configs_under_test
    config.MetaInfo.datatypes = the_config["datatypes"] if "datatypes" in the_config else {}
    
    if "parameterization" in scope.keys():
        config.MetaInfo.parameterization = scope["parameterization"]

    cargs = (12 if args.debug else 11)

    if "specialized" not in scope.keys():
        return pytest.main(sys.argv[cargs:] + [f'--junitxml=result_{args.scope}.xml', 'tests/default_tests.py'])
    else:
        config.MetaInfo.specialized = scope["specialized"]
        return pytest.main(sys.argv[cargs:] + [f'--junitxml=result_{args.scope}.xml', 'tests/specialized_tests.py'])


if __name__ == '__main__':
    main()
