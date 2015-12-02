#pragma once

#include <string>
#include <sstream>

struct File {
	std::string name;
	int startingBlock; //like i-nodes
};

class dir {

public:
	std::string dirName;
	dir* parentDir;
	dir** subDirs;
	File* files; //efter vi har allokerat och skapat en fil så ska vi kunna lägga in pekaren till filen i denna array
	int NrOfDirs;
	int NrOfFiles;
	int allocateFile;
	int allocateDir;
	int newFolders;
	


	dir();
	dir(const std::string &name);
	dir(const std::string &name, dir* parent);

	~dir();

	std::string getName();
	std::string getFiles(int index);
	std::string getFolders(int index);
	int getNrOfFolders();
	int getNrOfFiles();
	void expandDir();
	void expandFiles();
	File* createFile(std::string name, int position);
	void mkdir(std::string name, dir* parent);
	void rm(bool type, std::string name, int index);
	void deleteAll();

};
