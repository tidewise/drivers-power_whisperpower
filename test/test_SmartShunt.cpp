#include <gtest/gtest.h>
#include "Helpers.hpp"
#include <power_whisperpower/SmartShunt.hpp>

using namespace std;
using namespace power_whisperpower;
typedef SmartShuntStatus Status;

struct SmartShuntTest : public ::testing::Test, Helpers {
    SmartShunt shunt;

    SmartShuntTest()
        : shunt(2) {
    }

    void ALARM_TEST(array<uint8_t, 4> data, int alarm_index);
    void processReadReply(int msg_id, std::array<uint8_t, 4> data) {
        canbus::Message msg = makeReadReply(0x12, msg_id, 0, data);
        shunt.process(msg);
    }
};

TEST_F(SmartShuntTest, it_reads_the_relay_1_overrule_flag) {
    processReadReply(0x2100, { 0, 1, 0, 0 });
    ASSERT_TRUE(shunt.getStatus().relay_overrule_enable[0]);
    ASSERT_FALSE(shunt.getStatus().relay_overrule_enable[1]);
    ASSERT_FALSE(shunt.getStatus().relay_status[0]);
    ASSERT_FALSE(shunt.getStatus().relay_status[1]);
}

TEST_F(SmartShuntTest, it_reads_the_relay_2_overrule_flag) {
    processReadReply(0x2100, { 0, 2, 0, 0 });
    ASSERT_FALSE(shunt.getStatus().relay_overrule_enable[0]);
    ASSERT_TRUE(shunt.getStatus().relay_overrule_enable[1]);
    ASSERT_FALSE(shunt.getStatus().relay_status[0]);
    ASSERT_FALSE(shunt.getStatus().relay_status[1]);
}

TEST_F(SmartShuntTest, it_reads_the_relay_1_status_flag) {
    processReadReply(0x2100, { 0, 0, 0, 1 });
    ASSERT_FALSE(shunt.getStatus().relay_overrule_enable[0]);
    ASSERT_FALSE(shunt.getStatus().relay_overrule_enable[1]);
    ASSERT_TRUE(shunt.getStatus().relay_status[0]);
    ASSERT_FALSE(shunt.getStatus().relay_status[1]);
}

TEST_F(SmartShuntTest, it_reads_the_relay_2_status_flag) {
    processReadReply(0x2100, { 0, 0, 0, 2 });
    ASSERT_FALSE(shunt.getStatus().relay_overrule_enable[0]);
    ASSERT_FALSE(shunt.getStatus().relay_overrule_enable[1]);
    ASSERT_FALSE(shunt.getStatus().relay_status[0]);
    ASSERT_TRUE(shunt.getStatus().relay_status[1]);
}

void ASSERT_ALARMS_EQ(SmartShuntStatus const& status,
                      std::array<Status::AlarmLevel, 6> expected) {
    array<Status::AlarmLevel, 6> actual;
    copy(status.alarms, status.alarms + 6, actual.begin());
    ASSERT_EQ(expected, actual);
}

/** Factored out test for alarms, since there are so many of them */
void SmartShuntTest::ALARM_TEST(array<uint8_t, 4> data, int alarm_index) {
    array<Status::AlarmLevel, 6> expected =
        { Status::LEVEL_NONE, Status::LEVEL_NONE, Status::LEVEL_NONE,
          Status::LEVEL_NONE, Status::LEVEL_NONE, Status::LEVEL_NONE };

    auto can_data = data;
    expected[alarm_index] = Status::LEVEL_WARNING;
    processReadReply(0x2101, can_data);
    ASSERT_ALARMS_EQ(shunt.getStatus(), expected);

    expected[alarm_index] = Status::LEVEL_ALARM;
    can_data = data;
    for (auto& v : can_data) {
        v *= 2;
    }
    processReadReply(0x2101, can_data);
    ASSERT_ALARMS_EQ(shunt.getStatus(), expected);

    expected[alarm_index] = Status::LEVEL_ALARM;
    can_data = data;
    for (auto& v : can_data) {
        v *= 3;
    }
    processReadReply(0x2101, can_data);
    ASSERT_ALARMS_EQ(shunt.getStatus(), expected);
}

TEST_F(SmartShuntTest, it_reads_the_soc_low_alarm) {
    ALARM_TEST({ 0, 0, 1, 0 }, Status::ALARM_SOC_LOW);
}

TEST_F(SmartShuntTest, it_reads_the_soc_high_alarm) {
    ALARM_TEST({ 0, 0, 4, 0 }, Status::ALARM_SOC_HIGH);
}

TEST_F(SmartShuntTest, it_reads_the_under_voltage_alarm) {
    ALARM_TEST({ 0, 0, 0, 1 }, Status::ALARM_UNDER_VOLTAGE);
}

TEST_F(SmartShuntTest, it_reads_the_over_voltage_alarm) {
    ALARM_TEST({ 0, 0, 0, 4 }, Status::ALARM_OVER_VOLTAGE);
}

TEST_F(SmartShuntTest, it_reads_the_charge_over_current_alarm) {
    ALARM_TEST({ 0, 0, 0, 16 }, Status::ALARM_CHARGE_OVER_CURRENT);
}

