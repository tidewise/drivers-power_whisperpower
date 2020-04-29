#ifndef POWER_WHISPERPOWER_TEST_HELPERS_HPP
#define POWER_WHISPERPOWER_TEST_HELPERS_HPP

#include <cstdint>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <canbus/Message.hpp>
#include <power_whisperpower/Protocol.hpp>

struct Helpers {
    void ASSERT_DEVICE_READ(canbus::Message const& msg,
                            uint8_t node_id, uint16_t object_id, uint8_t object_sub_id);

    void ASSERT_DEVICE_WRITE(
        canbus::Message const& msg,
        uint8_t node_id, uint16_t object_id, uint8_t object_sub_id,
        std::array<uint8_t, 4> const& bytes
    );

    canbus::Message makeAbort(
        uint8_t node_id, uint16_t object_id, uint8_t object_sub_id, uint32_t code
    );

    canbus::Message makeReadReply(
        uint8_t node_id, uint16_t object_id, uint8_t object_sub_id,
        std::array<uint8_t, 4> const& bytes
    );

    canbus::Message makeWriteReply(
        uint8_t node_id, uint16_t object_id, uint8_t object_sub_id
    );
};

#endif
