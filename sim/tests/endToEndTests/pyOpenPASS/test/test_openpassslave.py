################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from main import Slave
import pytest

# this tests access files in resources - do not delete!

def test_slave_exists(): # needs file OpenPassSlave
    assert Slave('test/resources')

def test_slave_does_not_exist():
    with pytest.raises(SystemExit) as pytest_wrapped_e:
        Slave('does_not_exist')
    assert pytest_wrapped_e.type == SystemExit

def test_slave_without_log_defaults_to_openpassslave_log():
    s = Slave('test/resources')
    assert 'test/resources/OpenPassSlave.log' == s.log_file

def test_log_empty_file_is_empty():
    s = Slave('test/resources', 'test/resources/empty.log')
    assert s.log_empty()

def test_log_missing_file_is_empty():
    s = Slave('test/resources', 'does_not_exist')
    assert s.log_empty()

def test_log_with_content_is_not_empty():
    s = Slave('test/resources', 'test/resources/OpenPassSlave.log')
    assert not s.log_empty()

def test_log_with_content_returns_content():
    s = Slave('test/resources', 'test/resources/OpenPassSlave.log')
    assert len(s.get_log()) > 0

def test_log_without_content_returns_no_content():
    s = Slave('test/resources', 'test/resources/empty.log')
    assert len(s.get_log()) == 0

def test_missing_log_returns_no_content():
    s = Slave('test/resources', 'test/resources/empty.log')
    assert len(s.get_log()) == 0
