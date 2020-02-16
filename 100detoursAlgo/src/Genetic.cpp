#include "../include/Genetic.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <utility>
#include <thread>
#include <mutex>
#include "../include/Utils.h"
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
#include "../include/Solution.h"

const int Genetic::DEFAULT_POPULATION_SIZE = 100;
const double Genetic::DEFAULT_CROSSOVER_RATE = 0.5;
const double Genetic::DEFAULT_MUTATION_RATE = 0.006;
const int Genetic::DEFAULT_MEX_GENERATIONS = 2000;
const int Genetic::DEFAULT_SELECTION_METHOD = 1;
const int Genetic::DEFAULT_CROSSOVER_METHOD = 4;

Genetic::Genetic(Inputs *inputs, int population_size, double crossover_rate, double mutation_rate, int max_generations)
    :Algorithm(inputs), m_population_size((population_size / 4) * 4),
    m_crossover_rate(crossover_rate), m_mutation_rate(mutation_rate),
    m_max_generations(max_generations), m_best_fitness(-1)
{}

int Genetic::best_path(int nurse_index) const
{
    for (int i = 0; i < nurses(); ++i)
    {
        if (m_nurse_indexes[i] == nurse_index)
        {
            return i;
        }
    }
    return -1;
}

int Genetic::nurse_treatments(Solution const& sol, int nurse) const
{
	int counter = 0;
	for (int j = 0; j < sol.nurse_patients(nurse); ++j)
	{
		counter += m_inputs->get_patient(j)->treatments_size();
	}
	return counter;
}

int Genetic::fitness(Solution &sol, Inputs *inputs)
{
	// Initialize total fitness
	int total_fitness = 0;

	// Calculate the total car distance for this solution
	for (int i = 0; i < sol.nurses(); ++i)
	{
		// Initialize nurse fitness
		int nurse_fitness = sol.fitness(i);

		// Check if nurse fitness is already calculated
		if (nurse_fitness >= 0)
        {
            total_fitness += nurse_fitness;
            continue;
        }
        nurse_fitness = 0;

        // Calculate nurse fitness
		int current = sol.get(i, 0);
		int car = current;
		for (int j = 1; j < sol.nurse_patients(i); ++j)
		{
			int destination = sol.get(i, j);
			int dist = inputs->distance_between_patients(current, destination);
			if (dist > inputs->min_car_dist())
			{
				dist = inputs->distance_between_patients(car, destination);
				if (dist > inputs->min_car_dist())
                {
                    nurse_fitness += dist;
                    car = destination;

                    // Update distance in solution
                    sol.distance(i, j - 1, dist);
                }
                else
                {
                    // Update distance in solution
                    sol.distance(i, j - 1, 0);
                }
			}
			else
            {
                // Update distance in solution
                sol.distance(i, j - 1, 0);
            }
			current = destination;
		}

		// Update nurse fitness in solution
        sol.fitness(i, nurse_fitness);

        // Update total fitness
        total_fitness += nurse_fitness;
	}

	// Return total fitness
	return total_fitness;
}

std::vector<Solution> Genetic::generate_population(int population_size, int nurses, int patients_per_nurse)
{
	// Initialize population vector
	std::vector<Solution> population(population_size);

	// Generate a random initial population
	for (int i = 0; i < population_size; ++i)
	{
		// Generate a random solution
		Solution sol(nurses, patients_per_nurse, generate_random_permutation(nurses * patients_per_nurse));

		// Add it in the population
		population[i] = sol;
	}

	// Return population
	return population;
}

