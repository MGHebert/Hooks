#pragma once

#include <sys/timeb.h>
#include <time.h>
#include <atltime.h>

class CHiResTimeSpan
{
private:
    int m_days;
    int m_hours;
    int m_minutes;
    int m_seconds;
    int m_msec;
    __int64 m_timeSpan;
public:
    CHiResTimeSpan(int days, int hours, int minutes, int seconds, int milliseconds);
    CHiResTimeSpan(__int64 milliseconds);
    ~CHiResTimeSpan(void);
    __int64 GetDays() const throw();
	__int64 GetTotalHours() const throw();
	int GetHours() const throw();
	__int64 GetTotalMinutes() const throw();
	int GetMinutes() const throw();
	__int64 GetTotalSeconds() const throw();
	int GetSeconds() const throw();
    __int64 GetTotalMilliseconds() const throw();
	int GetMilliseconds() const throw();
	__int64 GetTimeSpan() const throw();
	CHiResTimeSpan operator+( CHiResTimeSpan span ) const throw();
    CHiResTimeSpan operator-( CHiResTimeSpan span ) const throw();
    CHiResTimeSpan& operator+=( CHiResTimeSpan span ) throw();
	CHiResTimeSpan& operator-=( CHiResTimeSpan span ) throw();
	bool operator==( CHiResTimeSpan span ) const throw();
	bool operator!=( CHiResTimeSpan span ) const throw();
	bool operator<( CHiResTimeSpan span ) const throw();
	bool operator>( CHiResTimeSpan span ) const throw();
	bool operator<=( CHiResTimeSpan span ) const throw();
	bool operator>=( CHiResTimeSpan span ) const throw();
};

class CHiResTime
{
private:
    int m_year;
    int m_month;
    int m_day;
    int m_hour;
    int m_minute;
    int m_second;
    int m_msec;
    __int64 m_time;
    void ExpandMillisecondsTime();
    void Initialise();
public:
    CHiResTime(void);
    CHiResTime(bool setToCurrent);
    CHiResTime(int year, int month, int day, int hour, int minute, int second, int millisecond);
    CHiResTime(__int64 milliseconds);
    ~CHiResTime(void);

    CHiResTime& operator=( __int64 time ) throw();
    CHiResTime operator+(CHiResTimeSpan& ts) const throw();
    CHiResTime& operator+=( CHiResTimeSpan span ) throw();
    CHiResTime operator-(CHiResTimeSpan& ts) const throw();
    CHiResTimeSpan operator-(CHiResTime& then) const throw();
    CHiResTime& operator-=( CHiResTimeSpan span ) throw();
    bool operator==(CHiResTime other) const throw();
    bool operator!=(CHiResTime other) const throw();
    bool operator<(CHiResTime other) const throw();
    bool operator<=(CHiResTime other) const throw();
    bool operator>(CHiResTime other) const throw();
    bool operator>=(CHiResTime other) const throw();

    __int64 GetTime();
    void SetToCurrentDateTime();
    int GetYear();
    int GetMonth();
    int GetDay();
    int GetHour();
    int GetMinute();
    int GetSecond();
    int GetMillisecond();
	int GetDayOfWeek();
    void AddSpan(int days, int hours, int mins, int seconds, int milliseconds);
    void AddTimeSpan(CTimeSpan ts, int milliseconds = 0);
	CAtlStringW AsCAtlString();
};
