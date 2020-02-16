#include "../include/Treatment.h"
#include <iostream>
#include <string>
#include "../include/Utils.h"
#include "../include/TreatmentType.h"
#include "../include/TimeInterval.h"
#include <cpprest/json.h>

Treatment::Treatment(unsigned int id, unsigned int type_id, TimeInterval schedule)
    :m_id(id), m_type_id(type_id), m_schedule(schedule), m_is_scheduled(false)
{}

Treatment::Treatment(web::json::value const& jvalue)
	:m_id(jvalue.at(L"_id").as_integer()), m_type_id(jvalue.at(L"treatmentTypeId").as_integer()), m_is_scheduled(false)
{
    // obsolete
	if (jvalue.has_object_field(L"schedule"))
    {
        m_schedule = TimeInterval(jvalue.at(L"schedule"));
    }
	else
	{
		m_schedule = TimeInterval();
	}
}

std::string Treatment::to_string() const
{
    // Convert boolean is_scheduled to string
    std::string is_scheduled = "false";
    if (m_is_scheduled)
    {
        is_scheduled = "true";
    }

    return "{ _id: " + int_to_string(m_id)
         + ", type id: " + int_to_string(m_type_id)
         + ", schedule: " + m_schedule.to_string()
         + ", is scheduled: " + is_scheduled
         + " }";
}

bool Treatment::is_valid_json(web::json::value const& jvalue, std::vector<unsigned int> type_ids)
{
	bool has_fields = jvalue.has_integer_field(L"_id")
				   && jvalue.has_integer_field(L"treatmentTypeId");
	if (!has_fields)
	{
		return false;
	}
	if (jvalue.at(L"_id").as_integer() < 0 || !find_element<unsigned int>(type_ids, jvalue.at(L"treatmentTypeId").as_integer()))
	{
		return false;
	}

	// obsolete
	if (jvalue.has_object_field(L"schedule") && !TimeInterval::is_valid_json(jvalue.at(L"schedule")))
	{
		return false;
	}

	return true;
}

bool operator==(Treatment const& treatment1, Treatment const& treatment2)
{
    return (treatment1.id() == treatment2.id());
}

std::ostream& operator<<(std::ostream &out, Treatment const& treatment)
{
    out << treatment.to_string();
    return out;
}
