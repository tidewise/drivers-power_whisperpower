#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <power_whisperpower/Device.hpp>
#include <power_whisperpower/Protocol.hpp>
#include "Helpers.hpp"

using namespace power_whisperpower;
using namespace testing;

struct MockDevice : public Device {
    using Device::Device;
    MOCK_METHOD3(processRead, void(uint16_t, uint8_t, uint8_t const*));
};

struct DeviceTest : public ::testing::Test,
                    public Helpers {
    Device device;

    static const int NODE_ID = 0x24;

    DeviceTest()
        : device(protocol::NODE_GROUP_GENERATOR, 0x4) {
    }
};

struct MockDeviceTest : public ::testing::Test,
                        public Helpers {
    MockDevice device;

    static const int NODE_ID = 0x24;

    MockDeviceTest()
        : device(protocol::NODE_GROUP_GENERATOR, 0x4) {
    }
};

TEST_F(DeviceTest, it_queries_a_write) {
    auto msg = device.queryWrite<int32_t>(0x4284, 0x12, 0x12345678);
    ASSERT_DEVICE_WRITE(msg, NODE_ID, 0x4284, 0x12, { 0x12, 0x34, 0x56, 0x78 });
}

TEST_F(DeviceTest, it_counts_wait_time_after_a_write_query) {
    device.queryWrite<uint16_t>(0x4284, 0x12, 0x102);
    base::Time tic = base::Time::now();
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_continues_counting_write_time_after_receiving_a_read_reply_for_the_same_object) {
    device.queryWrite<uint16_t>(0x4284, 0x12, 0x102);
    base::Time tic = base::Time::now();
    usleep(1000);
    device.process(makeReadReply(NODE_ID, 0x4284, 0x12, { 0, 0, 0, 0 }));
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_continues_counting_write_time_after_receiving_a_write_reply_for_another_object_id) {
    device.queryWrite<uint16_t>(0x4284, 0x12, 0x102);
    base::Time tic = base::Time::now();
    usleep(1000);
    device.process(makeWriteReply(NODE_ID, 0x4285, 0x12));
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_continues_counting_write_time_after_receiving_a_write_reply_for_another_object_sub_id) {
    device.queryWrite<uint16_t>(0x4284, 0x12, 0x102);
    auto tic = base::Time::now();
    usleep(1000);
    device.process(makeWriteReply(NODE_ID, 0x4284, 0x11));
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_continues_counting_write_time_after_receiving_a_write_reply_for_another_node) {
    device.queryWrite<uint16_t>(0x4284, 0x12, 0x102);
    base::Time tic = base::Time::now();
    usleep(1000);
    device.process(makeWriteReply(NODE_ID + 1, 0x4284, 0x12));
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_resets_wait_time_after_receiving_the_expected_write_reply) {
    device.queryWrite<uint16_t>(0x4284, 0x12, 0x102);
    device.process(makeWriteReply(NODE_ID, 0x4284, 0x12));
    ASSERT_TRUE(device.getElapsedWaitTime().isNull());
}

TEST_F(DeviceTest, it_processes_an_abort_message_related_to_the_pending_write) {
}

TEST_F(DeviceTest, it_resets_the_wait_write_state_once_the_abort_is_received) {
}

TEST_F(DeviceTest, it_ignores_an_abort_message_not_related_to_the_pending_write) {
}

TEST_F(DeviceTest, it_queries_a_read) {
    auto msg = device.queryRead(0x4284, 0x12);
    ASSERT_DEVICE_READ(msg, NODE_ID, 0x4284, 0x12);
}

