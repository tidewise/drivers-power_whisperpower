#ifndef POWER_WHISPERPOWER_SMARTSHUNTSTATUS_HPP
#define POWER_WHISPERPOWER_SMARTSHUNTSTATUS_HPP

#include <base/Float.hpp>
#include <base/Temperature.hpp>
#include <base/Time.hpp>

namespace power_whisperpower {
    /** Standard Smart Shunt status data
     */
    struct SmartShuntStatus {
        base::Time time;
        bool relay_overrule_enable[2] = { false, false };
        bool relay_status[2] = { false, false };

        enum AlarmType {
            ALARM_SOC_LOW,
            ALARM_SOC_HIGH,
            ALARM_UNDER_VOLTAGE,
            ALARM_OVER_VOLTAGE,
            ALARM_CHARGE_OVER_CURRENT,
            ALARM_DISCHARGE_OVER_CURRENT
        };
        enum AlarmLevel {
            LEVEL_NONE, LEVEL_WARNING, LEVEL_ALARM
        };
        AlarmLevel alarms[ALARM_DISCHARGE_OVER_CURRENT + 1];

        bool generator_require_signal_enabled = false;
        bool generator_require_status[3] = { false, false, false };

        float battery_voltage = base::unknown<float>();
        float shunt_current = base::unknown<float>();

        /** Percentage of the charge (0 to 1) */
        float battery_charge = base::unknown<float>();
        bool battery_time_remaining_valid = false;
        base::Time battery_time_remaining = base::Time();

        /** Capacity drawn since last full charge, in Ah */
        float capacity_drawn = base::unknown<float>();
        /** Capacity remaining since last full charge, in Ah
         *
         * The smart shunt uses Peukert curves
         */
        float capacity_remaining = base::unknown<float>();

        /** Temperature at the BTS sensor */
        base::Temperature temperature_bts;
    };
}

#endif