################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from main import Simulation
import pytest

# this tests access files in resources - do not delete!

def test_simulation_exists(): # needs file opSimulation
    assert Simulation('test/resources')

def test_simulation_does_not_exist():
    with pytest.raises(SystemExit) as pytest_wrapped_e:
        Simulation('does_not_exist')
    assert pytest_wrapped_e.type == SystemExit

def test_simulation_without_log_defaults_to_openpasssimulation_log():
    s = Simulation('test/resources')
    assert 'test/resources/opSimulation.log' == s.log_file

def test_log_empty_file_is_empty():
    s = Simulation('test/resources', 'test/resources/empty.log')
    assert s.log_empty()

def test_log_missing_file_is_empty():
    s = Simulation('test/resources', 'does_not_exist')
    assert s.log_empty()

def test_log_with_content_is_not_empty():
    s = Simulation('test/resources', 'test/resources/opSimulation.log')
    assert not s.log_empty()

def test_log_with_content_returns_content():
    s = Simulation('test/resources', 'test/resources/opSimulation.log')
    assert len(s.get_log()) > 0

def test_log_without_content_returns_no_content():
    s = Simulation('test/resources', 'test/resources/empty.log')
    assert len(s.get_log()) == 0

def test_missing_log_returns_no_content():
    s = Simulation('test/resources', 'test/resources/empty.log')
    assert len(s.get_log()) == 0
