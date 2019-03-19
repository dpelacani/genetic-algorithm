#include "Genetic_Algorithm.h"

using namespace std;

void intArrayToUnits(int *int_array, CUnit *circuit, int no_units)
{
    for(int i=1;i < no_units*2 + 1; i+=2){
		//cout << i << endl;
        circuit[i/2].conc_num = int_array[i];
        circuit[i/2].tails_num = int_array[i + 1];
		circuit[i / 2].id = i / 2;
    }
}

void unitsToIntArray(int *int_array, int feed_id, CUnit *units_to_convert, int no_units)
{
	//int_array is where to store answer
	//CUnit * is array of CUnits
	int_array[0] = feed_id; //source is always 0
	for (int i = 1; i < no_units * 2 + 1; i += 2)
	{
		int_array[i] = units_to_convert[i / 2].conc_num;
		int_array[i + 1] = units_to_convert[i / 2].tails_num;
	}
}


void computeFitness(CUnit** circuits, double*fitness, int no_circuits)
{
	for (int i = 0; i < no_circuits; i++)
	{
		fitness[i] = rand() % 100;
	}
}


void selectBestCircuit(CUnit**circuits, double* fitness, CUnit* best_circuit, int no_circuits, int no_units)
{
	int max_fit = 0;
	int i_max = 0;
	for (int i = 0; i < no_circuits; i++)
	{
		if (fitness[i] > max_fit)
		{
			fitness[i] = max_fit;
			i_max = i;
		}
	}

	for (int i = 0; i < no_units; i++)
	{
		best_circuit[i] = circuits[i_max][i];
	}
}


void generateCircuits(int no_units, int no_circuits, CCircuit* parents)
{
	int valid_count = 0;
	srand(time(NULL));
	int* circuit_array = new int[2 * no_units + 1];
	int indexConsenOut = 1 + 2 * (rand() % no_units);
	int indexTailingOut = 1 + (2 * (rand() % no_units) + 1);
	CCircuit Circuit(no_units);

	do
	{
		for (int i = 0; i < 2 * no_units + 1; i++)
		{
			if (i == indexConsenOut) circuit_array[i] = 10;
			else if (i == indexTailingOut) circuit_array[i] = 11;
			else circuit_array[i] = rand() % no_units;
		}

		for (int i = 0; i < 2 * no_units + 1; i++) Circuit.circuit_ints[i] = circuit_array[i];
		Circuit.feed_id = circuit_array[0];
		Circuit.no_units = no_units;
		intArrayToUnits(circuit_array, Circuit.circuit_units, no_units);

		if (checkValidity(Circuit))	// CHECK VALIDITY HERE!!
		{
			// Add circuit to parents grid
			parents[valid_count] = Circuit;

			// Increment number of valid circuits
			valid_count++;
		}
	} while (valid_count < no_circuits);

	delete[] circuit_array;
}


void crossOver(int *circuitA, int *circuitB, int no_units)
{
	int index = 1 + rand() % (2 * no_units - 1);
	for (int i = index; i < 2 * no_units + 1; i++)
	{
		int temp = circuitA[i];
		circuitA[i] = circuitB[i];
		circuitB[i] = temp;
	}
}


void mutate(int *circuit, int no_unit)
{
	double mute_rate; //come out a random rate

	double mute_limit = 0.01; //mutation probability

	for (int i = 0; i < 2 * no_unit + 1; i++)
	{
		mute_rate = ((double)rand()) / RAND_MAX;
		if (mute_rate <= mute_limit && i != 0)
		{
			circuit[i] = (circuit[i] + no_unit + 2 + rand() % (no_unit + 2)) % (no_unit + 2);
		}
		else if (mute_rate <= mute_limit && i == 0)
		{
			circuit[i] = (circuit[i] + no_unit + rand() % no_unit) % no_unit;
		}
	}
}


void pairParents(int ** circuits, int * parentA, int * parentB, int no_units, int no_circuits, double* fitness)
{
	double totalFitness;	//total fitness after fix
	int index1 = -1;		//index for parentA
	int index2 = -1;	//index for parentB
	double ref1, ref2;

	int min_fit = 0; //compensation number to make sure all fitness numbers are larger than 0
	for (int i = 0; i < no_circuits; i++)
	{
		if (fitness[i] < min_fit)
		{
			min_fit = fitness[i]; //!!!!!!!
		}
	}

	totalFitness = 0;
	for (int i = 0; i < no_circuits; i++) //fix the fitness array and calculate the totalFitness
	{
		fitness[i] -= min_fit;
		totalFitness += fitness[i];
	}

	ref1 = ((double)rand()) / RAND_MAX; //get a random number within the range of totalFitness
	double refNum1 = ref1 * totalFitness;
	double fitnessRef = 0; //use a reference number to get the index
	int* fitnessind = new int[int(totalFitness)];

	for (int i = 0; i < no_circuits; i++) //get the index from the random number
	{
		fitnessRef += fitness[i];
		if (fitnessRef >= refNum1)
		{
			index1 = i;
			break;
		}
	}
	fitnessRef = 0; //clear the fitness reference

	do
	{
		ref2 = ((double)rand()) / RAND_MAX; //get another random number
		double refNum2 = ref2 * totalFitness;

		for (int i = 1; i < no_circuits; i++)
		{
			fitnessRef += fitness[i];
			if (fitnessRef >= refNum2)
			{
				if (index1 != i) //make sure it's not equal to parentA
				{
					index2 = i;
				}
				break; //if it is equal, break anyways
			}
		}
	} while (index2 == -1);

	for (int i = 0; i < 2 * no_units + 1; i++) //assign it to the pointer of parentA and B
	{
		parentA[i] = circuits[index1][i];
		parentB[i] = circuits[index2][i];
	}
}
