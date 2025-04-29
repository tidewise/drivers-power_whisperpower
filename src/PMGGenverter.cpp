#include <power_whisperpower/PMGGenverter.hpp>

using namespace power_whisperpower;
using protocol::fromBigEndian;

PMGGenverter::PMGGenverter()
{
}

void PMGGenverter::process(canbus::Message const& msg)
{
    switch (msg.can_id) {
        case 0x200:
            m_status.time = base::Time::now();
            m_status.ac_voltage =
                static_cast<float>(fromBigEndian<uint16_t>(msg.data)) / 10;
            m_status.ac_current =
                static_cast<float>(fromBigEndian<uint16_t>(msg.data + 2)) / 10;
            m_status.engine_angular_speed =
                static_cast<float>(fromBigEndian<uint16_t>(msg.data + 4)) * 2 * M_PI / 60;
            m_status.inverter_temperature = base::Temperature::fromCelsius(
                fromBigEndian<uint16_t>(msg.data + 6) / 10.0);
            break;
        case 0x201:
            m_status.status = fromBigEndian<uint16_t>(msg.data);
            m_status.inverter_alarm = fromBigEndian<uint16_t>(msg.data + 2);
            m_status.inverter_warning = fromBigEndian<uint16_t>(msg.data + 4);
            m_status.engine_alarm = fromBigEndian<uint16_t>(msg.data + 6);
            break;
        case 0x202:
            m_status.stepper = fromBigEndian<uint16_t>(msg.data);
            m_status.oil_temperature = base::Temperature::fromCelsius(
                fromBigEndian<uint16_t>(msg.data + 2) / 10.0);
            m_status.delta_dc_bus = fromBigEndian<uint16_t>(msg.data + 4);
            m_status.pwm_scale = fromBigEndian<uint16_t>(msg.data + 6);
            break;
        case 0x203:
            m_status.test_ramp = msg.data[7];
            break;
        case 0x204:
            m_status.inverter_model = fromBigEndian<uint16_t>(msg.data);
            m_status.firmware_version = msg.data[2];
            m_status.firmware_subversion = msg.data[3];
            m_status.hardware_version = msg.data[4];
            m_status.hardware_subversion = msg.data[5];
            m_status.inverter_serial_number = fromBigEndian<uint16_t>(msg.data + 6);
            break;
        case 0x205:
            uint16_t total_hours = fromBigEndian<uint16_t>(msg.data);
            uint16_t total_minutes = fromBigEndian<uint16_t>(msg.data + 2);
            uint16_t maintenance_hours = fromBigEndian<uint16_t>(msg.data + 4);
            uint16_t maintenance_minutes = fromBigEndian<uint16_t>(msg.data + 6);
            m_run_time_state.time = base::Time::now();
            m_run_time_state.total =
                base::Time::fromSeconds((total_hours * 3600) + (total_minutes * 60));
            m_run_time_state.since_last_maintenance = base::Time::fromSeconds(
                (maintenance_hours * 3600) + (maintenance_minutes * 60));
            m_has_full_update = true;
            break;
    }
}

PMGGenverterStatus PMGGenverter::getStatus() const
{
    return m_status;
}

RunTimeState PMGGenverter::getRunTimeState() const
{
    return m_run_time_state;
}

bool PMGGenverter::hasFullUpdate() const
{
    return m_has_full_update;
}

void PMGGenverter::resetFullUpdate()
{
    m_status = PMGGenverterStatus();
    m_status.time = base::Time();
    m_has_full_update = false;
}

canbus::Message PMGGenverter::queryGeneratorCommand(bool start, bool stop)
{
    canbus::Message msg;
    msg.time = base::Time::now();
    msg.can_id = 0x210;
    msg.size = 8;
    msg.data[0] = start;
    msg.data[1] = stop;

    for (int i = 2; i < 7; i++) {
        msg.data[i] = 0;
    }

    m_run_ramp += 1;
    msg.data[7] = m_run_ramp;

    return msg;
}