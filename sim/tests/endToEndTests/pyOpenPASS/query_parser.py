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
from event_parser import normalize, normalize_column


class Query:
    class Pd:
        def __init__(self, parsed_query):
            match = re.match(
                r'([a-z]+)\s?\(\s?([A-Za-z0-9]+)\s?\|\s?(.*)\)\s?([~><=!]=?)\s([0-9\.]+)', parsed_query)

            self.group = match.group(1)
            self.column = match.group(2)
            self.filter = match.group(3)
            self.operator = match.group(4)
            self.value = match.group(5)

    def __init__(self, query):
        self.raw_query = query
        self.parsed_query = ''
        self.parsed_events = dict()
        self.shifted_columns = list()
        self.parse(query)
        self.pd = self.Pd(self.parsed_query)

    def parse(self, raw_query):

        query_for_events = r'#\(([A-Za-z0-9_ \/]+)\)(?:\:([^\s]+))?(?:.*?#\(\1\)\:([^\s]+))*?'
        matches = re.findall(query_for_events, raw_query)

        for key, value, _ in matches:
            if len(value):
                split = re.search(r'\s?([=<>!]|no|is|contains)', value)
                if split is not None:
                    value = value[:split.start(1)]

                try:
                    self.parsed_events[key].add(value)
                except KeyError:
                    self.parsed_events[key] = {value}
            else:
                self.parsed_events[key] = set()

        for event, parameters in self.parsed_events.items():
            for parameter in parameters:
                raw_query = raw_query.replace(
                    f'#({event}):{parameter}', f'Event_{normalize_column(parameter, event)}')
            raw_query = raw_query.replace(
                f'#({event})', f'Event_{normalize(event)}')

        matches = set(re.findall(r'(([A-Za-z\d_]+)-(\d+))', raw_query))

        for match, column, shift in matches:
            new_column = f'{column}_prev{shift}'
            self.shifted_columns.append((new_column, column, int(shift)))
            raw_query = raw_query.replace(match, new_column)

        self.parsed_query = raw_query

    @property
    def parsed(self):
        return self.parsed_query

    @property
    def events(self):
        return self.parsed_events.keys()
