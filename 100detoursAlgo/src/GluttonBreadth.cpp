#include "../include/GluttonBreadth.h"
#include <vector>
#include <queue>
#include "../include/Point.h"
#include "../include/Time.h"
#include "../include/TimeInterval.h"
#include "../include/TreatmentType.h"
#include "../include/Treatment.h"
#include "../include/Nurse.h"
#include "../include/Patient.h"
#include "../include/Appointment.h"
#include "../include/Inputs.h"
#include "../include/Algorithm.h"

GluttonBreadth::GluttonBreadth(Inputs *inputs)
    :Algorithm(inputs)
{}

void GluttonBreadth::find_appointments()
{
    // Initialize nurses priority queue
    std::priority_queue<Nurse*, std::vector<Nurse*>, ComparePointers> nurses_pqueue;
    for (int i = 0, len = m_inputs->nurses_size(); i < len; ++i)
    {
        nurses_pqueue.push(m_inputs->get_nurse(i));
    }

    // Find appointments
    int appointments_left = m_inputs->treatments_size();
    while (!nurses_pqueue.empty() && appointments_left > 0)
    {
        // Get nurse
        Nurse *nurse = nurses_pqueue.top();
        nurses_pqueue.pop();

        // Get nearest appointment available
        Appointment *appointment = nearest_appointment(*nurse);
        if (appointment != nullptr)
        {
            // Set treatment to scheduled
            appointment->treatment()->is_scheduled(true);
            --appointments_left;

            // Add appointment to the list
            m_appointments.push_back(appointment);

            // Update nurse
            nurse->position(appointment->location());
            nurse->available(appointment->schedule().end_time());

            // Check if the nurse has ended her day
            if (nurse->available() < nurse->timetable().end_time())
            {
                // Repush the nurse in the queue
                nurses_pqueue.push(nurse);
            }
        }
    }
}
