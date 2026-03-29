#pragma once

#include <cstddef>
#include <string>
#include <optional>
#include <vector>
#include <queue>
#include <functional>
#include <memory>
#include <filesystem>
#include <stdexcept>
#include <future>
#include <thread>
#include <mutex>
#include <type_traits>
#include <condition_variable>
#include <atomic>

#include "exqudens/threadpool/export.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens {

    class EXQUDENS_THREADPOOL_EXPORT ThreadPool {

        private:

            inline static std::function<void(
                const std::string& file,
                size_t line,
                const std::string& function,
                const std::string& id,
                unsigned short level,
                const std::string& message
            )> logFunction = {};

            size_t queueSize = 0;
            std::queue<std::function<void()>> queue = {};
            std::vector<std::thread> threads = {};
            std::mutex mutex = {};
            std::condition_variable condition = {};
            std::atomic<bool> started = {};

        public:

            static void setLogFunction(
                const std::function<void(
                    const std::string& file,
                    size_t line,
                    const std::string& function,
                    const std::string& id,
                    unsigned short level,
                    const std::string& message
                )>& value
            );

            static bool isSetLogFunction();

            static std::string getVersion();

            ThreadPool();

            bool isStarted();

            void start(
                const std::optional<size_t>& queueSize = {},
                const std::optional<size_t>& size = {}
            );

            template<class F, class... ARGS>
            auto submit(F&& f, ARGS&&... args) -> std::future<typename std::invoke_result<F, ARGS...>::type>;

            void stop();

            ~ThreadPool();

        private:

            static void log(
                const std::string& file,
                size_t line,
                const std::string& function,
                const std::string& id,
                unsigned short level,
                const std::string& message
            );

            static void logDebug(
                const std::string& file,
                size_t line,
                const std::string& function,
                const std::string& message
            );

            static void logWarning(
                const std::string& file,
                size_t line,
                const std::string& function,
                const std::string& message
            );

    };

}

namespace exqudens {

    template<class F, class... ARGS>
    EXQUDENS_THREADPOOL_INLINE
    auto ThreadPool::submit(F&& f, ARGS&&... args) -> std::future<typename std::invoke_result<F, ARGS...>::type> {
        try {
            if (queue.size() >= queueSize) {
                throw std::runtime_error(CALL_INFO + ": queue overflow");
            }
            using return_type = typename std::invoke_result<F, ARGS...>::type;
            auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<ARGS>(args)...));
            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(mutex);
                if (!started.load()) {
                    throw std::runtime_error(CALL_INFO + ": submitted on stopped");
                }
                queue.emplace([task] { (*task)(); });
            }
            condition.notify_one();
            return res;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO

// implementation ---

#include "exqudens/threadpool/versions.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOGGER_ID "exqudens.ThreadPool"

namespace exqudens {

    EXQUDENS_THREADPOOL_INLINE void ThreadPool::setLogFunction(
        const std::function<void(
            const std::string& file,
            size_t line,
            const std::string& function,
            const std::string& id,
            unsigned short level,
            const std::string& message
        )>& value
    ) {
        try {
            logFunction = value;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_THREADPOOL_INLINE bool ThreadPool::isSetLogFunction() {
        try {
            return (bool) logFunction;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_THREADPOOL_INLINE std::string ThreadPool::getVersion() {
        try {
            std::string result = {};
            result += std::to_string(PROJECT_VERSION_MAJOR);
            result += '.';
            result += std::to_string(PROJECT_VERSION_MINOR);
            result += '.';
            result += std::to_string(PROJECT_VERSION_PATCH);
            logDebug(__FILE__, __LINE__, __FUNCTION__, std::string("result: '") + result + "'");
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_THREADPOOL_INLINE ThreadPool::ThreadPool() = default;

    EXQUDENS_THREADPOOL_INLINE bool ThreadPool::isStarted() {
        try {
            return started.load();
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_THREADPOOL_INLINE void ThreadPool::start(
        const std::optional<size_t>& optionalSize,
        const std::optional<size_t>& optionalQueueSize
    ) {
        try {
            if (optionalQueueSize && optionalQueueSize.value() == 0) {
                throw std::runtime_error(CALL_INFO + ": 'optionalQueueSize' is zero");
            }
            if (optionalSize && optionalSize.value() == 0) {
                throw std::runtime_error(CALL_INFO + ": 'optionalSize' is zero");
            }
            size_t size = optionalSize.value_or(std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1);
            queueSize = optionalQueueSize.value_or(size);
            started.store(true);
            for (size_t i = 0; i < size; i++) {
                threads.emplace_back(
                    [this] {
                        while (true) {
                            std::function<void()> task;
                            {
                                std::unique_lock<std::mutex> lock(this->mutex);
                                this->condition.wait(
                                    lock,
                                    [this] {
                                        return !this->started.load() || !this->queue.empty();
                                    }
                                );
                                if (!this->started.load() && this->queue.empty()) {
                                    return;
                                }
                                task = std::move(this->queue.front());
                                this->queue.pop();
                            }
                            task();
                        }
                    }
                );
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_THREADPOOL_INLINE void ThreadPool::stop() {
        try {
            if (started.load()) {
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    started.store(false);
                }
                condition.notify_all();
                for (std::thread& thread : threads) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
            }
        } catch (const std::exception& e) {
            std::throw_with_nested(std::runtime_error(CALL_INFO + ": " + e.what()));
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO + ": uknown error"));
        }
    }

    EXQUDENS_THREADPOOL_INLINE ThreadPool::~ThreadPool() {
        try {
            stop();
        } catch (const std::exception& e) {
            logWarning(__FILE__, __LINE__, __FUNCTION__, std::string("error on stop: '") + e.what() + "'");
        } catch (...) {
            logWarning(__FILE__, __LINE__, __FUNCTION__, "uknown error on stop");
        }
    }

    EXQUDENS_THREADPOOL_INLINE void ThreadPool::log(
        const std::string& file,
        size_t line,
        const std::string& function,
        const std::string& id,
        unsigned short level,
        const std::string& message
    ) {
        try {
            if (logFunction) {
                std::string internalFile = std::filesystem::path(file).filename().string();
                logFunction(internalFile, line, function, id, level, message);
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_THREADPOOL_INLINE void ThreadPool::logDebug(
        const std::string& file,
        size_t line,
        const std::string& function,
        const std::string& message
    ) {
        try {
            log(file, line, function, LOGGER_ID, 5, message);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_THREADPOOL_INLINE void ThreadPool::logWarning(
        const std::string& file,
        size_t line,
        const std::string& function,
        const std::string& message
    ) {
        try {
            log(file, line, function, LOGGER_ID, 3, message);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef LOGGER_ID
#undef CALL_INFO
