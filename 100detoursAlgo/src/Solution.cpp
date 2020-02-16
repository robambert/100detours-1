#include "../include/Solution.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include "../include/Utils.h"

Solution::Solution()
	:m_nurses(0), m_patients_per_nurse(0)
{}

Solution::Solution(int nurses, int patients_per_nurse, std::vector<int> code)
	:m_nurses(nurses), m_patients_per_nurse(patients_per_nurse), m_code(nurses),
	m_distances(nurses), m_fitness_per_nurse(nurses, -1)
{
	// Initialize distances
	initialize_distances();

	// Initialize code
	int counter = 0;
	for (int i = 0; i < nurses; ++i)
	{
		std::vector<int> line(patients_per_nurse);
		for (int j = 0; j < patients_per_nurse; ++j)
		{
			line[j] = code[counter];
			++counter;
		}
		m_code[i] = line;
	}
}

Solution::Solution(Solution const& parent, int crossing_point)
	:m_nurses(parent.nurses()), m_patients_per_nurse(parent.patients_per_nurse()), m_code(parent.nurses()),
	m_distances(parent.nurses()), m_fitness_per_nurse(parent.nurses(), -1)
{
	// Initialize distances
	initialize_distances();

	// Initialize code
	for (int i = 0; i < m_nurses; ++i)
	{
		std::vector<int> line(m_patients_per_nurse);
		for (int j = 0; j < crossing_point; ++j)
		{
			line[j] = parent.get(i, j);
		}
		m_code[i] = line;
	}
}

Solution::Solution(Solution const& parent, int crossing_point1, int crossing_point2)
	:m_nurses(parent.nurses()), m_patients_per_nurse(parent.patients_per_nurse()), m_code(parent.nurses()),
	m_distances(parent.nurses()), m_fitness_per_nurse(parent.nurses(), -1)
{
	// Initialize distances
	initialize_distances();

	// Initialize code
	for (int i = 0; i < m_nurses; ++i)
	{
		std::vector<int> line(m_patients_per_nurse);
		for (int j = 0; j < crossing_point1; ++j)
		{
			line[j] = parent.get(i, j);
		}
		for (int j = crossing_point2; j < m_patients_per_nurse; ++j)
		{
			line[j] = parent.get(i, j);
		}
		m_code[i] = line;
	}
}

Solution::Solution(Solution const& parent, std::vector<int> const& crossing_points1,
                   std::vector<int> const& crossing_points2, bool keep_between_points)
    :m_nurses(parent.nurses()), m_patients_per_nurse(parent.patients_per_nurse()), m_code(parent.nurses()),
    m_distances(parent.nurses()), m_fitness_per_nurse(parent.nurses(), -1)
{
	// Initialize distances
	initialize_distances();

	// Initialize code
	for (int i = 0; i < m_nurses; ++i)
	{
		std::vector<int> line(m_patients_per_nurse);
		if (keep_between_points)
        {
            for (int j = crossing_points1[i]; j < crossing_points2[i]; ++j)
            {
                line[j] = parent.get(i, j);
            }
        }
        else
        {
            for (int j = 0; j < crossing_points1[i]; ++j)
            {
                line[j] = parent.get(i, j);
            }
            for (int j = crossing_points2[i]; j < m_patients_per_nurse; ++j)
            {
                line[j] = parent.get(i, j);
            }
        }
		m_code[i] = line;
	}
}

void Solution::distance(int i, int j, int val)
{
	if (j < m_patients_per_nurse - 1)
	{
		m_distances[i][j] = val;
	}
}

int Solution::fitness() const
{
    int total_fitness = 0;
    for (int i = 0; i < m_nurses; ++i)
    {
        int nurse_fitness = fitness(i);
        if (nurse_fitness < 0)
        {
            return -1;
        }
        total_fitness += nurse_fitness;
    }
    return total_fitness;
}

void Solution::set(int i, int j, int val)
{
    // Set value in code
    m_code[i][j] = val;

    // Reset nurse fitness
    m_fitness_per_nurse[i] = -1;
}

void Solution::insert(int i, int j, int val)
{
	// Insert value in code
	std::vector<int>::iterator it = m_code[i].begin();
	m_code[i].insert(it + j, val);

	// Reset nurse fitness
	m_fitness_per_nurse[i] = -1;
}

