#include <iostream>
#include <pwd.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "tools/Freelancing.h"
#include "tools/JJJUMP.h"
#include "tools/Minesveeper37.h"
#include "tools/Vespera.h"
#include "tools/VoidTicket.h"

using namespace std;

void waitForInput()
{
	std::cout << "\nPress enter to exit!\n";
	std::cin.get();
}

void genVT_full(const std::string& path, const std::string& name)
{
	V6Level* V6 = new V6Level(path + name);

	VoidTicket::fullLevel(V6);

	V6->saveAS(path + "Gen_" + name);
	delete V6;
}

void genVT_preview(const std::string& path, const std::string& name)
{
	V6Level* V6 = new V6Level(path + name);

	VoidTicket::demoLevel(V6);

	V6->saveAS(path + "Gen_" + name);
	delete V6;
}

void genFreelancingTemplate(const std::string& path, const std::string& name)
{
	V6Level* V6 = new V6Level(path + name);

	Freelancing::generateTemplate(V6);

	V6->saveAS(path + "Gen_" + name);
	delete V6;
}

void genFreelancing(const std::string& path, const std::string& name, const std::string& templateLevel)
{
	V6Level* TMP = new V6Level(path + templateLevel);
	V6Level* V6 = new V6Level(path + name);

	Freelancing::generateLevel(V6, TMP);

	delete TMP;
	V6->saveAS(path + "Gen_" + name);
	delete V6;
}

void genVespera(const std::string& path, const std::string& name, const std::string& credits)
{
	V6Level* V6 = new V6Level(path + name);
	V6Level* Credits = new V6Level(path + credits);

	Vespera::randomWalls(V6);
	Vespera::copyCreditScripts(V6, Credits);

	V6->saveAS(path + "Gen_" + name);
	delete V6;
	delete Credits;
}

int main(int argc,
		 char* argv[],
		 char* envp[])
{
	time_t start;
	time(&start);
	(void) envp;
	//---------------
	srand(time(NULL));
	// For some reason pugixml really does not like paths starting with ~
	// So just figure out the homedirectory and then use an absolute path
	std::string homeDir = std::string(getpwuid(getuid())->pw_dir);
	std::string path = homeDir + "/.local/share/VVVVVV/levels/";
	std::string levelName1 = "Void_Ticket_v29.vvvvvv";
	std::string levelName2 = "Void_Ticket_Demo.vvvvvv";
	std::string levelName3 = "moth2021.vvvvvv";
	std::string levelName4 = "Cloud_template.vvvvvv";
	std::string levelName5 = "Vespera_Scientifica_v1.1.vvvvvv";
	std::string levelName6 = "CreditPrototype.vvvvvv";
	if(argc == 1) {
		printf("Please select a task:\n"
			   "\tmines   : Compile Minesveeper 37\n"
			   "\tjjjump  : Compile JJJUMP\n"
			   "\tvoid    : Generate Void Ticket Background\n"
			   "\tvoid0   : Generate Void Ticket Demo Background\n"
			   "\tmoth21  : Generate Freelancing Clouds\n");
	} else {
		std::string arg1 = argv[1];
		if(arg1 == "mines") {
			printf("Compiling Minesveeper 37\n");
			Minesveeper::generateScripts(path);
		} else if(arg1 == "jjjump") {
			bool backPort = (argc > 2) && (std::string(argv[2]) == "backport");
			printf("Compiling JJJUMP (%s)\n", backPort ? "2.3.6" : "2.4+");
			JJJUMP::generateScripts(path, backPort);
		} else if(arg1 == "void") {
			printf("Generate Void Ticket Background\n");
			genVT_full(path, levelName1);
		} else if(arg1 == "void0") {
			printf("Generate Void Ticket Demo Background\n");
			genVT_preview(path, levelName2);
		} else if(arg1 == "moth21") {
			printf("Generate Freelancing Clouds\n");
			//genFreelancingTemplate(path, levelName4);
			genFreelancing(path, levelName3, levelName4);
		} else if(arg1 == "scriptTest") {
			//V6Level* V6 = new V6Level(path + levelName5);
			V6Level* V6 = new V6Level(path + levelName5); //"JJJUMP.vvvvvv");
			V6Script& Cr = V6->getScripts()["cs_final"];
			for(const std::string& l : Cr.getLines()) {
				printf("'%s'\n", l.c_str());
			}
			delete V6;
		} else {
			printf("Unknown argument: %s\n", arg1.c_str());
		}
	}
	//---------------
	time_t stop;
	time(&stop);
	double dt = stop - start;
	std::cout << "\nProgram took: " << dt << "s\n";
	//waitForInput();
}
