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
                static_cast<float>(fromBigEndian<uint16_t>(msg.data + 4)) * 60 * 2 * M_PI;
            m_status.inverter_temperature = base::Temperature::fromCelsius(
                fromBigEndian<uint16_t>(msg.data + 6) / 10.0);
            break;
        case 0x201:
            m_status.status = fromBigEndian<uint16_t>(msg.data + 0);
            m_status.inverter_alarm = fromBigEndian<uint16_t>(msg.data + 2);
            m_status.inverter_warning = fromBigEndian<uint16_t>(msg.data + 4);
            m_status.engine_alarm = fromBigEndian<uint16_t>(msg.data + 6);
            break;
        case 0x202:
            m_status.stepper = static_cast<float>(fromBigEndian<uint16_t>(msg.data));
            m_status.oil_temperature = base::Temperature::fromCelsius(
                fromBigEndian<uint16_t>(msg.data + 2) / 10.0);
            m_status.delta_dc_bus =
                static_cast<float>(fromBigEndian<uint16_t>(msg.data + 4));
            m_status.PWM_scale =
                static_cast<float>(fromBigEndian<uint16_t>(msg.data + 6));
            break;
        case 0x203:
            m_status.TEST_ramp = msg.data[7];
            break;
        case 0x204:
            m_status.inverter_model = static_cast<int>(fromBigEndian<uint16_t>(msg.data));
            m_status.firmware_version = static_cast<int>(msg.data[2]);
            m_status.firmware_subversion = static_cast<int>(msg.data[3]);
            m_status.hardware_version = static_cast<int>(msg.data[4]);
            m_status.hardware_subversion = static_cast<int>(msg.data[5]);
            m_status.inverter_serial_number =
                static_cast<int>(fromBigEndian<uint16_t>(msg.data + 6));
            break;
        case 0x205:
            m_status.total_work =
                base::Time::fromSeconds(fromBigEndian<uint16_t>(msg.data));
            m_status.maintenance_work =
                base::Time::fromSeconds(fromBigEndian<uint16_t>(msg.data + 4) * 3600);
            m_has_full_update = true;
            break;
    }
}

PMGGenverterStatus PMGGenverter::getStatus() const
{
    return m_status;
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
    msg.can_id = 0x210;
    msg.size = 8;
    msg.data[0] = start;
    msg.data[1] = stop;

    for (int i = 2; i < 7; i++) {
        msg.data[i] = 0;
    }

    m_run_ramp = (m_run_ramp + 1) % 256;
    msg.data[7] = m_run_ramp;

    return msg;
}