TEST_F(DeviceTest, it_counts_wait_time_after_a_read_query) {
    device.queryRead(0x4284, 0x12);
    base::Time tic = base::Time::now();
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_continues_counting_read_time_after_receiving_a_write_reply_for_the_same_object) {
    device.queryRead(0x4284, 0x12);
    base::Time tic = base::Time::now();
    usleep(1000);
    device.process(makeWriteReply(NODE_ID, 0x4284, 0x12));
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_continues_counting_read_time_after_receiving_a_read_reply_for_another_object_id) {
    device.queryRead(0x4284, 0x12);
    base::Time tic = base::Time::now();
    usleep(1000);
    device.process(makeReadReply(NODE_ID, 0x4285, 0x12, { 0, 0, 0, 0 }));
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_continues_counting_read_time_after_receiving_a_read_reply_for_another_object_sub_id) {
    device.queryRead(0x4284, 0x12);
    auto tic = base::Time::now();
    usleep(1000);
    device.process(makeReadReply(NODE_ID, 0x4284, 0x11, { 0, 0, 0, 0 }));
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_continues_counting_read_time_after_receiving_a_read_reply_for_another_node) {
    device.queryRead(0x4284, 0x12);
    base::Time tic = base::Time::now();
    usleep(1000);
    device.process(makeReadReply(NODE_ID + 1, 0x4284, 0x12, { 0, 0, 0, 0 }));
    usleep(1000);
    ASSERT_GE(device.getElapsedWaitTime(), base::Time::now() - tic);
}

TEST_F(DeviceTest, it_resets_wait_time_after_receiving_the_expected_read_reply) {
    device.queryRead(0x4284, 0x12);
    device.process(makeReadReply(NODE_ID, 0x4284, 0x12, { 0, 0, 0, 0 }));
    ASSERT_TRUE(device.getElapsedWaitTime().isNull());
}

TEST_F(MockDeviceTest, it_calls_processRead_when_receiving_a_read_reply) {
    EXPECT_CALL(device, processRead(0x4284, 0x12, _));
    device.process(makeReadReply(NODE_ID, 0x4284, 0x12, { 0x0, 0x3, 0x2, 0x1 }));
}

TEST_F(DeviceTest, it_queries_the_serial_number_object) {
    auto msg = device.querySerialNumber();
    ASSERT_DEVICE_READ(msg, NODE_ID, 0x1018, 0);
}

TEST_F(DeviceTest, it_returns_a_zero_serial_number_by_default) {
    ASSERT_EQ(0, device.getSerialNumber());
}

TEST_F(DeviceTest, it_returns_the_last_received_serial_number) {
    device.process(makeReadReply(NODE_ID, 0x1018, 0, { 0x12, 0x34, 0x56, 0x78 }));
    ASSERT_EQ(0x12345678, device.getSerialNumber());
}

TEST_F(DeviceTest, it_queries_the_transmit_period) {
    auto msg = device.queryTransmitPeriod();
    ASSERT_DEVICE_READ(msg, NODE_ID, 0x9100, 0);
}

TEST_F(DeviceTest, it_returns_that_it_does_not_have_the_period_by_default) {
    ASSERT_FALSE(device.hasTransmitPeriod());
}

TEST_F(DeviceTest, it_returns_the_last_received_transmit_period) {
    device.process(makeReadReply(NODE_ID, 0x9100, 0, { 0, 0, 0x12, 0x34 }));
    ASSERT_TRUE(device.hasTransmitPeriod());
    ASSERT_EQ(base::Time::fromMilliseconds(0x1234), device.getTransmitPeriod());
}

TEST_F(DeviceTest, it_returns_the_last_received_transmit_period_even_if_it_is_zero) {
    device.process(makeReadReply(NODE_ID, 0x9100, 0, { 0, 0, 0, 0 }));
    ASSERT_TRUE(device.hasTransmitPeriod());
    ASSERT_TRUE(device.getTransmitPeriod().isNull());
}

TEST_F(DeviceTest, it_normalizes_transmit_period_values_below_50ms_to_0) {
    // 0..49 is "disable", the class just normalizes
    device.process(makeReadReply(NODE_ID, 0x9100, 0, { 0, 0, 0, 49 }));
    ASSERT_TRUE(device.getTransmitPeriod().isNull());
}

TEST_F(DeviceTest, it_queries_to_disable_the_periodic_messages) {
    auto msg = device.queryDisableTransmitPeriod();
    ASSERT_DEVICE_WRITE(msg, NODE_ID, 0x9100, 0, { 0, 0, 0, 0 });
}

TEST_F(DeviceTest, it_queries_a_transmit_period_change) {
    auto msg = device.queryTransmitPeriodChange(base::Time::fromMilliseconds(0x1234));
    ASSERT_DEVICE_WRITE(msg, NODE_ID, 0x9100, 0, { 0, 0, 0x12, 0x34 });
}
