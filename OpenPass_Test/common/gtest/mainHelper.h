/********************************************************
* Copyright (c) 2019 in-tech GmbH on behalf of BMW AG
*********************************************************/

#pragma once

/*!
 * \brief Sets a default XML test result filename for gtest
 *
 * If the command line parameters to the test executable contain
 * the element `--default-xml`, the gtest `output` parameter is set
 * to write an XML file. The filename will be based on the test
 * executable's name.
 *
 * The argument `--default-xml` is removed from the argument list
 * before returning.
 *
 * \param[inout] argc   Command line arguments count
 * \param[inout] argv   Command line arguments values
 */
void SetDefaultXmlOutput(int* argc, char **argv);
