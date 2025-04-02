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

        // ID: 0x205 - Rate 25Hz
        // Words 0-3
        base::Time total_work;
        base::Time maintenance_work;
    };

    std::ostream& operator<<(std::ostream& io, RunTimeState const& state);
}

#endif