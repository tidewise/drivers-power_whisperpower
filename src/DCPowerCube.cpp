#include <power_whisperpower/DCPowerCube.hpp>

using namespace power_whisperpower;

DCPowerCube::DCPowerCube(uint8_t device_id)
    : Device(protocol::NODE_GROUP_CHARGER, device_id) {
}

void DCPowerCube::processRead(
    uint16_t object_id, uint8_t object_sub_id, uint8_t const* value
) {
    switch (object_id) {
        case 0x2100:
            m_status.time = base::Time::now();
            m_status.status = value[0];
            zeroGridIfAbsent();
            zeroGeneratorIfAbsent();
            m_status.io_status = value[1] & 0xF;
            m_status.dip_switch = value[2];
            break;
        case 0x2111:
            if (m_status.status & DCPowerCubeStatus::STATUS_GRID_PRESENT) {
                m_status.grid_voltage = protocol::fromBigEndian<uint16_t>(value);
                m_status.grid_current = value[2];
                m_status.grid_current_limit = value[3];
            }
            break;
        case 0x2112: {
            if (m_status.status & DCPowerCubeStatus::STATUS_GENERATOR_PRESENT) {
                m_status.generator_frequency = protocol::fromBigEndian<uint16_t>(value);
                float rpm = protocol::fromBigEndian<uint16_t>(value + 2);
                m_status.generator_rotational_velocity = rpm * 60 * 2 * M_PI;
            }
            break;
        }
        case 0x2113:
            if (m_status.status & DCPowerCubeStatus::STATUS_GENERATOR_PRESENT) {
                m_status.generator_phase_currents[0] = static_cast<float>(value[0]) / 10;
                m_status.generator_phase_currents[1] = static_cast<float>(value[1]) / 10;
                m_status.generator_phase_currents[2] = static_cast<float>(value[2]) / 10;
                m_status.generator_current_limit = static_cast<float>(value[3]) / 10;
            }
            break;
        case 0x2114:
            if (m_status.status & DCPowerCubeStatus::STATUS_GRID_PRESENT) {
                m_status.load_percentage_grid = static_cast<float>(value[0]) / 100;
            }
            if (m_status.status & DCPowerCubeStatus::STATUS_GENERATOR_PRESENT) {
                m_status.load_percentage_generator = static_cast<float>(value[1]) / 100;
            }
            m_status.load_percentage_dc_output = static_cast<float>(value[2]) / 100;
            break;
        case 0x2151:
            m_status.battery_type = static_cast<Status::BatteryType>(value[0]);
            m_status.charging_status = static_cast<Status::ChargingStatus>(value[1]);
            break;
        case 0x2152:
            m_status.dc_output_voltage = static_cast<float>(
                protocol::fromBigEndian<uint16_t>(value)
            ) / 1000;
            m_status.dc_slave_voltage = static_cast<float>(
                protocol::fromBigEndian<uint16_t>(value + 2)
            ) / 1000;
            break;
        case 0x2153:
            m_status.dc_output_current =
                protocol::fromBigEndian<uint16_t>(value);
            m_status.dc_output_current_limit =
                protocol::fromBigEndian<uint16_t>(value + 2);
            break;
        case 0x21A0:
            m_status.temperature_bts = base::Temperature::fromCelsius(
                static_cast<int8_t>(value[0])
            );
            for (int i = 0; i < 3; ++i) {
                m_status.temperature_internal[i] = base::Temperature::fromCelsius(
                    static_cast<int8_t>(value[1 + i])
                );
            }
            m_has_full_update = !m_status.time.isNull();
            break;
        case 0x5100:
            m_config.dc_output_enabled = value[3] & 0x1;
            m_config.force_to_float = value[3] & 0x2;
            break;
        case 0x5101:
            m_config.current_limit_charger = value[0];
            m_config.current_limit_grid = value[1];
            m_config.current_limit_generator = value[2];
            break;
        case 0x5102:
            m_config.charger_current_setpoint = value[0];
            m_config.charger_voltage_setpoint = static_cast<float>(
                protocol::fromBigEndian<uint16_t>(value + 1)
            ) / 1000;
            break;
        default:
            return Device::processRead(object_id, object_sub_id, value);
    }
}

std::vector<uint16_t> DCPowerCube::getConfigMessages() const {
    return std::vector<uint16_t>{ 0x5100, 0x5101, 0x5102 };
}

DCPowerCubeConfig DCPowerCube::getConfig() const {
    return m_config;
}

DCPowerCubeStatus DCPowerCube::getStatus() const {
    return m_status;
}

void DCPowerCube::zeroGridIfAbsent() {
    // "Correct" the status by zeroing grid and/or generator if they are not present
    if (m_status.status & DCPowerCubeStatus::STATUS_GRID_PRESENT) {
        return;
    }

    m_status.grid_current = base::unknown<float>();
    m_status.grid_voltage = base::unknown<float>();
    m_status.grid_current_limit = 0;
    m_status.load_percentage_grid = 0;
}

void DCPowerCube::zeroGeneratorIfAbsent() {
    if (m_status.status & DCPowerCubeStatus::STATUS_GENERATOR_PRESENT) {
        return;
    }

    m_status.generator_frequency = base::unknown<float>();
    m_status.generator_phase_currents[0] = base::unknown<float>();
    m_status.generator_phase_currents[1] = base::unknown<float>();
    m_status.generator_phase_currents[2] = base::unknown<float>();
    m_status.generator_rotational_velocity = base::unknown<float>();
    m_status.generator_current_limit = 0;
    m_status.load_percentage_generator = 0;
}

bool DCPowerCube::hasFullUpdate() const {
    return m_has_full_update;
}

void DCPowerCube::resetFullUpdate() {
    m_status = DCPowerCubeStatus();
    m_status.time = base::Time();
    m_has_full_update = false;
}
