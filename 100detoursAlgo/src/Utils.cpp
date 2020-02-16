#include "../include/Utils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../include/Inputs.h"
#include "../include/Algorithm.h"
#include "../include/GluttonBreadth.h"
#include "../include/GluttonDepth.h"
#include "../include/Genetic.h"
#include <cpprest/json.h>

std::string int_to_string(int i)
{
    // Do the converstion with a stringstream
    std::stringstream sstream;
    sstream << i;
    return sstream.str();
}

double random01()
{
    return ((double) rand()) / ((double) RAND_MAX);
}

int random(int inf, int sup)
{
    return inf + (rand() % (sup - inf + 1));
}

std::vector<int> random_vector(int inf, int sup, int n)
{
    // Create result vector
    std::vector<int> res(n);

    // Create an ordered vector of the integers >= inf and <= sup
    int buf_len = sup - inf + 1;
    std::vector<int> buf(buf_len);
    for (int i = inf; i <= sup; ++i)
    {
        buf[i] = i;
    }

    // Process Fisher–Yates shuffle algorithm
    for (int i = 0; i < n; ++i)
    {
        // Get a random number from the buffer
        int index = random(i, buf_len - 1);
        int val = buf[index];

        // Swap the numbers in the buffer
        std::swap(buf[i], buf[index]);

        // Update permutation
        res[i] = val;
    }

    // Return vector
    return res;
}

std::vector<int> generate_random_permutation(int n)
{
    return random_vector(0, n - 1, n);
}

void display_json(web::json::value const& jvalue, utility::string_t const& prefix)
{
	std::wcout << prefix << jvalue.serialize() << std::endl;
}

void run_algo(Inputs *inputs, Algo algo_name, web::json::value &answer)
{
	// Initialize inputs
	inputs->initialize();

	// Print inputs
	//std::cout << *inputs << std::endl << std::endl;
	
	// Initialize algo
    Algorithm *algo = 0;
    switch (algo_name)
    {
        case GLUTTON_BREADTH:
            std::cout << "GLUTTON BREADTH" << std::endl << std::endl;
            algo = new GluttonBreadth(inputs);
            break;

        case GLUTTON_DEPTH:
            std::cout << "GLUTTON DEPTH" << std::endl << std::endl;
            algo = new GluttonDepth(inputs);
            break;

        case GENETIC:
            std::cout << "GENETIC" << std::endl << std::endl;
            algo = new Genetic(inputs);
            break;

        default:
            return;
    }

    // Run algorithm
    algo->run();

    // Print output
    //std::cout << *algo << std::endl << std::endl;

	// Create the json
	if (!answer.is_null())
	{
		answer = algo->to_json();
	}

    // Delete algo
    delete algo;
}
