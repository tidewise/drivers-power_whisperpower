#include <power_whisperpower/DCPowerCubeConfig.hpp>
#include <iostream>

using namespace power_whisperpower;

std::ostream& power_whisperpower::operator << (
    std::ostream& io, DCPowerCubeConfig const& config
) {
    io << "DC Output: " << (config.dc_output_enabled ? "ENABLED" : "DISABLED") << "\n"
       << "Force to Float: " << (config.force_to_float ? "ENABLED" : "DISABLED") << "\n"
       << "Current Limits:\n"
       << "  Charger: " << config.current_limit_charger << "A\n"
       << "  Grid: " << config.current_limit_grid << "A\n"
       << "  Generator: " << config.current_limit_generator << "A\n"
       << "Charger Setpoints:\n"
       << "  Current: " << config.charger_current_setpoint << "A\n"
       << "  Voltage: " << config.charger_voltage_setpoint << "V\n"
       << std::flush;
    return io;
}