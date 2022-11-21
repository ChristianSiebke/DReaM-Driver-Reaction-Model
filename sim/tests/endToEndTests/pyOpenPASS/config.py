################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import os
import pandas as pd


class MetaInfo:
    simulation = None
    scope = ''
    base_config = None
    specialized = None
    parameterization = None
    configs_under_test = []
    datatypes = {}

class DEBUG:
    PRINT_CSV = True
    CSV_PATH = ('C:\\Temp\\' if os.name == 'nt' else '/tmp/')

    @staticmethod
    def EXPORT_CSV(df, file):
        if DEBUG.PRINT_CSV == True:
            df.to_csv(f'{DEBUG.CSV_PATH}{file}.csv', index=False)
