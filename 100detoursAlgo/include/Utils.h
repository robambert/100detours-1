#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <algorithm>
#include "Inputs.h"
#include "Algorithm.h"
#include <cpprest/json.h>

// Null json value only used as a default value for parameter answer in run_algo function
static web::json::value jnull = web::json::value::null();

/**
 * Convert an integer into a string
 * @param i The integer to convert
 * @return The integer as a string
 */
std::string int_to_string(int i);

/**
 * Generate a random double >= 0 and < 1
 * @return The random double
 */
double random01();

/**
 * Generate a random integer >= inf and <= sup
 * @param inf The minimal value that can be taken
 * @param sup The maximal value that can be taken
 * @return The random number
 */
int random(int inf, int sup);

/**
 * Generate a vector of random different integers >= inf and <= sup
 * @param inf The minimal value that can be taken
 * @param sup The maximal value that can be taken
 * @param n The size of the vector (must be > sup - inf)
 * @return The random vector
 */
std::vector<int> random_vector(int inf, int sup, int n);

/**
 * Generate a random permutation of integers between 0 and n - 1
 * @param n The size of the permutation
 * @return The random permutation
 */
std::vector<int> generate_random_permutation(int n);

/*
 * Check the presence of an element in a vector
 * @param vec The vector to search in
 * @param val The element to search for
 * @return A boolean standing for the presence of the element in the vector
 */
template<typename T>
bool find_element(std::vector<T> const& vec, T const& val) { return std::find(vec.begin(), vec.end(), val) != vec.end(); }

/*
 * Display a json value as a string
 * @param jvalue The json object to display
 * @param prefix A string to display before the json
 */
void display_json(web::json::value const& jvalue, utility::string_t const& prefix = L"");

/**
 * Class used to compare the value refered by the pointers
 * instead of the pointers itself
 */
class ComparePointers
{
    public:

        template<typename T>
        bool operator()(T *a, T *b) { return (*a) < (*b); }
};

/**
 * The different available algorithms to solve the problem
 */
enum Algo
{
    GLUTTON_BREADTH, // deprecated
    GLUTTON_DEPTH,   // deprecated
    GENETIC
};

/**
 * Run the chosen algorithm on the inputs and create a json answer
 * @param inputs The nurses and patients
 * @param algo_name The algorithm to use
 * @param answer The json object holding the answer
 */
void run_algo(Inputs *inputs, Algo algo_name = GENETIC, web::json::value &answer = jnull);

#endif // UTILS_H
