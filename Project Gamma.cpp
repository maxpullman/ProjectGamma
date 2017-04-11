// Project Gamma.cpp : Defines the entry point for the console application.
// Max Pullman
// April 4, 2017
// ME 493: Autonomy
// Project Gamma


//I would like to acknowledge Honi Ahmadian for a large majority of the structure help and Logan Yliniemi for the setup of 
//all Evolutionary Algotrithm functions especially relating to the non-use of pointers


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
		xcoor = rand() % 100;
		ycoor = rand() % 100;
		//LR.1: Program represents a city with some coordinates in the grid
		assert(xcoor >= 0 && xcoor <= 100);
		assert(ycoor >= 0 && ycoor <= 100);
	};

};


class agent {
public:
	double dist;
	double xcoor;
	double ycoor;
	double fitness;

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
		//Distance calculation will continue until the policy used is finished
		for (int i = 0; i < policy.size(); i++)
		{
			city New;
			New = Set.at(policy.at(i));
			dist += sqrt(pow((abs(xcoor - New.xcoor)), 2) + pow(abs((ycoor - New.ycoor)), 2));
			//LR.7: Program can determine the distance between two cities
			assert(dist >= 0);
			fitness = dist;
			//Makes sure that agent is now placed at the new city which it traveled to
			xcoor = New.xcoor;
			ycoor = New.ycoor;
		}
		xcoor = Set.at(0).xcoor;
		ycoor = Set.at(0).ycoor;
	};

};


class policy {
public:
	vector<int> A;
	double fitness;

	void init(int Cnum) {
		//Creating one policy that has an index representing each individual city
		for (int i = 0; i< Cnum; i++)
		{
			A.push_back(i);
		}

		//Random shuffles the indexing of the cities
		random_shuffle(A.begin() + 1, A.end());
		//Checking for matching number of cities between the policy and 
		//LR.3: Program has a complete set of cities which match an order in which to travel
		assert(A.size() == Cnum); 
		assert(*A.begin() == 0); 
		//LR.5: Checking to make sure the agent will always start at the first city in the created vector A

		fitness = -1.0;
	};

	void mutate(int Cnum) {
		int change;
		int change2;
		int change3;

		//Mutation process for different number of cities
		//Swaps two cities for larger number of cities and only 1 swap for smaller amount of cities
		//Randomizes a number between the cities but accounts for the first city traveled to
		change = rand() % (Cnum - 1) + 1; 
		change2 = rand() % (Cnum - 1) + 1; 
		change3 = rand() % (Cnum - 1) + 1;

		while (change == change2)
		{
			change2 = rand() % (Cnum - 1) + 1;
		}

		//LR.5: Checking to make sure the agent will always start at the first city in the created vector A
		assert(change != 0 && change2 != 0); 

		int place;
		place = A.at(change);
		A.at(change) = A.at(change2);
		A.at(change2) = place;

		if (Cnum > 20)
		{
			while (change2 == change3 || change == change3)
			{
				change3 = rand() % (Cnum - 1) + 1;
			}

			//LR.5: Checking to make sure the agent will always start at the first city in the created vector A
			assert(change2 != 0 && change3 != 0); 

			place = A.at(change2);
			A.at(change2) = A.at(change3);
			A.at(change3) = place;
		}
	};

};


vector<policy> EA_init(int Citynum, int popSize) {
	vector<policy> population;

	//Sets the inital starting population at half size
	for (int i = 0; i < popSize / 2; i++)
	{
		policy Cool;
		Cool.init(Citynum);
		population.push_back(Cool);
	}
	//MR.1: Program can completely initialize a certain population of policies
	assert(population.size() == popSize / 2); 

	return population;
};


vector<policy> EA_replicate(vector<policy> P, int popSize, int Cnum, agent S) {
	vector<policy> population;
	population = P;
	assert(population.size() == popSize / 2);

	while (population.size() < popSize)
	{
		int spot = rand()%(popSize/2);
		policy Now;
		Now = population.at(spot);
		Now.mutate(Cnum);
		population.push_back(Now);
		//LR.4: Program can mutate a policy slightly by swapping one or two cities based on the number of cities
		assert(population.at(spot).A != Now.A); 
	}

	
	assert(population.size() == popSize);

	return population;

};


vector<policy> EA_evaluate(vector<policy> P, int popSize, agent S, vector <city> Set) {
	vector<policy> population;
	population = P;
	assert(population.size() == popSize);

	
	for (int i = 0; i < population.size(); i++)
	{
		//Updating fitness and distance traveled
		S.update(population.at(i).A, Set);
		//LR.8: Program can calculate the distance of traveled cities
		//Set the distance criteria to the lowest possible value for 10 cities 
		assert(S.dist >= 45); 

		population.at(i).fitness = S.fitness;
		//MR.2: Program can pick the best fitness based on the traveled distance
		//MR.3: Program can set some fitness for a population of policies (Since the assert test is in a population loop)
		//Checks to make sure the fitness does not equal -1 (the default value)
		assert(population.at(i).fitness != -1); 
		S.fitness = 0;
	}

	return population;
};


