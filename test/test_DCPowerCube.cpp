#include <gtest/gtest.h>
#include "Helpers.hpp"
#include <power_whisperpower/DCPowerCube.hpp>

using namespace power_whisperpower;

struct DCPowerCubeTest : public ::testing::Test, Helpers {
    DCPowerCube cube;

    DCPowerCubeTest()
        : cube(2) {
    }
};

TEST_F(DCPowerCubeTest, it_passes_the_common_messages_to_the_base_class) {
    canbus::Message msg = makeReadReply(
        0x32, 0x1018, 0, { 0x89, 0xAB, 0xCD, 0xEF }
    );
    cube.process(msg);
    ASSERT_EQ(0x89ABCDEF, cube.getSerialNumber());
}

TEST_F(DCPowerCubeTest, it_reads_the_status_bytes) {
    canbus::Message msg = makeReadReply(
        0x32, 0x2100, 0, { 0b10101010, 0b01010101, 0b11001100 }
    );
    cube.process(msg);
    ASSERT_EQ(0b10101010, cube.getStatus().status);
    ASSERT_EQ(0b0101, cube.getStatus().io_status);
    ASSERT_EQ(0b11001100, cube.getStatus().dip_switch);
}

TEST_F(DCPowerCubeTest, it_reads_grid_voltage_and_current) {
    canbus::Message msg = makeReadReply(0x32, 0x2111, 0, { 0x12, 0x34, 0x56, 0x78 });
    cube.process(msg);
    ASSERT_FLOAT_EQ(0x1234, cube.getStatus().grid_voltage);
    ASSERT_FLOAT_EQ(0x56, cube.getStatus().grid_current);
    ASSERT_FLOAT_EQ(0x78, cube.getStatus().grid_current_limit);
}

TEST_F(DCPowerCubeTest, it_reads_generator_frequency_and_speed) {
    canbus::Message msg = makeReadReply(0x32, 0x2112, 0, { 0x12, 0x34, 0x56, 0x78 });
    cube.process(msg);
    ASSERT_FLOAT_EQ(0x1234, cube.getStatus().generator_frequency);
    ASSERT_FLOAT_EQ(
        static_cast<float>(0x5678) * 60 * M_PI * 2,
        cube.getStatus().generator_rotational_velocity
    );
}

TEST_F(DCPowerCubeTest, it_reads_generator_currents) {
    canbus::Message msg = makeReadReply(0x32, 0x2113, 0, { 0x12, 0x34, 0x56, 0x78 });
    cube.process(msg);
    ASSERT_FLOAT_EQ(static_cast<float>(0x12) / 10,
                    cube.getStatus().generator_phase_currents[0]);
    ASSERT_FLOAT_EQ(static_cast<float>(0x34) / 10,
                    cube.getStatus().generator_phase_currents[1]);
    ASSERT_FLOAT_EQ(static_cast<float>(0x56) / 10,
                    cube.getStatus().generator_phase_currents[2]);
    ASSERT_FLOAT_EQ(static_cast<float>(0x78) / 10,
                    cube.getStatus().generator_current_limit);
}

TEST_F(DCPowerCubeTest, it_reads_load_percentages) {
    canbus::Message msg = makeReadReply(0x32, 0x2114, 0, { 0x12, 0x34, 0x56, 0x78 });
    cube.process(msg);
    ASSERT_FLOAT_EQ(static_cast<float>(0x12) / 100,
                    cube.getStatus().load_percentage_grid);
    ASSERT_FLOAT_EQ(static_cast<float>(0x34) / 100,
                    cube.getStatus().load_percentage_generator);
    ASSERT_FLOAT_EQ(static_cast<float>(0x56) / 100,
                    cube.getStatus().load_percentage_dc_output);
}

TEST_F(DCPowerCubeTest, it_reads_battery_type_and_charger_state) {
    canbus::Message msg = makeReadReply(0x32, 0x2151, 0, { 3, 4, 0, 0 });
    cube.process(msg);
    ASSERT_EQ(3, cube.getStatus().battery_type);
    ASSERT_EQ(4, cube.getStatus().charging_status);
}

TEST_F(DCPowerCubeTest, it_reads_the_output_voltages) {
    canbus::Message msg = makeReadReply(0x32, 0x2152, 0, { 0x12, 0x34, 0x56, 0x78 });
    cube.process(msg);
    ASSERT_FLOAT_EQ(4.66f, cube.getStatus().dc_output_voltage);
    ASSERT_FLOAT_EQ(22.136f, cube.getStatus().dc_slave_voltage);
}

TEST_F(DCPowerCubeTest, it_reads_the_output_current) {
    canbus::Message msg = makeReadReply(0x32, 0x2153, 0, { 0x12, 0x34, 0x56, 0x78 });
    cube.process(msg);
    ASSERT_FLOAT_EQ(4660, cube.getStatus().dc_output_current);
    ASSERT_FLOAT_EQ(22136, cube.getStatus().dc_output_current_limit);
}

