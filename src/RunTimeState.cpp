#include <power_whisperpower/RunTimeState.hpp>

using namespace power_whisperpower;

using std::endl;

std::ostream& power_whisperpower::operator<<(std::ostream& io,
    power_whisperpower::RunTimeState const& state)
{
    io << "Time: " << state.time << "\n";
    io << "Run time since last maintenance" << state.total_run_time << "\n";
    io << "Run time since commissioning" << state.historical_run_time << "\n";

    return io;
}
