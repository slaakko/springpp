// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

// testing tuning 

module;
#include <ctime>

module util.time;

import util.text.util;

namespace util {

std::vector<int> monthDays;

struct InitMonthDays
{
    InitMonthDays();
};

InitMonthDays::InitMonthDays() 
{
    monthDays.push_back(0);
    monthDays.push_back(31);
    monthDays.push_back(28);
    monthDays.push_back(31);
    monthDays.push_back(30);
    monthDays.push_back(31);
    monthDays.push_back(30);
    monthDays.push_back(31);
    monthDays.push_back(31);
    monthDays.push_back(30);
    monthDays.push_back(31);
    monthDays.push_back(30);
    monthDays.push_back(31);
}

InitMonthDays initMonthDays;

int GetMonthDays(Month month, int year)
{
    if (month == Month::february && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
    {
        return 29;
    }
    return monthDays[static_cast<int8_t>(month)];
}

Date Date::AddDays(int n)
{
    if (n > 0)
    {
        int d = day + n;
        Month m = month;
        int y = year;
        int md = GetMonthDays(m, y);
        while (d > md)
        {
            d = d - md;
            if (m == Month::december)
            {
                m = Month::january;
                ++y;
            }
            else
            {
                m = static_cast<Month>(static_cast<int8_t>(static_cast<int8_t>(m) + 1));
            }
            md = GetMonthDays(m, y);
        }
        return Date(y, m, d);
    }
    else if (n < 0)
    {
        int d = day + n;
        Month m = month;
        int y = year;
        while (d < 1)
        {
            if (m == Month::january)
            {
                m = Month::december;
                --y;
            }
            else
            {
                m = static_cast<Month>(static_cast<int8_t>(static_cast<int8_t>(m) - 1));
            }
            d = d + GetMonthDays(m, y);
        }
        return Date(y, m, d);
    }
    else
    {
        return *this;
    }
}

Date Date::AddMonths(int n)
{
    if (n > 0)
    {
        int m = static_cast<int8_t>(month) + n;
        int y = year;
        int d = day;
        while (m > 12)
        {
            m = m - 12;
            ++y;
        }
        Month mnth = static_cast<Month>(static_cast<int8_t>(m));
        int md = GetMonthDays(mnth, y);
        if (d > md)
        {
            d = md;
        }
        return Date(y, mnth, d);
    }
    else if (n < 0)
    {
        int m = static_cast<int8_t>(month) + n;
        int y = year;
        int d = day;
        while (m < 1)
        {
            m = m + 12;
            --y;
        }
        Month mnth = static_cast<Month>(static_cast<int8_t>(m));
        int md = GetMonthDays(mnth, y);
        if (d > md)
        {
            d = md;
        }
        return Date(y, mnth, d);
    }
    else
    {
        return *this;
    }
}

Date Date::AddYears(int n)
{
    int y = year + n;
    int d = day;
    int md = GetMonthDays(month, y);
    if (d > md)
    {
        d = md;
    }
    return Date(y, month, d);
}

std::string Date::ToString() const
{
    return ToString(false);
}

std::string Date::ToString(bool omitDashes) const
{
    std::string date;
    date.append(1, static_cast<char>(static_cast<int16_t>('0') + ((year / 1000) % 10)));
    date.append(1, static_cast<char>(static_cast<int16_t>('0') + ((year / 100) % 10)));
    date.append(1, static_cast<char>(static_cast<int16_t>('0') + ((year / 10) % 10)));
    date.append(1, static_cast<char>(static_cast<int16_t>('0') + (year % 10)));
    if (!omitDashes)
    {
        date.append(1, '-');
    }
    date.append(1, static_cast<char>(static_cast<int8_t>('0') + ((static_cast<int8_t>(month) / 10) % 10)));
    date.append(1, static_cast<char>(static_cast<int8_t>('0') + (static_cast<int8_t>(month) % 10)));
    if (!omitDashes)
    {
        date.append(1, '-');
    }
    date.append(1, static_cast<char>(static_cast<int8_t>('0') + ((day / 10) % 10)));
    date.append(1, static_cast<char>(static_cast<int8_t>('0') + (day % 10)));
    return date;
}

#if defined(OTAVA)

Date GetCurrentDate()
{
    int yyyy = 0;
    int mm = 0;
    int dd = 0;
    current_date(yyyy, mm, dd);
    return Date(yyyy, static_cast<Month>(static_cast<int8_t>(mm)), dd);
}

#else

Date GetCurrentDate()
{
    std::time_t currentTime;
    std::time(&currentTime);
    struct tm* localTime = nullptr;
    localTime = std::localtime(&currentTime);
    return Date(1900 + localTime->tm_year, static_cast<Month>(1 + localTime->tm_mon), static_cast<int8_t>(localTime->tm_mday));
}

#endif

bool operator==(const Date& left, const Date& right)
{
    return left.Year() == right.Year() && left.GetMonth() == right.GetMonth() && left.Day() == right.Day();
}

bool operator<(const Date& left, const Date& right)
{
    if (left.Year() < right.Year()) return true;
    if (left.Year() > right.Year()) return false;
    if (left.GetMonth() < right.GetMonth()) return true;
    if (left.GetMonth() > right.GetMonth()) return false;
    return left.Day() < right.Day();
}

void ThrowRuntimeError(const std::string& message)
{
    throw std::runtime_error(message);
}

void ThrowInvalidDate(const std::string& s)
{
    ThrowRuntimeError("cannot parse date from string '" + s + "': not in format YYYY-MM-DD or YYYYMMDD");
}

void ThrowInvalidDateTime(const std::string& s)
{
    ThrowRuntimeError("cannot parse date time from string '" + s + "': not in format YYYY[-]MM[-]DD or YYYY[-]MM[-]DDTHH[[:]MM[[:]SS]");
}

Date ParseDate(const std::string& dateStr, int& dateEnd)
{
    if (dateStr.length() < 4 + 2 + 2)
    {
        ThrowInvalidDate(dateStr);
    }
    int16_t year = static_cast<int16_t>(std::stoi(dateStr.substr(0, 4)));
    int monthStart = 4;
    if (dateStr[4] == '-')
    {
        ++monthStart;
    }
    if (dateStr.length() < monthStart + 2)
    {
        ThrowInvalidDate(dateStr);
    }
    int8_t month = static_cast<int8_t>(std::stoi(dateStr.substr(monthStart, 2)));
    if (month < 1 || month > 12)
    {
        ThrowInvalidDate(dateStr);
    }
    int dayStart = monthStart + 2;
    if (dateStr[dayStart] == '-')
    {
        ++dayStart;
    }
    if (dateStr.length() < dayStart + 2)
    {
        ThrowInvalidDate(dateStr);
    }
    int8_t day = static_cast<int8_t>(std::stoi(dateStr.substr(dayStart, 2)));
    if (day < 1 || day > 31)
    {
        ThrowInvalidDate(dateStr);
    }
    dateEnd = dayStart + 2;
    return Date(year, static_cast<Month>(month), day);
}

Date ParseDate(const std::string& dateStr)
{
    int dateEnd = 0;
    return ParseDate(dateStr, dateEnd);
}

std::string DateTime::ToString() const
{
    return ToString(false, false, false, false);
}

std::string DateTime::ToString(bool omitDashes, bool omitColons, bool omitMins, bool omitSecs) const
{
    std::string dateTime;
    dateTime.append(date.ToString(omitDashes));
    dateTime.append(1, 'T');
    int32_t hh = Hours() % 24;
    int32_t mm = Minutes() % 60;
    int32_t ss = Seconds() % 60;
    dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + ((hh / 10) % 10)));
    dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + (hh % 10)));
    if (!omitMins)
    {
        if (!omitColons)
        {
            dateTime.append(1, ':');
        }
        dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + ((mm / 10) % 10)));
        dateTime.append(1, static_cast<char>(static_cast<int>('0') + (mm % 10)));
        if (!omitSecs)
        {
            if (!omitColons)
            {
                dateTime.append(1, ':');
            }
            dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + ((ss / 10) % 10)));
            dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + (ss % 10)));
        }
    }
    return dateTime;
}

