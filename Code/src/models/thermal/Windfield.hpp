#ifndef WINDFIELD_HPP_
#define WINDFIELD_HPP_

#include <vector>
#include "Thermals.hpp"
//#include <random>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>

class Windfield
{

private:	
	/* attributes */
	std::vector<Thermals> thermalvector;
	
	bool createThermalCenter(double*, double t);	
	//@param: location of centre as a double array [xc0,yc0] and time "t"
	//Returns true if a valid center xc0,yc0 can be created at a given instant "t" 
	//such that there is no overlapping of thermals. 
	 
	
public:

	double minX, maxX;
	double minY, maxY;
	double minZ, maxZ;
	double tstart, tend;
	double z_i;
	double dmin;

	double minLifeTime; // Thermal minimum existing time
	double maxLifeTime; // Thermal maximum existing time
	double minRestTime; // Thermal minimum rest Time
	double maxRestTime; // Thermal maximum rest time
		
    double ambientWindX;
	double ambientWindY;
	double wind[3];
	
	int MAX_NB_THERMALS; //The maximum number of thermals at any given time
        unsigned int NB_THERMALS;

	Windfield();
	//Default Construct with all properties of the windfield assigned
	
	Windfield(double, double);
	//Same as default construct but the start and end time of the simulation are given as input
	//@param1: start time of simulation
	//@param2: end time of simulation
	
	Windfield(double, double, double, double);
	//Same as default construct but the start time, end time of the simulation and the dimension of the domain are given as input
	//@param1: start time of simulation
	//@param2: end time of simulation
	//@param3: MaxX dimension
	//@param4: MaxY dimension
	
	Windfield(std::string);
	//An input file specifying all the property of the windfild is given and, based on which the windfield object is created.
	//The input file should be similar to the output of createScenario() method. 
	
	~Windfield()
	{
	}	
	double* calcWind(double x, double y, double z, double t);
	/*
	To calculate the effective wind from the windfield at the given point at a given time.
	The output is an array of the format [wind along x, wind along Y, wind along Z]
	*/
	
	int numberAliveAtTime(double t);
	//returns Number of alive Thermals at any given instant "t"
	
	int numberEffAtTime(double t); 
	//returns Number of effective Thermals at any given instant "t"
	
	void updateWindfield(double);
	/*
	*Updates the Windfield based on time elapsed and based on drift velocity.
	*A  thermal is considered dead if the lifeIndex of the thermal is more than the rest time and life time.
	*Irrespective of the simulation time, if the thermal is dead, it is considered that immediately another thermal has sprung to life
	*so at any given moment of time there will always be given number of thermals. Some active and some in resting phase.
	*/
	void saveConfig(std::string filename);
	
	void createScenario();
	
	double environSink(double z, double t);
	//Function to calculate the environmental sink rate for Allen Model.
	//@param z is the altitude at which the environmental sink rate has to be calculated
	//@param t is the simulation time at which it should be calculated

};
#endif

