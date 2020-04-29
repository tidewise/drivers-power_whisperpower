#include <gtest/gtest.h>
#include <power_whisperpower/Protocol.hpp>

using namespace std;
using namespace power_whisperpower;

struct ProtocolTest : public ::testing::Test {
};

TEST_F(ProtocolTest, getFunctionCode_returns_the_function_code_encoded_in_a_CAN_ID) {
    canbus::Message msg;
    msg.can_id = 0x580 + 0x15;
    ASSERT_EQ(0x580, protocol::getFunctionCode(msg));
}

TEST_F(ProtocolTest, getNodeID_returns_the_node_id_encoded_in_a_CAN_ID) {
    canbus::Message msg;
    msg.can_id = 0x580 + 0x15;
    ASSERT_EQ(0x15, protocol::getNodeID(msg));
}

TEST_F(ProtocolTest, it_creates_a_master_to_slave_write_message_with_the_whole_4_bytes) {
    array<uint8_t, 4> bytes { 1, 2, 3, 4 };
    auto msg = protocol::makeWriteMessage(0x15, 0x452, 0x1, bytes);

    ASSERT_EQ(0x615, msg.can_id);
    ASSERT_EQ(8, msg.size);
    ASSERT_EQ(0x23, msg.data[0]);
    ASSERT_EQ(0x04, msg.data[1]);
    ASSERT_EQ(0x52, msg.data[2]);
    ASSERT_EQ(0x1, msg.data[3]);
    ASSERT_EQ(1, msg.data[4]);
    ASSERT_EQ(2, msg.data[5]);
    ASSERT_EQ(3, msg.data[6]);
    ASSERT_EQ(4, msg.data[7]);
}

TEST_F(ProtocolTest, it_creates_a_master_to_slave_read_message_with_the_whole_4_bytes) {
    auto msg = protocol::makeReadMessage(0x15, 0x452, 0x1);

    ASSERT_EQ(0x615, msg.can_id);
    ASSERT_EQ(8, msg.size);
    ASSERT_EQ(0x40, msg.data[0]);
    ASSERT_EQ(0x04, msg.data[1]);
    ASSERT_EQ(0x52, msg.data[2]);
    ASSERT_EQ(0x1, msg.data[3]);
    ASSERT_EQ(0, msg.data[4]);
    ASSERT_EQ(0, msg.data[5]);
    ASSERT_EQ(0, msg.data[6]);
    ASSERT_EQ(0, msg.data[7]);
}
