#ifndef POWER_WHISPERPOWER_GENERATORSTATE_HPP
#define POWER_WHISPERPOWER_GENERATORSTATE_HPP

namespace power_whisperpower {
    /**
     * @brief The state of the WhisperPower generator
     *
     */
    enum GeneratorState {
        RUNNING,
        STOPPED,
        STOPPING,
        STARTING,
        FAILURE
    };
}

#endif