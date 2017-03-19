#include <iostream>
#include <FGFDMExec.h>
#include <fstream>
#include <string>

using namespace std;

int options(int, char**);
void PrintHelp(void);

string RootDir = "../";
string AircraftName;
string InitfileName;
string ScriptName;
string WindfieldFile;

int main(int argc, char **argv)
{  
  // *** INITIALIZATIONS *** //
  
  //Flight simulator objects
  JSBSim::FGFDMExec *FDMExec;
  JSBSim::FGInitialCondition *IC;
  double ts,tend,dt;
  int option;
  
  //Observation
  ofstream myfile;
  
  // *** PARSE OPTIONS PASSED INTO THIS SPECIFIC APPLICATION: //
  option = options(argc, argv);
  
  //Creating New simulation environment
  FDMExec = new JSBSim::FGFDMExec();
  FDMExec->SetAircraftPath(RootDir + "aircraft");
  FDMExec->SetEnginePath(RootDir + "engine");
  FDMExec->SetSystemsPath(RootDir + "systems");
  

  if (option ==1){ 
  	FDMExec->LoadScript(ScriptName);
  	while (FDMExec->Run());
  	goto quit;
  }
  else if(option ==2){
    
  	FDMExec->SetAtmosphereWindfield(WindfieldFile);
  	FDMExec->LoadScript(ScriptName);
  	while (FDMExec->Run());
  
  	goto quit;
  }
  
  else{
	myfile.open ("../../Output/output.txt");  

	// *** Definition *** //
	AircraftName ="sgs126";
	InitfileName ="initfile";
	ts=0.0;
	tend=100.0;
	dt=0.083;
	FDMExec->SetSimtime(ts,tend,dt);
	FDMExec->LoadModel( RootDir + "aircraft",
							   RootDir + "engine",
							   RootDir + "systems",
							   AircraftName);
	IC = FDMExec->GetIC();
	if ( ! IC->Load(InitfileName)) {
	  delete &FDMExec;
	  cerr << "Initialization unsuccessful" << endl;
	  exit(-1);
	}
	
	if(!WindfieldFile.empty())
	  FDMExec->SetAtmosphereWindfield(WindfieldFile);
	
	FDMExec->DoTrim(0);

	while (FDMExec->Run()){
		double x,y,z,t;
		t=FDMExec->GetPropertyValue("sim-time-sec");
		x=FDMExec->GetPropertyValue("position/distance-from-start-lat-mt");
		y=FDMExec->GetPropertyValue("position/distance-from-start-lon-mt");
		z=FDMExec->GetPropertyValue("position/h-sl-meters");
					
		if (t>30.0 && FDMExec->GetPropertyValue("attitude/phi-rad")>-0.62)
			FDMExec->SetAileronCmd(-0.5);
		else
			FDMExec->SetAileronCmd(0.0);

		myfile<<t<<"\t"<<x<<"\t"<<y<<"\t"<<z<<endl;
		}
	 myfile.close();
}
 	

quit:
  cout<<"The simulation has ended"<<endl;
  delete FDMExec;
  return 0;
}

int options(int count, char **arg)
{
  int i;
  if (count == 1) {
    return(-1);
  }

#define gripe cerr <<endl<< "Option '" << keyword 	\
	<< "' requires a value, as in '"	\
	<< keyword << "=something'" << endl << endl;

  for (i=1; i<count; i++) {
    string argument = string(arg[i]);
    string keyword(argument);
    string value;
    int n=argument.find("=");
    if (n != string::npos && n > 0) {
      keyword = argument.substr(0, n);
      value = argument.substr(n+1);
    }
    
    if (keyword == "--help") {
      PrintHelp();
      exit(1);
    } else if (keyword == "--script") {
      if (!value.empty()) {
        ScriptName = value;
      } else {
        gripe;
        exit(1);
      }
    } else if (keyword == "--thermal") {
      if (!value.empty()) {
        WindfieldFile = value;
      } else {
        gripe;
        exit(1);
      }
    } else {
      cerr << endl << "  Parameter: " << argument << " not understood" << endl;
      PrintHelp();
      exit(1);
    }
  }

  // Post-processing for script options.

  if (!WindfieldFile.empty() && !ScriptName.empty()) {
    return(2);
  }
  else if (WindfieldFile.empty() && !ScriptName.empty()) {
    return(1);
  }
  else if (!WindfieldFile.empty() && ScriptName.empty()) {
    return(-1);
  } 

}


void PrintHelp(void)
{
  cout << "  Usage: jsbsim <options>" << endl << endl;
  cout << "  options:" << endl;
    cout << "    --help  returns this message" << endl;
    cout << "    --script=<filename>  specifies a script to run" << endl;
    cout << "    --thermal=<filename>  specifies the thermal file to be used" << endl<<endl;

  cout << "  NOTE1: There can be no spaces around the = sign when" << endl;
  cout << "        an option is followed by a filename" << endl << endl;
  cout << "  NOTE2: The options 'script' and 'thermal' can be used together" << endl<<endl;
}
