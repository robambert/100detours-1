#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <iostream>
#include <string>
#include <vector>
#include "Utils.h"
#include "Point.h"
#include "Time.h"
#include "Treatment.h"
#include "Patient.h"
#include "Nurse.h"
#include "Appointment.h"
#include "Inputs.h"
#include <cpprest/json.h>

class Algorithm
{
    public:

        Algorithm(Inputs *inputs);

        virtual ~Algorithm();

        int appointments_size() const { return m_appointments.size(); }

        void reset_inputs() const;

		Time duration(unsigned int type_id) const;

        void run();

		web::json::value to_json() const;
        std::string to_string() const;


    protected:

        Treatment* is_available(Patient* patient, Time const& time, Nurse const& nurse) const; // deprecated
        Appointment* nearest_appointment(Nurse const& nurse);								   // deprecated

        virtual void find_appointments() = 0;

        // The inputs
        Inputs *m_inputs;

        // The output
        std::vector<Appointment*> m_appointments;
};

std::ostream& operator<<(std::ostream &out, Algorithm const& algo);

#endif // ALGORITHM_H
