#include <iostream>
#include <FGFDMExec.h>
#include <fstream>
#include <vector>

using namespace std;

/* The Fuction Controller takes input the observables from the simulation and 
   outputs the control inputs that will be passed back to the simulation. */ 
double* controller(double*);

string RootDir = "../";
int main(int argc, char **argv)
{  
  
  ofstream myfile;
  myfile.open ("Hello.txt");  
  
  
  JSBSim::FGFDMExec *FDMExec = new JSBSim::FGFDMExec();
  string AircraftName ="sgs126";
  string ResetName ="airborne";
  double ts,tend,dt;
  ts=0.0;
  tend=200.0;
  dt=0.083;
  FDMExec->SetAtmosphereWindfield("/Users/Sreekanth/Documents/ISAE/Project/JSBSIM/JSBSim/src/WindField_Config.txt");
  FDMExec->SetSimtime(ts,tend,dt);
  FDMExec->LoadModel( RootDir + "aircraft",
                               RootDir + "engine",
                               RootDir + "systems",
                               AircraftName);
  JSBSim::FGInitialCondition *IC = FDMExec->GetIC();
    if ( ! IC->Load(ResetName)) {
      delete &FDMExec;
      cerr << "Initialization unsuccessful" << endl;
      exit(-1);
	}
	
  FDMExec->DoTrim(0);
  double obs[3]; 
  double cmd[] = {0.0,0.0,0.0};
  double reward, vg,vgdot,udot,vdot,wdot;
  
  vector <double> rec;
  
  while (FDMExec->Run()){
  
  double x,y,z,t;
  t=FDMExec->GetPropertyValue("sim-time-sec");
  x=FDMExec->GetPropertyValue("position/distance-from-start-lat-mt");
  y=FDMExec->GetPropertyValue("position/distance-from-start-lon-mt");
  z=FDMExec->GetPropertyValue("position/h-sl-meters");
  
  obs[0] = 0.3048*FDMExec->GetPropertyValue("velocities/v-down-fps"); 
  obs[1] = FDMExec->GetPropertyValue("velocities/thetadot-rad_sec") - FDMExec->GetPropertyValue("aero/alphadot-rad_sec");
  obs[2] = FDMExec->GetPropertyValue("attitude/phi-rad");
  
  vg = 0.3048*FDMExec->GetPropertyValue("velocities/vg-fps");
  udot = FDMExec->GetPropertyValue("accelerations/udot-ft_sec2"); 
  vdot = FDMExec->GetPropertyValue("accelerations/vdot-ft_sec2");
  wdot = FDMExec->GetPropertyValue("accelerations/wdot-ft_sec2");
  vgdot = 0.3048*sqrt(udot*udot+vdot*vdot+wdot*wdot);
  
  reward = 0.3048*FDMExec->GetPropertyValue("velocities/h-dot-fps")+vg*vgdot/9.80665;
  
  rec.push_back(reward);
  
  bool thermSinceAWhile=0;
  
  if (rec.size()>10)
  {
  	thermSinceAWhile = 1;
  	for(int i=0;i<10;i++)
  	{
  		thermSinceAWhile = thermSinceAWhile*(rec.at(rec.size()-10+i)<0);
  	}
  	cout <<" cococococ    "<<x<<"     "<<reward<<endl;
  	
  }
  
  if(thermSinceAWhile)
  {
  	//while(obs[0]<0)
  	//{
  		//FDMExec->SetAileronCmd(0.4);
  	//}
  }
  else
   FDMExec->SetAileronCmd(0.0);
  
  myfile<<t<<"\t"<<x<<"\t"<<y<<"\t"<<z<<endl;

  }
  //cout<<"The simulation has ended"<<endl;
  myfile.close();
}
