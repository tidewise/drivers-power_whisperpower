#include <power_whisperpower/PMGGenverterStatus.hpp>

using namespace power_whisperpower;

using std::endl;

#define FLAG_OUT(field, flag)                                                            \
    if (field & power_whisperpower::PMGGenverterStatus::flag) {                          \
        io << "  " #flag "\n";                                                           \
    }

std::ostream& power_whisperpower::operator<<(std::ostream& io,
    power_whisperpower::PMGGenverterStatus const& state)
{
    io << "Time: " << state.time << "\n"
       << "State flags:\n";

    FLAG_OUT(state.status, UNKNOWN_STATUS);
    FLAG_OUT(state.status, GENERATION_ENABLED);
    FLAG_OUT(state.status, CALIBRATION_ENABLED);
    FLAG_OUT(state.status, ENGINE_ENABLED);
    FLAG_OUT(state.status, COOLING_MODE_ENABLED);
    FLAG_OUT(state.status, STEPPER_ENABLED);
    FLAG_OUT(state.status, CHARGE_BATTERY_ENABLED);
    FLAG_OUT(state.status, CHARGE_BATTERY_FLOAT);

    io << "AC Voltage: " << state.ac_voltage << " V\n";
    io << "AC Current: " << state.ac_current << " A\n";
    io << "Engine angular speed: " << state.engine_angular_speed << " rad/s\n";
    io << "Temperature: " << state.inverter_temperature << "\n";
    io << "Stepper: " << state.stepper << " CNT\n";
    io << "Oil Temperature: " << state.oil_temperature << "\n";
    io << "Delta DC Bus: " << state.delta_dc_bus << " CNT\n";
    io << "PWM Scale: " << state.PWM_scale << " CNT\n";
    io << "TEST Ramp: " << state.TEST_ramp << "\n";
    io << "Inverter Model: " << state.inverter_model << "\n";
    io << "Firmware Version: " << static_cast<int>(state.firmware_version) << "\n";
    io << "Firmware Sub Version: " << static_cast<int>(state.firmware_subversion) << "\n";
    io << "Hardware Version: " << static_cast<int>(state.hardware_version) << "\n";
    io << "Hardware Sub Version: " << static_cast<int>(state.hardware_subversion) << "\n";
    io << "Inverter Serial Number: " << state.inverter_serial_number << "\n";
    io << "Total Work: " << state.total_work << "\n";
    io << "Maintenance Work: " << state.maintenance_work << "\n";

    return io;
}