std::pair<Solution, Solution> Genetic::crossover(Solution const& parent1, Solution const& parent2, double rate, int method)
{
	// Initialize variables
	int nurses = parent1.nurses();
	int patients_per_nurse = parent1.patients_per_nurse();
	std::pair<Solution, Solution> children;

	switch (method)
	{
		// Single-point crossover
		case 0:
		{
			// Get crossing point
			int crossing_point = (int) std::floor((1.0f - rate) * ((double) patients_per_nurse));
			if (crossing_point == patients_per_nurse)
			{
				--crossing_point;
			}

			// Initialize children
			Solution child1(parent1, crossing_point);
			Solution child2(parent2, crossing_point);

			// Get unchanged values
			std::vector<int> parent1_buf;
			std::vector<int> parent2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				for (int j = 0; j < crossing_point; ++j)
				{
					parent1_buf.push_back(parent1.get(i, j));
					parent2_buf.push_back(parent2.get(i, j));
				}
			}

			// Get values to append
			std::vector<int> child1_buf;
			std::vector<int> child2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				for (int j = 0; j < patients_per_nurse; ++j)
				{
					int val = parent2.get(i, j);
					if (!find_element<int>(parent1_buf, val))
					{
						child1_buf.push_back(val);
					}
					val = parent1.get(i, j);
					if (!find_element<int>(parent2_buf, val))
					{
						child2_buf.push_back(val);
					}
				}
			}

			// Fill children with child buffers
			int counter = 0;
			for (int i = 0; i < nurses; ++i)
			{
				for (int j = crossing_point; j < patients_per_nurse; ++j)
				{
					child1.set(i, j, child1_buf[counter]);
					child2.set(i, j, child2_buf[counter]);
					++counter;
				}
			}

			// Put children in the pair
			children.first = child1;
			children.second = child2;

			break;
		}

		// Two-point crossover
		case 1:
		{
			// Get crossing length
			int crossing_length = (int) std::ceil(rate * ((double) patients_per_nurse));
			if (crossing_length == 0)
			{
				++crossing_length;
			}

            // Generate crossing points
			int crossing_point1 = random(0, patients_per_nurse - crossing_length);
			int crossing_point2 = crossing_point1 + crossing_length;

			// Initialize children
			Solution child1(parent1, crossing_point1, crossing_point2);
			Solution child2(parent2, crossing_point1, crossing_point2);

			// Get unchanged values
			std::vector<int> parent1_buf;
			std::vector<int> parent2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				for (int j = 0; j < crossing_point1; ++j)
				{
					parent1_buf.push_back(parent1.get(i, j));
					parent2_buf.push_back(parent2.get(i, j));
				}
				for (int j = crossing_point2; j < patients_per_nurse; ++j)
				{
					parent1_buf.push_back(parent1.get(i, j));
					parent2_buf.push_back(parent2.get(i, j));
				}
			}

			// Get values to append
			std::vector<int> child1_buf;
			std::vector<int> child2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				for (int j = 0; j < patients_per_nurse; ++j)
				{
					int val = parent2.get(i, j);
					if (!find_element<int>(parent1_buf, val))
					{
						child1_buf.push_back(val);
					}
					val = parent1.get(i, j);
					if (!find_element<int>(parent2_buf, val))
					{
						child2_buf.push_back(val);
					}
				}
			}

			// Fill children with child buffers
			int counter = 0;
			for (int i = 0; i < nurses; ++i)
			{
				for (int j = crossing_point1; j < crossing_point2; ++j)
				{
					child1.set(i, j, child1_buf[counter]);
					child2.set(i, j, child2_buf[counter]);
					++counter;
				}
			}

			// Put children in the pair
			children.first = child1;
			children.second = child2;

			break;
		}

		// Two-point crossover randomized for each nurse
		case 2:
		{
			// Get crossing length
			int crossing_length = (int) std::ceil(rate * ((double) patients_per_nurse));
			if (crossing_length == 0)
			{
				++crossing_length;
			}

            // Generate crossing points for each nurse
            std::vector<int> crossing_points1_parent1(nurses);
            std::vector<int> crossing_points2_parent1(nurses);
            std::vector<int> crossing_points1_parent2(nurses);
            std::vector<int> crossing_points2_parent2(nurses);
            for (int i = 0; i < nurses; ++i)
            {
                // For parent1
                int crossing_point = random(0, patients_per_nurse - crossing_length);
                crossing_points1_parent1[i] = crossing_point;
                crossing_points2_parent1[i] = crossing_point + crossing_length;

                // For parent2
                crossing_point = random(0, patients_per_nurse - crossing_length);
                crossing_points1_parent2[i] = crossing_point;
                crossing_points2_parent2[i] = crossing_point + crossing_length;
            }

			// Initialize children
			Solution child1(parent1, crossing_points1_parent1, crossing_points2_parent1);
			Solution child2(parent2, crossing_points1_parent2, crossing_points2_parent2);

			// Get unchanged values
			std::vector<int> parent1_buf;
			std::vector<int> parent2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				// For parent1
				for (int j = 0; j < crossing_points1_parent1[i]; ++j)
				{
					parent1_buf.push_back(parent1.get(i, j));
				}
				for (int j = crossing_points2_parent1[i]; j < patients_per_nurse; ++j)
				{
					parent1_buf.push_back(parent1.get(i, j));
				}

				// For parent2
				for (int j = 0; j < crossing_points1_parent2[i]; ++j)
				{
					parent2_buf.push_back(parent2.get(i, j));
				}
				for (int j = crossing_points2_parent2[i]; j < patients_per_nurse; ++j)
				{
					parent2_buf.push_back(parent2.get(i, j));
				}
			}

			// Get values to append
			std::vector<int> child1_buf;
			std::vector<int> child2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				for (int j = 0; j < patients_per_nurse; ++j)
				{
					int val = parent2.get(i, j);
					if (!find_element<int>(parent1_buf, val))
					{
						child1_buf.push_back(val);
					}
					val = parent1.get(i, j);
					if (!find_element<int>(parent2_buf, val))
					{
						child2_buf.push_back(val);
					}
				}
			}

			// Fill children with child buffers
			int counter1 = 0, counter2 = 0;
			for (int i = 0; i < nurses; ++i)
			{
				// For child1
				for (int j = crossing_points1_parent1[i]; j < crossing_points2_parent1[i]; ++j)
				{
					child1.set(i, j, child1_buf[counter1]);
					++counter1;
				}

				// For child2
				for (int j = crossing_points1_parent2[i]; j < crossing_points2_parent2[i]; ++j)
				{
					child2.set(i, j, child2_buf[counter2]);
					++counter2;
				}
			}

			// Put children in the pair
			children.first = child1;
			children.second = child2;

			break;
		}

		// Two-point crossover cutting longest car distance
		case 3:
		{
			// Generate crossing points for each nurse
            std::vector<int> crossing_points1_parent1(nurses);
            std::vector<int> crossing_points2_parent1(nurses);
            std::vector<int> crossing_points1_parent2(nurses);
            std::vector<int> crossing_points2_parent2(nurses);
            for (int i = 0; i < nurses; ++i)
            {
                // For parent1
                std::pair<int, int> points(parent1.longest_serial_car_distance(i));
                crossing_points1_parent1[i] = points.first;
                crossing_points2_parent1[i] = points.second;

                // For parent2
                points = parent2.longest_serial_car_distance(i);
                crossing_points1_parent2[i] = points.first;
                crossing_points2_parent2[i] = points.second;
            }

			// Initialize children
			Solution child1(parent1, crossing_points1_parent1, crossing_points2_parent1);
			Solution child2(parent2, crossing_points1_parent2, crossing_points2_parent2);

			// Get unchanged values
			std::vector<int> parent1_buf;
			std::vector<int> parent2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				// For parent1
				for (int j = 0; j < crossing_points1_parent1[i]; ++j)
				{
					parent1_buf.push_back(parent1.get(i, j));
				}
				for (int j = crossing_points2_parent1[i]; j < patients_per_nurse; ++j)
				{
					parent1_buf.push_back(parent1.get(i, j));
				}

				// For parent2
				for (int j = 0; j < crossing_points1_parent2[i]; ++j)
				{
					parent2_buf.push_back(parent2.get(i, j));
				}
				for (int j = crossing_points2_parent2[i]; j < patients_per_nurse; ++j)
				{
					parent2_buf.push_back(parent2.get(i, j));
				}
			}

			// Get values to append
			std::vector<int> child1_buf;
			std::vector<int> child2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				for (int j = 0; j < patients_per_nurse; ++j)
				{
					int val = parent2.get(i, j);
					if (!find_element<int>(parent1_buf, val))
					{
						child1_buf.push_back(val);
					}
					val = parent1.get(i, j);
					if (!find_element<int>(parent2_buf, val))
					{
						child2_buf.push_back(val);
					}
				}
			}

			// Fill children with child buffers
			std::vector<int> range1(generate_random_permutation(child1_buf.size()));
			std::vector<int> range2(generate_random_permutation(child2_buf.size()));
			int counter1 = 0, counter2 = 0;
			for (int i = 0; i < nurses; ++i)
			{
				// For child1
				for (int j = crossing_points1_parent1[i]; j < crossing_points2_parent1[i]; ++j)
				{
					child1.set(i, j, child1_buf[range1[counter1]]);
					++counter1;
				}

				// For child2
				for (int j = crossing_points1_parent2[i]; j < crossing_points2_parent2[i]; ++j)
				{
					child2.set(i, j, child2_buf[range2[counter2]]);
					++counter2;
				}
			}

			// Put children in the pair
			children.first = child1;
			children.second = child2;

			break;
		}

		// Two-point crossover keeping longest walk distance
		case 4:
		{
			// Generate crossing points for each nurse
            std::vector<int> crossing_points1_parent1(nurses);
            std::vector<int> crossing_points2_parent1(nurses);
            std::vector<int> crossing_points1_parent2(nurses);
            std::vector<int> crossing_points2_parent2(nurses);
            for (int i = 0; i < nurses; ++i)
            {
                // For parent1
                std::pair<int, int> points(parent1.longest_serial_walk_distance(i));
                crossing_points1_parent1[i] = points.first;
                crossing_points2_parent1[i] = points.second;

                // For parent2
                points = parent2.longest_serial_walk_distance(i);
                crossing_points1_parent2[i] = points.first;
                crossing_points2_parent2[i] = points.second;
            }

			// Initialize children
			Solution child1(parent1, crossing_points1_parent1, crossing_points2_parent1, true);
			Solution child2(parent2, crossing_points1_parent2, crossing_points2_parent2, true);

			// Get unchanged values
			std::vector<int> parent1_buf;
			std::vector<int> parent2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				// For parent1
				for (int j = crossing_points1_parent1[i]; j < crossing_points2_parent1[i]; ++j)
				{
					parent1_buf.push_back(parent1.get(i, j));
				}

				// For parent2
				for (int j = crossing_points1_parent2[i]; j < crossing_points2_parent2[i]; ++j)
				{
					parent2_buf.push_back(parent2.get(i, j));
				}
			}

			// Get values to append
			std::vector<int> child1_buf;
			std::vector<int> child2_buf;
			for (int i = 0; i < nurses; ++i)
			{
				for (int j = 0; j < patients_per_nurse; ++j)
				{
					int val = parent2.get(i, j);
					if (!find_element<int>(parent1_buf, val))
					{
						child1_buf.push_back(val);
					}
					val = parent1.get(i, j);
					if (!find_element<int>(parent2_buf, val))
					{
						child2_buf.push_back(val);
					}
				}
			}

			// Fill children with child buffers
			int counter1 = 0, counter2 = 0;
			for (int i = 0; i < nurses; ++i)
			{
				// For child1
				for (int j = 0; j < crossing_points1_parent1[i]; ++j)
				{
					child1.set(i, j, child1_buf[counter1]);
					++counter1;
				}
				for (int j = crossing_points2_parent1[i]; j < patients_per_nurse; ++j)
				{
					child1.set(i, j, child1_buf[counter1]);
					++counter1;
				}

				// For child2
				for (int j = 0; j < crossing_points1_parent2[i]; ++j)
				{
					child2.set(i, j, child2_buf[counter2]);
					++counter2;
				}
				for (int j = crossing_points2_parent2[i]; j < patients_per_nurse; ++j)
				{
					child2.set(i, j, child2_buf[counter2]);
					++counter2;
				}
			}

			// Put children in the pair
			children.first = child1;
			children.second = child2;

			break;
		}

		default:
			break;
	}

	// Return children
	return children;
}