TEST_F(DCPowerCubeTest, it_reads_the_temperatures) {
    canbus::Message msg = makeReadReply(0x32, 0x21A0, 0, { 10, 236, 120, 206 });
    cube.process(msg);
    ASSERT_FLOAT_EQ(10, cube.getStatus().temperature_bts.getCelsius());
    ASSERT_FLOAT_EQ(-20, cube.getStatus().temperature_internal[0].getCelsius());
    ASSERT_FLOAT_EQ(120, cube.getStatus().temperature_internal[1].getCelsius());
    ASSERT_FLOAT_EQ(-50, cube.getStatus().temperature_internal[2].getCelsius());
}

TEST_F(DCPowerCubeTest, it_reports_a_full_update_after_receiving_the_first_and_last_message) {
    canbus::Message msgs[] = {
        makeReadReply(0x32, 0x2100, 0, { 0, 0, 0, 0 }),
        makeReadReply(0x32, 0x21A0, 0, { 10, 236, 120, 206 })
    };
    cube.process(msgs[0]);
    cube.process(msgs[1]);
    ASSERT_TRUE(cube.hasFullUpdate());
}

TEST_F(DCPowerCubeTest, it_reports_a_partial_update_after_a_reset_if_the_first_message_is_not_received_again) {
    canbus::Message msgs[] = {
        makeReadReply(0x32, 0x2100, 0, { 0, 0, 0, 0 }),
        makeReadReply(0x32, 0x21A0, 0, { 10, 236, 120, 206 })
    };
    cube.process(msgs[0]);
    cube.process(msgs[1]);
    cube.resetFullUpdate();
    cube.process(msgs[1]);
    ASSERT_FALSE(cube.hasFullUpdate());
    cube.process(msgs[0]);
    ASSERT_FALSE(cube.hasFullUpdate());
    cube.process(msgs[1]);
    ASSERT_TRUE(cube.hasFullUpdate());
}

TEST_F(DCPowerCubeTest, it_reports_a_partial_update_if_the_last_message_is_received_but_the_first_was_never) {
    canbus::Message msg = makeReadReply(0x32, 0x21A0, 0, { 10, 236, 120, 206 });
    cube.process(msg);
    ASSERT_FALSE(cube.hasFullUpdate());
}

TEST_F(DCPowerCubeTest, it_parses_a_control_message_with_force_to_float_set) {
    canbus::Message msg = makeReadReply(0x32, 0x5100, 0, { 0, 0, 0, 0x2 });
    cube.process(msg);
    ASSERT_FALSE(cube.getConfig().dc_output_enabled);
    ASSERT_TRUE(cube.getConfig().force_to_float);
}

TEST_F(DCPowerCubeTest, it_parses_a_control_message_with_dc_output_on) {
    canbus::Message msg = makeReadReply(0x32, 0x5100, 0, { 0, 0, 0, 0x1 });
    cube.process(msg);
    ASSERT_TRUE(cube.getConfig().dc_output_enabled);
    ASSERT_FALSE(cube.getConfig().force_to_float);
}

TEST_F(DCPowerCubeTest, it_parses_a_control_message_with_both_dc_output_and_force_to_float_set) {
    canbus::Message msg = makeReadReply(0x32, 0x5100, 0, { 0, 0, 0, 0x3 });
    cube.process(msg);
    ASSERT_TRUE(cube.getConfig().dc_output_enabled);
    ASSERT_TRUE(cube.getConfig().force_to_float);
}

TEST_F(DCPowerCubeTest, it_parses_a_control_message_with_neither_dc_output_nor_force_to_float_set) {
    canbus::Message msg = makeReadReply(0x32, 0x5100, 0, { 0, 0, 0, 0x0 });
    cube.process(msg);
    ASSERT_FALSE(cube.getConfig().dc_output_enabled);
    ASSERT_FALSE(cube.getConfig().force_to_float);
}

TEST_F(DCPowerCubeTest, it_parses_the_current_limit_control_message) {
    canbus::Message msg = makeReadReply(0x32, 0x5101, 0, { 40, 10, 5, 0 });
    cube.process(msg);
    ASSERT_EQ(40, cube.getConfig().current_limit_charger);
    ASSERT_EQ(10, cube.getConfig().current_limit_grid);
    ASSERT_EQ(5, cube.getConfig().current_limit_generator);
}

TEST_F(DCPowerCubeTest, it_parses_the_charger_setpoints) {
    canbus::Message msg = makeReadReply(0x32, 0x5102, 0, { 40, 0x1, 0x23, 0 });
    cube.process(msg);
    ASSERT_EQ(40, cube.getConfig().charger_current_setpoint);
    ASSERT_EQ(static_cast<float>(0x123) / 1000, cube.getConfig().charger_voltage_setpoint);
}
