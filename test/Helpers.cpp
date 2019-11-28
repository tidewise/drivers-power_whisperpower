#include "Helpers.hpp"
#include <gmock/gmock.h>
#include <power_whisperpower/Protocol.hpp>

using namespace power_whisperpower;

void Helpers::ASSERT_DEVICE_READ(
    canbus::Message const& msg,
    uint8_t node_id, uint16_t object_id, uint8_t object_sub_id
) {

    auto expected = protocol::makeReadMessage(node_id, object_id, object_sub_id);

    ASSERT_EQ(msg.can_id, expected.can_id);
    ASSERT_EQ(msg.size, expected.size);
    ASSERT_THAT(msg.data, testing::ElementsAreArray(expected.data));
}

void Helpers::ASSERT_DEVICE_WRITE(
    canbus::Message const& msg,
    uint8_t node_id, uint16_t object_id, uint8_t object_sub_id,
    std::array<uint8_t, 4> const& bytes
) {
    auto expected = canbus::Message::Zeroed();
    expected.can_id = 0x600 + node_id;
    expected.size = 8;
    expected.data[0] = 0x23;
    expected.data[1] = (object_id & 0xFF00) >> 8;
    expected.data[2] = (object_id & 0x00FF) >> 0;
    expected.data[3] = object_sub_id;
    for (int i = 0; i < 4; ++i) {
        expected.data[4 + i] = bytes[i];
    }

    ASSERT_EQ(msg.can_id, expected.can_id);
    ASSERT_EQ(msg.size, expected.size);
    ASSERT_THAT(msg.data, testing::ElementsAreArray(expected.data));
}

canbus::Message Helpers::makeReadReply(
    uint8_t node_id, uint16_t object_id, uint8_t object_sub_id,
    std::array<uint8_t, 4> const& bytes
) {
    auto msg = canbus::Message::Zeroed();
    msg.can_id = 0x580 + node_id;
    msg.size = 8;
    msg.data[0] = 0x43;
    msg.data[1] = (object_id & 0xFF00) >> 8;
    msg.data[2] = (object_id & 0x00FF) >> 0;
    msg.data[3] = object_sub_id;
    for (int i = 0; i < 4; ++i) {
        msg.data[4 + i] = bytes[i];
    }
    return msg;
}

canbus::Message Helpers::makeWriteReply(
    uint8_t node_id, uint16_t object_id, uint8_t object_sub_id
) {
    auto msg = canbus::Message::Zeroed();
    msg.can_id = 0x580 + node_id;
    msg.size = 8;
    msg.data[0] = 0x60;
    msg.data[1] = (object_id & 0xFF00) >> 8;
    msg.data[2] = (object_id & 0x00FF) >> 0;
    msg.data[3] = object_sub_id;
    return msg;
}
