################################################################################
# Copyright (c) 2020, 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import pandas as pd
import re
import config
import event_parser


def execute_query(data: pd.DataFrame, qpd, run_id):
    result = ''
    config.DEBUG.EXPORT_CSV(data, run_id)

    print(f'run_id: {run_id}')

    print(f"  filter '{qpd.filter}'")
    data.query(qpd.filter, inplace=True)

    lhs = f'data[["{qpd.column}"]].{qpd.group}().to_list()[0]'

    if qpd.operator == '~=':
        lower_value = float(qpd.value) * (1 - 1e-6)
        upper_value = float(qpd.value) * (1 + 1e-6)
        eval_string = f'result > {lower_value} and result < {upper_value}'
    else:
        eval_string = f'result {qpd.operator} {qpd.value}'

    result = ''
    d = {"result": result, 'data': data}

    print(f"  exec   '{lhs}'", end=': ')
    exec(f"result = {lhs}", globals(), d)
    result = d['result']
    print(f"{result}")

    print(f"  eval   '{eval_string}'", end=': ')
    result = eval(eval_string)
    print(f"{result}")
    print('-' * 80)

    return result


def join_events_and_fill(df: pd.DataFrame, events: pd.DataFrame, queried_events):
    df = pd.merge(df, events, on=['Timestep', 'AgentId'], how='outer')

    for column in df.columns:
        for event in queried_events:
            event_colum = f'Event_{event_parser.normalize(event)}'
            if column == event_colum:
                df[column] = df.groupby('AgentId')[
                    column].fillna(method='ffill')
                # only for the event column itself, not it keys
                df[column] = df.groupby('AgentId')[column].fillna(False)
            elif column.startswith(event_colum):
                df[column] = df.groupby('AgentId')[
                    column].fillna(method='ffill')

    # fill default event columns
    for column in df.columns:
        if column in ['IsTriggering', 'IsAffected', 'Source', 'Name']:
            df[column] = df.groupby('AgentId')[column].fillna(method='ffill')

    return df


def add_shifted_column(data, shifted_column):
    (new_column, column, shift) = shifted_column
    data[new_column] = data[column].shift(-shift)

    # fill gaps due to shift (grouped by agend id)
    data[new_column] = data.groupby('AgentId')[new_column]. \
        fillna(method='ffill')

    # copy datatype, as shift might have made int to float (needs NaN for empty values)
    data[new_column] = data[new_column].astype(data[column].dtypes)

    return data


def query_output(data, events, run_id, query):
    if len(events):
        for event in events:
            data = join_events_and_fill(data, event[event.RunId == run_id].drop(
                columns={'RunId'}), query.parsed_events)

    for shifted_column in query.shifted_columns:
        data = add_shifted_column(data, shifted_column)

    for event, event_keys in query.parsed_events.items():
        event_column = f'Event_{event_parser.normalize( event)}'
        if event_column not in data.columns:
            data[event_column] = False
        for event_key in event_keys:
            key_column = f'Event_{event_parser.normalize_column(event_key, event)}'
            if key_column not in data.columns:
                data[key_column] = ''

    return execute_query(data, query.pd, run_id)


def prepare_output(csv_file):
    data = pd.read_csv(csv_file, skipinitialspace=True)

    # reformat columns => '00:TheCol' => 'TheCol:00'
    data.columns = [':'.join(c.split(':')[::-1]) for c in data.columns]

    # extract unique column names for each agent 'TheCol:00' => "TheCol"
    agent_columns = set()
    for pd_column in data.columns:
        c = pd_column.split(':')
        if len(c) > 1:
            agent_columns.add(c[0])

    # convert to long table, based on the indices (= AgentIds) after the separator
    data = pd.wide_to_long(data, i='Timestep', j='AgentId', stubnames=list(agent_columns), sep=':').reset_index()
    data.dropna(how='all', subset=agent_columns, inplace=True)
    return data
