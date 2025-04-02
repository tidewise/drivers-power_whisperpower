#ifndef POWER_WHISPERPOWER_PMGGENVERTER_HPP
#define POWER_WHISPERPOWER_PMGGENVERTER_HPP

#include <power_whisperpower/Device.hpp>
#include <power_whisperpower/PMGGenverterStatus.hpp>
#include <power_whisperpower/RunTimeState.hpp>

namespace power_whisperpower {
    /** Driver for the WhisperPower WP-PMG Genverter
     *
     * Behaves similarly to Smart Shunts which are connected through a CAN bus.
     * Use one CANdriver from Rock's drivers/canbus (or elsewhere) to receive/transmit
     * messages from the bus
     *
     * The main functionality of this driver is to interpret the suite of CAN
     * messages that form the Genverter status update. The pattern is to call
     * \c resetFullUpdate and check \c hasFullUpdate after each processed
     * CAN message. When \c hasFullUpdate is true, you know you the cube
     * status has been fully updated and can call \c getStatus.
     *
     * Then call \c resetFullUpdate to start a new cycle
     */
    class PMGGenverter {
        typedef PMGGenverterStatus Status;

        Status m_status;
        RunTimeState m_run_time_state;
        bool m_has_full_update = false;
        uint8_t m_run_ramp = 0;

    public:
        PMGGenverter();

        /** Process a single message from the bus */
        void process(canbus::Message const& msg);

        /** Returns the CAN message that enables or disables the generator
         */
        canbus::Message queryGeneratorCommand(bool start, bool stop);

        /** The current known status
         *
         * The status is reset to unknown by \c resetFullUpdate
         */
        PMGGenverterStatus getStatus() const;

        /** The current known runtime state
         */
        RunTimeState getRunTimeState() const;

        /** Check if a full update to the smart shunt status has been received
         *
         * True if the whole status structure has been updated at least once
         * since the last call to \c resetFullUpdate
         */
        bool hasFullUpdate() const;

        /** Reset \c hasFullUpdate to wait for a new full update
         */
        void resetFullUpdate();
    };
}

#endif