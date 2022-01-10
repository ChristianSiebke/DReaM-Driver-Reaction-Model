################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from query_parser import Query


def test_query_parser_simple():
    q = Query('count(aggregate | some > 1) = 0')
    assert(q.parsed == 'count(aggregate | some > 1) = 0')


def test_query_parser_base_event():
    q1 = Query(
        'count(aggregate | some > 1 and #(event) == True) = 0')
    assert(q1.parsed == 'count(aggregate | some > 1 and Event_event == True) = 0')

    q2 = Query(
        'count(aggregate | some > 1 and #(event)==True) = 0')
    assert(q2.parsed == 'count(aggregate | some > 1 and Event_event==True) = 0')


def test_query_parser_property_event():
    q = Query(
        'count(aggregate | some > 1 and #(event) == True and #(event):prop == False) = 0')
    assert(q.parsed == 'count(aggregate | some > 1 and Event_event == True and Event_event_prop == False) = 0')


def test_query_parser_property_event_only():
    q1 = Query(
        "count(aggregate | some == 0 and #(event):prop == 'someval') > 0")
    assert(q1.parsed ==
           "count(aggregate | some == 0 and Event_event_prop == 'someval') > 0")

    q2 = Query(
        "count(aggregate | some == 0 and #(event):prop=='someval') > 0")
    assert(q2.parsed ==
           "count(aggregate | some == 0 and Event_event_prop=='someval') > 0")


def test_query_parser_time_shift():
    q = Query(
        "count(aggregate | some == 0 and some-2 == 1) > 0")
    assert(q.parsed == "count(aggregate | some == 0 and some_prev2 == 1) > 0")
    assert(q.shifted_columns == [("some_prev2", "some", 2)])


def test_query_parser_time_shift_not_applied_to_quoted_strings():
    q = Query(
        "count(aggregate | some == 0 and 'some-2' == 1) > 0")
    assert(q.parsed == "count(aggregate | some == 0 and 'some-2' == 1) > 0")
    assert(len(q.shifted_columns) == 0)


def test_query_parser_time_shift_with_underscore_and_number_in_name():
    q = Query(
        "count(aggregate | some0_test == 0 and some0_test-1 == 1) > 0")
    assert(q.shifted_columns == [("some0_test_prev1", "some0_test", 1)])


def test_query_parser_quote_filter_elements_containing_dash():
    q = Query(
        "count(aggregate | name-with_dash == 0) == 0")
    assert(q.pd.filter == "`name-with_dash` == 0")


def test_query_parser_quote_filter_elements_containing_dashes():
    q = Query(
        "count(aggregate | name-with-dashes == 0) == 0")
    assert(q.pd.filter == "`name-with-dashes` == 0")


def test_query_parser_column_containing_dash():
    q = Query(
        "count(aggregate-0 | some == 0) == 0")
    assert(q.pd.column == "aggregate-0")


def test_query_parser_column_containing_dashes():
    q = Query(
        "count(aggregate-0-A | some == 0) == 0")
    assert(q.pd.column == "aggregate-0-A")


def test_query_pandas_factorizer():
    q = Query(
        "count(aggregate | #(event) == True and #(event):prop == False or some == 0 and some_prev1 == 1) > 123")
    assert(q.pd.group == "count")
    assert(q.pd.filter ==
           "Event_event == True and Event_event_prop == False or some == 0 and some_prev1 == 1")
    assert(q.pd.operator == ">")
    assert(q.pd.value == "123")