TEST_F(SmartShuntTest, it_reads_the_discharge_over_current_alarm) {
    ALARM_TEST({ 0, 0, 0, 64 }, Status::ALARM_DISCHARGE_OVER_CURRENT);
}

TEST_F(SmartShuntTest, it_reads_generator_require_enabled_flag) {
    processReadReply(0x2102, { 0, 0, 0, 0 });
    ASSERT_FALSE(shunt.getStatus().generator_require_signal_enabled);
    processReadReply(0x2102, { 1, 0, 0, 0 });
    ASSERT_TRUE(shunt.getStatus().generator_require_signal_enabled);
}

TEST_F(SmartShuntTest, it_reads_generator_require_status) {
    processReadReply(0x2102, { 0, 0, 0, 1 });
    ASSERT_TRUE(shunt.getStatus().generator_require_status[0]);
    ASSERT_FALSE(shunt.getStatus().generator_require_status[1]);
    ASSERT_FALSE(shunt.getStatus().generator_require_status[2]);
    processReadReply(0x2102, { 0, 0, 0, 2 });
    ASSERT_FALSE(shunt.getStatus().generator_require_status[0]);
    ASSERT_TRUE(shunt.getStatus().generator_require_status[1]);
    ASSERT_FALSE(shunt.getStatus().generator_require_status[2]);
    processReadReply(0x2102, { 0, 0, 0, 4 });
    ASSERT_FALSE(shunt.getStatus().generator_require_status[0]);
    ASSERT_FALSE(shunt.getStatus().generator_require_status[1]);
    ASSERT_TRUE(shunt.getStatus().generator_require_status[2]);
}

TEST_F(SmartShuntTest, it_reads_the_battery_voltage_and_shunt_current) {
    processReadReply(0x2111, { 0x12, 0x34, 0x56, 0x78 });
    ASSERT_FLOAT_EQ(4.66, shunt.getStatus().battery_voltage);
    ASSERT_FLOAT_EQ(2213.6, shunt.getStatus().shunt_current);
}

TEST_F(SmartShuntTest, it_reads_negative_shunt_currents) {
    processReadReply(0x2111, { 0x12, 0x34, 0xff, 0x50 });
    ASSERT_FLOAT_EQ(-17.6, shunt.getStatus().shunt_current);
}

TEST_F(SmartShuntTest, it_reads_battery_soc_and_time_remaining) {
    processReadReply(0x2112, { 0x12, 0x34, 0x56, 0x78 });
    ASSERT_FLOAT_EQ(0.18, shunt.getStatus().battery_charge);
    ASSERT_TRUE(shunt.getStatus().battery_time_remaining_valid);
    ASSERT_FLOAT_EQ(803880, shunt.getStatus().battery_time_remaining.toSeconds());
}

TEST_F(SmartShuntTest, it_handles_unknown_time_remaining) {
    processReadReply(0x2112, { 0x12, 0xff, 0xff, 0x78 });
    ASSERT_FALSE(shunt.getStatus().battery_time_remaining_valid);
    ASSERT_TRUE(shunt.getStatus().battery_time_remaining.isNull());
}

TEST_F(SmartShuntTest, it_reads_capacity_drawn_and_remaining) {
    processReadReply(0x2113, { 0x12, 0x34, 0x56, 0x78 });
    ASSERT_FLOAT_EQ(466, shunt.getStatus().capacity_drawn);
    ASSERT_FLOAT_EQ(2213.6, shunt.getStatus().capacity_remaining);
}

TEST_F(SmartShuntTest, it_reads_the_bts_temperature_sensor) {
    processReadReply(0x21A0, { 0x12, 0x34, 0x56, 0x78 });
    ASSERT_FLOAT_EQ(18, shunt.getStatus().temperature_bts.getCelsius());
}

TEST_F(SmartShuntTest, it_handles_negative_bts_temperatures) {
    processReadReply(0x21A0, { 0xf2, 0x34, 0x56, 0x78 });
    ASSERT_FLOAT_EQ(-14, shunt.getStatus().temperature_bts.getCelsius());
}

TEST_F(SmartShuntTest, it_reports_a_full_update_after_receiving_the_first_and_last_message) {
    processReadReply(0x2100, { 0, 0, 0, 0 });
    processReadReply(0x21A0, { 0, 0, 0, 0 });
    ASSERT_TRUE(shunt.hasFullUpdate());
}

TEST_F(SmartShuntTest, it_reports_a_partial_update_if_the_last_message_is_received_but_the_first_was_never) {
    processReadReply(0x21A0, { 0, 0, 0, 0 });
    ASSERT_FALSE(shunt.hasFullUpdate());
}

TEST_F(SmartShuntTest, it_reports_a_partial_update_after_a_reset_if_the_first_message_is_not_received_again) {
    processReadReply(0x2100, { 0, 0, 0, 0 });
    processReadReply(0x21A0, { 0, 0, 0, 0 });
    shunt.resetFullUpdate();
    processReadReply(0x21A0, { 0, 0, 0, 0 });
    ASSERT_FALSE(shunt.hasFullUpdate());
    processReadReply(0x2100, { 0, 0, 0, 0 });
    ASSERT_FALSE(shunt.hasFullUpdate());
    processReadReply(0x21A0, { 0, 0, 0, 0 });
    ASSERT_TRUE(shunt.hasFullUpdate());
}
