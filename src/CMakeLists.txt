rock_library(power_whisperpower
    SOURCES Device.cpp Protocol.cpp
            DCPowerCube.cpp DCPowerCubeStatus.cpp DCPowerCubeConfig.cpp
            SmartShunt.cpp
    HEADERS Device.hpp Protocol.hpp
            DCPowerCube.hpp DCPowerCubeStatus.hpp DCPowerCubeConfig.hpp
            SmartShunt.hpp SmartShuntStatus.hpp
    DEPS_PKGCONFIG base-types canbus)

rock_executable(power_whisperpower_ctl
    SOURCES Main.cpp
    HEADERS
    DEPS power_whisperpower
    DEPS_PKGCONFIG canbus)
