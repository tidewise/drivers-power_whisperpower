#include <power_whisperpower/RunTimeState.hpp>

using namespace power_whisperpower;

using std::endl;

std::ostream& power_whisperpower::operator<<(std::ostream& io,
    power_whisperpower::RunTimeState const& state)
{
    io << "Time: " << state.time << "\n";
    io << "Total Work: " << state.total_work << "\n";
    io << "Maintenance Work: " << state.maintenance_work << "\n";

    return io;
}
