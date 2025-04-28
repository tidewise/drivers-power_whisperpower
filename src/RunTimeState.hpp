#ifndef POWER_WHISPERPOWER_RUNTIMESTATE_HPP
#define POWER_WHISPERPOWER_RUNTIMESTATE_HPP

#include <base/Time.hpp>
#include <iostream>

using namespace std;

namespace power_whisperpower {
    /** Standard Genverter run time state data
     */
    struct RunTimeState {
        base::Time time;

        /** The run time since the last maintenance */
        base::Time total_run_time;
        /** The run time since commissioning of the generator */
        base::Time historical_run_time;
    };

    std::ostream& operator<<(std::ostream& io, RunTimeState const& state);
}

#endif