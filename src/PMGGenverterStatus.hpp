#ifndef POWER_WHISPERPOWER_PMGGENVERTERSTATUS_HPP
#define POWER_WHISPERPOWER_PMGGENVERTERSTATUS_HPP

#include <base/Float.hpp>
#include <base/Temperature.hpp>
#include <base/Time.hpp>
#include <iostream>

using namespace std;

namespace power_whisperpower {
    /** Standard Genverter status data
     */
    struct PMGGenverterStatus {
        base::Time time;

        // ID: 0x200 - Rate 50Hz
        // Words from 0-3 respectively:
        float ac_voltage = base::unknown<float>();
        float ac_current = base::unknown<float>();
        float engine_angular_speed = base::unknown<float>();
        base::Temperature inverter_temperature;

        // ID: 0x201 - Rate 50Hz
        // Word 0:
        enum Status {
            UNKNOWN_STATUS = 0x0,
            GENERATION_ENABLED = 0x1,
            CALIBRATION_ENABLED = 0x2,
            ENGINE_ENABLED = 0x4,
            COOLING_MODE_ENABLED = 0x8,
            STEPPER_ENABLED = 0x10,
            CHARGE_BATTERY_ENABLED = 0x20,
            CHARGE_BATTERY_FLOAT = 0x40
        };

        /** @meta bitfield /power_whisperpower/PMGGenverterStatus/Status */
        uint8_t status = 0;

        // Word 1:
        enum InverterAlarm {
            AC_SHORT_CIRCUIT = 0x1,
            AC_OVER_LOAD = 0x2,
            AC_UNDER_VOLTAGE = 0x4,
            OVER_TEMPERATURE = 0x8,
            LOST_REMOTE_COMMUNICATION = 0x10,
            PARAMETERS_ERROR = 0x20,
            INVERTER_VDRIVERLOW = 0x40,
            ALTERNATOR_MISSING_PHASE = 0x80,
            RPM_OVER_SPEED = 0x100
        };

        /** @meta bitfield /power_whisperpower/PMGGenverterStatus/InverterAlarm */
        uint8_t inverter_alarm = 0;

        // Word 2:
        enum InverterWarning {
            HIGH_AC_CURRENT = 0x1,
            HIGH_RPM = 0x2,
            LOW_RPM = 0x4
        };

        /** @meta bitfield /power_whisperpower/PMGGenverterStatus/InverterWarning */
        uint8_t inverter_warning = 0;

        // Word 3:
        enum EngineAlarm {
            OIL_PRESSURE = 0x1,
            OIL_AUX_PRESSURE = 0x2,
            OIL_TEMPERATURE = 0x4,
            EXHAUST_TEMPERATURE = 0x8,
            START_ENGINE_FAIL = 0x10,
            INESPECT_ENGINE_STOP = 0x20,
            ENGINE_OVER_SPEED = 0x40,
            // Bit 7 - EMPTY
            INVERTER_COMMUNICATION_ERROR = 0x100,
            CAN_COMMUNICATION_ERROR = 0x120
        };

        /** @meta bitfield /power_whisperpower/PMGGenverterStatus/EngineAlarm */
        uint8_t engine_alarm = 0;

        // ID: 0x202 - Rate 50Hz
        // Words 0-3 respectively:
        uint16_t stepper;
        base::Temperature oil_temperature;
        uint16_t delta_dc_bus;
        uint16_t pwm_scale;

        // ID: 0x203 - Rate 50Hz
        // Byte 7 - Increasing ramp for presence
        uint8_t test_ramp;

        // ID: 0x204 - Rate 25Hz
        // Words 0 and 3, bytes from 2-5
        uint16_t inverter_model;
        uint8_t firmware_version;
        uint8_t firmware_subversion;
        uint8_t hardware_version;
        uint8_t hardware_subversion;
        uint16_t inverter_serial_number;
    };

    std::ostream& operator << (std::ostream& io, PMGGenverterStatus const& state);
}

#endif