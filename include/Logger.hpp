#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <memory>

/**
 * Logger class to handle all logging operations using a singleton pattern
 */
class Logger {
public:
    /**
     * Log level enumeration from least to most severe
     */
    enum LogLevel {
        LDEBUG,
        LINFO,
        LWARNING,
        LERROR,
        LFATAL
    };

    /**
     * Get the singleton instance of the logger
     *
     * @return Reference to the logger instance
     */
    static Logger& getInstance();

    /**
     * Log message with specified level
     *
     * @param level The severity level of the message
     * @param message The message to log
     */
    void log(LogLevel level, const std::string& message);

    /**
     * Log a debug message
     *
     * @param message The message to log
     */
    void debug(const std::string& message);

    /**
     * Log an info message
     *
     * @param message The message to log
     */
    void info(const std::string& message);

    /**
     * Log a warning message
     *
     * @param message The message to log
     */
    void warning(const std::string& message);

    /**
     * Log an error message
     *
     * @param message The message to log
     */
    void error(const std::string& message);

    /**
     * Log a fatal message
     *
     * @param message The message to log
     */
    void fatal(const std::string& message);

    /**
     * Set minimum log level (messages below this level won't be logged)
     *
     * @param level The minimum level to log
     */
    void setLogLevel(LogLevel level);

    /**
     * Stream-like interface for logging
     */
    class LogStream {
    public:
        /**
         * Constructor for a log stream
         *
         * @param level The log level for this stream
         * @param logger Pointer to the logger
         */
        LogStream(LogLevel level, Logger* logger);

        /**
         * Destructor - flushes buffered log message
         */
        ~LogStream();

        // Delete copy constructor and assignment operator
        LogStream(const LogStream&) = delete;
        LogStream& operator=(const LogStream&) = delete;

        /**
         * Move constructor
         *
         * @param other The stream to move from
         */
        LogStream(LogStream&& other) noexcept;

        /**
         * Move assignment operator
         *
         * @param other The stream to move from
         * @return Reference to this stream
         */
        LogStream& operator=(LogStream&& other) noexcept;

        /**
         * Stream operator for adding data to the log message
         *
         * @param value Value to append to the log message
         * @return Reference to this stream for chaining
         */
        template<typename T>
        LogStream& operator<<(const T& value) {
            stream << value;
            return *this;
        }

    private:
        std::ostringstream stream;
        LogLevel level;
        Logger* logger;
        bool active; // Flag to track if this stream is still responsible for logging
    };

    /**
     * Create a new LogStream for the specified log level
     *
     * @param level The log level for the new stream
     * @return A new LogStream
     */
    LogStream createLogStream(LogLevel level);

private:
    /**
     * Private constructor for singleton pattern
     */
    Logger();

    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * Convert log level to string
     *
     * @param level The log level to convert
     * @return String representation of the log level
     */
    std::string levelToString(LogLevel level);

    LogLevel currentLevel;
};

// Macro for easy logging using temporary objects
#define LOG_DEBUG Logger::getInstance().createLogStream(Logger::LDEBUG)
#define LOG_INFO Logger::getInstance().createLogStream(Logger::LINFO)
#define LOG_WARNING Logger::getInstance().createLogStream(Logger::LWARNING)
#define LOG_ERROR Logger::getInstance().createLogStream(Logger::LERROR)
#define LOG_FATAL Logger::getInstance().createLogStream(Logger::LFATAL)