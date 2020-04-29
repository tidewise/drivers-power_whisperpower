#ifndef POWER_WHISPERPOWER_DCPOWERCUBE_HPP
#define POWER_WHISPERPOWER_DCPOWERCUBE_HPP

#include <power_whisperpower/Device.hpp>
#include <power_whisperpower/DCPowerCubeState.hpp>

namespace power_whisperpower {
    class DCPowerCube : public Device {
        typedef DCPowerCubeState State;

        State m_state;
        bool m_has_full_update = false;

    protected:
        virtual void processRead(
            uint16_t object_id, uint8_t object_sub_id, uint8_t const* value
        );

    public:
        DCPowerCube(uint8_t device_id);
        State getState() const;

        bool hasFullUpdate() const;
        void resetFullUpdate();
    };
}

#endif
