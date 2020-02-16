#ifndef PATIENT_H
#define PATIENT_H

#include <iostream>
#include <string>
#include <vector>
#include "Point.h"
#include "TreatmentType.h"
#include "Treatment.h"
#include <cpprest/json.h>

class Patient
{
    public:

        Patient(unsigned int id);
        Patient(unsigned int id, Point location, std::vector<Treatment> treatments);
        Patient(unsigned int id, Point location, Treatment treatment);
        Patient(unsigned int id, Point location, unsigned int treatment_id, unsigned int type_id);
        Patient(web::json::value const& jvalue);

        Patient& operator=(Patient const& other);

        unsigned int id() const { return m_id; }
        Point location() const { return m_location; }
        std::vector<Treatment> treatments() const { return m_treatments; }
        int bonds() const { return m_bonds; }
        void bonds(int val) { m_bonds = val; }

        int treatments_size() const { return m_treatments.size(); }
        int treatments_left() const;								// deprecated

        Treatment* get_treatment_by_index(int i);
        Treatment* get_treatment_by_id(unsigned int id);
        void add_treatment(Treatment treatment);
        std::vector<unsigned int> get_type_ids() const;

        void reset();

        std::string treatments_to_string() const;
        std::string to_string() const;

		static bool is_valid_json(web::json::value const& jvalue, std::vector<unsigned int> type_ids);


    private:

		static const int MIN_TREATMENTS;

        unsigned int m_id;
        Point m_location;
        std::vector<Treatment> m_treatments;
        int m_bonds;
};

bool operator==(Patient const& patient1, Patient const& patient2);
bool operator<(Patient const& patient1, Patient const& patient2);
std::ostream& operator<<(std::ostream &flux, Patient const& patient);

#endif // PATIENT_H
