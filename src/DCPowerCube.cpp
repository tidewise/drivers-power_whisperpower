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
            m_state.time = base::Time::now();
            m_state.status = value[0];
            m_state.io_status = value[1];
            m_state.dip_switch = value[2];
            break;
        case 0x2111:
            m_state.grid_voltage = protocol::fromBigEndian<uint16_t>(value);
            m_state.grid_current = value[2];
            m_state.grid_current_limit = value[3];
            break;
        case 0x2112:
            m_state.generator_frequency = protocol::fromBigEndian<uint16_t>(value);
            m_state.generator_rpm = protocol::fromBigEndian<uint16_t>(value + 2);
            break;
        case 0x2113:
            m_state.generator_phase_currents[0] = static_cast<float>(value[0]) / 10;
            m_state.generator_phase_currents[1] = static_cast<float>(value[1]) / 10;
            m_state.generator_phase_currents[2] = static_cast<float>(value[2]) / 10;
            m_state.generator_current_limit = static_cast<float>(value[3]) / 10;
            break;
        case 0x2114:
            m_state.load_percentage_grid = static_cast<float>(value[0]) / 100;
            m_state.load_percentage_generator = static_cast<float>(value[1]) / 100;
            m_state.load_percentage_dc_output = static_cast<float>(value[2]) / 100;
            break;
        case 0x2151:
            m_state.battery_type = static_cast<State::BatteryType>(value[0]);
            m_state.charging_state = static_cast<State::ChargingState>(value[1]);
            break;
        case 0x2152:
            m_state.dc_output_voltage = static_cast<float>(
                protocol::fromBigEndian<uint16_t>(value)
            ) / 1000;
            m_state.dc_slave_voltage = static_cast<float>(
                protocol::fromBigEndian<uint16_t>(value + 2)
            ) / 1000;
            break;
        case 0x2153:
            m_state.dc_output_current =
                protocol::fromBigEndian<uint16_t>(value);
            m_state.dc_output_current_limit =
                protocol::fromBigEndian<uint16_t>(value + 2);
            break;
        case 0x21A0:
            m_state.temperature_bts = base::Temperature::fromCelsius(
                static_cast<int8_t>(value[0])
            );
            for (int i = 0; i < 3; ++i) {
                m_state.temperature_internal[i] = base::Temperature::fromCelsius(
                    static_cast<int8_t>(value[1 + i])
                );
            }
            m_has_full_update = true;
            break;
    }
}

DCPowerCubeState DCPowerCube::getState() const {
    return m_state;
}

bool DCPowerCube::hasFullUpdate() const {
    return !m_state.time.isNull() && m_has_full_update;
}

void DCPowerCube::resetFullUpdate() {
    m_state.time = base::Time();
    m_has_full_update = false;
}

