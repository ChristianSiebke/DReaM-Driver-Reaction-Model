/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <vector>
#include <string>

class CSVParser
{
public:
    CSVParser() = default;
    ~CSVParser() = default;

    void ParseFile(const std::string& file);

    size_t GetNumberOfLines() const;

    std::string GetEntryAt(size_t row, size_t column) const;

private:
    std::vector<std::string> ParseLine(const std::string& line);
    std::vector<std::vector<std::string>> table;
};
