#ifndef POWER_WHISPERPOWER_DCPOWERCUBE_HPP
#define POWER_WHISPERPOWER_DCPOWERCUBE_HPP

#include <power_whisperpower/Device.hpp>
#include <power_whisperpower/DCPowerCubeStatus.hpp>
#include <power_whisperpower/DCPowerCubeConfig.hpp>

namespace power_whisperpower {
    /** Driver for the WhisperPower DC Cube
     *
     * Since DC power cubes are connected through a CAN bus, this class only
     * provides interpretation and generation of CAN messages. Use one CAN
     * driver from Rock's drivers/canbus (or elsewhere) to actually transmit/
     * receive messages from the bus
     *
     * The main functionality of this driver is to interpret the suite of CAN
     * messages that form the DC cube's status update. The pattern is to call
     * \c resetFullUpdate and check \c hasFullUpdate after each processed
     * CAN message. When \c hasFullUpdate is true, you know you the cube
     * status has been fully updated and can call \c getStatus.
     *
     * Then call \c resetFullUpdate to start a new cycle
     */
    class DCPowerCube : public Device {
        typedef DCPowerCubeStatus Status;
        typedef DCPowerCubeConfig Config;

        Status m_status;
        bool m_has_full_update = false;

        Config m_config;

    protected:
        virtual void processRead(
            uint16_t object_id, uint8_t object_sub_id, uint8_t const* value
        );

    public:
        DCPowerCube(uint8_t device_id);

        /** Return the set of messages that will update the DC cube configuration
         * structure
         *
         * Call queryRead to get the corresponding read message
         */
        std::vector<uint16_t> getConfigMessages() const;

        /** Return the last known configuration state
         *
         * All messages from queryConfig() must have been sent and be processed
         * for the return value to make sense
         */
        Config getConfig() const;

        /** Return the last known DC cube status */
        Status getStatus() const;

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