std::pair<int, int> Solution::longest_serial_car_distance(int nurse) const
{
    // Initialize variables
    int best_start_point = 0, best_end_point = 0;
    int current_start_point = 0, current_end_point = 0;
    int max_car_dist = -1, current_car_dist = 0;

    // Find longest serial car distance
    for (int j = 1; j < m_patients_per_nurse; ++j)
    {
        int dist = m_distances[nurse][j - 1];
        if (dist == 0)
        {
            // Update best
            if (current_car_dist > 0 && (current_car_dist > max_car_dist || max_car_dist < 0))
            {
                best_start_point = current_start_point;
                best_end_point = current_end_point;
                max_car_dist = current_car_dist;
            }

            // Reset current
            current_start_point = j;
            current_car_dist = 0;
        }
        else
        {
            // Update current
            current_end_point = j;
            current_car_dist += dist;
        }
    }

    // Update best
    if (current_car_dist > 0 && (current_car_dist > max_car_dist || max_car_dist < 0))
    {
        best_start_point = current_start_point;
        best_end_point = current_end_point;
    }

    // Tune best points
    if (best_end_point > 0)
    {
        if (best_end_point == m_patients_per_nurse - 1)
        {
            ++best_start_point;
            ++best_end_point;
        }
        else if (best_start_point > 0)
        {
            if (best_end_point - best_start_point > 1)
            {
                ++best_start_point;
            }
            else if (random(0, 1) == 1)
            {
                ++best_start_point;
                ++best_end_point;
            }
        }
    }

    // Return points that limit the longest serial car distance
    return std::pair<int, int>(best_start_point, best_end_point);
}

std::pair<int, int> Solution::longest_serial_walk_distance(int nurse) const
{
    // Initialize variables
    int best_start_point = 0, best_end_point = 0;
    int current_start_point = 0, current_end_point = 0;
    int max_walk_dist = -1, current_walk_dist = 0;

    // Find longest serial walk distance
    for (int j = 1; j < m_patients_per_nurse; ++j)
    {
        if (m_distances[nurse][j - 1] > 0)
        {
            // Update best
            if (current_walk_dist > 0 && (current_walk_dist > max_walk_dist || max_walk_dist < 0))
            {
                best_start_point = current_start_point;
                best_end_point = current_end_point;
                max_walk_dist = current_walk_dist;
            }

            // Reset current
            current_start_point = j;
            current_walk_dist = 0;
        }
        else
        {
            // Update current
            current_end_point = j;
            ++current_walk_dist;
        }
    }

    // Update best
    if (current_walk_dist > 0 && (current_walk_dist > max_walk_dist || max_walk_dist < 0))
    {
        best_start_point = current_start_point;
        best_end_point = current_end_point;
    }

    // Tune points
    ++best_end_point;

    // Return points that limit the longest serial walk distance
    return std::pair<int, int>(best_start_point, best_end_point);
}

void Solution::mutate(double rate)
{
	// For each element
	for (int i = 0; i < m_nurses; ++i)
	{
		for (int j = 0; j < m_patients_per_nurse; ++j)
		{
			// Check if there is a mutation
			if (random01() < rate)
			{
				// Swap the value with a random value in the code
				int k = random(0, m_nurses - 1);
				int l = random(0, m_patients_per_nurse - 1);
				int val = m_code[k][l];
				set(k, l, m_code[i][j]);
				set(i, j, val);
			}
		}
	}
}

std::string Solution::code_to_string() const
{
	std::string str = "  ";
	for (int i = 0; i < m_nurses; ++i)
	{
		for (int j = 0; j < nurse_patients(i); ++j)
		{
			str = str + "  " + int_to_string(m_code[i][j]);
		}
		if (i < m_nurses - 1)
		{
			str = str + "\n  ";
		}
	}
	return str;
}

std::string Solution::fitness_to_string() const
{
    std::string str = int_to_string(fitness()) + " { ";
	for (int i = 0; i < m_nurses; ++i)
	{
		if (i < m_nurses - 1)
        {
            str = str + int_to_string(fitness(i)) + ", ";
        }
		else
        {
            str = str + int_to_string(fitness(i)) + " }";
        }
	}
	return str;
}

std::string Solution::to_string() const
{
    return "{\n  nurses: " + int_to_string(m_nurses)
         + ",\n  patients per nurse: " + int_to_string(m_patients_per_nurse)
         + ",\n  fitness: " + fitness_to_string()
         + ",\n  average fitness per nurse: " + int_to_string(fitness() / m_nurses)
         + ",\n  code:\n" + code_to_string()
         + "\n}";
}

void Solution::initialize_distances()
{
    for (int i = 0; i < m_nurses; ++i)
    {
        std::vector<int> line(m_patients_per_nurse - 1, -1);
        m_distances[i] = line;
    }
}

void TEST_SET_DISTANCES(Solution &sol, std::vector<int> distances)
{
	int counter = 0;
	for (int i = 0; i < sol.nurses(); ++i)
	{
		int nurse_fitness = 0;
		for (int j = 0; j < sol.patients_per_nurse() - 1; ++j)
		{
			int dist = distances[counter];
			sol.distance(i, j, dist);
			++counter;
			nurse_fitness += dist;
		}
		sol.m_fitness_per_nurse[i] = nurse_fitness;
	}
}

std::ostream& operator<<(std::ostream &out, Solution const& sol)
{
    out << sol.to_string();
    return out;
}

Solution swap_patients(Solution const& sol, int i, int j, int k, int l)
{
    Solution new_sol = sol;
    int val = new_sol.get(k, l);
    new_sol.set(k, l, new_sol.get(i, j));
    new_sol.set(i, j, val);
    return new_sol;
}

Solution insert_patient(Solution const& sol, int i, int j, int val)
{
	Solution new_sol = sol;
	new_sol.insert(i, j, val);
	return new_sol;
}