void Genetic::generate_population()
{
	m_population = generate_population(m_population_size, nurses(), patients_per_nurse());
}

void Genetic::selection(std::vector<Solution> &new_population, int method)
{
	switch (method)
	{
		// Random selection
		case 0:
		{
			std::vector<int> range(random_vector(0, m_population_size - 1, m_population_size / 2));
			for (int i: range)
			{
				new_population.push_back(m_population[i]);
			}
			break;
		}

		// Rank selection
		case 1:
		{
			/*// Calculate each solution fitness in different threads
			int pop_size = m_population_size;
			std::vector<Solution> *pop = &m_population;
			Inputs *inputs = m_inputs;
			std::thread t1([pop_size, pop, inputs]()
			{
				for (int i = 0; i < pop_size; i += 4)
				{
					fitness((*pop)[i], inputs);
				}
			});
			std::thread t2([pop_size, pop, inputs]()
			{
				for (int i = 1; i < pop_size; i += 4)
				{
					fitness((*pop)[i], inputs);
				}
			});
			std::thread t3([pop_size, pop, inputs]()
			{
				for (int i = 2; i < pop_size; i += 4)
				{
					fitness((*pop)[i], inputs);
				}
			});
			std::thread t4([pop_size, pop, inputs]()
			{
				for (int i = 3; i < pop_size; i += 4)
				{
					fitness((*pop)[i], inputs);
				}
			});
			t1.join();
			t2.join();
			t3.join();
			t4.join();
			//*/

			// Select solutions with best fitness
			std::sort(m_population.begin(), m_population.end(), CompareSolutions(m_inputs));
			std::vector<int> range(generate_random_permutation(m_population_size / 2));
			for (int i: range)
			{
				new_population.push_back(m_population[i]);
			}
			break;
		}

		// Proportionate selection
		case 2:
		{
			// Get min and max fitness
			int fitness_min = std::min_element(m_population.begin(), m_population.end(), CompareSolutions(m_inputs))->fitness();
			int fitness_max = std::max_element(m_population.begin(), m_population.end(), CompareSolutions(m_inputs))->fitness();
			int inverser = fitness_max + fitness_min + 1;

			// Calculate the sum of all inversed fitnesses to normalize
			int total_fitness = 0;
			for (int i = 0; i < m_population_size; ++i)
            {
                total_fitness += inverser - fitness(m_population[i], m_inputs);
            }

            // Calculate the probability for each solution to get selected
			std::vector<double> proba(m_population_size);
			for (int i = 0; i < m_population_size; ++i)
            {
                proba[i] = ((double) (inverser - m_population[i].fitness())) / ((double) total_fitness);
            }

            // Proceed the stochastic selection
            int counter = 0, index = 0;
            std::vector<int> range(generate_random_permutation(m_population_size));
            std::vector<Solution*> selected_solutions(m_population_size / 2);
            while (counter < m_population_size / 2)
            {
                if (random01() < proba[range[index % m_population_size]])
                {
                    selected_solutions[counter] = &m_population[range[index % m_population_size]];
                    ++counter;
                }
                ++index;
            }

            // Push the selected solutions in new population in a random order
			range = generate_random_permutation(m_population_size / 2);
			for (int i: range)
			{
				new_population.push_back(*selected_solutions[i]);
			}
			break;
		}

		// Tournament selection
		case 3:
		{
			// Choose two random solutions and push the fitest in new population
			std::vector<int> range(generate_random_permutation(m_population_size));
			for (int i = 0; i < m_population_size; i += 2)
			{
				if (fitness(m_population[range[i]], m_inputs) < fitness(m_population[range[i + 1]], m_inputs))
                {
                    new_population.push_back(m_population[range[i]]);
                }
				else
                {
                    new_population.push_back(m_population[range[i + 1]]);
                }
			}
			break;
		}

		default:
			break;
	}
}

