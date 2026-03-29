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

        static void log(
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

};

TEST_F(FunctionalUnitTests, test_setLogFunction_isSetLogFunction_getVersion_isStarted_start_stop_1) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::log);
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

        logEvents.clear();
        ASSERT_TRUE(logEvents.empty());

        exqudens::ThreadPool pool = {};
        EXQUDENS_LOG_INFO(LOGGER_ID) << "pool.isStarted: " << pool.isStarted();

        ASSERT_FALSE(pool.isStarted());
        pool.start(1, 1);
        ASSERT_TRUE(pool.isStarted());
        pool.stop();
        ASSERT_FALSE(pool.isStarted());
        ASSERT_TRUE(logEvents.empty());

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

        exqudens::ThreadPool pool = {};
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

        exqudens::ThreadPool pool = {};
        pool.start(3);
        TestClass1 object;
        auto testLambda2 = [] (int value) { return value; };
        std::future<int> testLambdaFuture = pool.submit(testLambda2, 123);
        std::future<int> testFunctionFuture = pool.submit(&testFunction2, 456);
        std::future<int> testMethodFuture = pool.submit(std::bind(&TestClass1::testMethod2, object, std::placeholders::_1), 789);
        int testLambdaExpected = 123;
        int testFunctionExpected = 456;
        int testMethodExpected = 789;
        int testLambdaResult = testLambdaFuture.get();
        int testFunctionResult = testFunctionFuture.get();
        int testMethodResult = testMethodFuture.get();
        ASSERT_EQ(testLambdaExpected, testLambdaResult);
        ASSERT_EQ(testFunctionExpected, testFunctionResult);
        ASSERT_EQ(testMethodExpected, testMethodResult);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

TEST_F(FunctionalUnitTests, test_start_submit_stop_3) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::log);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();
        ASSERT_TRUE(exqudens::ThreadPool::isSetLogFunction());

        exqudens::ThreadPool pool = {};
        pool.start(3);
        TestClass1 object;
        auto testLambda1 = [] () { return 123; };
        std::future<int> testLambdaFuture = pool.submit(testLambda1);
        std::future<int> testFunctionFuture = pool.submit(&TestClass1::testStaticMethod1);
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

TEST_F(FunctionalUnitTests, test_start_submit_stop_4) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::log);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();
        ASSERT_TRUE(exqudens::ThreadPool::isSetLogFunction());

        exqudens::ThreadPool pool = {};
        pool.start(3);
        TestClass1 object;
        auto testLambda2 = [] (int value) { return value; };
        std::future<int> testLambdaFuture = pool.submit(testLambda2, 123);
        std::future<int> testFunctionFuture = pool.submit(&TestClass1::testStaticMethod2, 456);
        std::future<int> testMethodFuture = pool.submit(std::bind(&TestClass1::testMethod2, object, std::placeholders::_1), 789);
        int testLambdaExpected = 123;
        int testFunctionExpected = 456;
        int testMethodExpected = 789;
        int testLambdaResult = testLambdaFuture.get();
        int testFunctionResult = testFunctionFuture.get();
        int testMethodResult = testMethodFuture.get();
        ASSERT_EQ(testLambdaExpected, testLambdaResult);
        ASSERT_EQ(testFunctionExpected, testFunctionResult);
        ASSERT_EQ(testMethodExpected, testMethodResult);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

TEST_F(FunctionalUnitTests, test_start_submit_stop_5) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::log);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();
        ASSERT_TRUE(exqudens::ThreadPool::isSetLogFunction());

        exqudens::ThreadPool pool = {};
        pool.start(3);
        int result = 0;
        auto testLambda = [&result] { result = 123; };
        std::future<void> testLambdaFuture = pool.submit(testLambda);
        testLambdaFuture.get();
        int expected = 123;
        ASSERT_EQ(expected, result);

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

TEST_F(FunctionalUnitTests, test_start_submit_stop_6) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::log);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();
        ASSERT_TRUE(exqudens::ThreadPool::isSetLogFunction());

        exqudens::ThreadPool pool = {};
        ASSERT_THROW(pool.start(1, 0), std::exception);
        ASSERT_THROW(pool.start(0, 1), std::exception);
        ASSERT_THROW(pool.start(0, 0), std::exception);
        try {
            pool.start(0, 1);
        FAIL() << "'std::exception' is not thrown";
        } catch (const std::exception& e) {
            std::vector<std::string> errorMessages = TestUtils::toStringVector(e);
            ASSERT_EQ(2, errorMessages.size());
            EXQUDENS_LOG_INFO(LOGGER_ID) << "errorMessages.at(1): '" << errorMessages.at(1) << "'";
            ASSERT_NE(std::string::npos, errorMessages.at(1).find("'optionalSize' is zero"));
        } catch (...) {
            FAIL() << "'std::exception' is not thrown";
        }
        try {
            pool.start(1, 0);
            FAIL() << "'std::exception' is not thrown";
        } catch (const std::exception& e) {
            std::vector<std::string> errorMessages = TestUtils::toStringVector(e);
            ASSERT_EQ(2, errorMessages.size());
            EXQUDENS_LOG_INFO(LOGGER_ID) << "errorMessages.at(1): '" << errorMessages.at(1) << "'";
            ASSERT_NE(std::string::npos, errorMessages.at(1).find("'optionalQueueSize' is zero"));
        } catch (...) {
            FAIL() << "'std::exception' is not thrown";
        }
        try {
            pool.start(0, 0);
            FAIL() << "'std::exception' is not thrown";
        } catch (const std::exception& e) {
            std::vector<std::string> errorMessages = TestUtils::toStringVector(e);
            ASSERT_EQ(2, errorMessages.size());
            EXQUDENS_LOG_INFO(LOGGER_ID) << "errorMessages.at(1): '" << errorMessages.at(1) << "'";
            ASSERT_NE(std::string::npos, errorMessages.at(1).find("'optionalQueueSize' is zero"));
        } catch (...) {
            FAIL() << "'std::exception' is not thrown";
        }

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}

TEST_F(FunctionalUnitTests, test_start_submit_stop_7) {
    try {
        std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
        std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
        EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

        exqudens::ThreadPool::setLogFunction(&FunctionalUnitTests::log);
        EXQUDENS_LOG_INFO(LOGGER_ID) << "exqudens.ThreadPool.isSetLogFunction: " << exqudens::ThreadPool::isSetLogFunction();
        ASSERT_TRUE(exqudens::ThreadPool::isSetLogFunction());

        exqudens::ThreadPool pool = {};
        pool.start(3);
        auto testLambda = [] { throw std::invalid_argument("test!"); };
        std::future<void> testLambdaFuture = pool.submit(testLambda);
        try {
            testLambdaFuture.get();
            FAIL() << "'std::invalid_argument' is not thrown";
        } catch (const std::invalid_argument& e) {
            ASSERT_EQ(std::string("test!"), std::string(e.what()));
        } catch (...) {
            FAIL() << "'std::invalid_argument' is not thrown";
        }

        EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
    } catch (const std::exception& e) {
        std::string errorMessage = TestUtils::toString(e);
        std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
        FAIL() << errorMessage;
    }
}
