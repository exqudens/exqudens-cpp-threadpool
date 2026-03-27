#pragma once

#include <string>
#include <exception>
#include <cstddef>
#include <vector>
#include <map>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <exqudens/Log.hpp>
#include <exqudens/log/api/Logging.hpp>

#include "TestUtils.hpp"
#include "exqudens/ThreadPool.hpp"

int testFunction1() {
    return 123;
}

int testFunction2(int value) {
    return value;
}

class TestClass1 {

    public:

        static int testStaticMethod1() {
            return 123;
        }

        static int testStaticMethod2(int value) {
            return value;
        }

        int testMethod1() {
            return 123;
        }

        int testMethod2(int value) {
            return value;
        }
};

class FunctionalUnitTests: public testing::Test {

    public:

        inline static const char* LOGGER_ID = "FunctionalUnitTests";

    protected:

        inline static std::vector<std::map<std::string, std::string>> logEvents = {};

    protected:

        static void addLogEvent(
            const std::string& file,
            size_t line,
            const std::string& function,
            const std::string& id,
            unsigned short level,
            const std::string& message
        ) {
            std::map<std::string, std::string> logEvent = {
                {"file", file},
                {"line", std::to_string(line)},
                {"function", function},
                {"id", id},
                {"level", std::to_string(level)},
                {"message", message}
            };
            logEvents.emplace_back(logEvent);
        }

        static void log(
            const std::string& file,
            size_t line,
            const std::string& function,
            const std::string& id,
            unsigned short level,
            const std::string& message
        ) {
            exqudens::log::api::Logging::Writer(file, line, function, id, level) << message;
        }

};

TEST_F(FunctionalUnitTests, test_setLogFunction_isSetLogFunction_getVersion_1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::addLogEvent);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();

        ASSERT_TRUE(exqudens::ThreadPool::isSetLogFunction());

        std::string expectedVersion = TestUtils::readFileString((std::filesystem::path(TestUtils::getProjectSourceDir()) / "name-version.txt").generic_string());
        expectedVersion = TestUtils::split(expectedVersion, ":").at(1);
        expectedVersion = TestUtils::trim(expectedVersion);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expectedVersion: '" << expectedVersion << "'";

        std::string actualVersion = exqudens::ThreadPool::getVersion();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actualVersion: '" << actualVersion << "'";

        ASSERT_EQ(expectedVersion, actualVersion);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "logEvents.size: " << logEvents.size();

        ASSERT_FALSE(logEvents.empty());

        std::map<std::string, std::string> firstLogEvent = logEvents.at(0);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "firstLogEvent: " << TestUtils::toString(firstLogEvent);

        std::string expectedLogMessage = "result: '";
        expectedLogMessage += actualVersion;
        expectedLogMessage += "'";
        EXQUDENS_LOG_INFO(LOGGER_ID) << "expectedLogMessage: '" << expectedLogMessage << "'";

        std::string actualLogMessage = firstLogEvent.at("message");
        EXQUDENS_LOG_INFO(LOGGER_ID) << "actualLogMessage: '" << actualLogMessage << "'";

        ASSERT_EQ(expectedLogMessage, actualLogMessage);

        exqudens::ThreadPool::setLogFunction({});
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();

        ASSERT_FALSE(exqudens::ThreadPool::isSetLogFunction());

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

TEST_F(FunctionalUnitTests, test_start_submit_stop_1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::log);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();
        ASSERT_TRUE(exqudens::ThreadPool::isSetLogFunction());

        exqudens::ThreadPool pool;
        pool.start(3);
        TestClass1 object;
        auto testLambda1 = [] () { return 123; };
        std::future<int> testLambdaFuture = pool.submit(testLambda1);
        std::future<int> testFunctionFuture = pool.submit(&testFunction1);
        std::future<int> testMethodFuture = pool.submit(std::bind(&TestClass1::testMethod1, object));
        int testLambdaExpected = 123;
        int testFunctionExpected = 123;
        int testMethodExpected = 123;
        int testLambda1result = testLambdaFuture.get();
        int testFunction1result = testFunctionFuture.get();
        int testMethod1result = testMethodFuture.get();
        ASSERT_EQ(testLambdaExpected, testLambda1result);
        ASSERT_EQ(testFunctionExpected, testFunction1result);
        ASSERT_EQ(testMethodExpected, testMethod1result);
        pool.stop();

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

TEST_F(FunctionalUnitTests, test_start_submit_stop_2) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::log);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();
        ASSERT_TRUE(exqudens::ThreadPool::isSetLogFunction());

        //TODO

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

TEST_F(FunctionalUnitTests, test_start_submit_stop_999) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::log);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();
        ASSERT_TRUE(exqudens::ThreadPool::isSetLogFunction());

        //TODO

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}