std::string FormatTimeMs(int32_t milliseconds)
{
    int32_t hh = milliseconds / 3600000;
    int32_t mm = milliseconds / 60000 % 60;
    int32_t ss = milliseconds / 1000 % 60;
    int32_t ms = milliseconds % 1000;
    std::string time;
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((hh / 10) % 10)));
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + (hh % 10)));
    time.append(1, ':');
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((mm / 10) % 10)));
    time.append(1, static_cast<char>(static_cast<int>('0') + (mm % 10)));
    time.append(1, ':');
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((ss / 10) % 10)));
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + (ss % 10)));
    time.append(1, '.');
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((ms / 100) % 10)));
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((ms / 10) % 10)));
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + (ms % 10)));
    return time;
}

#ifdef OTAVA

DateTime GetCurrentDateTime()
{
    int yyyy = 0;
    int month = 0;
    int day = 0;
    int seconds = 0;
    current_date_time(yyyy, month, day, seconds);
    Date date(yyyy, static_cast<Month>(static_cast<int8_t>(month)), day);
    DateTime dt(date, seconds);
    return dt;
}

#else

DateTime GetCurrentDateTime()
{
    std::time_t currentTime;
    std::time(&currentTime);
    struct tm* localTime = nullptr;
    localTime = std::localtime(&currentTime);
    return DateTime(Date(1900 + localTime->tm_year, static_cast<Month>(1 + localTime->tm_mon), static_cast<int8_t>(localTime->tm_mday)), localTime->tm_hour * 3600 + localTime->tm_min * 60 + localTime->tm_sec);
}

