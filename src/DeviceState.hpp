#ifndef POWER_WHISPERPOWER_DEVICESTATE_HPP
#define POWER_WHISPERPOWER_DEVICESTATE_HPP

namespace power_whisperpower {
    /**
     * @brief The state of the WhisperPower device
     *
     */
    struct DeviceState {
        DeviceState() = default;
        DeviceState(bool running, bool stopped);
        bool running = false;
        bool stopped = false;
    };
}

#endif