#include <power_whisperpower/Device.hpp>

using namespace std;
using namespace power_whisperpower;

Device::Device(protocol::NodeIDGroups group, uint8_t device_id)
    : m_node_id(group + device_id) {
}

Device::~Device() {
}

canbus::Message Device::queryDeviceName() {
    return queryRead(OID_DEVICE_NAME);
}

std::string Device::getDeviceName() {
    return m_device_name;
}

canbus::Message Device::queryHardwareVersion() {
    return queryRead(OID_HARDWARE_VERSION);
}

std::string Device::getHardwareVersion() {
    return m_hardware_version;
}

canbus::Message Device::querySoftwareVersion() {
    return queryRead(OID_SOFTWARE_VERSION);
}

std::string Device::getSoftwareVersion() {
    return m_software_version;
}

canbus::Message Device::querySerialNumber() {
    return queryRead(OID_SERIAL_NUMBER);
}

uint32_t Device::getSerialNumber() const {
    return m_serial_number;
}

canbus::Message Device::querySetId(uint8_t id, uint32_t serial_number) {
    return queryWrite<uint32_t>(OID_CAN_ID, id, serial_number);
}

canbus::Message Device::queryTransmitPeriod() {
    return queryRead(OID_TRANSMIT_PERIOD);
}

canbus::Message Device::queryDisableTransmitPeriod() {
    return queryTransmitPeriodChange(base::Time());
}

canbus::Message Device::queryTransmitPeriodChange(base::Time const& period) {
    return queryWrite<uint16_t>(OID_TRANSMIT_PERIOD, period.toMilliseconds());
}

bool Device::hasTransmitPeriod() const {
    return m_has_transmit_period;
}

base::Time Device::getTransmitPeriod() const {
    return m_transmit_period;
}

void Device::process(canbus::Message const& message) {
    auto function_code = protocol::getFunctionCode(message);
    auto node_id = protocol::getNodeID(message);

    if (node_id != m_node_id) {
        return;
    }
    else if (function_code != protocol::FUNCTION_SEND) {
        throw std::invalid_argument(
            "received CAN message with unexpected function code"
        );
    }

    auto command = static_cast<protocol::Commands>(message.data[0]);
    uint16_t object_id = protocol::fromBigEndian<uint16_t>(message.data + 1);
    uint8_t object_sub_id = message.data[3];

    if (command == protocol::COMMAND_ABORT) {
        processAbortMessage(message, object_id, object_sub_id);
        // We reach here if the abort was not for the object we're manipulating
        return;
    }

    if (processWaitFinished(command, object_id, object_sub_id)) {
        m_state = STATE_NORMAL;
    }

    if (command == protocol::COMMAND_SLAVE_READ_SUCCESSFUL) {
        processRead(object_id, object_sub_id, message.data + 4);
    }
}

bool Device::isWaiting() const {
    return m_state != STATE_NORMAL;
}

void Device::setWaitState(States state, uint16_t object_id, uint8_t object_sub_id) {
    m_state = state;
    m_wait_start_time = base::Time::now();
    m_wait_object_id = object_id;
    m_wait_object_sub_id = object_sub_id;
}

base::Time Device::getElapsedWaitTime() const {
    if (m_state == STATE_NORMAL) {
        return base::Time();
    }
    else {
        return base::Time::now() - m_wait_start_time;
    }
}

void Device::processAbortMessage(canbus::Message const& message,
                                 uint16_t object_id, uint8_t object_sub_id) {
    if (m_wait_object_id != object_id ||
        m_wait_object_sub_id != object_sub_id) {
        return;
    }

    m_state = STATE_NORMAL;

    uint32_t code = protocol::fromBigEndian<uint32_t>(message.data + 4);
    switch (code) {
        case protocol::ABORT_COMMAND_INVALID:
            throw Abort("device reports: invalid command");
        case protocol::ABORT_READ_WRITE_ONLY_OBJECT:
            throw Abort("device reports: attempte to read a write-only object");
        case protocol::ABORT_WRITE_READ_ONLY_OBJECT:
            throw Abort("device reports: attempte to write a read-only object");
        case protocol::ABORT_OBJECT_DOES_NOT_EXIST:
            throw Abort("device reports: attempte to access a non-existent object");
        case protocol::ABORT_INVALID_VALUE:
            throw Abort("device reports: invalid value in write");
        default:
            throw Abort("abort received, but the code is undefined");
    }
}

bool Device::processWaitFinished(protocol::Commands command,
                                 uint16_t object_id, uint8_t object_sub_id) {

    if (m_state == STATE_NORMAL) {
        return false;
    }
    else if (m_state == STATE_WAIT_READ &&
             command != protocol::COMMAND_SLAVE_READ_SUCCESSFUL) {
        return false;
    }
    else if (m_state == STATE_WAIT_WRITE &&
             command != protocol::COMMAND_SLAVE_WRITE_SUCCESSFUL) {
        return false;
    }

    return m_wait_object_id == object_id &&
           m_wait_object_sub_id == object_sub_id;
}

void Device::processRead(
    uint16_t object_id, uint8_t object_sub_id, uint8_t const* value
) {
    switch (object_id) {
        case OID_SERIAL_NUMBER:
            m_serial_number = protocol::fromBigEndian<uint32_t>(value);
            return;
        case OID_DEVICE_NAME:
        {
            m_device_name = protocol::fromBigEndian<string>(value);
            return;
        }
        case OID_HARDWARE_VERSION:
            m_hardware_version = protocol::fromBigEndian<string>(value);
            return;
        case OID_SOFTWARE_VERSION:
            m_software_version = protocol::fromBigEndian<string>(value);
            return;
        case OID_TRANSMIT_PERIOD: {
            m_has_transmit_period = true;
            uint16_t transmit_period_ms = protocol::fromBigEndian<uint16_t>(value + 2);
            if (transmit_period_ms < 50) {
                m_transmit_period = base::Time();
            }
            else {
                m_transmit_period = base::Time::fromMilliseconds(transmit_period_ms);
            }
            return;
        }
    }
}

canbus::Message Device::queryRead(uint16_t object_id, uint8_t object_sub_id) {
    setWaitState(STATE_WAIT_READ, object_id, object_sub_id);
    return protocol::makeReadMessage(
        m_node_id, object_id, object_sub_id
    );
}
