#ifndef NURSE_H
#define NURSE_H

#include <iostream>
#include <string>
#include <vector>
#include "Point.h"
#include "Time.h"
#include "TimeInterval.h"
#include "TreatmentType.h"
#include <cpprest/json.h>

class Nurse
{
    public:

        Nurse(unsigned int id,
              Point starting_point = Point::GET_ORIGIN(),
              TimeInterval timetable = TimeInterval::GET_FULL_DAY());
        Nurse(unsigned int id,
			  unsigned int type_id,
              Point starting_point = Point::GET_ORIGIN(),
              TimeInterval timetable = TimeInterval::GET_FULL_DAY());
        Nurse(unsigned int id,
              std::vector<unsigned int> cannot_do,
              Point starting_point = Point::GET_ORIGIN(),
              TimeInterval timetable = TimeInterval::GET_FULL_DAY());
        Nurse(web::json::value const& jvalue);

        Nurse& operator=(Nurse const& other);

        unsigned int id() const { return m_id; }
        std::vector<unsigned int> cannot_do() const { return m_cannot_do; }
        Point starting_point() const { return m_starting_point; }			// deprecated
        Point position() const { return m_position; }						// deprecated
        void position(Point const& val) { m_position = val; }				// deprecated
        TimeInterval timetable() const { return m_timetable; }
        Time available() const { return m_available; }						// deprecated
        void available(Time const& val) { m_available = val; }				// deprecated

        void add_treatment_type(unsigned int type_id);
        bool can_do(unsigned int type_id) const;
        void reset();

        std::string cannot_do_to_string() const;
        std::string to_string() const;

		static bool is_valid_json(web::json::value const& jvalue);


    private:

        unsigned int m_id;
        std::vector<unsigned int> m_cannot_do;
        Point m_starting_point;
        Point m_position;
        TimeInterval m_timetable;
        Time m_available;
};

bool operator<(Nurse const& nurse1, Nurse const& nurse2);
std::ostream& operator<<(std::ostream &flux, Nurse const& nurse);

#endif // NURSE_H
