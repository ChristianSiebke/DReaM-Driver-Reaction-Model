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
import json
import glob
from reports import HtmlReport

from junitparser import JUnitXml

class SLAVEINFO:
    exe = 'OpenPassSlave' + ('.exe' if os.name == 'nt' else '')
    log_file = 'OpenPassSlave.log'
    configs = 'configs'
    results = 'results'


def terminate_program(message):
    logging.error(message)
    sys.exit(message)


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('-r', '--resources',
                        nargs='+',
                        help='path to the resources (e.g. /openPASS/bin/examples)', required=True)
    parser.add_argument('-c', '--config',
                        help='config file (e.g. end_to_end.json)', required=True)
    parser.add_argument('--scope',
                        nargs='+',
                        help='scope (e.g. generic)', required=True)
    parser.add_argument('--results',
                        help='path to test results', required=True)
    parser.add_argument('--output',
                        help='path to generated report', required=True)
    return parser.parse_args()


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


def get_parameterization_variants(scopes, directory):
    """
    Scans the config if a directory is available within a scope 
    without parameterization, with parameterization, or both
    
    Returns 
    - unparameterized True if in a scope without parameterization
    - unparameter     True if in a scope with parameterization
    - num_parameters  Number of parameters if parameterized
    """
    parameterized = False
    unparameterized = False
    num_parameters = 0
    
    for scope in scopes.values():
        if directory in scope['configurations']:
            if 'parameterization' in scope:
                parameterized = True
                num_parameters = len(scope['parameterization']['values'])
            else:
                unparameterized = True
    return unparameterized, parameterized, num_parameters
            

def apply_parameterization_variants(scopes, directories):
    """
    Configurations can be available with and without parameterization
    These variants result in different output directories.
    
    Returns an sorted and updated list of directories
    """
    add_list = []
    remove_list = [] 
        
    for d in directories:
        unparameterized, parameterized, num_parameters = get_parameterization_variants(scopes, d)
        if not unparameterized:
            remove_list.append(d)
        if parameterized:
            for i in range(0, num_parameters):
                add_list.append(f'{d}_{i}')

    return [d for d in directories if (d not in remove_list)] + add_list
   

def get_dirs(base_path, exclude_list=[]):
    """
    Retrieves the a list of directories for the given base path
    """
    if not os.path.isdir(base_path):
        terminate_program(f'Not a directory: {base_path}')
        
    return [d for d in os.listdir(base_path) if d not in exclude_list and os.path.isdir(os.path.join(base_path, d))]


def get_configuration_dirs(scopes, base_path, exclude_list=[]):
    """
    Retrieves the a list of configuration directories, matched against the available 
    scopes, and considering potential parameterization of individual configs
    """
    return apply_parameterization_variants(scopes, get_dirs(base_path, exclude_list))


class Directory:
    def __init__(self, name):
        self.name = name
        self.handled = False


class Scope:
    def __init__(self, name, config):
        self.name = name
        self.config = config
        self.test_results = dict()
        self.test_rates = dict()

    def add_result(self, result, rates):
        if result.test_name not in self.test_results.keys():
            self.test_results[result.test_name] = []
            self.test_rates[result.test_name] = []
        self.test_results[result.test_name].append(result)
        self.test_rates[result.test_name].append(rates)


class JUnitReport:
    def __init__(self, filepath):
        self.xml = JUnitXml.fromfile(filepath)

    def status(self, test_case=None):
        from junitparser import TestSuite, Failure
        if isinstance(self.xml, TestSuite):
            for test in self.xml:
                if f'[{test_case}]' in test.name:
                    return not isinstance(test.result, Failure), test.time
        return True, 0.0


class TestResult:
    def __init__(self, test_result_file, test_name):
        self.test_name = test_name
        self.status = False
        self.duration = 0.0
        self.data = ""
        self.parse_result_dir(test_result_file, test_name)

    def parse_result_dir(self, test_result_file, test_name):
        ju = JUnitReport(test_result_file)
        (self.status, self.duration) = ju.status(test_name)


def remove_testsuites(file):
    """
    This method removes the intermediate tag <testsuites> from the given file
    added on some systems but unnecessary - only makes parsing complicated
    :param file: a results xml file
    :return: None
    """
    fin = open(file, "rt")
    data = fin.read()
    data = data.replace(r'<testsuites>', '')
    data = data.replace(r'</testsuites>', '')
    fin.close()

    fin = open(file, "wt")
    fin.write(data)
    fin.close()

def get_rates(results_path, config_under_test):
    """
    Reads the rates csv and parses it into (expected, actual, failed_runs)
    """
    rates_file = os.path.join(results_path, config_under_test, 'rates.csv')
    if not os.path.exists(rates_file):
        return None

    lines = []
    with open(rates_file, "r") as rates: 
        lines = rates.readlines()
    
    rates = lines[1].split(";")
    return (float(rates[0]), float(rates[1]), rates[2])

def get_configs_under_test(scope):
    configs = scope['configurations']
    if not 'parameterization' in scope:
        return configs
    else:
        num_parameters = len(scope['parameterization']['values'])
        return [f'{c}_{i}' for c in configs for i in range(0,num_parameters)]

def main():
    args = parse_arguments()

    config = load_test_config(args.config)

    dir_names = []
    for resources_path in args.resources:
        dir_names += get_configuration_dirs(config['scopes'], resources_path, exclude_list=['common', 'sceneries', 'scripts'])

    dir_names = sorted(set(dir_names))

    all_config_dirs = [Directory(d) for d in dir_names]

    report = HtmlReport(args.output, all_config_dirs)

    for scope_name in args.scope:
        scope_config = get_scope(config, scope_name)
        scope = Scope(scope_name, scope_config)
        results_dir = os.path.join(args.results, scope_name)

        configs_under_test = get_configs_under_test(config['scopes'][scope_name])
        executed_dirs = get_dirs(results_dir)
        all_configs = sorted(list(set(configs_under_test + executed_dirs)))

        for config_under_test in all_configs:
            rates = None
            if config_under_test not in executed_dirs:
                report.add_missing_config(config_under_test, scope_name)
            else:
                rates = get_rates(results_dir, config_under_test)

            remove_testsuites(f'result_{scope_name}.xml')

            result = TestResult(f'result_{scope_name}.xml', config_under_test)
            scope.add_result(result, rates)

        report.add_scope(scope)

    report.generate()


if __name__ == '__main__':
    main()
