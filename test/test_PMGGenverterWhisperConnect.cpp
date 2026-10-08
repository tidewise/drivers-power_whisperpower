#include "base/Temperature.hpp"
#include "base/Time.hpp"
#include "canbus/Message.hpp"
#include "power_whisperpower/PMGGenverter.hpp"
#include "power_whisperpower/PMGGenverterStatus.hpp"
#include "power_whisperpower/Protocol.hpp"
#include <gtest/gtest.h>

using namespace power_whisperpower;

struct PMGGenverterWhisperConnectTest : public ::testing::Test {
    PMGGenverter genverter;
    canbus::Message msg;

    PMGGenverterWhisperConnectTest()
        : genverter(WHISPER_CONNECT)
    {
    }

    void setMsgData(uint16_t index, uint32_t data)
    {
        msg.can_id = protocol::FUNCTION_SEND + protocol::NODE_GROUP_GENERATOR;
        msg.size = 8;
        msg.data[0] = protocol::COMMAND_SLAVE_READ_SUCCESSFUL;
        msg.data[1] = (index >> 8) & 0xFF;
        msg.data[2] = index & 0xFF;
        msg.data[3] = 0; // Sub-index
        msg.data[4] = (data >> 24) & 0xFF;
        msg.data[5] = (data >> 16) & 0xFF;
        msg.data[6] = (data >> 8) & 0xFF;
        msg.data[7] = data & 0xFF;
    }
};

TEST_F(PMGGenverterWhisperConnectTest, it_processes_ac_voltage_0x2110)
{
    setMsgData(0x2110, 230000); // 230.0V * 1000
    genverter.process(msg);
    PMGGenverterStatus status = genverter.getStatus();
    ASSERT_NEAR(status.ac_voltage, 230.0, 1e-2);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_ac_current_0x2111)
{
    setMsgData(0x2111, 15500); // 15.5A * 1000
    genverter.process(msg);
    PMGGenverterStatus status = genverter.getStatus();
    ASSERT_NEAR(status.ac_current, 15.5, 1e-2);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_engine_angular_speed_0x2115)
{
    setMsgData(0x2115, 3000); // 3000 RPM
    genverter.process(msg);
    ASSERT_NEAR(genverter.getStatus().engine_angular_speed, 314.159, 1e-2);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_inverter_temperature_0x21A0)
{
    setMsgData(0x21A0, 45000); // 45.0C * 1000
    genverter.process(msg);
    ASSERT_NEAR(genverter.getStatus().inverter_temperature.getCelsius(), 45.0, 1e-2);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_status_0x2100)
{
    uint32_t val = PMGGenverterStatus::GENERATION_ENABLED | PMGGenverterStatus::ENGINE_ENABLED | PMGGenverterStatus::CHARGE_BATTERY_FLOAT;
    setMsgData(0x2100, val);
    genverter.process(msg);
    ASSERT_EQ(genverter.getStatus().status, val);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_inverter_alarm_0x2101)
{
    uint32_t val = PMGGenverterStatus::AC_SHORT_CIRCUIT | PMGGenverterStatus::OVER_TEMPERATURE | PMGGenverterStatus::RPM_OVER_SPEED;
    setMsgData(0x2101, val);
    genverter.process(msg);
    ASSERT_EQ(genverter.getStatus().inverter_alarm, val);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_inverter_warning_0x2102)
{
    uint32_t val = PMGGenverterStatus::HIGH_AC_CURRENT | PMGGenverterStatus::LOW_RPM;
    setMsgData(0x2102, val);
    genverter.process(msg);
    ASSERT_EQ(genverter.getStatus().inverter_warning, val);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_engine_alarm_0x2103)
{
    uint32_t val = PMGGenverterStatus::OIL_PRESSURE | PMGGenverterStatus::EXHAUST_TEMPERATURE | PMGGenverterStatus::CAN_COMMUNICATION_ERROR;
    setMsgData(0x2103, val);
    genverter.process(msg);
    ASSERT_EQ(genverter.getStatus().engine_alarm, val);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_stepper_0x2122)
{
    setMsgData(0x2122, 100);
    genverter.process(msg);
    ASSERT_EQ(genverter.getStatus().stepper, 100);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_oil_temperature_0x21A1)
{
    setMsgData(0x21A1, 85000); // 85.0C * 1000
    genverter.process(msg);
    ASSERT_NEAR(genverter.getStatus().oil_temperature.getCelsius(), 85.0, 1e-2);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_pwm_scale_0x2121)
{
    setMsgData(0x2121, 5);
    genverter.process(msg);
    ASSERT_EQ(genverter.getStatus().pwm_scale, 5);
}

TEST_F(PMGGenverterWhisperConnectTest, it_processes_total_runtime_0x2132)
{
    setMsgData(0x2132, 600); // 600 minutes
    genverter.process(msg);
    ASSERT_EQ(genverter.getRunTimeState().total.toSeconds(), 36000);
}

TEST_F(PMGGenverterWhisperConnectTest,
    it_processes_since_last_maintenance_0x2130_and_has_full_update)
{
    ASSERT_FALSE(genverter.hasFullUpdate());
    setMsgData(0x2130, 300); // 300 minutes
    genverter.process(msg);
    ASSERT_EQ(genverter.getRunTimeState().since_last_maintenance.toSeconds(), 18000);
    ASSERT_TRUE(genverter.hasFullUpdate());
}

TEST_F(PMGGenverterWhisperConnectTest, it_queries_generator_command_start)
{
    canbus::Message msg = genverter.queryGeneratorCommand(true, false);
    ASSERT_EQ(msg.can_id, protocol::FUNCTION_RECEIVE + protocol::NODE_GROUP_GENERATOR);
    ASSERT_EQ(msg.size, 8);
    ASSERT_EQ(msg.data[0], protocol::COMMAND_MASTER_WRITE_TO_SLAVE);

    // Object ID (Index) 0x5100
    ASSERT_EQ(msg.data[1], 0x51);
    ASSERT_EQ(msg.data[2], 0x00);
    // Sub index
    ASSERT_EQ(msg.data[3], 0x00);

    // Data bytes (0, 0, 0, 1)
    ASSERT_EQ(msg.data[4], 0x00);
    ASSERT_EQ(msg.data[5], 0x00);
    ASSERT_EQ(msg.data[6], 0x00);
    ASSERT_EQ(msg.data[7], 0x01);
}

TEST_F(PMGGenverterWhisperConnectTest, it_queries_generator_command_stop)
{
    canbus::Message msg = genverter.queryGeneratorCommand(false, true);
    ASSERT_EQ(msg.can_id, protocol::FUNCTION_RECEIVE + protocol::NODE_GROUP_GENERATOR);
    ASSERT_EQ(msg.size, 8);
    ASSERT_EQ(msg.data[0], protocol::COMMAND_MASTER_WRITE_TO_SLAVE);

    // Object ID (Index) 0x5101
    ASSERT_EQ(msg.data[1], 0x51);
    ASSERT_EQ(msg.data[2], 0x01);
    // Sub index
    ASSERT_EQ(msg.data[3], 0x00);

    // Data bytes (0, 0, 0, 1)
    ASSERT_EQ(msg.data[4], 0x00);
    ASSERT_EQ(msg.data[5], 0x00);
    ASSERT_EQ(msg.data[6], 0x00);
    ASSERT_EQ(msg.data[7], 0x01);
}
