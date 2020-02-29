SET (SOURCES sensor_driver.cpp
			 sensor_driverCalculations.cpp
			 sensor_driverImplementation.cpp)
SET (HEADERS sensor_driver.h
			 sensor_driverCalculations.h
			 sensor_driverImplementation.h
			 sensor_driverGlobal.h
			 Signals/sensorDriverSignal.h
			 Signals/sensor_driverDefinitions.h)

add_definitions(-DSENSOR_DRIVER_LIBRARY)

OPENPASS_add_component(Sensor_Driver "")
