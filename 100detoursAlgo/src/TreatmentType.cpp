#include "../include/TreatmentType.h"
#include <iostream>
#include <string>
#include "../include/Utils.h"
#include "../include/Time.h"
#include <cpprest/json.h>

TreatmentType::TreatmentType(unsigned int id, Time duration)
    :m_id(id), m_duration(duration)
{}

TreatmentType::TreatmentType(unsigned int id, short duration)
    :m_id(id), m_duration(duration)
{}

TreatmentType::TreatmentType(web::json::value const& jvalue)
	:m_id(jvalue.at(L"_id").as_integer()), m_duration(Time(jvalue.at(L"duration").as_integer()))
{}

std::string TreatmentType::to_string() const
{
    return "{ _id: " + int_to_string(m_id)
         + ", duration: " + m_duration.to_string()
         + " }";
}

bool TreatmentType::is_valid_json(web::json::value const& jvalue)
{
	bool has_fields = jvalue.has_integer_field(L"_id")
				   && jvalue.has_integer_field(L"duration");
	if (!has_fields)
	{
		return false;
	}
	if (jvalue.at(L"_id").as_integer() < 0 || jvalue.at(L"duration").as_integer() <= 0)
	{
		return false;
	}
	return true;
}

bool operator==(TreatmentType const& type1, TreatmentType const& type2)
{
    return (type1.id() == type2.id());
}

std::ostream& operator<<(std::ostream &out, TreatmentType const& type)
{
    out << type.to_string();
    return out;
}
