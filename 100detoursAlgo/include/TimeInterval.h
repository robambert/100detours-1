#ifndef TIMEINTERVAL_H
#define TIMEINTERVAL_H

#include <iostream>
#include <string>
#include "Time.h"
#include <cpprest/json.h>

class TimeInterval
{
    public:

		TimeInterval();
		TimeInterval(Time start_time, Time end_time);
        TimeInterval(short start_time, short end_time);
        TimeInterval(Time start_time, short duration);
        TimeInterval(web::json::value const& jvalue);

        static TimeInterval GET_FULL_DAY() { return FULL_DAY; }
        static void SET_FULL_DAY(TimeInterval val) { FULL_DAY = val; }

        Time start_time() const { return m_start_time; }
        Time end_time() const { return m_end_time; }

        bool contains(Time const& time) const;

        std::string to_string() const;

		static bool is_valid_json(web::json::value const& jvalue);


    private:

        static TimeInterval FULL_DAY;

        Time m_start_time;
        Time m_end_time;
};

bool operator==(TimeInterval const& interval1, TimeInterval const& interval2);
std::ostream& operator<<(std::ostream &flux, TimeInterval const& interval);

#endif // TIMEINTERVAL_H
