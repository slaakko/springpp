// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module util.log.file.writer;

import std.core;

export namespace util {

using CoutType = std::ostream;
using StandardEndLine = CoutType & (*)(CoutType&);

class LogFileWriter
{
public:
    LogFileWriter(const std::string& logFilePath_, bool open_);
    LogFileWriter(const std::string& logFilePath);
    void WriteCurrentDateTime();
    void WriteLine();
    void WriteLine(const std::string& line);
    void Write(const std::string& s);
    std::ostream& LogFile() { return logFile; }
    bool IsOpen() const { return open; }
private:
    bool open;
    std::string logFilePath;
    std::ofstream logFile;
};

LogFileWriter& operator<<(LogFileWriter& writer, StandardEndLine manip);
LogFileWriter& operator<<(LogFileWriter& writer, const std::string& s);
LogFileWriter& operator<<(LogFileWriter& writer, const char* s);
LogFileWriter& operator<<(LogFileWriter& writer, char c);
LogFileWriter& operator<<(LogFileWriter& writer, bool b);
LogFileWriter& operator<<(LogFileWriter& writer, int x);
LogFileWriter& operator<<(LogFileWriter& writer, double x);
LogFileWriter& operator<<(LogFileWriter& writer, int64_t x);
LogFileWriter& operator<<(LogFileWriter& writer, uint64_t x);

} // namespace util
