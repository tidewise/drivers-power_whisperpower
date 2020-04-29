#ifndef POWER_WHISPERPOWER_PROTOCOL_HPP
#define POWER_WHISPERPOWER_PROTOCOL_HPP

#include <array>
#include <canbus/Message.hpp>

namespace power_whisperpower {
    namespace protocol {
        static const int BROADCAST_ID = 0;

        /**
         * Node IDs are generated as a "device ID" between 0 and 0xF and a
         * device group below. The node ID is the lowest 7 bits of the CAN ID
         */
        enum NodeIDGroups {
            NODE_GROUP_BATTERY_MONITOR = 0x10,
            NODE_GROUP_GENERATOR = 0x20,
            NODE_GROUP_CHARGER = 0x30,
            NODE_GROUP_INVERTER = 0x40
        };

        /** Function codes
         *
         * "Receive" and "Send" is w.r.t. the device itself. I.e. the master
         * should use FUNCTION_RECEIVE to make a device receive something
         */
        enum FunctionCode {
            FUNCTION_SEND = 0x580,
            FUNCTION_RECEIVE = 0x600
        };

        /** The commands */
        enum Commands {
            COMMAND_MASTER_READ_FROM_SLAVE = 0x40,
            COMMAND_MASTER_WRITE_TO_SLAVE = 0x23,
            COMMAND_SLAVE_READ_SUCCESSFUL = 0x43,
            COMMAND_SLAVE_WRITE_SUCCESSFUL = 0x60,
            COMMAND_ABORT = 0x80
        };

        FunctionCode getFunctionCode(canbus::Message const& message);

        uint8_t getNodeID(canbus::Message const& message);

        canbus::Message makeWriteMessage(
            int8_t node_id,
            uint16_t object_id, uint8_t object_sub_id,
            std::array<uint8_t, 4> const& bytes
        );

        canbus::Message makeReadMessage(
            int8_t node_id,
            uint16_t object_id, uint8_t object_sub_id
        );

        static const uint32_t ABORT_COMMAND_INVALID = 0x05040001;
        static const uint32_t ABORT_READ_WRITE_ONLY_OBJECT = 0x06010001;
        static const uint32_t ABORT_WRITE_READ_ONLY_OBJECT = 0x06010002;
        static const uint32_t ABORT_OBJECT_DOES_NOT_EXIST = 0x06020000;
        static const uint32_t ABORT_INVALID_VALUE = 0x06090030;

        template<typename T> void toBigEndian(uint8_t* data, T value);
        template<> inline void toBigEndian(uint8_t* data, uint8_t value)
        {
            data[0] = value;
        }
        template<> inline void toBigEndian(uint8_t* data, uint16_t value)
        {
            data[0] = (value >> 8) & 0xFF;
            data[1] = (value >> 0) & 0xFF;
        }
        template<> inline void toBigEndian(uint8_t* data, uint32_t value)
        {
            data[0] = (value >> 24) & 0xFF;
            data[1] = (value >> 16) & 0xFF;
            data[2] = (value >> 8) & 0xFF;
            data[3] = (value >> 0) & 0xFF;
        }
        template<> inline void toBigEndian(uint8_t* data, int8_t value)
        {
            return toBigEndian<uint8_t>(data, reinterpret_cast<uint8_t&>(value));
        }
        template<> inline void toBigEndian(uint8_t* data, int16_t value)
        {
            return toBigEndian<uint16_t>(data, reinterpret_cast<uint16_t&>(value));
        }
        template<> inline void toBigEndian(uint8_t* data, int32_t value)
        {
            return toBigEndian<uint32_t>(data, reinterpret_cast<uint32_t&>(value));
        }

        template<typename T> T fromBigEndian(uint8_t const* data);
        template<> inline uint8_t fromBigEndian(uint8_t const* data)
        {
            return data[0];
        }
        template<> inline uint16_t fromBigEndian(uint8_t const* data)
        {
            return static_cast<uint16_t>(data[0]) << 8 |
                   static_cast<uint16_t>(data[1]) << 0;
        }
        template<> inline uint32_t fromBigEndian(uint8_t const* data)
        {
            return static_cast<uint32_t>(data[0]) << 24  |
                   static_cast<uint32_t>(data[1]) << 16  |
                   static_cast<uint32_t>(data[2]) << 8 |
                   static_cast<uint32_t>(data[3]) << 0;
        }
        template<> inline int8_t fromBigEndian(uint8_t const* data)
        {
            return reinterpret_cast<int8_t const&>(data[0]);
        }
        template<> inline int16_t fromBigEndian(uint8_t const* data)
        {
            uint16_t result = fromBigEndian<uint16_t>(data);
            return reinterpret_cast<int16_t const&>(result);
        }
        template<> inline int32_t fromBigEndian(uint8_t const* data)
        {
            uint32_t result = fromBigEndian<uint32_t>(data);
            return reinterpret_cast<int32_t const&>(result);
        }
    }
}

#endif