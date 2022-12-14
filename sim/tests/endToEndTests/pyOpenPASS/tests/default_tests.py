################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import config
import os
from distutils import dir_util
from itertools import product

from distutils import dir_util
from xml_util import XmlUtil, XmlInfo, XmlSetter

class TestingData:
    """
    Wrapper for a single configuration under test including an optional xml setter
    Without index or xml setter, it only wraps the configuration under test.
    
    The class is used in two scenarios:
    
    - Parameterization not set: Wrap the configuration under test for a single test.
    - Parameterization is set:  For each value an index and xml setter is created.
                                In such cases, the index is appended to the 
                                name of the config as identifer.
    
    In any case, the identifer is used as subdirectory to keep individual 
    parameterizations independent
    """
    config_under_test = None
    identifier = None
    xml_setter = None
    __test__ = False # prevent pytest from collecting as test case

    def __init__(self, config_under_test, idx=None, xml_setter:XmlSetter=None):
        self.config_under_test = config_under_test
        if idx is not None:
            self.identifier = f'{config_under_test.name}_{idx}'
        else:
            self.identifier = config_under_test.name
        self.xml_setter = xml_setter


def pytest_generate_tests(metafunc):
    """
    Generates the individual testing data units executed by TestExecution
    """
    test_set = []
    
    if config.MetaInfo.parameterization is not None:
        values = config.MetaInfo.parameterization['values']
        
        # create all possible combinations between configs and values
        combinations = list(product(config.MetaInfo.configs_under_test, values))

        for idx, (config_under_test, value) in enumerate(combinations):
            xml_file = config.MetaInfo.parameterization['file']
            xpath_expr = config.MetaInfo.parameterization['xpath']

            test_set.append(TestingData(config_under_test, idx, XmlSetter(XmlInfo(xml_file, xpath_expr), value)))
    
    else:
        for config_under_test in config.MetaInfo.configs_under_test:
            test_set.append(TestingData(config_under_test))
    
    metafunc.parametrize('testing_data', test_set, ids=[t.identifier for t in test_set])


def get_timestamp():
    from datetime import datetime
    return datetime.utcnow().strftime('%Y%m%d_%H%M%S_%f')[:-3]


def clean_and_create(folder):
    if os.path.isfile(folder) or os.path.islink(folder):
        os.unlink(folder)

    if os.path.exists(folder):
        dir_util.remove_tree(folder)

    os.makedirs(folder)

def print_simulation_log():
    return 'Simulation execution failed'

def run(testing_data: TestingData, simulation_config, subdir):
    assert(testing_data.config_under_test.exists()), f': Unable to find {testing_data.config_under_test}'

    simulation = config.MetaInfo.simulation.copy()
    simulation.configs = os.path.join(config.MetaInfo.simulation.configs, subdir)
    simulation.results = os.path.join(config.MetaInfo.simulation.results, subdir)
    simulation.log_file = os.path.join(simulation.results, 'test.log')

    # Copy Configurations
    clean_and_create(simulation.results)
    testing_data.config_under_test.copy_to(simulation.configs)

    # Update Configurations
    XmlUtil.update(simulation.configs, simulation_config)
    if testing_data.xml_setter is not None:
        XmlUtil.custom_update(simulation.configs, testing_data.xml_setter)

    # Execute Simulation
    simulation_state = simulation.execute()
    
    # Collect Results
    simulation.copy_artifacts(os.path.join('artifacts', subdir))

    assert(simulation_state == 0), f'Simulation execution failed\n{simulation.get_log()}'
    assert(simulation.log_empty()), f'Simulation log not empty\n{simulation.get_log()}'
    print(simulation.get_log())


class TestExecution:
    def test_execution(self, testing_data: TestingData):
        run(testing_data, config.MetaInfo.base_config,
            f'{config.MetaInfo.scope}/{testing_data.identifier}')

    def init_random_seed(self, config_under_test):
        """Get random seed value of current config if not set in 'baseConfig' section"""
        if 'randomSeed' not in config.MetaInfo.base_config.keys():
            return XmlUtil.get_as_int(config_under_test, XmlUtil.CONFIG_RANDOM_SEED)
        return config.MetaInfo.base_config['randomSeed']

    def test_determinism(self, testing_data: TestingData):
        invocations = 3
        subdir = f'{config.MetaInfo.scope}/{testing_data.identifier}/determinism'
        simulation_config = config.MetaInfo.base_config
        simulation_config["invocations"] = invocations
        random_seed = self.init_random_seed(testing_data.config_under_test)

        run(testing_data, simulation_config, f'{subdir}_base')

        for i in range(0, invocations):
            simulation_config["invocations"] = 1
            simulation_config["randomSeed"] = random_seed + i

            run(testing_data, simulation_config, f'{subdir}_run{i}')

            import os
            base_run = os.path.join(
                'artifacts', f'{subdir}_base', f'Cyclics_Run_{i:03}.csv')
            assert(os.path.exists(base_run))

            re_run = os.path.join(
                'artifacts', f'{subdir}_run{i}', f'Cyclics_Run_000.csv')
            assert(os.path.exists(re_run))

            import filecmp
            assert filecmp.cmp(
                base_run, re_run, False), f'{base_run} and {re_run} do not match'
