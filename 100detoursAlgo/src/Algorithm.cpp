#include "../include/Algorithm.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "../include/Utils.h"
#include "../include/Point.h"
#include "../include/Time.h"
#include "../include/TimeInterval.h"
#include "../include/TreatmentType.h"
#include "../include/Treatment.h"
#include "../include/Patient.h"
#include "../include/Nurse.h"
#include "../include/Appointment.h"
#include "../include/Inputs.h"
#include <cpprest/json.h>

Algorithm::Algorithm(Inputs *inputs)
    :m_inputs(inputs)
{}

Algorithm::~Algorithm()
{
    // Delete appointments
    for (int i = 0, len = appointments_size(); i < len; ++i)
    {
        delete m_appointments[i];
    }
}

void Algorithm::reset_inputs() const
{
    m_inputs->reset();
}

Time Algorithm::duration(unsigned int type_id) const
{
	return m_inputs->get_type_by_id(type_id)->duration();
}

void Algorithm::run()
{
    // Find appointments
    find_appointments();

    // Reset inputs
    reset_inputs();
}

web::json::value Algorithm::to_json() const
{
	web::json::value jvalue = web::json::value::object();
	int len = appointments_size();
	web::json::value appointments = web::json::value::array(len);
	for (int i = 0; i < len; ++i)
	{
		appointments[i] = m_appointments[i]->to_json();
	}
	jvalue[L"appointments"] = appointments;
	return jvalue;
}

std::string Algorithm::to_string() const
{
    std::string str = "*** Appointments ***";

    for (int i = 0, len_nurses = m_inputs->nurses_size(); i < len_nurses; ++i)
    {
        unsigned int nurse_id = m_inputs->get_nurse(i)->id();
        str = str + "\n\nNurse id " + int_to_string(nurse_id) + ": [";
        bool first = true;
        for (int j = 0, len_appointments = appointments_size(); j < len_appointments; ++j)
        {
            Appointment *appointment = m_appointments[j];
            if (appointment->nurse_id() == nurse_id)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    str = str + ",";
                }
                str = str + "\n" + appointment->to_string();
            }

        }
        str = str + " ]";
    }

    return str;
}

Treatment* Algorithm::is_available(Patient *patient, Time const& time, Nurse const& nurse) const
{
    for (int i = 0, len = patient->treatments_size(); i < len; ++i)
    {
        Treatment *treatment = patient->get_treatment_by_index(i);
        if (!treatment->is_scheduled() && treatment->schedule().contains(time) && nurse.can_do(treatment->type_id()))
        {
            return treatment;
        }
    }
    return nullptr;
}

Appointment* Algorithm::nearest_appointment(Nurse const& nurse)
{
    Appointment *appointment = nullptr;
    int min_distance = -1;

    for (int i = 0, len = m_inputs->patients_size(); i < len; ++i)
    {
        Patient *patient = m_inputs->get_patient(i);
        int dist = distance(nurse.position(), patient->location());
        if (dist < min_distance || min_distance < 0)
        {
            Time arrival = nurse.available() + time_to_go(dist, m_inputs->car_speed());
            Treatment *treatment = is_available(patient, arrival, nurse);
            if (treatment != nullptr)
            {
                Time end_time = arrival + duration(treatment->type_id());
                if (end_time <= nurse.timetable().end_time())
                {
                    appointment = new Appointment(nurse.id(), patient, treatment->id(), TimeInterval(arrival, end_time));
                    min_distance = dist;
                }
            }
        }
    }

    return appointment;
}

std::ostream& operator<<(std::ostream &out, Algorithm const& algo)
{
    out << algo.to_string();
    return out;
}