void Genetic::crossover(std::vector<Solution> &new_population, double rate, int method) const
{
	/*// Generate children in different threads
	int pop_size = m_population_size / 4;
	std::mutex new_pop_lock;
	std::thread t1([pop_size, &new_population, rate, method, &new_pop_lock]()
	{
		int psize = pop_size / 4;
		for (int i = 0; i < psize; ++i)
		{
			// Recombination between two selected solutions
			std::pair<Solution, Solution> children(crossover(new_population[i], new_population[i + pop_size], rate, method));

			// Add children in new population
			new_pop_lock.lock();
			new_population.push_back(children.first);
			new_population.push_back(children.second);
			new_pop_lock.unlock();
		}
	});
	std::thread t2([pop_size, &new_population, rate, method, &new_pop_lock]()
	{
		int psize = pop_size / 4;
		for (int i = psize; i < 2 * psize; ++i)
		{
			// Recombination between two selected solutions
			std::pair<Solution, Solution> children(crossover(new_population[i], new_population[i + pop_size], rate, method));

			// Add children in new population
			new_pop_lock.lock();
			new_population.push_back(children.first);
			new_population.push_back(children.second);
			new_pop_lock.unlock();
		}
	});
	std::thread t3([pop_size, &new_population, rate, method, &new_pop_lock]()
	{
		int psize = pop_size / 4;
		for (int i = 2 * psize; i < 3 * psize; ++i)
		{
			// Recombination between two selected solutions
			std::pair<Solution, Solution> children(crossover(new_population[i], new_population[i + pop_size], rate, method));

			// Add children in new population
			new_pop_lock.lock();
			new_population.push_back(children.first);
			new_population.push_back(children.second);
			new_pop_lock.unlock();
		}
	});
	std::thread t4([pop_size, &new_population, rate, method, &new_pop_lock]()
	{
		int psize = pop_size / 4;
		for (int i = 3 * psize; i < pop_size; ++i)
		{
			// Recombination between two selected solutions
			std::pair<Solution, Solution> children(crossover(new_population[i], new_population[i + pop_size], rate, method));

			// Add children in new population
			new_pop_lock.lock();
			new_population.push_back(children.first);
			new_population.push_back(children.second);
			new_pop_lock.unlock();
		}
	});
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	//*/
	
	// Generate offspring
	for (int i = 0; i < m_population_size / 2; i += 2)
	{
		// Recombination between two selected solutions
		std::pair<Solution, Solution> children(crossover(new_population[i], new_population[i + 1], rate, method));

		// Add children in new population
		new_population.push_back(children.first);
		new_population.push_back(children.second);
	}
}

