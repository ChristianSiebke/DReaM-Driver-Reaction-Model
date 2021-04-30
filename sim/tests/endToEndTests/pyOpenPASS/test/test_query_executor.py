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

csv_test_data = """
    Timestep, 00:some, 00:other, 01:some, 01:other
    0, 1, 2, 3, 4
    1, 5, 6, 7, 8
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
