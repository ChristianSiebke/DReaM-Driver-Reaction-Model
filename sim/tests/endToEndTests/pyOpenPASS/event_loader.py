################################################################################
# Copyright (c) 2020, 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import re
import config

from os import path

from lxml import etree as et
from pandas import DataFrame, concat
import pandas as pd
import event_parser


class Event:
    def __init__(self):
        self.meta = dict()
        self.triggering = []
        self.affected = []
        self.parameters = dict()

    def eval_triggering_and_affected(self):
        rows = []
        for t in self.triggering:
            row = {'AgentId': int(t),
                   'IsTriggering': True,
                   'IsAffected': True if t in self.affected else False}
            rows.append(row)

        for a in self.affected:
            if a not in self.triggering:
                row = {'AgentId': int(a),
                       'IsTriggering': False,
                       'IsAffected': True}
            rows.append(row)
        return rows

    def get(self):
        rows = []
        for ta in self.eval_triggering_and_affected():
            row = self.meta.copy()
            row.update(ta)
            for k, v in self.parameters.items():
                row.update({'Key': k, 'Value': v})
                rows.append(row.copy())
            if not len(self.parameters):
                row.update({'Key': '', 'Value': ''})
                rows.append(row)

        return rows


def load(file) -> DataFrame:
    """Loads all event data from a simulation output

    :param str file: absolute or relative file path
    :return: pandas DataFrame with event data
    :rtype: DataFrame
    :raises FileNotFoundError: when file does not exist
    """
    if path.exists(file):
        return _load_events(file)
    else:
        raise FileNotFoundError('Input file not found')


def _transform_flat_dict_to_dataframe(dictionary) -> DataFrame:
    """Transform a dictionary into a pandas dataframe

    :param list[dict] or dict dictionary: data as dictionary (row based)
    :return: pandas DataFrame
    :rtype: DataFrame
    """
    if isinstance(dictionary, list):
        return DataFrame(dictionary)
    else:
        return DataFrame(dictionary, index=[0])


def _load_events(simulation_output) -> DataFrame:
    """Loads all event data from a simulation output, without checking file existence

    :param str simulation_output: externally validated file
    :return: DataFrame holding events
    :rtype: DataFrame
    """
    parser = et.XMLParser(ns_clean=True)
    tree = et.parse(simulation_output, parser)
    return _extract_events_from_trace(tree.getroot())


def _extract_events_from_trace(root) -> DataFrame:
    rows = []

    for run_result in root.findall('RunResults/RunResult'):
        for e in _parse_events(run_result):
            rows.append(_transform_flat_dict_to_dataframe(e.get()))

    return concat(rows, ignore_index=True) if len(rows) else pd.DataFrame()


def _parse_events(run_result):
    events = []

    for event in run_result.find('Events'):
        e = Event()
        e.meta = {'RunId': int(run_result.attrib['RunId']),
                  'Timestep': int(event.attrib['Time']),
                  'Source': event.attrib['Source'],
                  'Name': event.attrib['Name']}

        for event_parameter in event.findall('Parameters/Parameter'):
            e.parameters.update(
                {event_parameter.attrib['Key']: event_parameter.attrib['Value']})

        e.triggering = _parse_entities('TriggeringEntities', event)
        e.affected = _parse_entities('AffectedEntities', event)

        events.append(e)

    return events


def _parse_entities(tag, event):
    parsed_entities = []

    for entities in event.findall(tag):
        for entity in entities.findall('Entity'):
            parsed_entities.append(entity.attrib['Id'])
    return parsed_entities


def get_events(file, events):

    if len(events):
        df = load(file)
        config.DEBUG.EXPORT_CSV(df, 'events')
    else:
        return []

    return list(event_parser.parse_regular(df, event) for event in events)
