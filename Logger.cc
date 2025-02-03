// SPDX-License-Identifier: MIT
// Based on plog - https://github.com/SergiusTheBest/plog
// Copyright (c) 2022 Sergey Podobry

#include <cstdlib> // for time()
#include <ctime>   // for localtime()
#include <iostream>

#include "Logger.h"

namespace logger
{

// Initialize static members
Logger *Logger::instance_ = 0;
Severity Logger::severity_ = kDebug;

Record::Record(Severity severity, const char *file, int line)
    : severity_(severity), file_(std::string(file)), line_(line)
{
    time_t now = time(0);
    time_ = localtime(&now);
}

Record::~Record()
{
}

const Severity Record::getSeverity() const
{
    return severity_;
}

const std::string Record::getTimestamp() const
{
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_);
    return std::string(timestamp);
}

const std::string Record::getFile() const
{
    return file_;
}

int Record::getLine() const
{
    return line_;
}

std::string Record::getBuffer() const
{
    return buffer_;
}

Logger::Logger()
{
}
Logger::~Logger()
{
}

Logger *Logger::getInstance()
{
    if (!instance_)
        instance_ = new Logger();
    return instance_;
}

std::string Logger::formatRecord(const Record &record)
{
    std::ostringstream str_stream;

    str_stream << "[" << record.getTimestamp() << "] [" << severityToString(record.getSeverity()) << "]\t["
               << record.getFile() << ":" << record.getLine() << "] " << record.getBuffer() << std::endl;

    return str_stream.str();
}

void Logger::setSeverity(Severity severity)
{
    severity_ = severity;
}

bool Logger::checkSeverity(Severity severity)
{
    return severity_ <= severity;
}

void Logger::operator+=(const Record &record)
{
    if (checkSeverity(record.getSeverity()))
    {
        std::clog << formatRecord(record);
    }
}

// Disallow copy and assignment
Logger::Logger(const Logger &)
{
}
Logger &Logger::operator=(const Logger &)
{
    return *this;
}

} // namespace logger
