#include "dir.h"



dir::dir()
{

	this->allocateDir = 10;
	this->allocateFile = 10;
	this->subDirs = new dir*[this->allocateDir];
	this->files = new File[this->allocateFile];
	this->dirName = "";
	this->parentDir = NULL;
	this->NrOfDirs = 0;
	this->NrOfFiles = 0;
	this->newFolders = 0;

	for (int i = 0; i < 10; i++) {
		this->subDirs[i] = nullptr;
	}
}


dir::dir(const std::string &name) {
	this->allocateDir = 10;
	this->allocateFile = 10;
	this->subDirs = new dir*[this->allocateDir];
	this->files = new File[this->allocateFile];
	this->dirName = name;
	this->parentDir = nullptr;
	this->NrOfDirs = 0;
	this->NrOfFiles = 0;
	this->newFolders = 0;
	

	for (int i = 0; i < 10; i++) {
		this->subDirs[i] = nullptr;
	}
}

dir::dir(const std::string &name, dir* parent) {
	this->allocateDir = 10;
	this->allocateFile = 10;
	this->subDirs = new dir*[this->allocateDir];
	this->files = new File[this->allocateFile];
	this->dirName = name;
	this->parentDir = parent;
	this->NrOfDirs = 0;
	this->NrOfFiles = 0;
	this->newFolders = 0;
	
	

	for (int i = 0; i < 10; i++) {
		this->subDirs[i] = nullptr;
	}
}

dir::~dir()
{
	//free the allocated mem
	for (int i = 0; i < NrOfDirs; i++) {
		delete subDirs[i];
	}
	
	delete[] subDirs;
	delete[] files;
}

std::string dir::getName(){
	return this->dirName;
}

std::string dir::getFiles(int index) {
	return this->files[index].name;
}

std::string dir::getFolders(int index) {
	return this->subDirs[index]->dirName;
}

int dir::getNrOfFolders() {
	return this->NrOfDirs;
}

int dir::getNrOfFiles() {
	return this->NrOfFiles;
}

void dir::expandDir() {
	this->allocateDir = this->allocateDir * 2;
	dir** largerArr = new dir*[this->allocateDir];

	for (int i = 0; i < this->allocateDir / 2; i++)
		largerArr[i] = this->subDirs[i];


	delete[] subDirs;

	for (int i = 0; i < this->allocateDir / 2; i++)
		this->subDirs[i] = largerArr[i];


	delete[] largerArr;


}

void dir::expandFiles() {

	this->allocateFile = this->allocateFile * 2;
	File* largerArr = new File[this->allocateFile];

	for (int i = 0; i < this->allocateFile / 2; i++)
		largerArr[i] = this->files[i];

	delete[] this->files;
	this->files = new File[this->allocateFile];

	for (int i = 0; i < this->allocateFile / 2; i++)
		this->files[i] = largerArr[i];


	delete[] largerArr;



}

File* dir::createFile(std::string name, int position) {

	if (this->NrOfFiles == this->allocateFile) {
		expandFiles();
	}
	this->files[this->NrOfFiles].name = name;
	this->files[this->NrOfFiles].startingBlock = position;
	this->NrOfFiles++;

	return &files[this->NrOfFiles - 1];
}

void dir::mkdir(std::string name, dir* parent) {

	if (this->NrOfDirs == this->allocateFile) {
		expandDir();
	}

	if (name == "") {
		std::stringstream ss;
		ss << "NewFolder" << this->newFolders;
		name = ss.str();
		this->newFolders++;
	}
	this->subDirs[this->NrOfDirs] = new dir(name, parent);
	this->NrOfDirs++;
}

void dir::rm(bool type, std::string name, int index) {
	//ta bort name 



	if (type) {
		//dir

		for (int i = index; i < this->NrOfDirs; i++) {
			this->subDirs[i] = this->subDirs[i + 1];
		}
		this->NrOfDirs--;

	}
	else {
		//files
		for (int i = index; i < this->NrOfFiles; i++) {
			this->files[i] = this->files[i + 1];
		}
		this->NrOfFiles--;
	}

}

void dir::deleteAll() {
	for (int i = 0; i < NrOfDirs; i++)
		delete subDirs[i];

	delete[] files;

}