#endif

bool operator==(const DateTime& left, const DateTime& right)
{
    return left.GetDate() == right.GetDate() && left.Seconds() == right.Seconds();
}

bool operator<(const DateTime& left, const DateTime& right)
{
    if (left.GetDate() < right.GetDate()) return true;
    if (left.GetDate() > right.GetDate()) return false;
    return left.Seconds() < right.Seconds();
}

DateTime ParseDateTime(const std::string& dateTimeStr)
{
    int dateEnd = 0;
    Date date = ParseDate(dateTimeStr, dateEnd);
    int hours = 0;
    int mins = 0;
    int secs = 0;
    if (dateTimeStr.length() > dateEnd)
    {
        if (dateTimeStr[dateEnd] == 'T')
        {
            int hoursStart = dateEnd + 1;
            hours = std::stoi(dateTimeStr.substr(hoursStart, 2));
            if (hours < 0 || hours > 24)
            {
                ThrowInvalidDateTime(dateTimeStr);
            }
            if (dateTimeStr.length() > hoursStart + 2)
            {
                int minsStart = hoursStart + 2;
                if (dateTimeStr[minsStart] == ':')
                {
                    ++minsStart;
                }
                mins = std::stoi(dateTimeStr.substr(minsStart, 2));
                if (mins < 0 || mins >= 60)
                {
                    ThrowInvalidDateTime(dateTimeStr);
                }
                if (dateTimeStr.length() > minsStart + 2)
                {
                    int secsStart = minsStart + 2;
                    if (dateTimeStr[secsStart] == ':')
                    {
                        ++secsStart;
                    }
                    secs = std::stoi(dateTimeStr.substr(secsStart, 2));
                    if (secs < 0 || secs > 60) // 60 is valid if leap second exists
                    {
                        ThrowInvalidDateTime(dateTimeStr);
                    }
                }
            }
        }
    }
    int totalSecs = hours * 3600 + mins * 60 + secs;
    return DateTime(date, totalSecs);
}

