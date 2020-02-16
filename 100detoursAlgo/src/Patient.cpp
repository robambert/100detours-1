#include "../include/Patient.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "../include/Utils.h"
#include "../include/Point.h"
#include "../include/TreatmentType.h"
#include "../include/Treatment.h"
#include <cpprest/json.h>

const int Patient::MIN_TREATMENTS = 1;

Patient::Patient(unsigned int id)
    :m_id(id), m_location(), m_bonds(-1)
{}

Patient::Patient(unsigned int id, Point location, std::vector<Treatment> treatments)
    :m_id(id), m_location(location), m_treatments(treatments), m_bonds(-1)
{}

Patient::Patient(unsigned int id, Point location, Treatment treatment)
    :m_id(id), m_location(location), m_bonds(-1)
{
    m_treatments.push_back(treatment);
}

Patient::Patient(unsigned int id, Point location, unsigned int treatment_id, unsigned int type_id)
    :m_id(id), m_location(location), m_bonds(-1)
{
    m_treatments.push_back(Treatment(treatment_id, type_id, TimeInterval::GET_FULL_DAY()));
}

Patient::Patient(web::json::value const& jvalue)
	:m_id(jvalue.at(L"_id").as_integer()), m_location(jvalue.at(L"location")), m_bonds(-1)
{
	// Set treatments
	web::json::array treatments = jvalue.at(L"treatments").as_array();
	for (web::json::value const& treatment: treatments)
	{
		m_treatments.push_back(treatment);
	}
}

Patient& Patient::operator=(Patient const& other)
{
    if(this != &other)
    {
        m_id = other.id();
        m_location = other.location();
        m_treatments = other.treatments();
        m_bonds = other.bonds();
    }
    return *this;
}

int Patient::treatments_left() const
{
    int res = 0;
    for (Treatment const& treatment: m_treatments)
    {
        if (!treatment.is_scheduled())
        {
            ++res;
        }
    }
    return res;
}

Treatment* Patient::get_treatment_by_index(int i)
{
    if (i >= 0 && i < treatments_size())
    {
        return &m_treatments[i];
    }
    return nullptr;
}

Treatment* Patient::get_treatment_by_id(unsigned int id)
{
    for (int i = 0, len = treatments_size(); i < len; ++i)
    {
        Treatment *treatment = &m_treatments[i];
        if (treatment->id() == id)
        {
            return treatment;
        }
    }
    return nullptr;
}

void Patient::add_treatment(Treatment treatment)
{
    m_treatments.push_back(treatment);
}

std::vector<unsigned int> Patient::get_type_ids() const
{
    std::vector<unsigned int> types;
    for (Treatment const& treatment: m_treatments)
    {
        types.push_back(treatment.type_id());
    }
    return types;
}

void Patient::reset()
{
    for (Treatment& treatment: m_treatments)
    {
        treatment.is_scheduled(false);
    }
}

std::string Patient::treatments_to_string() const
{
    std::string str = "[ ";

    for (int i = 0, len = treatments_size(); i < len; ++i)
    {
        str = str + "{ _id: " + int_to_string(m_treatments[i].id())
            + ", type id: " + int_to_string(m_treatments[i].type_id())
            + ", schedule: " + m_treatments[i].schedule().to_string() + " }";
        if (i < len - 1)
        {
            str = str + ", ";
        }
    }
    str = str + " ]";

    return str;
}

std::string Patient::to_string() const
{
    return "{\n  _id: " + int_to_string(m_id)
         + ",\n  location: " + m_location.to_string()
         + ",\n  treatments: " + treatments_to_string()
		 + ",\n  bonds: " + int_to_string(m_bonds)
         + "\n}";
}

bool Patient::is_valid_json(web::json::value const& jvalue, std::vector<unsigned int> type_ids)
{
	// Check fields
	bool has_fields = jvalue.has_integer_field(L"_id")
				   && jvalue.has_object_field(L"location")
				   && jvalue.has_array_field(L"treatments");
	if (!has_fields)
	{
		return false;
	}
	if (jvalue.at(L"_id").as_integer() < 0 || !Point::is_valid_json(jvalue.at(L"location")))
	{
		return false;
	}

	// Check treatments array
	web::json::array jarray = jvalue.at(L"treatments").as_array();
	int jarray_size = jarray.size();
	if (jarray_size < MIN_TREATMENTS)
	{
		return false;
	}
	std::vector<unsigned int> treatment_ids;
	for (int i = 0; i < jarray_size; ++i)
	{
		web::json::value jval = jarray.at(i);
		if (!jval.is_object() || !Treatment::is_valid_json(jval, type_ids))
		{
			return false;
		}
		unsigned int treatment_id = jval.at(L"_id").as_integer();
		if (find_element<unsigned int>(treatment_ids, treatment_id))
		{
			return false;
		}
		treatment_ids.push_back(treatment_id);
	}

	return true;
}

bool operator==(Patient const& patient1, Patient const& patient2)
{
    return patient1.id() == patient2.id();
}

bool operator<(Patient const& patient1, Patient const& patient2)
{
    return patient1.bonds() < patient2.bonds();
}

std::ostream& operator<<(std::ostream &out, Patient const& patient)
{
    out << patient.to_string();
    return out;
}
