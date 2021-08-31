################################################################################
# Copyright (c) 2020, 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import config
import os
import shutil
from distutils import dir_util
from xml_util import XmlUtil

from query_parser import Query

import query_executor
import event_loader


def pytest_generate_tests(metafunc):
    metafunc.parametrize('specialized_config_under_test', config.MetaInfo.configs_under_test,
                         ids=[c.name for c in config.MetaInfo.configs_under_test])


def get_timestamp():
    from datetime import datetime
    return datetime.utcnow().strftime('%Y%m%d_%H%M%S_%f')[:-3]


def clean_and_create(folder):
    if os.path.isfile(folder) or os.path.islink(folder):
        os.unlink(folder)

    if os.path.exists(folder):
        dir_util.remove_tree(folder)

    os.makedirs(folder)


def run(config_under_test, slave_config, subdir):
    assert(config_under_test.exists()), f': Unable to find {config_under_test}'

    slave = config.MetaInfo.slave.copy()
    slave.configs = os.path.join(config.MetaInfo.slave.configs, subdir)
    slave.results = os.path.join(config.MetaInfo.slave.results, subdir)
    slave.log_file = os.path.join(slave.results, 'test.log')

    clean_and_create(slave.results)
    config_under_test.copy_to(slave.configs)

    XmlUtil.update(slave.configs, slave_config)

    slave_state = slave.execute()
    slave.copy_artifacts(os.path.join('artifacts', subdir))

    assert(slave_state == 0), f'Slave execution failed\n{slave.get_log()}'
    assert(slave.log_empty()), f'Slave log not empty\n{slave.get_log()}'
    print(slave.get_log())


class TestSpecialized:
    @staticmethod
    def query_result(base_run, events, run_id, query):
        output = query_executor.prepare_output(base_run)
        return query_executor.query_output(output, events, run_id, query)

    @staticmethod
    def report_rates(rates_file, expected, actual, failed_runs):
        """
        This method writes all metainfo as bridge for the report
        """
        with open(rates_file, "w") as rates: 
            rates.writelines(f'expected;actual;failed_runs\n')
            rates.writelines(f'{expected};{actual};{failed_runs}')

    def test_specialized(self, specialized_config_under_test):
        test_info = config.MetaInfo
        subdir = f'{test_info.scope}/{specialized_config_under_test.name}'
        run(specialized_config_under_test, test_info.base_config, subdir)

        invocations = test_info.base_config["invocations"]
        query = Query(test_info.specialized["query"])

        failed_runs = list()

        simulation_output = os.path.join('artifacts', subdir, 'simulationOutput.xml')
        events = event_loader.get_events(simulation_output, query.events)

        for i in range(0, invocations):
            base_run = os.path.join('artifacts', subdir, f'Cyclics_Run_{i:03}.csv')
            if not self.query_result(base_run, events, i, query):
                failed_runs.append(i)

        expected_rate = test_info.specialized["success_rate"]
        actual_rate = (invocations - len(failed_runs)) / invocations

        # make sure file is in both folders!
        rates_results_file = os.path.join(test_info.slave.results, subdir, 'rates.csv')
        rates_artifacts_file = os.path.join('artifacts', subdir, 'rates.csv')
        
        self.report_rates(rates_results_file, expected_rate, actual_rate, failed_runs)
        self.report_rates(rates_artifacts_file, expected_rate, actual_rate, failed_runs)

        assert(actual_rate >= expected_rate), \
               f'Success rate  [Actual {actual_rate*100:.2f}% | Expected {expected_rate*100:.2f}%]\n' \
               f'{" "*14}Failed RunIds {failed_runs}'
