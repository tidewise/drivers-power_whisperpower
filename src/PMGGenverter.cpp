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
            m_status.ac_voltage = static_cast<float>(fromBigEndian<uint16_t>(value)) / 10;
            m_status.ac_current =
                static_cast<float>(fromBigEndian<uint16_t>(value + 2)) / 10;
            m_status.engine_angular_speed =
                static_cast<float>(fromBigEndian<uint16_t>(value + 8)) * 60 * 2 * M_PI;
            m_status.inverter_temperature = base::Temperature::fromCelsius(
                fromBigEndian<uint16_t>(value + 32) / 10.0);
            break;
        case 0x201:
            m_status.status = value[0];
            m_status.inverter_alarm = value[1];
            m_status.inverter_warning = value[2];
            m_status.engine_alarm = value[3];
            break;
        case 0x202:
            m_status.stepper = static_cast<float>(fromBigEndian<uint16_t>(value));
            m_status.oil_temperature =
                base::Temperature::fromCelsius(fromBigEndian<uint16_t>(value + 2) / 10.0);
            m_status.delta_dc_bus =
                static_cast<float>(fromBigEndian<uint16_t>(value + 8));
            m_status.PWM_scale = static_cast<float>(fromBigEndian<uint16_t>(value + 32));
            break;
        case 0x203:
            m_status.TEST_ramp = static_cast<float>(value[7]);
            m_has_full_update = !m_status.time.isNull();
            break;
        case 0x204:
            m_status.inverter_model = fromBigEndian<std::string>(value);
            m_status.firmware_version = value[2];
            m_status.firmware_subversion = value[3];
            m_status.hardware_version = value[4];
            m_status.hardware_subversion = value[5];
            m_status.inverter_serial_number = fromBigEndian<std::string>(value + 64);
            break;
        case 0x205:
            m_status.total_hour_work =
                base::Time::fromSeconds(fromBigEndian<uint16_t>(value) * 3600);
            m_status.total_minute_work =
                base::Time::fromSeconds(fromBigEndian<uint16_t>(value + 2) * 60);
            m_status.maintenance_hour_work =
                base::Time::fromSeconds(fromBigEndian<uint16_t>(value + 8) * 3600);
            m_status.maintenance_minute_work =
                base::Time::fromSeconds(fromBigEndian<uint16_t>(value + 32) * 60);
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