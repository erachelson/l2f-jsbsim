#include "Windfield.hpp"
#include <cstdlib>

using namespace std;

Windfield::Windfield()
{
	dmin = 250.0;
	minX = 0.0;
	maxX = 1000.0;
	minY = 0.0;
	maxY = 1000.0;
	minZ = 0.0;
	maxZ = 1400.0;
	tstart = 0.0;
	tend = 100.0;
	z_i = 1400.0;
	minLifeTime =600.0;
	maxLifeTime =840.0;
	minRestTime =1.2;
	maxRestTime =12.0;
	
	MAX_NB_THERMALS = 0.6*(maxX-minX)*(maxY-minY)/(z_i*dmin);

	ambientWindX = 0.0;
	ambientWindY = 0.0;
	
	createScenario();
}

Windfield::Windfield(double ts, double te):tstart(ts),tend(te)
{
	dmin = 250.0;
	minX = -5000.0;
	maxX = 5000.0;
	minY = -5000.0;
	maxY = 5000.0;
	minZ = 0.0;
	maxZ = 1400.0;
	z_i = 1400.0;
	minLifeTime =600.0;
	maxLifeTime =840.0;
	minRestTime =1.2;
	maxRestTime =12.0;
	
	MAX_NB_THERMALS = 0.6*(maxX-minX)*(maxY-minY)/(z_i*dmin);

	ambientWindX = 0.0;
	ambientWindY = 0.0;
	
	createScenario();
}

Windfield::Windfield(double ts, double te, double mX, double mY):tstart(ts),tend(te),maxX(mX),maxY(mY)
{
	dmin = 250.0;
	minX = 0.0;
	minY = 0.0;
	minZ = 0.0;
	maxZ = 1400.0;
	z_i = 1400.0;
	minLifeTime =600.0;
	maxLifeTime =840.0;
	minRestTime =1.2;
	maxRestTime =12.0;
	
	MAX_NB_THERMALS = 0.6*(maxX-minX)*(maxY-minY)/(z_i*dmin);

	ambientWindX = 0.0;
	ambientWindY = 0.0;
	
	createScenario();
}

Windfield::Windfield(string filename)
{
	std::ifstream file(filename.c_str());
	std::string   line;
	std::vector<double> data;
	std::vector<std::vector<double> > datavector;
	int readline=1;
	while(std::getline(file, line))
	{
		std::istringstream stream(line);
		if(line[0]=='#')
			continue;
		switch(readline)
		{
			case 1: stream>>minX>>maxX;break;
			case 2: stream>>minY>>maxY;break;
			case 3: stream>>minZ>>maxZ;break;
			case 4: stream>>tstart>>tend;break;
			case 5: stream>>minLifeTime>>maxLifeTime;break;
			case 6: stream>>minRestTime>>maxRestTime;break;
			case 7: stream>>ambientWindX>>ambientWindY;break;
			case 8: stream>>z_i;break;
		}

		if(readline>8)
		{	
			double th[]={0.0,0.0,0.0,0.0,0.0,0,0};
			stream>>th[0]>>th[1]>>th[2]>>th[3]>>th[4]>>th[5];
			data.push_back(th[0]);
			data.push_back(th[1]);
			data.push_back(th[2]);
			data.push_back(th[3]);
			data.push_back(th[4]);
			data.push_back(th[5]);
			datavector.push_back(data);
			data.clear();
		}
		readline++;
	}
	file.close();
	int nth=0,eth=0;
	for(int i=0; i<datavector.size(); ++i) {
//	for(auto data: datavector) {
		std::vector<double> d = datavector.at(i);
		int err=0;
		if(d[0]<minX||d[0]>maxX||d[1]<minY||d[1]>maxY)
		{
			cout<<"Thermal Centre out of the domain"<<endl;
			err=1;
		}
		if(d[2]<tstart||d[2]>tend)
		{
			cout<<"Thermal birth is out of time domain"<<endl;
			err=1;
		}
		if(err==1)
		{
			cerr<<"!!!Error thermal cannot be created for data provided at line : "<<nth<<endl;
			eth++; 
		}
		if(err!=1)
		{
			Thermals newth(d[0],d[1],z_i,d[2],d[3],d[4],d[5]);
			thermalvector.push_back(newth);
		}
		nth++;
	}
	cout<<(nth-eth)<<" configurations read and thermals are created for the same."<<endl;
	string outfilename ="WindField_Config.txt";
	cout<<"Creating and Saving Configuration to file..."<<endl<<outfilename<<endl;
	saveConfig(outfilename);
}

