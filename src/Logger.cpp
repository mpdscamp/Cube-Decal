#include "Logger.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#endif

Logger::Logger() : currentLevel(LINFO) {
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
    case LDEBUG:   return "DEBUG";
    case LINFO:    return "INFO";
    case LWARNING: return "WARNING";
    case LERROR:   return "ERROR";
    case LFATAL:   return "FATAL";
    default:       return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLevel) {
        return;
    }

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream timestamp;

    std::tm timeinfo;
#ifdef _WIN32
    localtime_s(&timeinfo, &time);
#else
    localtime_r(&time, &timeinfo);
#endif

    timestamp << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    timestamp << '.' << std::setfill('0') << std::setw(3) << ms.count();

    std::string formattedMessage =
        "[" + timestamp.str() + "] [" + levelToString(level) + "] " + message;

    // Choose appropriate stream based on level
    std::ostream& stream = (level >= LWARNING) ? std::cerr : std::cout;
    stream << formattedMessage << std::endl;
}

void Logger::debug(const std::string& message) {
    log(LDEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LINFO, message);
}

void Logger::warning(const std::string& message) {
    log(LWARNING, message);
}

void Logger::error(const std::string& message) {
    log(LERROR, message);
}

void Logger::fatal(const std::string& message) {
    log(LFATAL, message);
}

Logger::LogStream::LogStream(LogLevel level, Logger* logger)
    : level(level), logger(logger), active(true) {
}

Logger::LogStream::~LogStream() {
    if (active && logger) {
        logger->log(level, stream.str());
    }
}

Logger::LogStream::LogStream(LogStream&& other) noexcept
    : stream(std::move(other.stream)),
    level(other.level),
    logger(other.logger),
    active(other.active) {
    other.active = false;
}

Logger::LogStream& Logger::LogStream::operator=(LogStream&& other) noexcept {
    if (this != &other) {
        stream = std::move(other.stream);
        level = other.level;
        logger = other.logger;
        active = other.active;

        other.active = false;
    }
    return *this;
}

Logger::LogStream Logger::createLogStream(LogLevel level) {
    return LogStream(level, this);
}