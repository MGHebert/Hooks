#include "HiResTime.h"

/***********************************************************************
*    CHiResTimeSpan Methods                                          *
***********************************************************************/
CHiResTimeSpan::CHiResTimeSpan(int days, int hours, int minutes, int seconds, int milliseconds)
{
    m_days = days;
    m_hours = hours;
    m_minutes = minutes;
    m_seconds = seconds;
    m_msec = milliseconds;
    m_timeSpan = GetTotalMilliseconds();
}

CHiResTimeSpan::CHiResTimeSpan(__int64 milliseconds)
{
    m_timeSpan = milliseconds;
    m_msec = milliseconds % 1000;
    __int64 seconds = milliseconds / 1000;
    
    m_seconds = seconds % 60;
    __int64 minutes = seconds / 60;

    m_minutes = minutes % 60;
    __int64 hours = minutes / 60;

    m_hours = hours % 24;
    
    m_days = (int)hours / 24;
}

CHiResTimeSpan::~CHiResTimeSpan(void)
{
    
}

__int64 CHiResTimeSpan::GetDays() const throw()
{
	return( m_timeSpan/(24*3600*1000) );
}

__int64 CHiResTimeSpan::GetTotalHours() const throw()
{
	return( m_timeSpan/(3600*1000) );
}

int CHiResTimeSpan::GetHours() const throw()
{
	return( int( GetTotalHours()-(GetDays()*24) ) );
}

__int64 CHiResTimeSpan::GetTotalMinutes() const throw()
{
	return( m_timeSpan/(60*1000) );
}

int CHiResTimeSpan::GetMinutes() const throw()
{
	return( int( GetTotalMinutes()-(GetTotalHours()*60) ) );
}

__int64  CHiResTimeSpan::GetTotalSeconds() const throw()
{
	return( m_timeSpan / 1000 );
}

int CHiResTimeSpan::GetSeconds() const throw()
{
	return( int( GetTotalSeconds()-(GetTotalMinutes()*60) ) );
}

__int64 CHiResTimeSpan::GetTotalMilliseconds() const throw()
{
    __int64 days = (m_days * 24 * 60 * 60);
    days = days * 1000;
    __int64 hours = (m_hours * 60 * 60 * 1000);
    __int64 minutes = (m_minutes * 60 * 1000);
    __int64 seconds = (m_seconds * 1000);

    return  (days + hours + minutes + seconds + m_msec);
}

int CHiResTimeSpan::GetMilliseconds() const throw()
{
	return( int( GetTotalMilliseconds()-(GetTotalSeconds()*1000) ) );
}

__int64 CHiResTimeSpan::GetTimeSpan() const throw()
{
	return( m_timeSpan );
}

CHiResTimeSpan CHiResTimeSpan::operator+( CHiResTimeSpan span ) const throw()
{
	return( CHiResTimeSpan( m_timeSpan + span.m_timeSpan ) );
}

CHiResTimeSpan CHiResTimeSpan::operator-( CHiResTimeSpan span ) const throw()
{
	return( CHiResTimeSpan( m_timeSpan - span.m_timeSpan ) );
}

CHiResTimeSpan& CHiResTimeSpan::operator+=( CHiResTimeSpan span ) throw()
{
	m_timeSpan += span.m_timeSpan;
	return( *this );
}

CHiResTimeSpan& CHiResTimeSpan::operator-=( CHiResTimeSpan span ) throw()
{
	m_timeSpan -= span.m_timeSpan;
	return( *this );
}

bool CHiResTimeSpan::operator==( CHiResTimeSpan span ) const throw()
{
	return( m_timeSpan == span.m_timeSpan );
}

bool CHiResTimeSpan::operator!=( CHiResTimeSpan span ) const throw()
{
	return( m_timeSpan != span.m_timeSpan );
}

bool CHiResTimeSpan::operator<( CHiResTimeSpan span ) const throw()
{
	return( m_timeSpan < span.m_timeSpan );
}

bool CHiResTimeSpan::operator>( CHiResTimeSpan span ) const throw()
{
	return( m_timeSpan > span.m_timeSpan );
}

bool CHiResTimeSpan::operator<=( CHiResTimeSpan span ) const throw()
{
	return( m_timeSpan <= span.m_timeSpan );
}

bool CHiResTimeSpan::operator>=( CHiResTimeSpan span ) const throw()
{
	return( m_timeSpan >= span.m_timeSpan );
}

/***********************************************************************
*    CHiResTime Methods                                              *
***********************************************************************/
CHiResTime::CHiResTime(void)
{
    Initialise();
    m_time = GetTime();
}

CHiResTime::CHiResTime(bool setToCurrent)
{
    Initialise();
    if (0 == setToCurrent)
    {
        SetToCurrentDateTime();
    }
    m_time = GetTime();
}

CHiResTime::CHiResTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
{
    m_year = year;
    m_month = month;
    m_day = day;
    m_hour = hour;
    m_minute = minute;
    m_second = second;
    m_msec = millisecond;
    m_time = GetTime();
}