void Genetic::mutation(std::vector<Solution> &new_population) const
{
	for (int i = 0; i < m_population_size; ++i)
	{
		new_population[i].mutate(m_mutation_rate);
	}
}

void Genetic::update_best_solution()
{
	Solution sol = *std::min_element(m_population.begin(), m_population.end(), CompareSolutions(m_inputs));
	int min_fitness = fitness(sol, m_inputs);
	if (min_fitness < m_best_fitness || m_best_fitness < 0)
	{
		m_best_fitness = min_fitness;
		m_best_solution = sol;
	}
}

int Genetic::process()
{
	// Initialize population
	generate_population();

	// Evolution process
	int generation = 0;
	while (generation < m_max_generations && m_best_fitness != 0)
	{
		// Create new population
		std::vector<Solution> new_population;

		// Selection process
		selection(new_population);

		// Crossover process
		double rate = m_crossover_rate;
		//rate *=  1.0 - (((double) generation) / ((double) m_max_generations * 2.0));
		crossover(new_population, rate);

		// Mutation process
		mutation(new_population);

		// Set new population to new population
		m_population = new_population;

		// Update best solution
		update_best_solution();

		// Increment generation counter
		++generation;

		// Print best solution
		if (generation % 100 == 0 || m_best_fitness == 0)
		{
			print_best_solution(generation);
		}
	}

	// Return last generation
	return generation;
}

