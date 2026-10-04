#include "base/Time.hpp"
#include "power_whisperpower/PMGGenverter.hpp"
#include "power_whisperpower/PMGGenverterStatus.hpp"
#include <gtest/gtest.h>

using namespace power_whisperpower;

struct PMGGenverterTest : public ::testing::Test {
    PMGGenverter genverter;
};

TEST_F(PMGGenverterTest, it_resets_full_update)
{
    genverter.resetFullUpdate();
    PMGGenverterStatus status = genverter.getStatus();
    ASSERT_FALSE(genverter.hasFullUpdate());
    ASSERT_EQ(status.time, base::Time());
}

TEST_F(PMGGenverterTest, it_throws_if_an_unknown_protocol_is_given_to_process)
{
    PMGGenverter genverter(static_cast<PMGGenverterProtocol>(99));
    canbus::Message msg;
    ASSERT_THROW(genverter.process(msg), std::runtime_error);
}

TEST_F(PMGGenverterTest,
    it_throws_if_an_unknown_protocol_is_given_to_queryGeneratorCommand)
{
    PMGGenverter genverter(static_cast<PMGGenverterProtocol>(99));
    ASSERT_THROW(genverter.queryGeneratorCommand(true, false), std::runtime_error);
}
