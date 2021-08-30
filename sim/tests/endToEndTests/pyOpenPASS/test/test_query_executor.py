################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import query_executor
from io import StringIO
from pandas import DataFrame as pd
import pytest

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
    expected_types = pd.from_dict({"some": [1], "other": ['2'], "other2": [
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

