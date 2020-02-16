#include "../include/Appointment.h"
#include <iostream>
#include <string>
#include "../include/Utils.h"
#include "../include/Point.h"
#include "../include/TimeInterval.h"
#include "../include/Treatment.h"
#include <cpprest/json.h>

Appointment::Appointment(unsigned int nurse_id, Patient *patient, unsigned int treatment_id, TimeInterval schedule)
    :m_nurse_id(nurse_id), m_patient(patient), m_treatment_id(treatment_id), m_schedule(schedule)
{}

web::json::value Appointment::to_json() const
{
	web::json::value jvalue = web::json::value::object();
	jvalue[L"nurseId"] = web::json::value::number(m_nurse_id);
	jvalue[L"patientId"] = web::json::value::number(patient_id());
	jvalue[L"treatmentId"] = web::json::value::number(m_treatment_id);
	jvalue[L"schedule"] = web::json::value::number(m_schedule.start_time().minutes());
	return jvalue;
}

std::string Appointment::to_string() const
{
    return "{\n  nurse id: " + int_to_string(m_nurse_id)
         + ",\n  patient: { id: " + int_to_string(m_patient->id()) + ", location: " + location().to_string() + " }"
         + ",\n  treatment id: " + int_to_string(m_treatment_id)
         + ",\n  schedule: " + m_schedule.to_string()
         + "\n}";
}

std::ostream& operator<<(std::ostream &out, Appointment const& appointment)
{
    out << appointment.to_string();
    return out;
}