void Genetic::print_solution(Solution const& sol)
{
	std::cout << sol << std::endl;
}

void Genetic::print_population(std::vector<Solution> const& population)
{
	std::cout << "--Population--" << std::endl;
	std::cout << "Size: " << population.size() << std::endl;
	for (int i = 0, len = population.size(); i < len; ++i)
	{
		std::cout << "N°" << i << std::endl;
		print_solution(population[i]);
	}
}

void Genetic::print_population() const
{
	print_population(m_population);
	std::cout << std::endl;
}

void Genetic::print_best_solution(int generation) const
{
	std::cout << "--Best Solution--" << std::endl;
	std::cout << "Generation: " << generation << std::endl;
	std::cout << "Fitness: " << m_best_fitness << std::endl;
	print_solution(m_best_solution);
	std::cout << std::endl;
}

std::string Genetic::nurse_indexes_to_string() const
{
	std::string str = "Nurse indexes: { " + int_to_string(m_nurse_indexes[0]);
	for (int i = 1; i < nurses(); ++i)
	{
		str = str + ", " + int_to_string(m_nurse_indexes[i]);
	}
	str = str + " }";
	return str;
}

void Genetic::set_nurse_indexes(Solution &sol)
{
    // Initialize nurse indexes
    m_nurse_indexes = std::vector<int>(nurses(), -1);

    // Initialize faults
    std::vector<std::vector<bool> > faults(nurses());

    // For each nurse
    for (int nurse_index = 0; nurse_index < nurses(); ++nurse_index)
    {
        // Get nurse
        Nurse *nurse = m_inputs->get_nurse(nurse_index);

        // Skip if the nurse has all the skills
        if (nurse->cannot_do().size() == 0)
        {
            for (int i = 0; i < nurses(); ++i)
            {
                if (m_nurse_indexes[i] < 0)
                {
                    m_nurse_indexes[i] = nurse_index;
                    break;
                }
            }
            continue;
        }

        // Initialize variables to get best path for the nurse
        int min_faults = -1;
        int best_path;
        std::vector<bool> best_nurse_faults;

        // For each path, count the number of treatments the nurse cannot cure
        for (int i = 0; i < nurses(); ++i)
        {
            // Check if the path is not already taken
            if (m_nurse_indexes[i] >= 0)
            {
                continue;
            }

            // Initialize counter
            int cannot_cure = 0;

            // Initialize nurse faults
            std::vector<bool> nurse_faults(patients_per_nurse(), false);

            // For each patient in the path
            for (int j = 0; j < patients_per_nurse(); ++j)
            {
                // Get patient treatment type ids
                int patient_index = sol.get(i, j);
                std::vector<unsigned int> type_ids = m_inputs->get_patient(patient_index)->get_type_ids();

                // Check if the nurse can cure this patient
                for (unsigned int type_id: nurse->cannot_do())
                {
                    if (find_element<unsigned int>(type_ids, type_id))
                    {
                        ++cannot_cure;
                        nurse_faults[j] = true;
                    }
                }
            }

            // Check if the nurse can use this path and update best path
            if (cannot_cure == 0)
            {
                best_path = i;
                best_nurse_faults = nurse_faults;
                break;
            }
            else if (cannot_cure < min_faults || min_faults < 0)
            {
                min_faults = cannot_cure;
                best_path = i;
                best_nurse_faults = nurse_faults;
            }
        }

        // Set nurse index
        m_nurse_indexes[best_path] = nurse_index;
        faults[nurse_index] = best_nurse_faults;
    }

    // Correct each path
    for (int nurse_index = 0; nurse_index < nurses(); ++nurse_index)
    {
        // Get treatment types the nurse cannot cure
        std::vector<unsigned int> cannot_do = m_inputs->get_nurse(nurse_index)->cannot_do();

        // Skip if the nurse has all the skills
        if (cannot_do.size() == 0)
        {
            continue;
        }

        // Get nurse best path
        int nurse_best_path = best_path(nurse_index);

        // For each patient in the path
        for (int k = 0; k < patients_per_nurse(); ++k)
        {
            // Check if the nurse can cure the patient
            if (faults[nurse_index][k])
            {
                // Get patient treatment type ids
                std::vector<unsigned int> type_ids = m_inputs->get_patient(sol.get(nurse_best_path, k))->get_type_ids();

                // Initialize variables to find the best new solution
                int best_fit = -1;
                Solution best_sol;

                // Find the best correction
                for (int i = 0; i < nurses(); ++i)
                {
                    // Check if it's a different nurse
                    if (i != nurse_best_path)
                    {
                        // Get nurse
                        Nurse *nurse = m_inputs->get_nurse(m_nurse_indexes[i]);

                        // Check if the nurse can cure this patient
                        bool can_cure = true;
                        for (unsigned int type_id: nurse->cannot_do())
                        {
                            if (find_element<unsigned int>(type_ids, type_id))
                            {
                                can_cure = false;
                                break;
                            }
                        }
                        if (!can_cure)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }

                    // Try to swap with each patient
                    for (int j = 0; j < patients_per_nurse(); ++j)
                    {
                        // Get patient
                        int patient_index = sol.get(i, j);

                        // Get patient treatment type ids
                        std::vector<unsigned int> patient_type_ids = m_inputs->get_patient(patient_index)->get_type_ids();

                        // Check if the nurse can cure this patient
                        bool can_cure = true;
                        for (unsigned int type_id: cannot_do)
                        {
                            if (find_element<unsigned int>(patient_type_ids, type_id))
                            {
                                can_cure = false;
                                break;
                            }
                        }
                        if (can_cure)
                        {
                            // Calculate new solution
                            Solution new_sol = swap_patients(sol, nurse_best_path, k, i, j);
                            int fit = fitness(new_sol, m_inputs);

                            // Update new best solution
                            if (fit < best_fit || best_fit < 0)
                            {
                                best_fit = fit;
                                best_sol = new_sol;
                            }
                        }
                    }
                }

                // Set new best solution
                if (best_fit >= 0)
                {
                    sol = best_sol;
                }
            }
        }
    }
}

