#include <iostream>
#include <sstream>
#include "filesystem.h"
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
using namespace std;

const int MAXCOMMANDS = 8;
const int NUMAVAILABLECOMMANDS = 15;

string availableCommands[NUMAVAILABLECOMMANDS] = {
    "quit","format","ls","create","cat","save","read",
    "rm","copy","append","rename","mkdir","cd","pwd","help"
};

/* Takes usercommand from input and returns number of commands, commands are stored in strArr[] */
int parseCommandString(const string &userCommand, string strArr[]);
int findCommand(string &command);


/* Splits a filepath into multiple strings */
std::vector<string> split(const string &filePath, const char delim = '/');
std::vector<string> getArg(const string &filepath); //delim = whitespace
string help();


//Functions
void ls(FileSystem &fs, string &path);
void create(FileSystem &fs, std::string &path);
void mkdir(FileSystem &fs, std::string &path);
void cd(FileSystem &fs, std::string &path);
void rm(FileSystem &fs, string &path);
void cat(FileSystem &fs, string &path);
void pathtest(string &path);
void copy(FileSystem &fs, string &args);

int main(void) {
	FileSystem fs;
    string userCommand, commandArr[MAXCOMMANDS];
    string user = "MasmoGendz";    // Change this if you want another user to be displayed
    string currentDir = fs.pwd();    // current directory, used for output

    std::string tmp;
    std::vector<string> args;

    bool bRun = true;


    do {
        cout << user << ": " << currentDir << " ~ $: ";
        getline(cin, userCommand);
		
        int nrOfCommands = parseCommandString(userCommand, commandArr);
        if (nrOfCommands > 0) {

            int cIndex = findCommand(commandArr[0]);
            switch(cIndex) {

            case 0: // quit
                bRun = false;
                cout << "Exiting\n";
                break;
            case 1: // format 
                // Call fileSystem.format()
				fs.format();
                break;
            case 2: // ls - klar
				ls(fs, userCommand);
                break;
            case 3: // create - klar
				create(fs, userCommand);
                break;
            case 4: // cat - klar
				cat(fs, userCommand);
                break;
            case 5: // save
				args = getArg(userCommand);
				fs.save(args[1]);
                break;
            case 6: // read
				args = getArg(userCommand);
				fs.read(args[1]);
                break;
            case 7: // rm - klar
				rm(fs, userCommand);
                break;

            case 8: // copy - klar
				copy(fs, userCommand);
                break;

            case 9: // append
				//i'll do this if i have the time
                break;

            case 10: // rename
				args = getArg(userCommand);
				fs.rename(args[1], args[2]);
                break;

            case 11: // mkdir
				mkdir(fs, userCommand);
                break;
            case 12: // cd
				cd(fs, userCommand);
                break;
            case 13: // pwd
            	cout << fs.pwd() << endl;
                break;

            case 14: // help
                cout << help() << endl;
                break;

            default:
                cout << "Unknown command: " << commandArr[0] << endl;

            }
        }
        currentDir = fs.pwd();
		
    } while (bRun == true);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    return 0;
}

void pathtest(string &path) {
	vector<string> tmp = getArg(path);
	vector<string> from;
	vector<string> to;
	string t;

	if (tmp.size() > 2) {
		from = split(tmp[1]);
		to = split(tmp[2]);
	}
	
	cout << "command: " << tmp[0] << endl;

	for (int i = 0; i < from.size(); i++) {
		cout << "From: " << i << ": " << from[i] << endl;
	}
	
	for (int i = 0; i < to.size(); i++) {
		cout << "To: " << i << ": " << to[i] << endl;
	}
	/* NOTE to self. 0 is always the file name OR i case of cd, target directory. vector.back() - 1 is alwasy subdir of root (frst file to look for). bector.back() is ., .., or nothing*/
	/*tmp = getArg(t);

	for (int i = 0; i < tmp.size(); i++) {
		cout << i << ": " << tmp[i] << endl;
	}*/
}

void copy(FileSystem &fs, string &path) {
	vector<string> tmp = getArg(path);
		
	if (tmp.size() > 2) {
		string source = tmp[1];
		string dest = tmp[2];

		fs.copy(source, dest);
	}
	else {
		return;
	}
	
}

void cat(FileSystem &fs, string &path) {
	vector<string> tmp = getArg(path);
	std::string file;

	if (tmp.size() > 1) {
		file = tmp[1];
	}

	cout << fs.cat(file) << endl;
}