CHiResTime::CHiResTime(__int64 milliseconds)
{
     m_time = milliseconds;
     ExpandMillisecondsTime();
}

CHiResTime::~CHiResTime(void)
{
}

void CHiResTime::ExpandMillisecondsTime()
{
    __int64 sec = m_time / 1000;
    CTime t(sec);
    m_year = t.GetYear();
    m_month = t.GetMonth();
    m_day = t.GetDay();
    m_hour = t.GetHour();
    m_minute = t.GetMinute();
    m_second = t.GetSecond();
    m_msec = m_time % 1000;
}

void CHiResTime::Initialise()
{
    m_year = 1970;
    m_month = 1;
    m_day = 1;
    m_hour = 0;
    m_minute = 0;
    m_second = 0;
    m_msec = 0;
}

CHiResTime& CHiResTime::operator=( __int64 time ) throw()
{
	m_time = time;
    ExpandMillisecondsTime();
	return( *this );
}

CHiResTime CHiResTime::operator+(CHiResTimeSpan& ts) const throw()
{
    __int64 msec = ts.GetTotalMilliseconds();
    CHiResTime newTime(m_time + msec);
    return newTime;
}

CHiResTime& CHiResTime::operator+=( CHiResTimeSpan span ) throw()
{
    m_time += span.GetTotalMilliseconds();
    ExpandMillisecondsTime();

	return( *this );
}

CHiResTime CHiResTime::operator-(CHiResTimeSpan& ts) const throw()
{
    __int64 msec = m_time - ts.GetTotalMilliseconds();
    CHiResTime newTime(msec);
    return newTime;
}

CHiResTimeSpan CHiResTime::operator-(CHiResTime& then) const throw()
{
    CHiResTimeSpan ts(m_time - then.GetTime());
    return ts;
}

CHiResTime& CHiResTime::operator-=( CHiResTimeSpan span ) throw()
{
    m_time -= span.GetTotalMilliseconds();
    ExpandMillisecondsTime();

	return( *this );
}

bool CHiResTime::operator==(CHiResTime other) const throw()
{
    return ( m_time == other.m_time );
}

bool CHiResTime::operator!=(CHiResTime other) const throw()
{
    return ( m_time != other.m_time );
}

bool CHiResTime::operator<(CHiResTime other) const throw()
{
    return ( m_time < other.m_time );
}

bool CHiResTime::operator<=(CHiResTime other) const throw()
{
    return ( m_time <= other.m_time );
}

bool CHiResTime::operator>(CHiResTime other) const throw()
{
    return (m_time > other.m_time);
}

bool CHiResTime::operator>=(CHiResTime other) const throw()
{
    return (m_time >= other.m_time);
}

void CHiResTime::SetToCurrentDateTime()
{
    struct _timeb timebuffer;
    if ( 0 == _ftime64_s( &timebuffer ) )
    {
        CTime now( timebuffer.time );
        m_year = now.GetYear();
        m_month = now.GetMonth();
        m_day = now.GetDay();
        m_hour = now.GetHour();
        m_minute = now.GetMinute();
        m_second = now.GetSecond();
        m_msec = timebuffer.millitm;
    }
}

__int64 CHiResTime::GetTime()
{
    CTime t(m_year, m_month, m_day, m_hour, m_minute, m_second);
    return ((1000 * t.GetTime()) + m_msec);
}

int CHiResTime::GetYear()
{
    return m_year;
}

int CHiResTime::GetMonth()
{
    return m_month;
}

int CHiResTime::GetDay()
{
    return m_day;
}

int CHiResTime::GetHour()
{
    return m_hour;
}

int CHiResTime::GetMinute()
{
    return m_minute;
}

int CHiResTime::GetSecond()
{
    return m_second;
}

int CHiResTime::GetMillisecond()
{
    return m_msec;
}

int CHiResTime::GetDayOfWeek()
{
	CTime t(m_time/1000);
	return t.GetDayOfWeek();
}

void CHiResTime::AddSpan(int days, int hours, int mins, int seconds, int milliseconds)
{
    CTimeSpan ts(days, hours, mins, seconds);
    AddTimeSpan(ts, milliseconds);
}

void CHiResTime::AddTimeSpan(CTimeSpan ts, int milliseconds)
{
    __int64 msec = GetTime() + ( (ts.GetTotalSeconds() * 1000) + milliseconds );
    __int64 sec = msec / 1000;
    CTime t(sec);
    m_year = t.GetYear();
    m_month = t.GetMonth();
    m_day = t.GetDay();
    m_hour = t.GetHour();
    m_minute = t.GetMinute();
    m_second = t.GetSecond();
    m_msec = msec % 1000;
}

CAtlString CHiResTime::AsCAtlString()
{
	CAtlString s;
	s.Format("%i-%2i-%2i %2i:%2i:%2i.%3i", m_year, m_month, m_day, m_hour, m_minute, m_second, m_msec);
	return s;
}
