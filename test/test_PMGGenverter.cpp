#include "base/Temperature.hpp"
#include "base/Time.hpp"
#include "canbus/Message.hpp"
#include "power_whisperpower/PMGGenverter.hpp"
#include "power_whisperpower/PMGGenverterStatus.hpp"
#include <gtest/gtest.h>

using namespace power_whisperpower;

struct PMGGenverterTest : public ::testing::Test {
    PMGGenverter genverter;
    canbus::Message msg;
};

TEST_F(PMGGenverterTest, it_processes_message_200)
{
    msg.can_id = 0x200;
    // AC Voltage (230.0 V)
    msg.data[0] = 0x08;
    msg.data[1] = 0xFC;
    // AC Current (15.5 A)
    msg.data[2] = 0x00;
    msg.data[3] = 0x9B;
    // Engine angular speed (3000 RPM)
    msg.data[4] = 0x0B;
    msg.data[5] = 0xB8;
    // Temperature (45.0°C)
    msg.data[6] = 0x01;
    msg.data[7] = 0xC2;

    genverter.process(msg);
    PMGGenverterStatus status = genverter.getStatus();

    ASSERT_EQ(status.ac_voltage, 230.0);
    ASSERT_EQ(status.ac_current, 15.5);
    ASSERT_NEAR(status.engine_angular_speed, 314.159, 1e-3);
    ASSERT_EQ(status.inverter_temperature.getCelsius(), 45.0);
}

TEST_F(PMGGenverterTest, it_processes_message_201)
{
    msg.can_id = 0x201;
    msg.data[0] = 0x00;
    msg.data[1] = PMGGenverterStatus::Status::GENERATION_ENABLED |
                  PMGGenverterStatus::Status::ENGINE_ENABLED;
    msg.data[2] = 0x00;
    msg.data[3] = PMGGenverterStatus::InverterAlarm::AC_OVER_LOAD |
                  PMGGenverterStatus::InverterAlarm::OVER_TEMPERATURE;
    msg.data[4] = 0x00;
    msg.data[5] = PMGGenverterStatus::InverterWarning::HIGH_RPM;
    msg.data[6] =
        0x00 | (PMGGenverterStatus::EngineAlarm::INVERTER_COMMUNICATION_ERROR >> 8);
    msg.data[7] = PMGGenverterStatus::EngineAlarm::OIL_PRESSURE |
                  PMGGenverterStatus::EngineAlarm::EXHAUST_TEMPERATURE;

    genverter.process(msg);
    PMGGenverterStatus status = genverter.getStatus();

    ASSERT_EQ(status.status, 0x5);
    ASSERT_EQ(status.inverter_alarm, 0xA);
    ASSERT_EQ(status.inverter_warning, 0x2);
    ASSERT_EQ(status.engine_alarm, 0x9);
}

TEST_F(PMGGenverterTest, it_processes_message_202)
{
    msg.can_id = 0x202;
    // Stepper position
    msg.data[0] = 0x01;
    msg.data[1] = 0xF4;
    // Oil temperature
    msg.data[2] = 0x03;
    msg.data[3] = 0x52;
    // Delta DC bus voltage
    msg.data[4] = 0x00;
    msg.data[5] = 0xF6;
    // PWM scale factor
    msg.data[6] = 0x00;
    msg.data[7] = 0x05;

    genverter.process(msg);
    PMGGenverterStatus status = genverter.getStatus();

    ASSERT_EQ(status.stepper, 500);
    ASSERT_EQ(status.oil_temperature.getCelsius(), 85);
    ASSERT_EQ(status.delta_dc_bus, 246);
    ASSERT_EQ(status.PWM_scale, 5);
}

TEST_F(PMGGenverterTest, it_processes_message_203)
{
    msg.can_id = 0x203;
    msg.data[7] = 0x10;

    genverter.process(msg);
    PMGGenverterStatus status = genverter.getStatus();

    ASSERT_EQ(status.TEST_ramp, 16);
}

TEST_F(PMGGenverterTest, it_processes_message_204)
{
    msg.can_id = 0x204;
    // Inverter model
    msg.data[0] = 0x49; 
    msg.data[1] = 0x4D;
    // Firmware version
    msg.data[2] = 0x01;
    msg.data[3] = 0x00;
    // Hardware version
    msg.data[4] = 0x03;
    msg.data[5] = 0x00;
    // Inverter serial number
    msg.data[6] = 0x53;
    msg.data[7] = 0x4E;

    genverter.process(msg);
    PMGGenverterStatus status = genverter.getStatus();

    ASSERT_EQ(status.inverter_model, "IM");
    ASSERT_EQ(status.firmware_version, 0x01);
    ASSERT_EQ(status.firmware_subversion, 0x00);
    ASSERT_EQ(status.hardware_version, 0x03);
    ASSERT_EQ(status.hardware_subversion, 0x00);
    ASSERT_EQ(status.inverter_serial_number, "SN");
}

TEST_F(PMGGenverterTest, it_processes_message_205)
{
    msg.can_id = 0x205;
    // Total Hour Work
    msg.data[0] = 0x00;
    msg.data[1] = 0x0A;
    // Total Minute Work
    msg.data[2] = 0x00;
    msg.data[3] = 0x00;
    // Maintenence Hour Work
    msg.data[4] = 0x00;
    msg.data[5] = 0x05;
    // Maintenence Minute Work
    msg.data[6] = 0x00;
    msg.data[7] = 0x00;

    genverter.process(msg);
    PMGGenverterStatus status = genverter.getStatus();

    ASSERT_EQ(status.total_work.toSeconds(), 36000);
    ASSERT_EQ(status.maintenance_work.toSeconds(), 18000);
    ASSERT_TRUE(genverter.hasFullUpdate());
}

TEST_F(PMGGenverterTest, it_queries_generator_command_start)
{
    canbus::Message msg = genverter.queryGeneratorCommand(true, false);
    ASSERT_EQ(msg.can_id, 0x210);
    ASSERT_EQ(msg.size, 8);
    ASSERT_EQ(msg.data[0], 1);
    ASSERT_EQ(msg.data[1], 0);
}

TEST_F(PMGGenverterTest, it_queries_generator_command_stop)
{
    canbus::Message msg = genverter.queryGeneratorCommand(false, true);
    ASSERT_EQ(msg.can_id, 0x210);
    ASSERT_EQ(msg.size, 8);
    ASSERT_EQ(msg.data[0], 0);
    ASSERT_EQ(msg.data[1], 1);
}

TEST_F(PMGGenverterTest, it_queries_generator_command_no_action)
{
    canbus::Message msg = genverter.queryGeneratorCommand(false, false);
    ASSERT_EQ(msg.can_id, 0x210);
    ASSERT_EQ(msg.size, 8);
    ASSERT_EQ(msg.data[0], 0);
    ASSERT_EQ(msg.data[1], 0);
}

TEST_F(PMGGenverterTest, it_queries_generator_command_both_start_and_stop)
{
    canbus::Message msg = genverter.queryGeneratorCommand(true, true);
    ASSERT_EQ(msg.can_id, 0x210);
    ASSERT_EQ(msg.size, 8);
    ASSERT_EQ(msg.data[0], 1);
    ASSERT_EQ(msg.data[1], 1);
}

TEST_F(PMGGenverterTest, it_resets_full_update)
{
    genverter.resetFullUpdate();
    PMGGenverterStatus status = genverter.getStatus();
    ASSERT_FALSE(genverter.hasFullUpdate());
    ASSERT_EQ(status.time, base::Time());
}
