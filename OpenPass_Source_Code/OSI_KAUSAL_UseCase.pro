TEMPLATE = subdirs
SUBDIRS = \
    openPASS/OpenPass_OSI.pro \
    openPASS/Components/Algorithm_ModularDriver \
    openPASS/Components/Sensor_Modular_Driver \
    openPASS/Components/Sensor_Criticality

include(defaults.pri)

DIR_BIN = $$DESTDIR_SLAVE
DIR_PRO = $${PWD}

win32 {
DIR_PRO ~= s,/,\\,g
DIR_BIN ~= s,/,\\,g
copydata.commands = xcopy $$DIR_PRO\openPASS_Resource\OSI_KAUSAL_UseCase $$DIR_BIN /S /D
} else {
copydata.commands = cp -ru $$DIR_PRO/openPASS_Resource/OSI_KAUSAL_UseCase $$DIR_BIN
}
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
