rock_library(power_whisperpower
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(power_whisperpower_bin Main.cpp
    DEPS power_whisperpower)

