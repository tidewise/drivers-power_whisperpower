#ifndef POWER_WHISPERPOWER_DCPOWERCUBESTATE_HPP
#define POWER_WHISPERPOWER_DCPOWERCUBESTATE_HPP

#include <cstdint>
#include <base/Time.hpp>
#include <base/Float.hpp>
#include <base/Temperature.hpp>

namespace power_whisperpower {
    struct DCPowerCubeState {
        base::Time time;

        enum Status {
            STATUS_POWER_LIMIT = 0x01,
            STATUS_TEMPERATURE_COMPENSATION = 0x02,
            STATUS_DIODE_COMPENSATION = 0x04,
            STATUS_FAILURE = 0x08,
            STATUS_OVER_VOLTAGE = 0x10,
            STATUS_OVER_TEMPERATURE = 0x20,
            STATUS_GRID_PRESENT = 0x40,
            STATUS_GENERATOR_PRESENT = 0x80
        };

        /** @meta bitfield /power_whisperpower/DCPowerCubeState/Status */
        uint8_t status;

        enum DigitalIO {
            DIO_IN1 = 0x1,
            DIO_IN2 = 0x2,
            DIO_OUT1 = 0x4,
            DIO_OUT2 = 0x8
        };

        /** @meta bitfield /power_whisperpower/DCPowerCubeState/DigitalIO */
        uint8_t io_status;

        uint8_t dip_switch = 0;

        float grid_voltage = base::unknown<float>();
        float grid_current = base::unknown<float>();
        float grid_current_limit = base::unknown<float>();

        float generator_frequency = base::unknown<float>();
        float generator_rotational_velocity = base::unknown<float>();

        float generator_phase_currents[3] = {
            base::unknown<float>(),
            base::unknown<float>(),
            base::unknown<float>()
        };
        float generator_current_limit = base::unknown<float>();

        float load_percentage_grid = base::unknown<float>();
        float load_percentage_generator = base::unknown<float>();
        float load_percentage_dc_output = base::unknown<float>();

        enum BatteryType {
            BATTERY_UNKNOWN = -1,
            BATTERY_LEAD_ACID = 0,
            BATTERY_AGM_GEL = 1,
            BATTERY_TRACTION = 2,
            BATTERY_LI_ION = 3,
            BATTERY_USER = 4
        };

        enum ChargingState {
            CHARGING_UNKNOWN = -1,
            CHARGING_BULK = 1,
            CHARGING_20_40 = 2,
            CHARGING_ABSORPTION = 3,
            CHARGING_60_80 = 4,
            CHARGING_FLOAT = 5,
            CHARGING_MAINTENANCE = 6
        };

        BatteryType battery_type = BATTERY_UNKNOWN;
        ChargingState charging_state = CHARGING_UNKNOWN;

        float dc_output_voltage = base::unknown<float>();
        float dc_slave_voltage = base::unknown<float>();
        float dc_output_current = base::unknown<float>();
        float dc_output_current_limit = base::unknown<float>();

        base::Temperature temperature_bts;
        base::Temperature temperature_internal[3];
    };

    std::ostream& operator << (std::ostream& io, DCPowerCubeState const& state);
}

#endif
