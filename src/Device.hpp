#ifndef POWER_WHISPERPOWER_DEVICE_HPP
#define POWER_WHISPERPOWER_DEVICE_HPP

#include <array>
#include <power_whisperpower/Protocol.hpp>

namespace power_whisperpower {
    struct Abort : public std::runtime_error {
        Abort(std::string const& msg)
            : std::runtime_error(msg) {}
    };

    /**
     * Base interaction with a WhisperPower device
     */
    class Device {
    public:
        enum States {
            STATE_NORMAL = 0,
            STATE_WAIT_WRITE = 1,
            STATE_WAIT_READ = 2
        };

    private:
        uint8_t m_node_id;
        States m_state = STATE_NORMAL;
        base::Time m_wait_start_time;
        uint16_t m_wait_object_id;
        uint16_t m_wait_object_sub_id;

        uint32_t m_serial_number = 0;
        bool m_has_transmit_period = false;
        base::Time m_transmit_period;

        bool processWaitFinished(protocol::Commands command,
                                 uint16_t object_id, uint8_t object_sub_id);

        void setWaitState(States state, uint16_t object_id, uint8_t object_sub_id);

        void processAbortMessage(canbus::Message const& message,
                                 uint16_t object_id, uint8_t object_sub_id);

        static const int OID_SERIAL_NUMBER = 0x1018;
        static const int OID_TRANSMIT_PERIOD = 0x9100;

    protected:
        virtual void processRead(
            uint16_t object_id, uint8_t object_sub_id, uint8_t const* value
        );

    public:
        explicit Device(protocol::NodeIDGroups group, uint8_t device_id = 0);
        virtual ~Device();

        bool isWaiting() const;

        /** How long we have been waiting for a specific reply from the device
         */
        base::Time getElapsedWaitTime() const;

        /** Return a message that queries the serial number from the device */
        canbus::Message querySerialNumber();

        /** Return the serial number, or zero if unknown */
        uint32_t getSerialNumber() const;

        /** Query the current periodic transmit period */
        canbus::Message queryTransmitPeriod();

        /** Query that the device stops emitting periodic messages */
        canbus::Message queryDisableTransmitPeriod();

        /** Query a modification to the current periodic transmit period
         *
         * Max period is 65535 ms
         */
        canbus::Message queryTransmitPeriodChange(base::Time const& period);

        /** Whether the transmit period has been read at least once
         */
        bool hasTransmitPeriod() const;

        /** Return the current device transmit period
         *
         * Use @c hasTransmitPeriod to check whether the value is known.
         * A null time means that periodi transmission is disabled
         */
        base::Time getTransmitPeriod() const;

        template<typename T>
        canbus::Message queryWrite(uint16_t object_id, T value) {
            return queryWrite<T>(object_id, 0, value);
        }

        template<typename T>
        canbus::Message queryWrite(uint16_t object_id, uint8_t object_sub_id, T value) {
            std::array<uint8_t, 4> buffer;
            buffer.fill(0);
            protocol::toBigEndian(&buffer[4 - sizeof(T)], value);
            setWaitState(STATE_WAIT_WRITE, object_id, object_sub_id);
            return protocol::makeWriteMessage(
                m_node_id,
                object_id, object_sub_id,
                buffer
            );
        }

        canbus::Message queryRead(uint16_t object_id, uint8_t object_sub_id = 0);

        /** Process a message received from the device */
        void process(canbus::Message const& message);
    };
}

#endif
