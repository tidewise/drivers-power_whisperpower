#include <iostream>
#include <canbus/Driver.hpp>
#include <power_whisperpower/PMGGenverter.hpp>
#include <memory>

#include <iostream>

#include <chrono>
#include <thread>

using namespace std;
using namespace power_whisperpower;

void usage(std::ostream& out) {
    out << "power_whisperpower_pmg_ctl CMD CAN_DEVICE CAN_TYPE\n"
        << "where:\n"
        << "  CMD is one of the commands below\n"
        << "  CAN_DEVICE is the name of the CAN device, as e.g. can0\n"
        << "    its meaning is type-dependent, see drivers/canbus for more options\n"
        << "  CAN_TYPE is the type of the CAN device, as e.g. socket\n"
        << "\n"
        << "Available commands:\n"
        << "  info: display information sent by the genverter\n"
        << "  run: run the generator\n"
        << endl;
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
    using namespace std::chrono_literals;

    if (argc < 6) {
        usage(cerr);
        return 1;
    }

    string cmd(argv[1]);
    string can_device_name(argv[2]);
    string can_device_type(argv[3]);

    unique_ptr<canbus::Driver> can_device(
        canbus::openCanDevice(can_device_name, can_device_type)
    );
    can_device->setReadTimeout(10000);

    power_whisperpower::PMGGenverter wp_device;

    if (cmd == "info") {
        while (!wp_device.hasFullUpdate()) {
            auto msg = can_device->read();
            wp_device.process(msg);
        }

        std::cout << wp_device.getStatus() << std::endl;
    }
    else if (cmd == "run") {
        while (true) {
            auto msg = wp_device.queryGeneratorRun();
            wp_device.process(msg);

            std::this_thread::sleep_for(20ms);
        }
    }

    return 0;
}

