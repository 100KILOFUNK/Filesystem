#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <sstream>
#include "memblockdevice.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include "dir.h"

struct list{
	int number;
	char type;
	std::string content;
};


class FileSystem
{
private:
	/* 
	 * I could do class dir as a seperate class, instead of an inner one since it has 
	 * become more than a datastructure, but it seems excessice really since FileSystem 
	 * is the only class which uses it. Also, time consuming to change it now.
	 */

    MemBlockDevice mMemblockDevice;
    dir* curDir;
    dir* root;
    std::string path;
	std::vector<list> tmpList;
	int nrOfentries;


public:
    FileSystem();
	~FileSystem();
    void format(); //formaterar disken, tar bort allt i filsystemet
    std::vector<std::string> ls(std::vector <std::string> &path);  // optional
    bool create(std::vector<std::string> &filePath, std::string &klar); //snart content, behöver lägga till path
	bool create(std::string name, std::string content, dir* &path);
	std::string cat(std::string &file); //klar
    void save(const std::string &saveFile) const; //save as a "real" file
    void read(const std::string &saveFile); //read from file
    bool rm(std::string &file); //ta bort fil eller mapp. ska inte kunna ta bort mapp om den innehåller saker.
    
	//find source, create new dest, copy char to another with the copyconstructor
    bool copy(std::string source, std::string dest);

    /* Optional - ill do these if i have the time or maybe version 2.0 */
    std::string append(const std::string &source, const std::string &app);
    std::string rename(std::string &source, std::string &newName);
    std::string chmod(int permission, const std::string &file);

    /* Add your own member-functions if needed */
    void mkdir(const std::string &dirName); //create a dir at the current location
    std::string pwd() const;
    int getNrOfDirs() const;
    int getNrOfFiles() const;
    int cd(std::string &dirName);
	bool tmpPath(dir* &tmp, std::vector<std::string> &filePath, std::string dir);
	std::vector<std::string> getArg(const std::string &arg);
	int getDirIndex(std::string name) const;
	int getFileIndex(std::string name) const;
	void inorderTraverse(dir* &paraPtr, std::ofstream &structure, std::ofstream &directories, std::ofstream &files) const;
	void inorderTraverse(dir* &ptr); //for use with format function
	int deSerialize(dir* &paraPtr, std::ifstream &structure, std::ifstream &directories, std::ifstream &files);

};

#endif // FILESYSTEM_H
