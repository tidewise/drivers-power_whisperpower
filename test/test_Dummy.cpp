#include <boost/test/unit_test.hpp>
#include <power_whisperpower/Dummy.hpp>

using namespace power_whisperpower;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    power_whisperpower::DummyClass dummy;
    dummy.welcome();
}
