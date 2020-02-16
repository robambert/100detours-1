#include "../include/Inputs.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../include/Utils.h"
#include "../include/Nurse.h"
#include "../include/Patient.h"
#include <cpprest/json.h>

const int Inputs::MIN_TYPES = 1;
const int Inputs::MIN_NURSES = 1;
const int Inputs::MIN_PATIENTS = 1;

Inputs::Inputs(int car_speed, int walk_speed, int min_car_dist)
    :m_car_speed(car_speed), m_walk_speed(walk_speed), m_min_car_dist(min_car_dist)
{}

Inputs::Inputs(int car_speed, int walk_speed, int min_car_dist, std::vector<TreatmentType> types, 
			   std::vector<Nurse> nurses, std::vector<Patient> patients)
    :m_car_speed(car_speed), m_walk_speed(walk_speed), m_min_car_dist(min_car_dist),
    m_types(types), m_nurses(nurses), m_patients(patients)
{}

Inputs::Inputs(web::json::value const& jvalue)
	:m_car_speed(jvalue.at(L"carSpeed").as_integer()),
	m_walk_speed(jvalue.at(L"walkSpeed").as_integer()),
	m_min_car_dist(jvalue.at(L"minCarDist").as_integer())
{
	// Set treatment types
	web::json::array types = jvalue.at(L"treatmentTypes").as_array();
	for (web::json::value const& type: types)
	{
		m_types.push_back(type);
	}

	// Set nurses
	web::json::array nurses = jvalue.at(L"nurses").as_array();
	for (web::json::value const& nurse: nurses)
	{
		m_nurses.push_back(nurse);
	}

	// Set patients
	web::json::array patients = jvalue.at(L"patients").as_array();
	for (web::json::value const& patient: patients)
	{
		m_patients.push_back(patient);
	}
}

int Inputs::patients_per_nurse() const
{
    int nurses = nurses_size();
    if (nurses == 0)
    {
        return 0;
    }
    return patients_size() / nurses;
}

TreatmentType* Inputs::get_type_by_index(int i)
{
    if (i >= 0 && i < types_size())
    {
        return &m_types[i];
    }
    return nullptr;
}

TreatmentType* Inputs::get_type_by_id(unsigned int id)
{
	for (int i = 0, len = types_size(); i < len; ++i)
	{
		if (m_types[i].id() == id)
		{
			return &m_types[i];
		}
	}
	return nullptr;
}

void Inputs::add_type(TreatmentType type)
{
    m_types.push_back(type);
}

Nurse* Inputs::get_nurse(int i)
{
    if (i >= 0 && i < nurses_size())
    {
        return &m_nurses[i];
    }
    return nullptr;
}

void Inputs::add_nurse(Nurse nurse)
{
	m_nurses.push_back(nurse);
}

Patient* Inputs::get_patient(int i)
{
    if (i >= 0 && i < patients_size())
    {
        return &m_patients[i];
    }
    return nullptr;
}

void Inputs::add_patient(Patient patient)
{
    m_patients.push_back(patient);
}

int Inputs::treatments_size() const
{
    int res = 0;
    for (Patient const& patient: m_patients)
    {
        res += patient.treatments_size();
    }
    return res;
}

int Inputs::treatments_left() const
{
    int res = 0;
    for (Patient const& patient: m_patients)
    {
        res += patient.treatments_left();
    }
    return res;
}

void Inputs::set_patient_bonds()
{
    // Reset bonds
	for (int i = 0; i < patients_size(); ++i)
	{
		m_patients[i].bonds(0);
	}

	// Calculate each patient bonds
    for (int i = 0; i < patients_size() - 1; ++i)
    {
		for (int j = i + 1; j < patients_size(); ++j)
        {
            if (distance(m_patients[i].location(), m_patients[j].location()) <= m_min_car_dist)
            {
				m_patients[i].bonds(m_patients[i].bonds() + 1);
                m_patients[j].bonds(m_patients[j].bonds() + 1);
            }
        }
    }
}

void Inputs::set_distances()
{
    m_distances = std::vector<std::vector<int> >(patients_size());
    for (int i = 0; i < patients_size(); ++i)
    {
        std::vector<int> line(patients_size());
        line[i] = 0;
        m_distances[i] = line;
    }
    for (int i = 0; i < patients_size() - 1; ++i)
    {
        for (int j = i + 1; j < patients_size(); ++j)
        {
            int dist = distance(m_patients[i].location(), m_patients[j].location());
            m_distances[i][j] = dist;
            m_distances[j][i] = dist;
        }
    }
}

