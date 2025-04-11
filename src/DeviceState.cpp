#include "DeviceState.hpp"
#include <power_whisperpower/DeviceState.hpp>

using namespace power_whisperpower;

DeviceState::DeviceState(bool running, bool stopped)
    : running(running)
    , stopped(stopped)
{
}