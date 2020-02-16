#include "../include/TimeInterval.h"
#include <iostream>
#include <string>
#include "../include/Time.h"
#include <cpprest/json.h>

TimeInterval TimeInterval::FULL_DAY = TimeInterval(0, Time::MINUTES_MAX);

TimeInterval::TimeInterval()
	:m_start_time(0), m_end_time(Time::MINUTES_MAX)
{}

TimeInterval::TimeInterval(Time start_time, Time end_time)
    :m_start_time(start_time), m_end_time(end_time)
{}

TimeInterval::TimeInterval(short start_time, short end_time)
    :m_start_time(start_time), m_end_time(end_time)
{}

TimeInterval::TimeInterval(Time start_time, short duration)
    :m_start_time(start_time), m_end_time(start_time.minutes() + duration)
{}

TimeInterval::TimeInterval(web::json::value const& jvalue)
	:m_start_time(jvalue.at(L"start").as_integer()), m_end_time(jvalue.at(L"end").as_integer())
{}

bool TimeInterval::contains(Time const& time) const
{
    return (m_start_time <= time) && (time <= m_end_time);
}

std::string TimeInterval::to_string() const
{
    return "(" + m_start_time.to_string() + " -> " + m_end_time.to_string() + ")";
}

bool TimeInterval::is_valid_json(web::json::value const& jvalue)
{
	bool has_fields = jvalue.has_integer_field(L"start")
				   && jvalue.has_integer_field(L"end");
	if (!has_fields)
	{
		return false;
	}
	if (jvalue.at(L"start").as_integer() < 0 || jvalue.at(L"end").as_integer() < 0)
	{
		return false;
	}
	return true;
}

bool operator==(TimeInterval const& interval1, TimeInterval const& interval2)
{
    return (interval1.start_time() == interval2.start_time())
        && (interval1.end_time() == interval2.end_time());
}

std::ostream& operator<<(std::ostream &out, TimeInterval const& interval)
{
    out << interval.to_string();
    return out;
}
