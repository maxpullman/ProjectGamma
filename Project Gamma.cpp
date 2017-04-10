// Project Gamma.cpp : Defines the entry point for the console application.
// Max Pullman
// April 4, 2017
// ME 493: Autonomy
// Project Gamma

#include "stdafx.h"
#include <iostream>
#include <assert.h>
#include <random>
#include <time.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <fstream>

using namespace std;

class city {
public:
	double xcoor;
	double ycoor;

	void init() {
		//Positions will randomize between 0 and 100
		//LR.1: Program represents a city with some coordinates
		xcoor = rand() % 100;
		ycoor = rand() % 100;
	};

};


class salesman {
public:
	double dist;
	double xcoor;
	double ycoor;
	double fitness;

	//LR.2: Program can represent an agent at a certain city 
	void init(vector<city> citySet) {
		//initialize salesman at first city with no distance covered
		//Setting the fitness equal to the distance traveled 
		dist = 0.0;
		xcoor = citySet.at(0).xcoor;
		ycoor = citySet.at(0).ycoor;
		fitness = dist;
	};

	void update(vector<int> policy, vector<city> Set) {
		dist = 0.0;
		//Distance to be calculated using pythagorean's theorem
		//Distance calculation will continue until the policy is finsihed
		for (int i = 0; i < policy.size(); i++)
		{
			city New;
			New = Set.at(policy.at(i));
			//LR.7: Calculating distance between two cities
			dist += sqrt(pow((abs(xcoor - New.xcoor)), 2) + pow(abs((ycoor - New.ycoor)), 2));
			fitness = dist;
			//Makes sure that agent is now placed at the new city traveled to
			xcoor = New.xcoor;
			ycoor = New.ycoor;
		}
	};

};


class policy {
public:
	vector<int> A;
	double fitness;

	void init(int numCities) {
		// Create one policy which is a vector of integers. Each integer represents index in city vector
		for (int k = 0; k< numCities; k++)
		{
			A.push_back(k);
		}

		// randomly shuffle all cities but keep first one in same position
		random_shuffle(A.begin() + 1, A.end());

		/*for (int i = 0; i < P.size(); i++)
		{
		cout << P.at(i) << " ";
		}
		cout << endl;*/
		assert(A.size() == numCities); // check that policy has same size as total number of cities
		assert(*A.begin() == 0); // check that every policy starts at the first city
								 // previous asserts fulfill test that agent has set of policies that represent its path (LR.3)
								 // previous asserts also partially fulfill test that agent always starts at same city (LR.5)

		fitness = -1.0;
	};
};


vector<policy> EA_init(int numCities, int popSize) {
	vector<policy> population;

	for (int i = 0; i < popSize / 2; i++)
	{
		policy p;
		p.init(numCities);
		population.push_back(p);
	}
	assert(population.size() == popSize / 2); // fulfills test that a population of policies is created (MR.1)

	return population;
};


vector<policy> EA_replicate(vector<policy> P, int popSize, int numCities, salesman S, vector<city> citySet) {
	vector<policy> population;
	population = P;
	assert(population.size() == popSize / 2);

	while (population.size() != popSize)
	{
		int index;
		index = rand() % (popSize / 2);
		policy Start;
		Start = population.at(index);

		int index2;
		int index3;

		// mutation process
		// swap two cities in policy
		index2 = rand() % (numCities - 1) + 1; //gets number between one and the maximum number of cities
		index3 = rand() % (numCities - 1) + 1; //ensures that the first city is never mutated, so salesman always starts at same city 

		while (index2 == index3)
		{
			index3 = rand() % (numCities - 1) + 1;
		}

		assert(index2 != 0 && index3 != 0); // fulfills test that agent always starts in same city (LR.5)

		int temp;
		temp = Start.A.at(index2);
		Start.A.at(index2) = Start.A.at(index3);
		Start.A.at(index3) = temp;

		population.push_back(Start);
		assert(population.at(index).A != Start.A); //fulfills test that a policy is mutated by the program
	}
	assert(population.size() == popSize);

	return population;
};


