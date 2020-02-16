#ifndef TIME_H
#define TIME_H

#include <iostream>
#include <string>
#include "Point.h"

class Time
{
    public:

        static const short MINUTES_MAX;

        Time();
        Time(short minutes);
        Time(short hours, short minutes);

        short minutes() const { return m_minutes; };

        short to_hours() const;
        short to_minutes() const;

        std::string to_string() const;


    private:

        short m_minutes;
};

Time operator+(Time const& time1, Time const& time2);
bool operator==(Time const& time1, Time const& time2);
bool operator<(Time const& time1, Time const& time2);
bool operator<=(Time const& time1, Time const& time2);
std::ostream& operator<<(std::ostream &flux, Time const& time);

Time time_to_go(int distance, int speed); // speed in km/h
Time time_to_go(Point const& from, Point const& to, int speed); // speed in km/h

#endif // TIME_H