void Windfield::createScenario()
{
	double t=tstart;
	int nb_Alive_t,total=0;
	while(t<tend)
	{
		nb_Alive_t=numberAliveAtTime(t);
		
		while(nb_Alive_t<MAX_NB_THERMALS)
		{
			double tLife,tRest,center[2];
			tLife= minLifeTime + ( maxLifeTime - minLifeTime ) * (double) rand() / (RAND_MAX);
			tRest= minRestTime + ( maxRestTime - minRestTime ) * (double) rand() / (RAND_MAX);
			
			if(createThermalCenter(center,t))
			{	
				//Thermals newth(center[0],center[1],z_i, 3.64,t,tRest,tLife);
				//thermalvector.push_back(newth);
				thermalvector.push_back(Thermals(center[0],center[1],z_i, 3.64,t,tRest,tLife));
				total++;
			}
			nb_Alive_t=numberAliveAtTime(t);		
		}	
		if(nb_Alive_t>MAX_NB_THERMALS)
		{
			cout<<"Error at time = "<<t<<". Number of thermals is more than the maximum allowed."<<endl<<
					"Number of alive thermals = "<<nb_Alive_t<<". Maximum allowed = "<<MAX_NB_THERMALS<<endl;
		}
		t+=1.0;
	}
	cout<<"Number of Thermals created is "<<total<<endl;
		
	time_t now=time(0);
	
	tm *lct=localtime(&now);
	
	string filename ="WindField_Config_created.txt";
	cout<<"Creating and Saving Configuration to file..."<<endl<<filename<<endl;
	saveConfig(filename); 
}

int Windfield::numberAliveAtTime(double t)
{
	int count=0;
	for(int i=0; i<thermalvector.size(); ++i) {
	//for(auto th: thermalvector)	{
		Thermals& th=thermalvector.at(i);
		th.updateThermal(t);
		if(th.isAlive)
			count++;
	}
	return count;	
	
}
int Windfield::numberEffAtTime(double t)
{
	int count=0;
	for(int i=0; i<thermalvector.size(); ++i) {
	//for(auto th: thermalvector)	{
		Thermals& th = thermalvector.at(i);
		th.updateThermal(t);
		if(th.isEffective)
			count++;
	}
	return count;	
	
}

bool Windfield::createThermalCenter(double* center, double t)
{
	bool newCenterIsValid = false;
	int maxWhileIterations = 10000;
	int whileIterations = 0;

	//Repeat until the center is valid
	while (!newCenterIsValid)
	{
		//Increment while iterations counter
		whileIterations = whileIterations + 1;

		//Abort if too many iterations
		if (whileIterations > maxWhileIterations)
		{
			break;
		}

		//Obtain new center randomly
		center[0] = minX + dmin + (maxX-minX-2.0*dmin)*(double) rand() / (RAND_MAX);
		center[1] = minY + dmin + (maxY-minY-2.0*dmin)*(double) rand() / (RAND_MAX);

		//Suppose it is valid
		newCenterIsValid = true;
		
		//Check it is valid for all the other centers
		for(int i=0; i<thermalvector.size(); ++i) {
		// for(auto th: thermalvector) {
			Thermals& th=thermalvector.at(i);
			th.updateThermal(t);
			if(th.isAlive)
			{
				newCenterIsValid =sqrt((center[0]-th.xc)*(center[0]-th.xc) + (center[1]-th.yc)*(center[1]-th.yc))> 2.0*dmin;
			}
			if(!newCenterIsValid)
				break;				
		}
		
	}
	if (!newCenterIsValid)
	{
		cout<<"!!!Warning a Valid centre could not be created for the Thermal !!!"<<endl;
	}

	return(newCenterIsValid);
}

