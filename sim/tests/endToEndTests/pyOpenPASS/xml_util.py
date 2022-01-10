################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

import logging
import sys
import os.path

from lxml import etree

class XmlInfo:
    """
    Packs an xml file and the corresponding xpath for file manipulation
    """
    xml_file = None
    xpath_expr = None

    def __init__(self, xml_file, xpath_expr):
        self.xml_file = xml_file
        self.xpath_expr = xpath_expr

class XmlSetter:
    """
    Packs an xml file manipulation info and the corresponding value
    """
    xml_info = None
    value = None

    def __init__(self, xml_info: XmlInfo, value):
        self.xml_info = xml_info
        self.value = str(value)


def terminate_program(message):
    logging.error(message)
    sys.exit(message)


def get_file_or_default(config_under_test, file):
    """If avilable, returns the file from the current config path, else from the default path"""
    in_config_folder = os.path.join(config_under_test.base_path, config_under_test.name, file)
    if os.path.isfile(in_config_folder):
        return in_config_folder

    in_default_folder = os.path.join(config_under_test.default_path, file)
    if os.path.isfile(in_default_folder):
        return in_default_folder

    terminate_program(
        f'No file "{file}" in "{config_under_test.base_path}" or "{config_under_test.default_path}"')


def set_node(base_path, xml_setter: XmlSetter):
    """ Sets the node given by the (xml_file, xpath_expr) to value """
    xml_file = os.path.join(base_path, xml_setter.xml_info.xml_file)
    value = xml_setter.value.replace('${configFolder}', base_path)

    from lxml import etree
    tree = etree.parse(xml_file)
    expr = xml_setter.xml_info.xpath_expr.split('/@')
    try:
        node = tree.xpath(expr[0])[0]
    except IndexError:
        terminate_program(
            f'Could not update {xml_file} given xpath "{xml_setter.xml_info.xpath_expr}"')
    else:
        if len(expr) == 1:
            node.text = value
        else:
            node.attrib[expr[1]] = value

    with open(xml_file, 'w') as f:
        f.write(etree.tostring(tree, encoding="unicode", pretty_print=True))


class XmlUtil:
    """
    Utilily for xml manipulation w.r.t. openPASS configurations
    Also defines some manipulation presets starting with 'CONFIG_*'
    """
    CONFIG_RANDOM_SEED = XmlInfo('simulationConfig.xml', '//Experiment/RandomSeed')
    CONFIG_NUMBER_OF_INVOCATIONS = XmlInfo('simulationConfig.xml', '//Experiment/NumberOfInvocations')
    CONFIG_LOGGING_TO_CSV = XmlInfo('simulationConfig.xml', "//Bool[@Key='LoggingCyclicsToCsv']/@Value")
    CONFIG_DURATION = XmlInfo('Scenario.xosc', "//StopTrigger/ConditionGroup/Condition[@name='EndTime']/ByValueCondition/SimulationTimeCondition/@value")

    @staticmethod
    def get_as_int(config_under_test, xml_info: XmlInfo):
        """"Retrieves the node, given by the XPATH expression as integer"""
        xml_file = get_file_or_default(config_under_test, xml_info.xml_file)

        from lxml import etree
        tree = etree.parse(xml_file)
        expr = xml_info.xpath_expr.split('/@')
        try:
            node = tree.xpath(expr[0])[0]
        except IndexError:
            terminate_program(
                f'Could not update {xml_file} given xpath "{xml_info.xpath_expr}"')
        else:
            if len(expr) == 1:
                return int(node.text)
            else:
                return int(node.attrib[expr[1]])

    @staticmethod
    def update(configs_path, base_config):
        """Updates configuration files to log into CSV and set the values defined by the given dictionary

        Keyword arguments:
        configs_path -- Path to config set, which shall be manipulated
        base_config  -- "baseConfig" part of the json file
                        Mandatory:  invocations, duration
                        Optional:   random seed
        """
        set_node(configs_path, XmlSetter(XmlUtil.CONFIG_LOGGING_TO_CSV, 'true'))
        set_node(configs_path, XmlSetter(XmlUtil.CONFIG_NUMBER_OF_INVOCATIONS, base_config["invocations"]))
        if 'randomSeed' in base_config.keys():
            set_node(configs_path, XmlSetter(XmlUtil.CONFIG_RANDOM_SEED, base_config['randomSeed']))
        set_node(configs_path, XmlSetter(XmlUtil.CONFIG_DURATION, base_config["duration"]))

    @staticmethod
    def custom_update(configs_path, xml_setter: XmlSetter):
        """Updates a single configuration files based on the given xml_setter

        Keyword arguments:
        configs_path -- Path to config set, which shall be manipulated
        xml_setter   -- A valid xml setter
        """

        set_node(configs_path, xml_setter)
