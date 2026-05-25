#include "core/AppLogger.h"

#include <gtest/gtest.h>

TEST(AppLogger, ReturnsSingletonInstance) {
    EXPECT_EQ(&AppLogger::getInstance(), &AppLogger::getInstance());
}

TEST(AppLogger, LogsInfoMessagesWithoutThrowing) {
    EXPECT_NO_THROW(AppLogger::getInstance().log(LogLevel::INFO, "info test message"));
}

TEST(AppLogger, LogsWarnMessagesWithoutThrowing) {
    EXPECT_NO_THROW(AppLogger::getInstance().log(LogLevel::WARN, "warn test message"));
}

TEST(AppLogger, LogsErrorMessagesWithoutThrowing) {
    EXPECT_NO_THROW(AppLogger::getInstance().log(LogLevel::ERROR, "error test message"));
}

TEST(AppLogger, LogsEmptyMessagesWithoutThrowing) {
    EXPECT_NO_THROW(AppLogger::getInstance().log(LogLevel::INFO, ""));
}

TEST(AppLogger, HandlesRepeatedMessagesWithoutThrowing) {
    EXPECT_NO_THROW({
        for (int index = 0; index < 10; ++index) {
            AppLogger::getInstance().log(LogLevel::INFO, "repeat");
        }
    });
}