double* Windfield::calcWind(double x, double y, double z, double t)
{
	double position[] = {x,y,z};
	double w[3];
	w[0]=0.0;
	w[1]=0.0;
	w[2]=0.0;
	double w_e=0.0;
	
	if(x<minX||x>maxX||y<minY||y>maxY||z>maxZ){
		cout<<"WARNING!! The Specified location {"<<x<<","<<y<<","<<z<<"} in outside the area of the Windfild"<<endl
			<<"The wind output will be Zero"<<endl;
		return(w);
		} 
	int model,total=0;
	// 1.Lenschow with Gaussian distribution
	// 2.Lenschow with Geodon model
	// 3.Allen
	// 4.Childress
	// 5.Lawrance
	model=3;
	wind[0]=ambientWindX;
	wind[1]=ambientWindY;
	wind[2]=0.0;
	
	if(model==3)
		w_e=environSink(z,t);
	
	for(int i=0; i<thermalvector.size(); ++i) {
		Thermals& th=thermalvector.at(i);
	// for(auto th: thermalvector) 	{
		th.calcThWind(model,w,position,t,ambientWindX,ambientWindY,w_e);

		wind[0]+=w[0];
		wind[1]+=w[1];
		wind[2]+=w[2];
	}
	if(model==3 && wind[2]==0.0)
		wind[2] = w_e;
	return wind;
}

void Windfield::saveConfig(std::string filename)
{
	ofstream myfile;
  	myfile.open (filename.c_str());
  	myfile<<"# Definition of the domain :\n# MinX and MaxX"<<endl
  			<<minX<<" "<<maxX<<endl
			<<"# MinY and MaxY"<<endl
			<<minY<<" "<<maxY<<endl
			<<"# MinZ and MaxZ"<<endl
			<<minZ<<" "<<maxZ<<endl
			<<"# Starting and Ending time of simulation"<<endl
			<<tstart<<" "<<tend<<endl
			<<"# MinLifeTime and MaxLifeTime of Thermal"<<endl
			<<minLifeTime<<" "<<maxLifeTime<<endl
			<<"# MinRestTime and MaxRestTime of Thermal"<<endl
			<<minRestTime<<" "<<maxRestTime<<endl
			<<"# Ambient WindX and Ambient WindY"<<endl
			<<ambientWindX<<" "<<ambientWindY<<endl
			<<"# Convective Mixing layer thickness"<<endl
			<<z_i<<endl;
	
	myfile<<"# The list of Thermals created :"<<endl
			<<"# CentreX   CentreY   wStar   tBirth   tRest   tLife"<<endl;
	for(int i=0; i<thermalvector.size(); ++i) {
		Thermals& th=thermalvector.at(i);
	//for(auto th: thermalvector)	{
		myfile<<th.xc0<<" "<<th.yc0<<" "<<th.w_star<<" "<<th.tBirth<<" "<<th.restTime<<" "<<th.lifeTime<<endl;
	}
  	myfile.close();
}

double Windfield::environSink(double z, double t)
{
	double areaTh=0.0, massFlowTh=0.0, w_e;
	double avgUpdraft,radius,swd;
	double regX=maxX-minX;
	double regY=maxY-minY;
	for(int i=0; i<thermalvector.size(); ++i) {
		Thermals& th=thermalvector.at(i);
	//for(auto th: thermalvector) 	{
		avgUpdraft=0.0;
		radius=0.0;
		th.updateThermal(t);		
		if ((0.5 < (z / th.zi))&& ((z / th.zi) < 0.9))
		{
			swd = 2.5 * (z/th.zi - 0.5);
		}
		else
		{ 
			swd = 0;
		}
		avgUpdraft=th.w_star * pow((z / th.zi),1.0/3.0) * (1.0 - 1.1*z/th.zi)*(1-swd)*th.timeCoeff();
		radius = 0.102 * pow((z / th.zi),1/3) * (1 - 0.25*z/th.zi) * th.zi;
		massFlowTh += avgUpdraft*PI*radius*radius;
		
		if(th.timeCoeff()!=0)
			areaTh += PI*radius*radius;
	}
	
	w_e = - massFlowTh / (regX*regY - areaTh);
	
	if(w_e>0.0)
		w_e=0.0;
	
	return(w_e);	
}
