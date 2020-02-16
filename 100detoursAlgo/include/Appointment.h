#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <iostream>
#include <string>
#include "Point.h"
#include "TimeInterval.h"
#include "Treatment.h"
#include "Patient.h"
#include <cpprest/json.h>

class Appointment
{
    public:

        Appointment(unsigned int nurse_id, Patient *patient, unsigned int treatment_id, TimeInterval schedule);

        unsigned int nurse_id() const { return m_nurse_id; }
        Patient* patient() const { return m_patient; }
        unsigned int treatment_id() const { return m_treatment_id; }
        TimeInterval schedule() const { return m_schedule; }

        unsigned int patient_id() const { return m_patient->id(); }
        Point location() const { return m_patient->location(); }
        Treatment* treatment() { return m_patient->get_treatment_by_id(m_treatment_id); }

		web::json::value to_json() const;
        std::string to_string() const;


    private:

        const unsigned int m_nurse_id;
        Patient *m_patient;
        const unsigned int m_treatment_id;
        const TimeInterval m_schedule;
};

std::ostream& operator<<(std::ostream &flux, Appointment const& appointment);

#endif // APPOINTMENT_H
