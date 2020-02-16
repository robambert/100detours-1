#ifndef INPUTS_H
#define INPUTS_H

#include <iostream>
#include <string>
#include <vector>
#include "Nurse.h"
#include "Patient.h"
#include <cpprest/json.h>

class Inputs
{
    public:

        Inputs(int car_speed, int walk_speed, int min_car_dist);
        Inputs(int car_speed, int walk_speed, int min_car_dist, std::vector<TreatmentType> types, 
			   std::vector<Nurse> nurses, std::vector<Patient> patients);
		Inputs(web::json::value const& jvalue);

        int car_speed() const { return m_car_speed; }
        int walk_speed() const { return m_walk_speed; }
        int min_car_dist() const { return m_min_car_dist; }
        std::vector<TreatmentType> types() const { return m_types; }
        std::vector<Nurse> nurses() const { return m_nurses; }
        std::vector<Patient> patients() const { return m_patients; }
        int distance_between_patients(int i, int j) const { return m_distances[i][j]; }

        int types_size() const { return m_types.size(); }
        int nurses_size() const { return m_nurses.size(); }
        int patients_size() const { return m_patients.size(); }
        int patients_per_nurse() const;

        TreatmentType* get_type_by_index(int i);
		TreatmentType* get_type_by_id(unsigned int id);
        void add_type(TreatmentType type);
        Nurse* get_nurse(int i);
        void add_nurse(Nurse nurse);
        Patient* get_patient(int i);
        void add_patient(Patient patient);

        int treatments_size() const;
        int treatments_left() const; // deprecated

        void set_patient_bonds();
        void set_distances();
		void initialize();

        void reset();

        std::string to_string() const;

		static bool is_valid_json(web::json::value const& jvalue);


    private:

		static const int MIN_TYPES;
		static const int MIN_NURSES;
		static const int MIN_PATIENTS;

        const int m_car_speed;
        const int m_walk_speed;
        const int m_min_car_dist;

        std::vector<TreatmentType> m_types;
        std::vector<Nurse> m_nurses;
        std::vector<Patient> m_patients;

        std::vector<std::vector<int> > m_distances;

        /**
		 * Class used to compare nurses according to their skills
		 */
		class CompareNurseSkills
		{
		    public:

		        bool operator()(Nurse const& nurse1, Nurse const& nurse2) { return nurse1.cannot_do().size() > nurse2.cannot_do().size(); }
		};
};

std::ostream& operator<<(std::ostream &flux, Inputs const& inputs);

#endif // INPUTS_H
