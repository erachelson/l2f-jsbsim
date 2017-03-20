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
  JSBSim::FGFDMExec *L2F;
  JSBSim::FGInitialCondition *IC;
  double ts,tend,dt;
  int option;
  
  //Observation
  ofstream myfile;
  
  // *** PARSE OPTIONS PASSED INTO THIS SPECIFIC APPLICATION: //
  option = options(argc, argv);
  
  //Creating New simulation environment
  L2F = new JSBSim::FGFDMExec();
  L2F->SetAircraftPath(RootDir + "aircraft");
  L2F->SetEnginePath(RootDir + "engine");
  L2F->SetSystemsPath(RootDir + "systems");
  

  if (option ==1){ 
  	L2F->LoadScript(ScriptName);
  	while (L2F->Run());
  	goto quit;
  }
  else if(option ==2){
    
  	L2F->SetAtmosphereWindfield(WindfieldFile);
  	L2F->LoadScript(ScriptName);
  	while (L2F->Run());
  
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
	L2F->SetSimtime(ts,tend,dt);
	L2F->LoadModel( RootDir + "aircraft",
							   RootDir + "engine",
							   RootDir + "systems",
							   AircraftName);
	IC = L2F->GetIC();
	if ( ! IC->Load(InitfileName)) {
	  delete &L2F;
	  cerr << "Initialization unsuccessful" << endl;
	  exit(-1);
	}
	
	if(!WindfieldFile.empty())
	  L2F->SetAtmosphereWindfield(WindfieldFile);
	
	L2F->DoTrim(0);

	while (L2F->Run()){
		double x,y,z,t;
		t=L2F->GetPropertyValue("sim-time-sec");
		x=L2F->GetPropertyValue("position/distance-from-start-lat-mt");
		y=L2F->GetPropertyValue("position/distance-from-start-lon-mt");
		z=L2F->GetPropertyValue("position/h-sl-meters");
					
		if (t>30.0 && L2F->GetPropertyValue("attitude/phi-rad")>-0.62)
			L2F->SetAileronCmd(-0.5);
		else
			L2F->SetAileronCmd(0.0);

		myfile<<t<<"\t"<<x<<"\t"<<y<<"\t"<<z<<endl;
		}
	 myfile.close();
}
 	

quit:
  cout<<"The simulation has ended"<<endl;
  delete L2F;
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