void Genetic::add_side_patients(Solution &sol)
{
	// Add side patients to the solution
	for (int side_patient = nurses() * patients_per_nurse(); side_patient < patients(); ++side_patient)
	{
		// Get patient
		Patient *patient = m_inputs->get_patient(side_patient);
		
		// Get potential nurses
		std::vector<int> potential_nurses;
		for (int i = 0; i < nurses(); ++i)
		{
			// Check if the nurse can cure the patient treatments
			bool potential = true;
			Nurse *nurse = m_inputs->get_nurse(i);
			for (int j = 0; j < patient->treatments_size(); ++j)
			{
				Treatment *treatment = patient->get_treatment_by_index(j);
				unsigned int treatment_id = treatment->type_id();
				if (!nurse->can_do(treatment_id))
				{
					potential = false;
					break;
				}
			}

			// Add nurse to the potentials
			if (potential)
			{
				potential_nurses.push_back(i);
			}
		}
		
		// Initialize variables
		int best_fitness = -1, best_nurse = 0, best_position = 2;

		// For each nurse
		for (int i: potential_nurses)
		{
			// For each patient
			for (int j = 0; j <= sol.nurse_patients(i); ++j)
			{
				// Create and evaluate new solution
				Solution new_sol = insert_patient(sol, i, j, side_patient);
				int new_fitness = fitness(new_sol, m_inputs);

				// Update new best solution
				if (new_fitness < best_fitness || best_fitness < 0)
				{
					best_fitness = new_fitness;
					best_nurse = i;
					best_position = j;
				}
			}
		}
		
		// Update best solution
		sol.insert(best_nurse, best_position, side_patient);
	}
}

