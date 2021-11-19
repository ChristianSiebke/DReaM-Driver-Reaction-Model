################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import pandas as pd
import re


def normalize_column(column, event):
    # already prefixed
    normalized_column = f'{event}_' + re.sub(f'^({event}_?)', '', column)
    # special chars
    normalized_column = re.sub(r'[^a-zA-Z1-9_]', '_', normalized_column)
    # duplicate _
    normalized_column = re.sub(r'([_])\1+', r'\1', normalized_column)
    # leading and trailing _
    normalized_column = re.sub(r'^_|_$', '', normalized_column)
    return normalized_column


def normalize(event):
    return normalize_column(event, event)


def parse_regular(events, event_name):
    if len(events.index) == 0:
        return pd.DataFrame(
            columns=['RunId', 'Timestep', 'Source', 'Name', 'AgentId', 'IsTriggering', 'IsAffected',
                     f'Event_{normalize(event_name)}']
        )

    # subset all events of specific type
    df = events[events.Name == event_name].copy()

    # store current columns except the one dropped in the next step
    columns = df.columns.drop(['Key', 'Value'])

    # Merge Key/Value columns into a single Column
    df_tmp = pd.pivot_table(
        df, values='Value', index=df.index, columns='Key', aggfunc='first')

    # Concat with original events
    df = pd.concat([df[columns], df_tmp], axis='columns')

    # New columns (= Key) get prefix 'Event_Name_' and are properly normalized
    new_columns = df.columns.drop(columns)
    mapping = dict()
    for new_column in new_columns:
        mapping.update(
            {new_column: f'Event_{normalize_column(new_column, event_name)}'})
    df.rename(columns=mapping, inplace=True)

    # Add Event_Name to dataframe
    df[f'Event_{normalize(event_name)}'] = True

    return df
