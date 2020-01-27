rock_library(power_whisperpower
    SOURCES Device.cpp Protocol.cpp DCPowerCube.cpp
    HEADERS Device.hpp Protocol.hpp DCPowerCube.hpp DCPowerCubeState.hpp
    DEPS_PKGCONFIG base-types canbus)

