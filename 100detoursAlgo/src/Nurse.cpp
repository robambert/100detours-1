#include "../include/Nurse.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../include/Utils.h"
#include "../include/Point.h"
#include "../include/Time.h"
#include "../include/TimeInterval.h"
#include "../include/TreatmentType.h"
#include <cpprest/json.h>

Nurse::Nurse(unsigned int id, Point starting_point, TimeInterval timetable)
    :m_id(id), m_starting_point(starting_point), m_position(starting_point),
    m_timetable(timetable), m_available(timetable.start_time())
{}

Nurse::Nurse(unsigned int id, unsigned int type_id, Point starting_point, TimeInterval timetable)
    :m_id(id), m_starting_point(starting_point), m_position(starting_point),
    m_timetable(timetable), m_available(timetable.start_time())
{
    m_cannot_do.push_back(type_id);
}

Nurse::Nurse(unsigned int id, std::vector<unsigned int> cannot_do, Point starting_point, TimeInterval timetable)
    :m_id(id), m_cannot_do(cannot_do), m_starting_point(starting_point), m_position(starting_point),
    m_timetable(timetable), m_available(timetable.start_time())
{}

Nurse::Nurse(web::json::value const& jvalue)
	:m_id(jvalue.at(L"_id").as_integer()), m_timetable(jvalue.at(L"startTime").as_integer(), 0)
{
	// Set cannot_do
	web::json::array type_ids = jvalue.at(L"treatmentTypeIds").as_array();
	for (web::json::value const& type_id: type_ids)
	{
		m_cannot_do.push_back(type_id.as_integer());
	}

	// obsolete
	if (jvalue.has_object_field(L"position"))
	{
		m_starting_point = Point(jvalue.at(L"position"));
	}
	else
	{
		m_starting_point = Point::GET_ORIGIN();
	}
	m_position = m_starting_point;

	if (jvalue.has_object_field(L"timetable"))
	{
		m_timetable = TimeInterval(jvalue.at(L"timetable"));
	}

	m_available = m_timetable.start_time();
}

Nurse& Nurse::operator=(Nurse const& other)
{
    if(this != &other)
    {
        m_id = other.id();
        m_cannot_do = other.cannot_do();
        m_starting_point = other.starting_point();
        m_position = other.position();
        m_timetable = other.timetable();
        m_available = other.available();
    }
    return *this;
}

void Nurse::add_treatment_type(unsigned int type_id)
{
    m_cannot_do.push_back(type_id);
}

bool Nurse::can_do(unsigned int type_id) const
{
    return !find_element<unsigned int>(m_cannot_do, type_id);
}

void Nurse::reset()
{
    m_position = m_starting_point;
    m_available = m_timetable.start_time();
}

std::string Nurse::cannot_do_to_string() const
{
    std::string str = "[ ";

    for (int i = 0, len = m_cannot_do.size(); i < len; ++i)
    {
        str = str + int_to_string(m_cannot_do[i]);
        if (i < len - 1)
        {
            str = str + ", ";
        }
    }
    str = str + " ]";

    return str;
}

std::string Nurse::to_string() const
{
    return "{\n  _id: " + int_to_string(m_id)
         + ",\n  cannot do: " + cannot_do_to_string()
         + ",\n  starting point: " + m_starting_point.to_string()
         + ",\n  position: " + m_position.to_string()
         + ",\n  timetable: " + m_timetable.to_string()
         + ",\n  available: " + m_available.to_string()
         + "\n}";
}

bool Nurse::is_valid_json(web::json::value const& jvalue)
{
	// Check fields
	bool has_fields = jvalue.has_integer_field(L"_id")
				   && jvalue.has_array_field(L"treatmentTypeIds")
				   && jvalue.has_integer_field(L"startTime");
	if (!has_fields)
	{
		return false;
	}
	if (jvalue.at(L"_id").as_integer() < 0 || jvalue.at(L"startTime").as_integer() < 0)
	{
		return false;
	}

	// Check treatmentTypeIds array
	web::json::array jarray = jvalue.at(L"treatmentTypeIds").as_array();
	int jarray_size = jarray.size();
	for (int i = 0; i < jarray_size; ++i)
	{
		web::json::value jval = jarray.at(i);
		if (!jval.is_integer() || jval.as_integer() < 0)
		{
			return false;
		}
	}

	// obsolete
	if (jvalue.has_object_field(L"position") && !Point::is_valid_json(jvalue.at(L"position")))
	{
		return false;
	}
	if (jvalue.has_object_field(L"timetable") && !TimeInterval::is_valid_json(jvalue.at(L"timetable")))
	{
		return false;
	}

	return true;
}

bool operator<(Nurse const& nurse1, Nurse const& nurse2)
{
    return nurse2.available() < nurse1.available();
}

std::ostream& operator<<(std::ostream &out, Nurse const& nurse)
{
    out << nurse.to_string();
    return out;
}