#ifdef OTAVA

std::int64_t CurrentMs()
{
    return current_ms();
}

int64_t GetCurrentTime()
{
    return current_time();
}

#else

std::int64_t CurrentMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point()).count();
}

int64_t GetCurrentTime()
{
    std::time_t currentTime;
    std::time(&currentTime);
    return currentTime;
}

#endif

int64_t Hours(int64_t nanosecs)
{
    return nanosecs / (3600ll * int64_t(1000000000ll));
}

int64_t Minutes(int64_t nanosecs)
{
    return nanosecs / (60ll * int64_t(1000000000ll));
}

int64_t Seconds(int64_t nanosecs)
{
    return nanosecs / int64_t(1000000000ll);
}

int64_t Milliseconds(int64_t nanosecs)
{
    return nanosecs / int64_t(1000000ll);
}

int64_t Microseconds(int64_t nanosecs)
{
    return nanosecs / int64_t(1000ll);
}

#ifndef OTAVA

std::string DurationStr(const std::chrono::nanoseconds& duration)
{
    std::string s;
    int64_t hh = Hours(duration.count()) % 24;
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(hh / 10 % 10)));
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(hh % 10)));
    s.append(1, ':');
    int64_t mm = Minutes(duration.count()) % 60;
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(mm / 10 % 10)));
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(mm % 10)));
    s.append(1, ':');
    int64_t ss = Seconds(duration.count()) % 60;
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(ss / 10 % 10)));
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(ss % 10)));
    s.append(1, '.');
    int64_t ms = Milliseconds(duration.count()) % 1000;
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(ms / 100 % 10)));
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(ms / 10 % 10)));
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(ms % 10)));
    s.append(1, '.');
    int64_t us = Microseconds(duration.count()) % 1000;
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(us / 100 % 10)));
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(us / 10 % 10)));
    s.append(1, static_cast<char>(static_cast<uint8_t>('0') + static_cast<uint8_t>(us % 10)));
    return s;
}

#endif

#ifdef OTAVA

std::time_t Time()
{
    return current_time();
}

std::time_t MkTime(const DateTime& dt)
{
    Date date = dt.GetDate();
    return make_time(date.Year(), static_cast<int8_t>(date.GetMonth()), date.Day(), dt.Seconds());
}

#else

std::time_t Time()
{
    std::time_t time = std::time(nullptr);
    return time;
}

std::time_t MkTime(const DateTime& dt)
{
    std::time_t time = std::time(nullptr);
    std::tm* ltm = std::localtime(&time);
    std::tm tm{};
    tm.tm_year = dt.GetDate().Year() - 1900;
    tm.tm_mon = static_cast<int>(dt.GetDate().GetMonth()) - static_cast<int>(Month::january);
    tm.tm_mday = dt.GetDate().Day();
    tm.tm_hour = dt.Hours() % 24;
    tm.tm_min = dt.Minutes() % 60;
    tm.tm_sec = dt.Seconds() % 60;
    tm.tm_isdst = ltm->tm_isdst;
    return std::mktime(&tm);
}

#endif

#ifndef OTAVA

std::string TimeToString(std::time_t time)
{
    std::tm* ptm = std::localtime(&time);
    char buffer[20];
    // Format: 15.06.2009 20:20:00
    std::strftime(buffer, 20, "%d.%m.%Y %H:%M:%S", ptm);
    return buffer;
}

DateTime ToDateTime(time_t time)
{
    struct tm* localTime = std::localtime(&time);
    return DateTime(Date(1900 + localTime->tm_year, static_cast<Month>(1 + localTime->tm_mon), static_cast<int8_t>(localTime->tm_mday)), localTime->tm_hour * 3600 + localTime->tm_min * 60 + localTime->tm_sec);
}

#endif

} // namespace util
