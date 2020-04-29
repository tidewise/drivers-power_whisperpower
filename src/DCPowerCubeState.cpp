#include <power_whisperpower/DCPowerCubeState.hpp>

using namespace power_whisperpower;

using std::endl;

std::string to_string(DCPowerCubeState::ChargingState state) {
    switch(state) {
        case DCPowerCubeState::CHARGING_UNKNOWN:
            return "UNKNOWN";
        case DCPowerCubeState::CHARGING_BULK:
            return "BULK";
        case DCPowerCubeState::CHARGING_20_40:
            return "20_40";
        case DCPowerCubeState::CHARGING_ABSORPTION:
            return "ABSORPTION";
        case DCPowerCubeState::CHARGING_60_80:
            return "60_80";
        case DCPowerCubeState::CHARGING_FLOAT:
            return "FLOAT";
        case DCPowerCubeState::CHARGING_MAINTENANCE:
            return "MAINTENANCE";
    }
    throw std::runtime_error("invalid charging state received");
}

std::string to_string(DCPowerCubeState::BatteryType battery) {
    switch(battery) {
        case DCPowerCubeState::BATTERY_UNKNOWN:
            return "UNKNOWN";
        case DCPowerCubeState::BATTERY_LEAD_ACID:
            return "LEAD_ACID";
        case DCPowerCubeState::BATTERY_AGM_GEL:
            return "AGM_GEL";
        case DCPowerCubeState::BATTERY_TRACTION:
            return "TRACTION";
        case DCPowerCubeState::BATTERY_LI_ION:
            return "LI_ION";
        case DCPowerCubeState::BATTERY_USER:
            return "USER";
    }
    throw std::runtime_error("invalid charging state received");
}

#define FLAG_OUT(field, flag) \
    if (field & DCPowerCubeState::flag) { \
        io << "  " #flag "\n"; \
    }

std::ostream& power_whisperpower::operator << (std::ostream& io, DCPowerCubeState const& state) {
    io << "Time: " << state.time << "\n"
       << "State flags:\n";

    FLAG_OUT(state.status, STATUS_POWER_LIMIT);
    FLAG_OUT(state.status, STATUS_TEMPERATURE_COMPENSATION);
    FLAG_OUT(state.status, STATUS_DIODE_COMPENSATION);
    FLAG_OUT(state.status, STATUS_FAILURE);
    FLAG_OUT(state.status, STATUS_OVER_VOLTAGE);
    FLAG_OUT(state.status, STATUS_OVER_TEMPERATURE);
    FLAG_OUT(state.status, STATUS_GRID_PRESENT);
    FLAG_OUT(state.status, STATUS_GENERATOR_PRESENT);
    FLAG_OUT(state.io_status, DIO_IN1);
    FLAG_OUT(state.io_status, DIO_IN2);
    FLAG_OUT(state.io_status, DIO_OUT1);
    FLAG_OUT(state.io_status, DIO_OUT2);

    io << "DIP Switches: ";
    for (int i = 0; i < 8; ++i) {
        if (state.dip_switch & (1 << i)) {
            io << " " << i;
        }
    }
    io << "\n";

    io << "Grid: " << state.grid_voltage << "V " << state.grid_current << "A\n";
    io << "Generator: " << state.generator_frequency << "Hz " << state.generator_rpm << "rpm\n";
    io << "Generator Phase Currents: "
        << state.generator_phase_currents[0] << "A "
        << state.generator_phase_currents[1] << "A "
        << state.generator_phase_currents[2] << "A\n";

    io << "Current Limits:\n"
       << "  Grid: " << state.generator_current_limit << "A\n" 
       << "  Generator: " << state.generator_current_limit << "A\n";

    io << "Load Percentages:\n"
       << "  Grid: " << state.load_percentage_grid * 100 << "%\n" 
       << "  Generator: " << state.load_percentage_generator * 100 << "%\n"
       << "  DC Output: " << state.load_percentage_dc_output * 100 << "%\n";

    io << "Battery Type: " << to_string(state.battery_type) << "\n";
    io << "Charging State: " << to_string(state.charging_state) << "\n";

    io << "DC Output: " << state.dc_output_voltage << "V " << state.dc_output_current << "A\n";
    io << "DC Current Limit: " << state.dc_output_current_limit << "A\n";

    io << "Temperatures:\n"
       << "  BTS: " << state.temperature_bts << "\n"
       << "  internal[0]: " << state.temperature_internal[0] << "\n"
       << "  internal[1]: " << state.temperature_internal[1] << "\n"
       << "  internal[2]: " << state.temperature_internal[2];

    return io;
}

