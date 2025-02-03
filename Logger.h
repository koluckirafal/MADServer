// SPDX-License-Identifier: MIT
// Based on plog - https://github.com/SergiusTheBest/plog
// Copyright (c) 2022 Sergey Podobry

#ifndef _LOGGER_H
#define _LOGGER_H

#include <ctime>
#include <sstream>
#include <string>

#define IF_LOGGER(severity)                                                                                            \
    if (!logger::Logger::getInstance() || !logger::Logger::getInstance()->checkSeverity(severity))                     \
    {                                                                                                                  \
        ;                                                                                                              \
    }                                                                                                                  \
    else
#define LOGGER(severity)                                                                                               \
    IF_LOGGER(severity)(*logger::Logger::getInstance()) += logger::Record(severity, __FILE__, __LINE__)
#define LOG_DEBUG LOGGER(logger::kDebug)
#define LOG_INFO LOGGER(logger::kInfo)
#define LOG_WARNING LOGGER(logger::kWarning)
#define LOG_ERROR LOGGER(logger::kError)
#define LOG_FATAL LOGGER(logger::kFatal)
#define LOG_NONE LOGGER(logger::kNone)

namespace logger
{
enum Severity
{
    kDebug = 0,
    kInfo = 1,
    kWarning = 2,
    kError = 3,
    kFatal = 4,
    kNone = 5,
};

inline std::string severityToString(Severity severity)
{
    switch (severity)
    {
    case kDebug:
        return "DEBUG";
    case kInfo:
        return "INFO";
    case kWarning:
        return "WARN";
    case kError:
        return "ERROR";
    case kFatal:
        return "FATAL";
    default:
        return "NONE";
    }
}

inline Severity severityFromString(std::string str)
{
    switch (std::toupper(str.at(0)))
    {
    case 'D':
        return kDebug;
    case 'I':
        return kInfo;
    case 'W':
        return kWarning;
    case 'E':
        return kError;
    case 'F':
        return kFatal;
    default:
        return kNone;
    }
}

class Record
{
  private:
    std::string buffer_;
    const Severity severity_;
    const struct tm *time_;
    const std::string file_;
    const int line_;

  public:
    Record(Severity severity, const char *file, int line);
    ~Record();
    const Severity getSeverity() const;
    const std::string getTimestamp() const;
    const std::string getFile() const;
    int getLine() const;
    std::string getBuffer() const;
    Record &ref() const;

    // template<typename T>
    // Record& operator<<(const T& value);
    template <typename T> Record &operator<<(const T &value)
    {
        std::stringstream ss;
        ss << value;
        buffer_ += ss.str();
        return *this;
    }
};

class Logger
{
  private:
    static Logger *instance_;
    static Severity severity_;

  public:
    static Logger *getInstance();
    void setSeverity(Severity severity);
    bool checkSeverity(Severity severity);
    std::string formatRecord(const Record &record);
    void operator+=(const Record &record);

  private:
    Logger();
    ~Logger();

    // Disallow copy and assignment
    Logger(const Logger &);
    Logger &operator=(const Logger &);
};

} // namespace logger

#endif // _LOGGER_H
