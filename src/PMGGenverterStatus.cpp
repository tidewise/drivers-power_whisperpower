#include <power_whisperpower/PMGGenverterStatus.hpp>

using namespace power_whisperpower;

using std::endl;

std::string to_string(PMGGenverterStatus::InverterAlarm inverter_alarm)
{
    switch (inverter_alarm) {
        case PMGGenverterStatus::AC_SHORT_CIRCUIT:
            return "AC_SHORT_CIRCUIT";
        case PMGGenverterStatus::AC_OVER_LOAD:
            return "AC_OVER_LOAD";
        case PMGGenverterStatus::AC_UNDER_VOLTAGE:
            return "AC_UNDER_VOLTAGE";
        case PMGGenverterStatus::OVER_TEMPERATURE:
            return "OVER_TEMPERATURE";
        case PMGGenverterStatus::LOST_REMOTE_COMMUNICATION:
            return "LOST_REMOTE_COMMUNICATION";
        case PMGGenverterStatus::PARAMETERS_ERROR:
            return "PARAMETERS_ERROR";
        case PMGGenverterStatus::INVERTER_VDRIVERLOW:
            return "INVERTER_VDRIVERLOW";
        case PMGGenverterStatus::ALTERNATOR_MISSING_PHASE:
            return "ALTERNATOR_MISSING_PHASE";
        case PMGGenverterStatus::RPM_OVER_SPEED:
            return "RPM_OVER_SPEED";
    }
    throw std::runtime_error("invalid inverter alarm received");
}

std::string to_string(PMGGenverterStatus::EngineAlarm engine_alarm)
{
    switch (engine_alarm) {
        case PMGGenverterStatus::OIL_PRESSURE:
            return "OIL_PRESSURE";
        case PMGGenverterStatus::OIL_AUX_PRESSURE:
            return "OIL_AUX_PRESSURE";
        case PMGGenverterStatus::OIL_TEMPERATURE:
            return "OIL_TEMPERATURE";
        case PMGGenverterStatus::EXHAUST_TEMPERATURE:
            return "EXHAUST_TEMPERATURE ";
        case PMGGenverterStatus::START_ENGINE_FAIL:
            return "START_ENGINE_FAIL ";
        case PMGGenverterStatus::INESPECT_ENGINE_STOP:
            return "INESPECT_ENGINE_STOP ";
        case PMGGenverterStatus::ENGINE_OVER_SPEED:
            return "ENGINE_OVER_SPEED ";
        case PMGGenverterStatus::INVERTER_COMMUNICATION_ERROR:
            return "INVERTER_COMMUNICATION_ERROR ";
        case PMGGenverterStatus::CAN_COMMUNICATION_ERROR:
            return "CAN_COMMUNICATION_ERROR ";
    }
    throw std::runtime_error("invalid inverter alarm received");
}

std::string to_string(PMGGenverterStatus::InverterWarning inverter_warning)
{
    switch (inverter_warning) {
        case PMGGenverterStatus::HIGH_AC_CURRENT:
            return "HIGH_AC_CURRENT";
        case PMGGenverterStatus::HIGH_RPM:
            return "HIGH_RPM";
        case PMGGenverterStatus::LOW_RPM:
            return "LOW_RPM";
    }
    throw std::runtime_error("invalid inverter warning received");
}

#define FLAG_OUT(field, flag)                                                            \
    if (field & power_whisperpower::PMGGenverterStatus::flag) {                                              \
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
    int rpm = round(state.engine_rpm / 60 / 2 / M_PI);
    io << "Engine RPM: " << rpm << " RPM\n";
    io << "Temperature: " << state.inverter_temperature << " Celsius\n";
    io << "Stepper: " << state.stepper << "CNT \n";
    io << "Oil Temperature: " << state.oil_temperature << " Celsius\n";
    io << "Delta DC Bus: " << state.delta_dc_bus << " CNT\n";
    io << "PWM Scale: " << state.PWM_scale << "CNT\n";
    io << "TEST Ramp: " << state.TEST_ramp << "\n";
    io << "Inverter Model: " << state.inverter_model << "\n";
    io << "Firmware Version: " << state.firmware_version << "\n";
    io << "Firmware Sub Version: " << state.firmware_subversion << "\n";
    io << "Hardware Version: " << state.hardware_version << "\n";
    io << "Hardware Sub Version: " << state.hardware_subversion << "\n";
    io << "Inverter Serial Number: " << state.inverter_serial_number << "\n";
    io << "Total Hour Work: " << state.total_hour_work << "\n";
    io << "Total Minute Work: " << state.total_minute_work << "\n";
    io << "Maintenance Hour Work: " << state.maintenance_hour_work << "\n";
    io << "Maintenance Minute Work: " << state.maintenance_minute_work << "\n";

    return io;
}
