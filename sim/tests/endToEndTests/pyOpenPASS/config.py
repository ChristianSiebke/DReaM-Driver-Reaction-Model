import os
import pandas as pd


class MetaInfo:
    slave = None
    scope = ''
    base_config = None
    specialized = None
    parameterization = None
    configs_under_test = []


class DEBUG:
    PRINT_CSV = True
    CSV_PATH = ('C:\\Temp\\' if os.name == 'nt' else '/tmp/')

    @staticmethod
    def EXPORT_CSV(df, file):
        if DEBUG.PRINT_CSV == True:
            df.to_csv(f'{DEBUG.CSV_PATH}{file}.csv', index=False)
