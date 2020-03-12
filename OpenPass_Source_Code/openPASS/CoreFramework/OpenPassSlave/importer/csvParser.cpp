/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "csvParser.h"

void CSVParser::ParseFile(const std::string& file)
{
    std::ifstream infile(file.c_str());
    std::string line;

    while (std::getline(infile, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        std::vector<std::string> lineData = ParseLine(line);
        table.push_back(lineData);
    }
}

std::vector<std::string> CSVParser::ParseLine(const std::string& line)
{
    std::vector<std::string> lineData;
    std::stringstream ss;
    ss.str(line);
    std::string item;

    while (std::getline(ss, item, ';'))
    {
        lineData.push_back(item);
    }

    return lineData;
}

size_t CSVParser::GetNumberOfLines() const
{
    return table.size();
}

std::string CSVParser::GetEntryAt(size_t row, size_t column) const
{
    try
    {
        return table.at(row).at(column);
    }
    catch (const std::out_of_range&)
    {
        return "";
    }
}
