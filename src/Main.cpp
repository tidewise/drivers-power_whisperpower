#include <iostream>
#include <canbus/Driver.hpp>
#include <power_whisperpower/Device.hpp>
#include <power_whisperpower/DCPowerCube.hpp>
#include <memory>

#include <iostream>

using namespace std;
using namespace power_whisperpower;

void usage(std::ostream& out) {
    out << "power_whisperpower_ctl CMD CAN_DEVICE CAN_TYPE DEVICE_GROUP DEVICE_ID ARGS\n"
        << "where:\n"
        << "  CMD is one of the commands below\n"
        << "  CAN_DEVICE is the name of the CAN device, as e.g. can0\n"
        << "    its meaning is type-dependent, see drivers/canbus for more options\n"
        << "  CAN_TYPE is the type of the CAN device, as e.g. socket\n"
        << "  DEVICE_GROUP is one of: battery_monitor, generator, charger, inverter\n"
        << "  DEVICE_ID is the target device's ID\n"
        << "\n"
        << "Available commands:\n"
        << "  info: general device info\n"
        << "  dc-cube: read and display information about a DC Cube\n"
        << endl;
}

protocol::NodeIDGroups deviceGroupFromString(string name) {
    if (name == "battery_monitor") {
        return protocol::NODE_GROUP_BATTERY_MONITOR;
    }
    else if (name == "generator") {
        return protocol::NODE_GROUP_GENERATOR;
    }
    else if (name == "charger") {
        return protocol::NODE_GROUP_CHARGER;
    }
    else if (name == "inverter") {
        return protocol::NODE_GROUP_INVERTER;
    }
    else {
        throw std::invalid_argument("unknown group name '" + name + "'");
    }
}

void waitResult(canbus::Driver& can_device, power_whisperpower::Device& wp_device, canbus::Message const& query) {
    can_device.write(query);
    while (wp_device.isWaiting()) {
        auto msg = can_device.read();
        wp_device.process(msg);
    }
}

int main(int argc, char** argv)
{
    if (argc < 6) {
        usage(cerr);
        return 1;
    }

    string cmd(argv[1]);
    string can_device_name(argv[2]);
    string can_device_type(argv[3]);
    auto device_group = deviceGroupFromString(argv[4]);
    int device_id(std::stoi(argv[5]));

    unique_ptr<canbus::Driver> can_device(
        canbus::openCanDevice(can_device_name, can_device_type)
    );
    can_device->setReadTimeout(10000);

    if (cmd == "info") {
        power_whisperpower::Device device(device_group, device_id);
        waitResult(*can_device, device, device.queryDeviceName());
        waitResult(*can_device, device, device.queryHardwareVersion());
        waitResult(*can_device, device, device.querySoftwareVersion());
        waitResult(*can_device, device, device.querySerialNumber());
        waitResult(*can_device, device, device.queryTransmitPeriod());

        cout << "Device Name: " << device.getDeviceName() << endl;
        cout << "Hardware Version: " << device.getHardwareVersion() << endl;
        cout << "Software Version: " << device.getSoftwareVersion() << endl;
        cout << "Transmit Period: " << device.getTransmitPeriod() << endl;
        cout << "Serial Number: " << device.getSerialNumber() << endl;
    }
    else if (cmd == "dc-cube") {
        power_whisperpower::DCPowerCube wp_device(device_id);

        while (!wp_device.hasFullUpdate()) {
            auto msg = can_device->read();
            wp_device.process(msg);
        }
        std::cout << wp_device.getStatus() << std::endl;

        for (auto msg_id: wp_device.getConfigMessages()) {
            waitResult(*can_device, wp_device, wp_device.queryRead(msg_id));
        }
        std::cout << wp_device.getConfig() << std::endl;
    }

    return 0;
}

