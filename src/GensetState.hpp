#ifndef POWER_WHISPERPOWER_GENSETSTATE_HPP
#define POWER_WHISPERPOWER_GENSETSTATE_HPP

#include <base/Time.hpp>

namespace power_whisperpower {
    /**
     * @brief The state of the WhisperPower genset
     *
     */
    struct GensetState {
        base::Time time;

        /**
         * @brief The operation stage of the genset
         *
         */
        enum Stage {
            GENSET_STAGE_UNKNOWN,
            GENSET_STAGE_RUNNING,
            GENSET_STAGE_STOPPED,
            GENSET_STAGE_STOPPING,
            GENSET_STAGE_STARTING
        };
        Stage stage{GENSET_STAGE_UNKNOWN};
        /**
         * @brief Indicates when there is a failure in the genset
         *
         */
        bool failure_detected = false;
    };
}

#endif