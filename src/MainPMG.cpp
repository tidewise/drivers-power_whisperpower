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

    if (argc < 3) {
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
        wp_device.resetFullUpdate();
        std::cout << wp_device.getStatus() << std::endl;
    }
    else if (cmd == "run") {
        uint16_t init_counter = 0;
        while (true) {
            while (true) {
                auto msg = can_device->read();
                wp_device.process(msg);
                if (msg.can_id == 0x204 || msg.can_id == 0x205) {
                    break;
                }
            }

            std::this_thread::sleep_for(2ms);

            if (init_counter > 1500) {
                auto msg = wp_device.queryGeneratorCommand(false, true);
                can_device->write(msg);
            }
            else if (init_counter > 15) {
                auto msg = wp_device.queryGeneratorCommand(true, false);
                can_device->write(msg);
                ++init_counter;
            }
            else {
                auto msg = wp_device.queryGeneratorCommand(false, false);
                can_device->write(msg);
                ++init_counter;
            }
        }
    }

    return 0;
}
