rock_gtest(test_suite
   SOURCES suite.cpp test_Protocol.cpp test_Device.cpp Helpers.cpp
                     test_DCPowerCube.cpp test_SmartShunt.cpp
                     test_PMGGenverter.cpp
   DEPS power_whisperpower)
