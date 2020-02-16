#ifndef GENETIC_H
#define GENETIC_H

#include <vector>
#include <string>
#include <utility>
#include "Algorithm.h"
#include "Inputs.h"
#include "Solution.h"

class Genetic : public Algorithm
{
    public:

        Genetic(Inputs *inputs, 
				int population_size = DEFAULT_POPULATION_SIZE, 
				double crossover_rate = DEFAULT_CROSSOVER_RATE, 
				double mutation_rate = DEFAULT_MUTATION_RATE,
				int max_generations = DEFAULT_MEX_GENERATIONS);

        int best_fitness() const { return m_best_fitness; }
        Solution best_solution() const { return m_best_solution; }

        int nurses() const { return m_inputs->nurses_size(); }
        int patients() const { return m_inputs->patients_size(); }
        int patients_per_nurse() const { return m_inputs->patients_per_nurse(); }

        int best_path(int nurse_index) const;
		int nurse_treatments(Solution const& sol, int nurse) const;

        static int fitness(Solution &sol, Inputs *inputs); // the lower the fitter
        static std::vector<Solution> generate_population(int population_size, int nurses, int patients_per_nurse);
        static std::pair<Solution, Solution> crossover(Solution const& parent1, Solution const& parent2, double rate, 
													   int method = DEFAULT_CROSSOVER_METHOD);

        void generate_population();
        void selection(std::vector<Solution> &new_population, int method = DEFAULT_SELECTION_METHOD);
        void crossover(std::vector<Solution> &new_population, double rate, int method = DEFAULT_CROSSOVER_METHOD) const;
        void mutation(std::vector<Solution> &new_population) const;
        void update_best_solution();

        int process();

        static void print_solution(Solution const& sol);
        static void print_population(std::vector<Solution> const& population);
        void print_population() const;
        void print_best_solution(int generation) const;
		std::string nurse_indexes_to_string() const;


    private:

        void set_nurse_indexes(Solution &sol);
        void add_side_patients(Solution &sol);
    	void convert_to_appointments(Solution const& sol);

    	virtual void find_appointments();

		static const int DEFAULT_POPULATION_SIZE;
		static const double DEFAULT_CROSSOVER_RATE;
		static const double DEFAULT_MUTATION_RATE;
		static const int DEFAULT_MEX_GENERATIONS;
		static const int DEFAULT_SELECTION_METHOD;
		static const int DEFAULT_CROSSOVER_METHOD;

    	const int m_population_size;
    	const double m_crossover_rate;
    	const double m_mutation_rate;
    	const int m_max_generations;

    	std::vector<Solution> m_population;
    	int m_best_fitness;
    	Solution m_best_solution;

    	std::vector<int> m_nurse_indexes;

    	/**
		 * Class used to compare solutions using the fitness function
		 */
		class CompareSolutions
		{
		    public:

		    	CompareSolutions(Inputs *inputs):m_inputs(inputs) {}

		        bool operator()(Solution &sol1, Solution &sol2) { return fitness(sol1, m_inputs) < fitness(sol2, m_inputs); }


		    private:

		    	Inputs *m_inputs;
		};


    // Friend functions for testing purposes only
    friend void TEST_SET_POPULATION(Genetic &genetic, std::vector<Solution> population) { genetic.m_population = population; }
    friend void TEST_SET_BEST_FITNESS(Genetic &genetic, int best_fitness) { genetic.m_best_fitness = best_fitness; }
    friend void TEST_SET_BEST_SOLUTION(Genetic &genetic, Solution best_solution) { genetic.m_best_solution = best_solution; }
    friend void TEST_SET_NURSE_INDEXES(Genetic &genetic, std::vector<int> nurse_indexes) { genetic.m_nurse_indexes = nurse_indexes; }
};

#endif // GENETIC_H
