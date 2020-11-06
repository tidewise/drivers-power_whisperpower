#ifndef POWER_WHISPERPOWER_DCPOWERCUBECONFIG_HPP
#define POWER_WHISPERPOWER_DCPOWERCUBECONFIG_HPP

#include <cstdint>
#include <iosfwd>

namespace power_whisperpower {
    /** DC Power Cube control registers
     */
    struct DCPowerCubeConfig {
        bool dc_output_enabled = false;
        bool force_to_float = false;
        float current_limit_charger;
        float current_limit_grid;
        float current_limit_generator;
        float charger_current_setpoint;
        float charger_voltage_setpoint;
    };

    std::ostream& operator << (std::ostream& io, DCPowerCubeConfig const& config);
}

#endif