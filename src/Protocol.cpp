#include <power_whisperpower/Protocol.hpp>

using namespace std;
using namespace power_whisperpower;

protocol::FunctionCode protocol::getFunctionCode(canbus::Message const& message) {
    return static_cast<protocol::FunctionCode>(message.can_id & 0x780);
}

uint8_t protocol::getNodeID(canbus::Message const& message) {
    return message.can_id & 0x7f;
}

canbus::Message protocol::makeWriteMessage(
    int8_t node_id,
    uint16_t object_id, uint8_t object_sub_id,
    std::array<uint8_t, 4> const& bytes
) {
    canbus::Message msg;
    msg.can_id = protocol::FUNCTION_RECEIVE + node_id;
    msg.size = 8;
    msg.data[0] = protocol::COMMAND_MASTER_WRITE_TO_SLAVE;
    toBigEndian(msg.data + 1, object_id);
    toBigEndian(msg.data + 3, object_sub_id);
    for (size_t i = 0; i < 4; ++i) {
        msg.data[4 + i] = bytes[i];
    }

    return msg;
}

canbus::Message protocol::makeReadMessage(
    int8_t node_id,
    uint16_t object_id, uint8_t object_sub_id
) {
    canbus::Message msg = canbus::Message::Zeroed();

    msg.can_id = protocol::FUNCTION_RECEIVE + node_id;
    msg.size = 8;
    msg.data[0] = protocol::COMMAND_MASTER_READ_FROM_SLAVE;
    toBigEndian(msg.data + 1, object_id);
    toBigEndian(msg.data + 3, object_sub_id);
    return msg;
}
