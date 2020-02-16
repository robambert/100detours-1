#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "include/Utils.h"
#include "include/Point.h"
#include "include/Time.h"
#include "include/TimeInterval.h"
#include "include/TreatmentType.h"
#include "include/Treatment.h"
#include "include/Nurse.h"
#include "include/Patient.h"
#include "include/Appointment.h"
#include "include/Inputs.h"
#include "include/Algorithm.h"
#include "include/GluttonBreadth.h"
#include "include/GluttonDepth.h"
#include "include/Solution.h"
#include "include/Genetic.h"

#include "include/WebServer.h"
#include <cpprest/json.h>

/* STATIC VARIABLES */
static const unsigned int SEED = 4895; // seed to set the random

static const short CAR_SPEED = 15; // in km/h
static const short WALK_SPEED = 5; // in km/h
static const short MIN_CAR_DIST = 1000; // in m
static const Point ORIGIN = Point(0, 0);
static const TimeInterval FULL_DAY = TimeInterval(6 * 60, 20 * 60);
static const Time START_TIME = FULL_DAY.start_time(), END_TIME = FULL_DAY.end_time();

static const utility::string_t ADDRESS = L"http://localhost";
static const utility::string_t PORT = L"4242";

int main()
{
    /* INITIALIZE RANDOM */
    srand(SEED);

    /* SET STATIC ATTRIBUTES */
    Point::SET_ORIGIN(ORIGIN);
    TimeInterval::SET_FULL_DAY(FULL_DAY);

    /*// SET TEST INPUTS

	// Treatment types
	TreatmentType flu(0, 30);
	TreatmentType cancer(1, 90);

	// Inputs 1
    Inputs inputs1 = Inputs(CAR_SPEED, WALK_SPEED, MIN_CAR_DIST);
    inputs1.add_type(flu);
    inputs1.add_type(cancer);
    inputs1.add_nurse(Nurse(0, 1));
    inputs1.add_patient(Patient(0, Point(0, 1000), 0, 0));

	// Inputs 2
    Inputs inputs2 = Inputs(CAR_SPEED, WALK_SPEED, MIN_CAR_DIST);
    inputs2.add_type(flu);
    inputs2.add_type(cancer);
    inputs2.add_nurse(Nurse(0));
    inputs2.add_nurse(Nurse(1, 1));
    inputs2.add_patient(Patient(0, Point(0, 1000), 0, 0));
    inputs2.add_patient(Patient(1, Point(1000, 2000), 1, 0));

	// Inputs 3
    Inputs inputs3 = Inputs(CAR_SPEED, WALK_SPEED, MIN_CAR_DIST);
    inputs3.add_type(flu);
    inputs3.add_nurse(Nurse(0));
    inputs3.add_nurse(Nurse(1));
    inputs3.add_nurse(Nurse(2));
    inputs3.add_patient(Patient(0, Point(0, 0), 0, 0));
    inputs3.add_patient(Patient(1, Point(0, 100), 1, 0));
    inputs3.add_patient(Patient(2, Point(100, 0), 2, 0));
    inputs3.add_patient(Patient(3, Point(5000, 5000), 3, 0));
    inputs3.add_patient(Patient(4, Point(5000, 5100), 4, 0));
    inputs3.add_patient(Patient(5, Point(5100, 5000), 5, 0));
    inputs3.add_patient(Patient(6, Point(-5000, -5000), 6, 0));
    inputs3.add_patient(Patient(7, Point(-5000, -5100), 7, 0));
    inputs3.add_patient(Patient(8, Point(-5100, -5000), 8, 0));

	// Inputs 4, 5, 6
    Inputs inputs4 = Inputs(CAR_SPEED, WALK_SPEED, MIN_CAR_DIST);
    Inputs inputs5 = Inputs(CAR_SPEED, WALK_SPEED, MIN_CAR_DIST);
    Inputs inputs6 = Inputs(CAR_SPEED, WALK_SPEED, MIN_CAR_DIST);
    inputs4.add_type(flu);
    inputs5.add_type(flu);
    inputs6.add_type(flu);
    int nurses = 20, patients_per_nurse = 12, patient_counter = 0;
    for (int i = 0; i < nurses; ++i)
    {
        inputs4.add_nurse(Nurse(i));
        inputs5.add_nurse(Nurse(i));
        inputs6.add_nurse(Nurse(i));
        for (int j = 0; j < patients_per_nurse; ++j)
        {
            inputs4.add_patient(Patient(patient_counter, Point(400 * patient_counter, 400 * patient_counter), patient_counter, 0));
            inputs5.add_patient(Patient(patient_counter, Point(i * 1500, j * 10), patient_counter, 0));
            inputs6.add_patient(Patient(patient_counter, Point(i * 400, j * 400), patient_counter, 0));
            ++patient_counter;
        }
    }

	// Inputs 7
	Inputs inputs7 = Inputs(CAR_SPEED, WALK_SPEED, MIN_CAR_DIST);
	inputs7.add_type(flu);
	inputs7.add_nurse(Nurse(0));
	inputs7.add_nurse(Nurse(1));
	inputs7.add_patient(Patient(0, Point(0, 0), 0, 0));
	inputs7.add_patient(Patient(1, Point(0, 1000), 1, 0));
	inputs7.add_patient(Patient(2, Point(1000, 0), 2, 0));
	inputs7.add_patient(Patient(3, Point(0, -1000), 3, 0));
	inputs7.add_patient(Patient(4, Point(-1000, 0), 4, 0));

	// Inputs 8
	Inputs inputs8 = Inputs(CAR_SPEED, WALK_SPEED, MIN_CAR_DIST);
	inputs8.add_type(flu);
	inputs8.add_type(cancer);
	inputs8.add_nurse(Nurse(0));
	inputs8.add_nurse(Nurse(1, 1));
	inputs8.add_patient(Patient(0, Point(0, 0), 0, 0));
	inputs8.add_patient(Patient(1, Point(0, 1000), 1, 1));
	inputs8.add_patient(Patient(2, Point(2000, 0), 2, 0));
	inputs8.add_patient(Patient(3, Point(2000, 1000), 3, 1));

	// Random inputs
    Inputs random_inputs = Inputs(CAR_SPEED, WALK_SPEED, MIN_CAR_DIST);
    random_inputs.add_type(flu);
    random_inputs.add_type(cancer);
    nurses = 20, patients_per_nurse = 10;
    const short min_xy = 0, max_xy = 5000; // in m
    for (int i = 0; i < nurses; ++i)
    {
		const int rint = random(0, 5);
		if (rint < 2)
		{
			random_inputs.add_nurse(Nurse(i, rint));
		}
		else
		{
			random_inputs.add_nurse(Nurse(i));
		}
    }
    for (int i = 0; i < nurses * patients_per_nurse; ++i)
    {
		const Point location(random(min_xy, max_xy), random(min_xy, max_xy));
		const unsigned int type = random(0, 1);
        random_inputs.add_patient(Patient(i, location, i, type));
    }

    // Select inputs
    Inputs *inputs = &inputs5;
	//*/

    /*// TEST GLUTTON BREADTH
    run_algo(inputs, GLUTTON_BREADTH);
    //*/

    /*// TEST GLUTTON DEPTH
    run_algo(inputs, GLUTTON_DEPTH);
    //*/

	/*// TEST GENETIC
	run_algo(inputs, GENETIC);
	//*/
	
	/*// TEST JSON
	std::cout << "*** TEST: JSON ***" << std::endl;
	utility::string_t jstring = L"{\"carSpeed\": 15, \"walkSpeed\": 5, \"minCarDist\": 1000, \"treatmentTypes\": [{\"_id\": 0, \"duration\": 30}], \"nurses\": [{\"_id\": 0, \"treatmentTypeIds\": [], \"startTime\": 480}, {\"_id\": 1, \"treatmentTypeIds\": [], \"startTime\": 600}], \"patients\": [{\"_id\": 0, \"location\": {\"x\": 0, \"y\": 0}, \"treatments\": [{\"_id\": 0, \"treatmentTypeId\": 0}]}, {\"_id\": 1, \"location\": {\"x\": 0, \"y\": 1000}, \"treatments\": [{\"_id\": 1, \"treatmentTypeId\": 0}]}, {\"_id\": 2, \"location\": {\"x\": 1000, \"y\": 0}, \"treatments\": [{\"_id\": 2, \"treatmentTypeId\": 0}]}, {\"_id\": 3, \"location\": {\"x\": 1000, \"y\": 1000}, \"treatments\": [{\"_id\": 3, \"treatmentTypeId\": 0}]}]}";
	web::json::value jvalue = web::json::value::parse(jstring);
	std::cout << "is valid json input (expected 1): " << Inputs::is_valid_json(jvalue) << std::endl;

	jstring = L"{\"carSpeed\": 15, \"walkSpeed\": 0, \"minCarDist\": 1000, \"treatmentTypes\": [{\"_id\": 0, \"duration\": 30}], \"nurses\": [{\"_id\": 0, \"treatmentTypeIds\": [], \"startTime\": 480}, {\"_id\": 1, \"treatmentTypeIds\": [], \"startTime\": 600}], \"patients\": [{\"_id\": 0, \"location\": {\"x\": 0, \"y\": 0}, \"treatments\": [{\"_id\": 0, \"treatmentTypeId\": 0}]}, {\"_id\": 1, \"location\": {\"x\": 0, \"y\": 1000}, \"treatments\": [{\"_id\": 1, \"treatmentTypeId\": 0}]}, {\"_id\": 2, \"location\": {\"x\": 1000, \"y\": 0}, \"treatments\": [{\"_id\": 2, \"treatmentTypeId\": 0}]}, {\"_id\": 3, \"location\": {\"x\": 1000, \"y\": 1000}, \"treatments\": [{\"_id\": 3, \"treatmentTypeId\": 0}]}]}";
	jvalue = web::json::value::parse(jstring);
	std::cout << "is valid json input (expected 0): " << Inputs::is_valid_json(jvalue) << std::endl;

	jstring = L"{\"carSpeed\": 15, \"walkSpeed\": 5, \"minCarDist\": 1000, \"treatmentTypes\": [{\"_id\": 0}], \"nurses\": [{\"_id\": 0, \"treatmentTypeIds\": [], \"startTime\": 480}, {\"_id\": 1, \"treatmentTypeIds\": [], \"startTime\": 600}], \"patients\": [{\"_id\": 0, \"location\": {\"x\": 0, \"y\": 0}, \"treatments\": [{\"_id\": 0, \"treatmentTypeId\": 0}]}, {\"_id\": 1, \"location\": {\"x\": 0, \"y\": 1000}, \"treatments\": [{\"_id\": 1, \"treatmentTypeId\": 0}]}, {\"_id\": 2, \"location\": {\"x\": 1000, \"y\": 0}, \"treatments\": [{\"_id\": 2, \"treatmentTypeId\": 0}]}, {\"_id\": 3, \"location\": {\"x\": 1000, \"y\": 1000}, \"treatments\": [{\"_id\": 3, \"treatmentTypeId\": 0}]}]}";
	jvalue = web::json::value::parse(jstring);
	std::cout << "is valid json input (expected 0): " << Inputs::is_valid_json(jvalue) << std::endl;

	jstring = L"{\"carSpeed\": 15, \"walkSpeed\": 5, \"minCarDist\": 1000, \"treatmentTypes\": [{\"_id\": 0, \"duration\": 30}], \"nurses\": [], \"patients\": [{\"_id\": 0, \"location\": {\"x\": 0, \"y\": 0}, \"treatments\": [{\"_id\": 0, \"treatmentTypeId\": 0}]}, {\"_id\": 1, \"location\": {\"x\": 0, \"y\": 1000}, \"treatments\": [{\"_id\": 1, \"treatmentTypeId\": 0}]}, {\"_id\": 2, \"location\": {\"x\": 1000, \"y\": 0}, \"treatments\": [{\"_id\": 2, \"treatmentTypeId\": 0}]}, {\"_id\": 3, \"location\": {\"x\": 1000, \"y\": 1000}, \"treatments\": [{\"_id\": 3, \"treatmentTypeId\": 0}]}]}";
	jvalue = web::json::value::parse(jstring);
	std::cout << "is valid json input (expected 0): " << Inputs::is_valid_json(jvalue) << std::endl;

	jstring = L"{\"carSpeed\": 15, \"walkSpeed\": 5, \"minCarDist\": 1000, \"treatmentTypes\": [{\"_id\": 0, \"duration\": 30}], \"nurses\": [{\"_id\": 0, \"treatmentTypeIds\": [], \"startTime\": 480}, {\"_id\": 0, \"treatmentTypeIds\": [], \"startTime\": 600}], \"patients\": [{\"_id\": 0, \"location\": {\"x\": 0, \"y\": 0}, \"treatments\": [{\"_id\": 0, \"treatmentTypeId\": 0}]}, {\"_id\": 1, \"location\": {\"x\": 0, \"y\": 1000}, \"treatments\": [{\"_id\": 1, \"treatmentTypeId\": 0}]}, {\"_id\": 2, \"location\": {\"x\": 1000, \"y\": 0}, \"treatments\": [{\"_id\": 2, \"treatmentTypeId\": 0}]}, {\"_id\": 3, \"location\": {\"x\": 1000, \"y\": 1000}, \"treatments\": [{\"_id\": 3, \"treatmentTypeId\": 0}]}]}";
	jvalue = web::json::value::parse(jstring);
	std::cout << "is valid json input (expected 0): " << Inputs::is_valid_json(jvalue) << std::endl;

	jstring = L"{\"carSpeed\": 15, \"walkSpeed\": 5, \"minCarDist\": 1000, \"treatmentTypes\": [{\"_id\": 0, \"duration\": 30}], \"nurses\": [{\"_id\": 0, \"treatmentTypeIds\": [], \"startTime\": 480}, {\"_id\": 1, \"treatmentTypeIds\": [], \"startTime\": 600}], \"patients\": [{\"_id\": 0, \"location\": {\"x\": 0, \"y\": 0}, \"treatments\": [{\"_id\": 0, \"treatmentTypeId\": 1}]}, {\"_id\": 1, \"location\": {\"x\": 0, \"y\": 1000}, \"treatments\": [{\"_id\": 1, \"treatmentTypeId\": 0}]}, {\"_id\": 2, \"location\": {\"x\": 1000, \"y\": 0}, \"treatments\": [{\"_id\": 2, \"treatmentTypeId\": 0}]}, {\"_id\": 3, \"location\": {\"x\": 1000, \"y\": 1000}, \"treatments\": [{\"_id\": 3, \"treatmentTypeId\": 0}]}]}";
	jvalue = web::json::value::parse(jstring);
	std::cout << "is valid json input (expected 0): " << Inputs::is_valid_json(jvalue) << std::endl;
	std::cout << std::endl;
	//*/
	
	/*// TEST RANDOM
	std::cout << "*** TEST: RANDOM ***" << std::endl;
	int random_counter = 0, total = 10000;
	double rate = 0.5;
	for (int i = 0; i < total; ++i)
	{
		if (random01() < rate)
		{
			++random_counter;
		}
	}
	double random_rate = ((double) random_counter) / ((double) total);
	std::cout << "Rate: " << rate << std::endl;
	std::cout << "Random rate: " << random_rate << std::endl;
	std::cout << std::endl;
	//*/

    /*// TEST PERMUTATIONS
    std::cout << "*** TEST: RANDOM PERMUTATIONS ***" << std::endl;
    for (int i = 0; i < 5; ++i)
    {
        std::vector<int> permutation(generate_random_permutation(9));
        for (int j = 0; j < 9; ++j)
        {
            std::cout << "  " << permutation[j] << std::endl;
        }
    }
    std::cout << std::endl;
    //*/

	/*// TEST SOLUTION CLASS
	std::cout << "*** TEST: INSERT ***" << std::endl;
	Solution sol(2, 2, { 0, 1, 2, 3 });
	Solution new_sol = insert_patient(sol, 0, 1, 4);
	std::cout << sol << std::endl;
	std::cout << new_sol << std::endl;
	std::cout << std::endl;
	//*/

    /*// TEST GENETIC CLASS
    Genetic gen(inputs, 4);

    std::cout << "*** TEST: FITNESS ***" << std::endl << std::endl;
	std::vector<int> code(9);
	for (int i = 0; i < 9; ++i)
	{
		code[i] = i;
	}
	Solution sol(3, 3, code);
    Genetic::print_solution(sol);
    std::cout << "Fitness (expected 0): " << Genetic::fitness(sol, inputs) << std::endl;
    code[2] = 3;
    code[3] = 2;
    sol = Solution(3, 3, code);
    Genetic::print_solution(sol);
    std::cout << "Fitness (expected 20000): " << Genetic::fitness(sol, inputs) << std::endl;
	std::cout << std::endl;

    std::cout << "*** TEST: GENERATE POPULATION ***" << std::endl << std::endl;
	gen.generate_population();
    gen.print_population();
    std::cout << std::endl;

	std::cout << "*** TEST: SELECTION ***" << std::endl << std::endl;
    std::vector<Solution> new_pop;
    gen.selection(new_pop);
    Genetic::print_population(new_pop);
    std::cout << std::endl;
	//*/

    /// START WEB SERVER
	std::unique_ptr<WebServer> server;
	server = std::unique_ptr<WebServer>(new WebServer(ADDRESS + L":" + PORT));
	server->run();
    //*/

    return 0;
}