vector<policy> EA_downselect(vector<policy> P, int popSize) {
	vector<policy> population;
	assert(P.size() == popSize);

	//Completing the binary tournament and halfing the size of the policy vector where the smaller fitness is better
	//Compares two random spots in the population and sends the smaller fitness to the next round
	while (population.size() != popSize / 2)
	{
		int spot = rand() % (popSize);
		int spot2 = rand() % (popSize);

		while (spot == spot2)
		{
			spot2 = rand() % popSize;
		}
		assert(spot != spot2);

		if (P.at(spot).fitness < P.at(spot2).fitness)
		{
			population.push_back(P.at(spot));
		}
		else
		{
			population.push_back(P.at(spot2));
		}
	}
	//MR.4: Program can downselect based on a binary tournament configuration with distance being the fitness
	assert(population.size() == popSize / 2); 

	return population;
};




int main()
{
	srand(time(NULL));
	ofstream fout;

	int popSize = 100;
	int Cnum;
	
	cout << "How many cities would you like to travel to? ";
	cin >> Cnum;
	

	vector<city> Set;

	//Initializing cities and puts them into a standard vector 
	//Want to determine optimal distance for number of cities at 10 so created a special case
	
	if (Cnum == 10) {
		for (int i = 0; i < Cnum; i++)
		{
			city c;
			c.xcoor = 5 * i;
			c.ycoor = 10;
			Set.push_back(c);
			//LR.1: A city can be represented in the program within the grid bounds
			assert(c.xcoor >= 0 && c.xcoor <= 100); 
			assert(c.ycoor >= 0 && c.ycoor <= 100); 
		}
	}
	else {
		for (int i = 0; i < Cnum; i++)
		{
			city c;
			c.init();
			Set.push_back(c);
			//LR.1: A city can be represented in the program within the grid bounds
			assert(c.xcoor >= 0 && c.xcoor <= 100); 
			assert(c.ycoor >= 0 && c.ycoor <= 100); 
		}
	}

	//Creating an agent
	agent Smith;
	Smith.init(Set);
	//LR.2: Program can create some sort of agent
	assert(Smith.xcoor == Set[0].xcoor && Smith.ycoor == Set[0].ycoor); 

	vector<policy> population;

	population = EA_init(Cnum, popSize);
	//MR.1: Program can initialize a population of policies
	assert(population.size() == popSize / 2); 
	
	int Generations = 300;

	//Setting up the text file for the learning curve
	fout.clear();
	fout.open("LearningCurve100.txt");

	for (int stat = 0; stat < Generations; stat++)
	{
		population = EA_replicate(population, popSize, Cnum, Smith);
		//MR.5: Program can repopulate to the carry capacity of the population by making a mutated copy of the orginal
		assert(population.size() == popSize); 


		//I would like to recognize Honi Ahmadian and Sierra Gonzales, from whom I recieved this code from which determines if every
		//particular integer within the policy integer vector is different than every other integer to ensure no city is visited twice

		for (int i = 0; i < popSize; i++)
		{
			//LR.5: Agent will always start at the same city during one particular experiment
			assert(population.at(i).A.at(0) == 0); 

			for (int j = 0; j < Cnum; j++)
			{
				for (int k = j + 1; k < Cnum; k++)
				{
					//LR.6: The agent will never visit the same city more than once	
					assert(population.at(i).A.at(j) != population.at(i).A.at(k)); 							  
				}
			}
		}

		population = EA_evaluate(population, popSize, Smith, Set);
		assert(population.size() == popSize);

		population = EA_downselect(population, popSize);
		assert(population.size() == popSize / 2);

		fout << "Gen:" << "\t" << stat + 1 << "\t" << "Fit: " << "\t";
		for (int policy = 0; policy < popSize / 2; policy++)
		{
			fout << population.at(policy).fitness << "\t";
		}
		fout << endl;

		int counter = 0;

		if (Cnum == 10 && stat == Generations - 1) {
			for (int i = 0; i < population.size(); i++) {
				if (population.at(i).fitness == 45) {
					counter++;
				}
			}
			//HR.2: Program develops good policies to show significance for 10 cities
			assert(counter >= 1);
		}

		//Need to reset the fitness every single time for each policy
		for (int Index = 0; Index < popSize / 2; Index++)
		{
			population.at(Index).fitness = 0;
		}
	}

	cout << "Agent has calculated the best path, please check the text file." << endl;

	system("pause");
	return 0;
}




