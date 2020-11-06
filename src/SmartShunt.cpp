#include <power_whisperpower/SmartShunt.hpp>

using namespace power_whisperpower;
using protocol::fromBigEndian;

SmartShunt::SmartShunt(uint8_t device_id)
    : Device(protocol::NODE_GROUP_BATTERY_MONITOR, device_id) {
}

SmartShuntStatus::AlarmLevel parseAlarm(int8_t field, int offset) {
    int flags = field >> offset;
    if (flags & 0x2) {
        return SmartShuntStatus::LEVEL_ALARM;
    }
    else if (flags & 0x1) {
        return SmartShuntStatus::LEVEL_WARNING;
    }
    else {
        return SmartShuntStatus::LEVEL_NONE;
    }
}

void SmartShunt::processRead(
    uint16_t object_id, uint8_t object_sub_id, uint8_t const* value
) {
    switch (object_id) {
        case 0x2100:
            m_status.time = base::Time::now();
            m_status.relay_overrule_enable[0] = value[1] & 0x1;
            m_status.relay_overrule_enable[1] = value[1] & 0x2;
            m_status.relay_status[0] = value[3] & 0x1;
            m_status.relay_status[1] = value[3] & 0x2;
            break;
        case 0x2101:
            m_status.alarms[Status::ALARM_SOC_LOW] = parseAlarm(value[2], 0);
            m_status.alarms[Status::ALARM_SOC_HIGH] = parseAlarm(value[2], 2);
            m_status.alarms[Status::ALARM_UNDER_VOLTAGE] = parseAlarm(value[3], 0);
            m_status.alarms[Status::ALARM_OVER_VOLTAGE] = parseAlarm(value[3], 2);
            m_status.alarms[Status::ALARM_CHARGE_OVER_CURRENT] = parseAlarm(value[3], 4);
            m_status.alarms[Status::ALARM_DISCHARGE_OVER_CURRENT] = parseAlarm(value[3], 6);
            break;
        case 0x2102:
            m_status.generator_require_signal_enabled = value[0] & 0x1;
            m_status.generator_require_status[0] = value[3] & 0x1;
            m_status.generator_require_status[1] = value[3] & 0x2;
            m_status.generator_require_status[2] = value[3] & 0x4;
            break;
        case 0x2111:
            m_status.battery_voltage =
                static_cast<float>(fromBigEndian<uint16_t>(value)) / 1000;
            m_status.shunt_current =
                static_cast<float>(fromBigEndian<int16_t>(value + 2)) / 10;
            break;
        case 0x2112: {
            m_status.battery_charge = static_cast<float>(value[0]) / 100;
            int16_t minutes = fromBigEndian<int16_t>(value + 1);
            bool valid = minutes > 0;
            m_status.battery_time_remaining_valid = valid;
            m_status.battery_time_remaining =
                valid ? base::Time::fromSeconds(minutes * 60)
                      : base::Time();
            break;
        }
        case 0x2113:
            m_status.capacity_drawn = static_cast<float>(
                fromBigEndian<uint16_t>(value)
            ) / 10;
            m_status.capacity_remaining = static_cast<float>(
                fromBigEndian<uint16_t>(value + 2)
            ) / 10;
            break;
        case 0x21A0:
            m_status.temperature_bts = base::Temperature::fromCelsius(
                fromBigEndian<int8_t>(value)
            );
            m_has_full_update = !m_status.time.isNull();
            break;
        default:
            return Device::processRead(object_id, object_sub_id, value);
    }
}

SmartShuntStatus SmartShunt::getStatus() const {
    return m_status;
}

bool SmartShunt::hasFullUpdate() const {
    return m_has_full_update;
}

void SmartShunt::resetFullUpdate() {
    m_status = SmartShuntStatus();
    m_status.time = base::Time();
    m_has_full_update = false;
}
