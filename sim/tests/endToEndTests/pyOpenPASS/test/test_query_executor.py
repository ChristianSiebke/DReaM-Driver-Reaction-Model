################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import numpy
import query_executor
from io import StringIO
import pytest
import pandas as pd

csv_test_data = """
    Timestep, 00:some, 00:other, 01:some, 01:other, 01:other2
    0, 1, 2, 3, 4, 8
    1, 5, 6, 7, 8, 9
"""

csv_test_data_despawned_agent = """
    Timestep, 00:some, 00:other, 01:some, 01:other
    0, 1, 2, 3, 4
    1, 5, 6, 7, 8
    2, 9, 10, ,
    3, 11, 12, ,
"""


def test_query_executor_parses_csv():
    csv = StringIO(csv_test_data)
    df = query_executor.prepare_output(csv)
    assert(len(df) == 4)


def test_query_executor_parses_csv_with_despawned_agents():
    csv = StringIO(csv_test_data_despawned_agent)
    df = query_executor.prepare_output(csv)
    assert(len(df) == 6)


def test_query_executor_obeys_datatypes():
    explicit_types = {"other": "string", "other2": "float"}
    expected_types = pd.DataFrame.from_dict({"some": [1], "other": ['2'], "other2": [
                                  3.0]}).astype({"other": "string"})

    csv = StringIO(csv_test_data)
    df = query_executor.prepare_output(csv, explicit_types)
    assert((df.dtypes[["some", "other", "other2"]] ==
           expected_types.dtypes[["some", "other", "other2"]]).all())


def test_query_executor_ignores_unknown_columns():
    explicit_types = {"unknown": "string"}

    csv = StringIO(csv_test_data)

    try:
        df = query_executor.prepare_output(csv, explicit_types)
    except KeyError:
        pytest.fail("Unknown column not ignored during datatype conversion")


def test_query_executor__shifting_column_by_one_timestep__shifts_values_per_agent_one_rows():
    prepared_output = pd.DataFrame.from_dict({
        "Timestep": [0, 0, 1, 1, 2, 2],
        "AgentId": [0, 1, 0, 1, 0, 1],
        "Value": [0, 10, 1, 11, 2, 12]})

    df = query_executor.add_shifted_column(
        prepared_output, ('Value_prev1', 'Value', 1))
    assert df['Value_prev1'].tolist() == [0, 10, 0, 10, 1, 11]


def test_query_executor__shifting_column_by_two_timesteps__shifts_values_per_agent_two_rows():
    prepared_output = pd.DataFrame.from_dict({
        "Timestep": [0, 0, 1, 1, 2, 2],
        "AgentId": [0, 1, 0, 1, 0, 1],
        "Value": [0, 10, 1, 11, 2, 12]})

    df = query_executor.add_shifted_column(
        prepared_output, ('Value_prev2', 'Value', 2))
    assert df['Value_prev2'].tolist() == [0, 10, 0, 10, 0, 10]


def test_query_executor__shifting_string_column_with_gap__keeps_gap():
    prepared_output = pd.DataFrame.from_dict({
        "Timestep": [0, 0, 1, 1, 2, 2],
        "AgentId": [0, 1, 0, 1, 0, 1],
        "Value": ["0", "10", "", "11", "2", "12"]})

    df = query_executor.add_shifted_column(
        prepared_output, ('Value_prev1', 'Value', 1))
    results = df['Value_prev1'].tolist()
    assert results == ['0', '10', '0', '10', '', '11']


def test_query_executor__shifting_int_column_with_gap__keeps_gap():
    prepared_output = pd.DataFrame.from_dict({
        "Timestep": [0, 0, 1, 1, 2, 2],
        "AgentId": [0, 1, 0, 1, 0, 1],
        "Value": [0, 10, None, 11, 2, 12]})

    prepared_output.Value = prepared_output.Value.astype('Int64')

    df = query_executor.add_shifted_column(
        prepared_output, ('Value_prev1', 'Value', 1))
    results = df['Value_prev1'].tolist()
    assert results[0:4] + [results[5]] == [0, 10, 0, 10, 11]
    assert pd.isna(results[4])