void Genetic::convert_to_appointments(Solution const& sol)
{
	// For each nurse
	for (int i = 0; i < nurses(); ++i)
    {
        // Get nurse info
        Nurse *nurse = m_inputs->get_nurse(m_nurse_indexes[i]);
        unsigned int nurse_id = nurse->id();

        // Initialize variables
        Time current_time = nurse->timetable().start_time();
        int current_patient_index = -1;
        int car_patient_index;

        // For each patient
        for (int j = 0, patients = sol.nurse_patients(i); j < patients; ++j)
        {
            // Get new patient index
            int next_patient_index = sol.get(i, j);

            // Calculate travel time
            if (current_patient_index >= 0)
            {
                // Get distance
                int dist = m_inputs->distance_between_patients(current_patient_index, next_patient_index);

                // Calculate time to go
                Time time;
                if (dist <= m_inputs->min_car_dist())
                {
                    time = time_to_go(dist, m_inputs->walk_speed());
                }
                else
                {
                    // Check if it's worth to go back to the car
                    int dist_from_car = m_inputs->distance_between_patients(car_patient_index, next_patient_index);
                    if (dist_from_car <= m_inputs->min_car_dist())
                    {
                        // Walk to next patient
                        time = time_to_go(dist, m_inputs->walk_speed());
                    }
                    else
                    {
                        // Walk to the car then take it to next patient
                        int dist_to_car = m_inputs->distance_between_patients(current_patient_index, car_patient_index);
                        time = time_to_go(dist_to_car, m_inputs->walk_speed()) + time_to_go(dist_from_car, m_inputs->car_speed());

                        // Update car position
                        car_patient_index = next_patient_index;
                    }
                }

                // Update current time
                current_time = current_time + time;
            }
            else
            {
                // Update car position
                car_patient_index = next_patient_index;
            }

            // Update patient index
            current_patient_index = next_patient_index;

            // Get patient
            Patient *patient = m_inputs->get_patient(current_patient_index);

            // For each treatment
            for (int k = 0, treatments = patient->treatments_size(); k < treatments; ++k)
            {
                // Get treatment
                Treatment *treatment = patient->get_treatment_by_index(k);
                unsigned int treatment_id = treatment->id();

                // Calculate schedule
                short dur = duration(treatment->type_id()).minutes();
                TimeInterval schedule(current_time, dur);

                // Update current time
                current_time = schedule.end_time();

                // Create and add appointment
                Appointment *appointment = new Appointment(nurse_id, patient, treatment_id, schedule);
                m_appointments.push_back(appointment);
            }
        }
    }
}

void Genetic::find_appointments()
{
	// Process genetic algorithm
	int generation = process();

	// Tune best solution
	std::cout << "TUNING PROCESS" << std::endl;
	set_nurse_indexes(m_best_solution);
	std::cout << nurse_indexes_to_string() << std::endl;
	add_side_patients(m_best_solution);

	// Print new best solution
	m_best_fitness = fitness(m_best_solution, m_inputs);
	print_best_solution(generation);

	// Set appointments from best solution
	convert_to_appointments(m_best_solution);
}