void Inputs::initialize()
{
	// Sort nurses according to their skills
	std::sort(m_nurses.begin(), m_nurses.end(), CompareNurseSkills());

	// Set patient bonds
	set_patient_bonds();

	// Sort patients according to their bonds
	std::sort(m_patients.begin(), m_patients.end());

	// Set distances
	set_distances();
}

void Inputs::reset()
{
    // Reset nurses position
    for (Nurse& nurse: m_nurses)
    {
        nurse.reset();
    }

    // Reset patients treatments
    for (Patient& patient: m_patients)
    {
        patient.reset();
    }
}

std::string Inputs::to_string() const
{
    // Add consts to the string
    std::string str = "*** Consts ***";
    str = str + "\nCar speed: " + int_to_string(m_car_speed);
    str = str + "\nWalk speed: " + int_to_string(m_walk_speed);
    str = str + "\nMin car speed: " + int_to_string(m_min_car_dist);

    // Add types to the string
    str = str + "\n\n*** " + int_to_string(types_size()) + " Treatment type(s) ***";
    for (int i = 0, len = types_size(); i < len; ++i)
    {
        str = str + "\n" + m_types[i].to_string();
        if (i < len - 1)
        {
            str = str + ",";
        }
    }

    // Add nurses to the string
    str = str + "\n\n*** " + int_to_string(nurses_size()) + " Nurse(s) ***";
    for (int i = 0, len = nurses_size(); i < len; ++i)
    {
        str = str + "\n" + m_nurses[i].to_string();
        if (i < len - 1)
        {
            str = str + ",";
        }
    }

    // Add patients to the string
    str = str + "\n\n*** " + int_to_string(patients_size()) + " Patient(s) ***";
    for (int i = 0, len = patients_size(); i < len; ++i)
    {
        str = str + "\n" + m_patients[i].to_string();
        if (i < len - 1)
        {
            str = str + ",";
        }
    }

    return str;
}

bool Inputs::is_valid_json(web::json::value const& jvalue)
{
	// Check fields
	bool has_fields = jvalue.has_integer_field(L"carSpeed")
				   && jvalue.has_integer_field(L"walkSpeed")
				   && jvalue.has_integer_field(L"minCarDist")
				   && jvalue.has_array_field(L"treatmentTypes")
				   && jvalue.has_array_field(L"nurses")
				   && jvalue.has_array_field(L"patients");
	if (!has_fields)
	{
		return false;
	}
	if (jvalue.at(L"carSpeed").as_integer() <= 0 || jvalue.at(L"walkSpeed").as_integer() <= 0 || jvalue.at(L"minCarDist").as_integer() < 0)
	{
		return false;
	}

	// Check treatmentTypes array
	web::json::array jarray = jvalue.at(L"treatmentTypes").as_array();
	int jarray_size = jarray.size();
	if (jarray_size < MIN_TYPES)
	{
		return false;
	}
	std::vector<unsigned int> type_ids;
	for (int i = 0; i < jarray_size; ++i)
	{
		web::json::value jval = jarray.at(i);
		if (!jval.is_object() || !TreatmentType::is_valid_json(jval))
		{
			return false;
		}
		unsigned int type_id = jval.at(L"_id").as_integer();
		if (find_element<unsigned int>(type_ids, type_id))
		{
			return false;
		}
		type_ids.push_back(type_id);
	}

	// Check nurses array
	jarray = jvalue.at(L"nurses").as_array();
	jarray_size = jarray.size();
	if (jarray_size < MIN_NURSES)
	{
		return false;
	}
	std::vector<unsigned int> nurse_ids;
	for (int i = 0; i < jarray_size; ++i)
	{
		web::json::value jval = jarray.at(i);
		if (!jval.is_object() || !Nurse::is_valid_json(jval))
		{
			return false;
		}
		unsigned int nurse_id = jval.at(L"_id").as_integer();
		if (find_element<unsigned int>(nurse_ids, nurse_id))
		{
			return false;
		}
		nurse_ids.push_back(nurse_id);
	}

	// Check patients array
	jarray = jvalue.at(L"patients").as_array();
	jarray_size = jarray.size();
	if (jarray_size < MIN_PATIENTS)
	{
		return false;
	}
	std::vector<unsigned int> patient_ids;
	for (int i = 0; i < jarray_size; ++i)
	{
		web::json::value jval = jarray.at(i);
		if (!jval.is_object() || !Patient::is_valid_json(jval, type_ids))
		{
			return false;
		}
		unsigned int patient_id = jval.at(L"_id").as_integer();
		if (find_element<unsigned int>(patient_ids, patient_id))
		{
			return false;
		}
		patient_ids.push_back(patient_id);
	}

	return true;
}

std::ostream& operator<<(std::ostream &out, Inputs const& inputs)
{
    out << inputs.to_string();
    return out;
}