void mkdir(FileSystem &fs, string &path) {
	vector<string> tmp = getArg(path);
	vector<string> file;
	string ret;

	if (tmp.size() > 1) {
		file = split(tmp[1]);
	}

	fs.mkdir(file[0]);
}

void cd(FileSystem &fs, string &path) {
	vector<string> tmp = getArg(path);
	string dir;
	string ret;

	if (tmp.size() > 1) {
		dir = tmp[1];
	}

	if (fs.cd(dir) != 0) {
		cout << "Error" << endl;
	}
}

void rm(FileSystem &fs, std::string &path) {
	vector<string> tmp = getArg(path);
	std::string file;
	string ret;

	if (tmp.size() > 1) {
		file = tmp[1];
	}
	else {
		return;
	}

	if (fs.rm(file)) {
		cout << "Successfully removed." << endl;
	}
	else {
		cout << "Failed removing entry." << endl;
	}


}
void ls(FileSystem &fs, std::string &path){
	vector<string> tmp = getArg(path);
	vector<string> from;
	string t;

	if (tmp.size() > 1) {
		from = split(tmp[1]);
		
	}
	
	std::vector<string> retVal = fs.ls(from);

	if(retVal.empty()){
		cout << "Nothing in directory" << endl;
	}else{
		for(int i = 0; i < retVal.size(); i++){
			cout << retVal[i] << "		";

		}
		cout << endl;
	}


}

void create(FileSystem &fs, std::string &path) {
	
	vector<string> tmp = getArg(path);
	vector<string> from;
	string t;

	if (tmp.size() > 1) {
		from = split(tmp[1]);

	}
	
	std::string content;
	getline(cin, content);
	tmp.resize(512);



	if (fs.create(from, content)){
	     cout << "successfully created file " << endl;
	}else{
	     cout << "failed to create " << endl;
	}


	
}

std::vector<string> getArg(const std::string &arg){
	std::vector <string> ret;
	istringstream ss(arg);

	do{
		std::string tmp;
		ss >> tmp;
		ret.push_back(tmp);

	}while(ss);

	return ret;
}

int parseCommandString(const string &userCommand, string strArr[]) {
    stringstream ssin(userCommand);
    int counter = 0;
    while (ssin.good() && counter < MAXCOMMANDS) {
        ssin >> strArr[counter];
        counter++;
    }
    if (strArr[0] == "") {
        counter = 0;
    }
    return counter;
}

int findCommand(string &command) {
    int index = -1;
    for (int i = 0; i < NUMAVAILABLECOMMANDS && index == -1; ++i) {
        if (command == availableCommands[i]) {
            index = i;
        }
    }
    return index;
}

std::vector<string> split(const string &filePath, const char delim) {
    vector<string> output;
    string cpy = filePath;
	string test;
    size_t end = cpy.find_last_of(delim);
    if (cpy.length() > end+1) {
        output.push_back(cpy.substr(end+1, cpy.length()));
    }

    while (end != 0 && end!= std::string::npos) {

        cpy = cpy.substr(0, cpy.find_last_of('/'));
        //cout << cpy << endl;
        end = cpy.find_last_of(delim);
        output.push_back(cpy.substr(end+1, cpy.length()));

    }

    return output;
}

string help() {
    string helpStr;
    helpStr += "OSD Disk Tool .oO Help Screen Oo.\n";
    helpStr += "-----------------------------------------------------------------------------------\n" ;
    helpStr += "* quit:                             Quit OSD Disk Tool\n";
    helpStr += "* format;                           Formats disk\n";
	helpStr += "* ls;			                    Lists contents of the current directory.\n";
    helpStr += "* create <path>:                    Creates a file and stores contents in <path>\n";
    helpStr += "* cat    <path>:                    Dumps contents of <file>.\n";
    helpStr += "* save   <real-file>:               Saves disk to <real-file>\n";
    helpStr += "* read   <real-file>:               Reads <real-file> onto disk\n";
    helpStr += "* rm     <file>:                    Removes <file>\n";
    helpStr += "* copy   <source>    <destination>: Copy <source> to <destination>. Can't copy cross-directory.\n";
    helpStr += "* append <source>    <destination>: Appends contents of <source> to <destination>\n";
    helpStr += "* rename <old-file>  <new-file>:    Renames <old-file> to <new-file>\n";
    helpStr += "* mkdir  <directory>:               Creates a new directory called <directory>\n";
    helpStr += "* cd     <directory>:               Changes current working directory to <directory>\n";
    helpStr += "* pwd:                              Get current working directory\n";
    helpStr += "* help:                             Prints this help screen\n";
    return helpStr;
}
