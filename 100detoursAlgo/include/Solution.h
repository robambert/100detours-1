#ifndef SOLUTION_H
#define SOLUTION_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>

class Solution
{
    public:

        Solution();
        Solution(int nurses, int patients_per_nurse, std::vector<int> code);
        Solution(Solution const& parent, int crossing_point);
        Solution(Solution const& parent, int crossing_point1, int crossing_point2);
        Solution(Solution const& parent, std::vector<int> const& crossing_points1,
                 std::vector<int> const& crossing_points2, bool keep_between_points = false);

        int nurses() const { return m_nurses; }
        int patients_per_nurse() const { return m_patients_per_nurse; }

        int treatments() const { return m_nurses * m_patients_per_nurse; }

		void distance(int i, int j, int val);

        int fitness() const;
        int fitness(int nurse) const { return m_fitness_per_nurse[nurse]; }
        void fitness(int nurse, int val) { m_fitness_per_nurse[nurse] = val; }

        int get(int i, int j) const { return m_code[i][j]; }
        void set(int i, int j, int val);
		void insert(int i, int j, int val);

		int nurse_patients(int nurse) const { return m_code[nurse].size(); }

        std::pair<int, int> longest_serial_car_distance(int nurse) const;
        std::pair<int, int> longest_serial_walk_distance(int nurse) const;

        void mutate(double rate);

        std::string code_to_string() const;
        std::string fitness_to_string() const;
        std::string to_string() const;


    private:

        void initialize_distances();

    	int m_nurses;
    	int m_patients_per_nurse;
    	std::vector<std::vector<int> > m_code;
    	std::vector<std::vector<int> > m_distances;
    	std::vector<int> m_fitness_per_nurse;

	// Friend function for testing purposes only
	friend void TEST_SET_DISTANCES(Solution &sol, std::vector<int> distances);
};

std::ostream& operator<<(std::ostream &flux, Solution const& sol);

/**
 * Swap two patients of a solution to create a new solution
 * @param sol The template solution
 * @param i The nurse of the first patient to swap
 * @param j The index of the first patient to swap
 * @param k The nurse of the second patient to swap
 * @param l The index of the second patient to swap
 * @return The new solution
 */
Solution swap_patients(Solution const& sol, int i, int j, int k, int l);

/**
 * Insert a patient in a solution to create a new solution
 * @param sol The template solution
 * @param i The nurse where to insert the patient
 * @param j The index where to insert the patient
 * @param val The index of the patient to insert
 * @return The new solution
 */
Solution insert_patient(Solution const& sol, int i, int j, int val);

#endif // SOLUTION_H
