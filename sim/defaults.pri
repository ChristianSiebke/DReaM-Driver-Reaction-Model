#/*******************************************************************************
#* Copyright (c) 2019 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/
message("***** PARSING defaults.pri *****")

win32 {
    isEmpty(DESTDIR) {
        DESTDIR_SLAVE="C:\OpenPASS\bin"
        DESTDIR_TESTS="C:\OpenPASS\bin\tests"
    } else {
        DESTDIR_SLAVE=$$DESTDIR
        DESTDIR_TESTS=$$DESTDIR
    }

    isEmpty(DESTDIR_GUI){
        DESTDIR_GUI = "C:\OpenPASS\bin"
    }

    isEmpty(EXTRA_INCLUDE_PATH) {
        EXTRA_INCLUDE_PATH="C:\OpenPASS\thirdParty\include"
    }

    isEmpty(EXTRA_LIB_PATH) {
        EXTRA_LIB_PATH="C:\OpenPASS\thirdParty\lib"
    }
}

unix {
    isEmpty(DESTDIR) {
        DESTDIR_SLAVE="/OpenPASS/bin"
        DESTDIR_TESTS="/OpenPASS/bin/tests"
    } else {
        DESTDIR_SLAVE=$$DESTDIR
        DESTDIR_TESTS=$$DESTDIR
    }

    isEmpty(DESTDIR_GUI){
        DESTDIR_GUI = "/OpenPASS/bin"
    }

    isEmpty(EXTRA_INCLUDE_PATH) {
        EXTRA_INCLUDE_PATH="/OpenPASS/thirdParty/include"
    }

    isEmpty(EXTRA_LIB_PATH) {
        EXTRA_LIB_PATH=/OpenPASS/thirdParty/lib
    }
}