vector<policy> EA_eval(vector<policy> P, int popSize, int numCities, salesman S, vector <city> citySet) {
	vector<policy> population;
	population = P;
	assert(population.size() == popSize);

	// update the distance travelled 
	// update the fitness
	// assert that the fitness does not equal -1 (the default value)
	for (int i = 0; i < population.size(); i++)
	{
		//assert(population.at(i).fitness == -1); //ensures that initial fitness is at default value (-1)

		S.update(population.at(i).P, citySet);
		assert(S.distance >= 100); //fulfills test that program can calculate total distance in policy (minimum dist is 100 because 100x100 grid) (LR.8)

		population.at(i).fitness = S.fitness;
		assert(population.at(i).fitness != -1); //fulfills test that program can set fitness of policy based on distance (MR.2)
												// doing it in loop fulfills test that program can set fitness of each policy in population (MR.3)
		S.fitness = 0;
		//cout << population.at(i).fitness << " ";
	}

	return population;
};


vector<policy> EA_downselect(vector<policy> P, int popSize) {
	vector<policy> population;
	assert(population.size() == 0);

	// binary tournament
	// halving size of vector
	// trying to minimize fitness
	// compare two random spots in population, send lower fitness to downselected population
	while (population.size() != popSize / 2)
	{
		int index = rand() % (popSize);
		int index2 = rand() % (popSize);

		// assure that index2 is not the same as index
		while (index == index2)
		{
			index2 = rand() % popSize;
		}

		//cout << "Index 1: " << index << " Index 2: " << index2 << endl;

		if (P.at(index).fitness < P.at(index2).fitness)
		{
			population.push_back(P.at(index));
		}
		else
		{
			population.push_back(P.at(index2));
		}
	}
	assert(population.size() == popSize / 2); //fulfills test that population can be downselected - size of vector halved (MR.4)

	return population;
};




int main()
{
	srand(time(NULL));
	ofstream fout;

	int numCities = 25;
	int popSize = 10;

	// creat vector of cities
	vector<city> citySet;

	// initialize cities and put them into vector
	for (int i = 0; i < numCities; i++)
	{
		city c;
		c.init();
		citySet.push_back(c);
		assert(c.xpos >= 0 && c.xpos <= xmax); // ensures city is within bounds. 
		assert(c.ypos >= 0 && c.ypos <= ymax); //fulfills test that program can represent a city (LR.1)
	}

	// check if size of vector is the same as the number of cities needed
	assert(citySet.size() == numCities);

	// create salesman
	salesman S;
	S.init(citySet);

	assert(S.xpos == citySet[0].xpos && S.ypos == citySet[0].ypos); //checks to see if agent is in first city
																	//fulfills test that program can represent an agent (LR.2)

	vector<policy> population;

	population = EA_init(numCities, popSize);
	assert(population.size() == popSize / 2); //fulfills test that program can initiate population of policies (MR.1)

	int numGenerations = 300;

	fout.clear();
	fout.open("LearningCurve.txt");

	for (int gen = 0; gen < numGenerations; gen++)
	{
		population = EA_replicate(population, popSize, numCities, S, citySet);
		assert(population.size() == popSize); // fulfills test that population is back at max capacity with mutated copies (MR.5)

		population = EA_eval(population, popSize, numCities, S, citySet);
		assert(population.size() == popSize);

		population = EA_downselect(population, popSize);
		assert(population.size() == popSize / 2);

		fout << "Generation:" << "\t" << gen + 1 << "\t" << "Fitnesses: " << "\t";
		for (int policy = 0; policy < popSize / 2; policy++)
		{
			fout << population.at(policy).fitness << "\t";
		}
		fout << endl;

		//reset fitness 
		for (int polIndex = 0; polIndex < popSize / 2; polIndex++)
		{
			population.at(polIndex).fitness = 0;
		}
	}

	system("pause");
	return 0;
}




