/*
 * filesystem.cpp
 *
 *  Created on: 16 okt. 2015
 *  Author: Nimagoniband
 *	Only God and I know what this code does. 
 *  Currently, Only God knows.
 */

#include "filesystem.h"
//#include <unistd.h>

#include <windows.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <sstream>


FileSystem::FileSystem(){
	//allokera hdd
	
	this->root = new dir("root");
	this->curDir = this->root;
	this->path = "/";
	
	

}

FileSystem::~FileSystem() {
	delete this->root;
}

void FileSystem::format(){
	//the function below overwrites the content in all the blocks. sets char = 0.
	mMemblockDevice.reset();

	//traverse inorder and remove all files and subdirs (if empty)
	dir* walker = this->root;
	inorderTraverse(walker);
}

void FileSystem::inorderTraverse(dir* &ptr) {
	if (ptr->NrOfDirs + ptr->NrOfFiles > 0) {
		for (int i = 0; i < ptr->NrOfDirs; i++) {
			inorderTraverse(ptr);
			int index;
			std::string file = ptr->subDirs[i]->dirName;
			for (int i = 0; i < ptr->NrOfDirs; i++) {
				if (file == ptr->subDirs[i]->dirName) {
					index = i;
				}
			}
			ptr->rm(true, file, index);
		}
		for (int i = 0; i < ptr->NrOfFiles; i++) {
			int index;
			std::string file = ptr->files[i].name;
			for (int i = 0; i < ptr->NrOfDirs; i++) {
				if (file == ptr->files[i].name) {
					index = i;
				}
			}
			ptr->rm(false, file, index);
		}
	}
	else {
		return;
	}
}

std::vector<std::string> FileSystem::ls(std::vector<std::string> &path) {

	dir* workingDir = curDir;
	std::vector<std::string> ret;
	
	if (path.size() == 0) {
		//do nothing
	}
	else {
		//Now; check the string to see where you should go
		std::string whereToGo = path.front();
		std::string checkLevel = path.back();
		if (checkLevel == "..") {
			workingDir = workingDir->parentDir;
			path.pop_back();
			if (!tmpPath(workingDir, path, whereToGo)) {
				return ret;
			}
		}
		else if (checkLevel == ".") {
			//No need to change directory
			path.pop_back();
			if (!tmpPath(workingDir, path, whereToGo)) {
				return ret;
			}
		}
		else {
			workingDir = this->root;
			if (!tmpPath(workingDir, path, whereToGo)) {
				return ret;
			}
		}

		

	}


	if(workingDir->NrOfDirs != 0){
		for(int i = 0; i < workingDir->NrOfDirs; i++){
			ret.push_back(workingDir->getFolders(i) + "/");
		}

	}

	if(workingDir->NrOfFiles != 0){
		for(int i = 0; i < workingDir->NrOfFiles; i++){
			ret.push_back(workingDir->getFiles(i));

		}
	}

	return ret;
	

}

bool FileSystem::create(std::vector<std::string> &path, std::string &content) {


	if (path.size() == 0) {
		return false;
	}
	dir* workingDir = curDir;
	std::string file = path[0];
	
	
	if(path.size() > 0) {
		//Now; check the string to see where you should go
		if (path.size() > 1) {
			std::string whereToGo = path[1];
			std::string checkLevel = path.back();

			if (checkLevel == "..") {
				workingDir = workingDir->parentDir;
				path.pop_back();
				if (!tmpPath(workingDir, path, whereToGo)) {
					return false;
				}
			}
			else if (checkLevel == ".") {
				//No need to change directory
				path.pop_back();
				if (!tmpPath(workingDir, path, whereToGo)) {
					return false;
				}
			}
			else {
				workingDir = this->root;
				if (!tmpPath(workingDir, path, whereToGo)) {
					return false;
				}
			}

		}

	}

	content.resize(512);

	for (int i = 0; i < 250; i++) {
		if (mMemblockDevice.used(i) == false) {
			workingDir->createFile(file, i);
			mMemblockDevice.setUsed(i, true);
			mMemblockDevice.writeBlock(i, content);
			return true;
		}
	}



	return false;
}

bool FileSystem::create(std::string name, std::string content, dir* &path) {
	content.resize(512);

	for (int i = 0; i < 250; i++) {
		if (mMemblockDevice.used(i) == false) {
			path->createFile(name, i);
			mMemblockDevice.setUsed(i, true);
			mMemblockDevice.writeBlock(i, content);
			return true;
		}
	}

	return false;
}

std::string FileSystem::cat(std::string &file) {
	//filepath
	
	dir* workingDir = this->curDir;
	std::string ret;
	int index = -1;
	
	

	for(int i = 0; i < workingDir->NrOfFiles; i++){
		if(file == workingDir->files[i].name){
			index = workingDir->files[i].startingBlock;

		}
	}

	if(index != -1){
		ret = mMemblockDevice.getContent(index);
	}else{
		ret = "No such file.\n";
	}

	return ret;

} //Still a bit buggy

bool FileSystem::rm(std::string &file) {

	dir* workingDir = this->curDir;
	int index = -1;
	bool indicator = NULL;

	for (int i = 0; i < workingDir->NrOfFiles; i++) {
		if (file == workingDir->files[i].name) {
			index = workingDir->files[i].startingBlock;
			indicator = false;
		}
	}

	for (int i = 0; i < workingDir->NrOfDirs; i++) {
		if (file == workingDir->subDirs[i]->dirName) {
			index = i;
			indicator = true;
		}
	}

	if (index != -1) {
		if (!indicator) {
			mMemblockDevice.reset(index);
			mMemblockDevice.setUsed(index, false);
			workingDir->rm(indicator, file, index);
		}
		else {
			int items = workingDir->subDirs[index]->NrOfDirs + workingDir->subDirs[index]->NrOfFiles;
			if (items > 0) {
				std::cout << "Can't remove folder containing items." << std::endl;
			}
			else {
				workingDir->rm(indicator, file, index);
			}
		}
	}

	return true;
}

std::string FileSystem::rename(std::string &source, std::string &newName) {
	int index = -1;
	bool indicator = false;
	bool ret = false;

	for (int i = 0; i < this->curDir->NrOfFiles; i++) {
		if (source == this->curDir->files[i].name) {
			index = i;
			indicator = false;
		}
	}

	for (int i = 0; i < this->curDir->NrOfDirs; i++) {
		if (source == this->curDir->subDirs[i]->dirName) {
			index = i;
			indicator = true;
		}
	}


	if (indicator) {
		this->curDir->subDirs[index]->dirName = newName;
	}
	else {
		this->curDir->files[index].name = newName;
	}

	return "Success";
}


void FileSystem::mkdir (const std::string &name){ //return 0 if sucessfull, -1 if not
	//create in root, add to the root list of ptr to new dirs
	//if node list
	this->curDir->mkdir(name, curDir);
}

std::string FileSystem::pwd () const{
	return this->path;
}

int FileSystem::cd(std::string &dirName) {
	
	int index = -1;
	std::string ret = "";

	if (dirName != "..") {
		for (int i = 0; i < this->curDir->NrOfDirs; i++) {
			if (this->curDir->subDirs[i]->dirName == dirName) {
				index = i;
			}
		}

		if (index != -1) {
			this->curDir = this->curDir->subDirs[index];
			this->path = this->path + this->curDir->dirName + "/";
		}
		else {
			ret = -1; //no such directory
		}
	}
	else {
		if (this->curDir->parentDir != nullptr) {
			std::string parse = this->curDir->dirName;
			std::string::size_type i = this->path.find(parse);

			if (i != std::string::npos)
				this->path.erase(i, parse.length());

			this->curDir = this->curDir->parentDir;
		}
		else {
			ret = -2;
		}
	}

	return 0;


}

int FileSystem::getNrOfDirs() const {
	return this->curDir->NrOfDirs;
}

int FileSystem::getNrOfFiles() const {
	return this->curDir->NrOfFiles;
}

std::vector<std::string> FileSystem::getArg(const std::string &arg) {
	std::vector <std::string> ret;
	std::istringstream ss(arg);

	do {
		std::string tmp;
		ss >> tmp;
		ret.push_back(tmp);

	} while (ss);

	return ret;
}

bool FileSystem::copy(std::string source, std::string dest) {
	
	int Sindex = -1;
	int Dindex = -1;
	bool exists = false; //assume false;
	std::string content;
	content.resize(512);
	for (int i = 0; i < this->curDir->NrOfFiles; i++) {
		if (this->curDir->files[i].name == source) {
			Sindex = i;
		}
	}

	if (Sindex == -1) {
		return false;
	}
	else {
		for (int i = 0; i < this->curDir->NrOfFiles; i++) {
			if (this->curDir->files[i].name == dest) {
				Dindex = i;
			}
		}

		if (Dindex == -1) {
			int tmp = this->curDir->files[Sindex].startingBlock;
			
			content = mMemblockDevice.getContent(tmp);
			std::vector<std::string> tmpv;
			tmpv.push_back(dest);
			create(tmpv, content);
			
		}
		else {
			int dtmp = this->curDir->files[Dindex].startingBlock;
			int stmp = this->curDir->files[Sindex].startingBlock;
			content = mMemblockDevice.getContent(stmp);
			
			mMemblockDevice.writeBlock(dtmp, content);
		}

	}


	return true;



}

int FileSystem::getDirIndex(std::string name) const {
	int ret = -1;
	
	for (int i = 0; i < this->curDir->NrOfDirs; i++) {
		if (name + "/" == this->curDir->subDirs[i]->dirName) {
			ret = i;

		}
	}


	return ret;
}


int FileSystem::getFileIndex(std::string name) const {
	int ret = -1;

	for (int i = 0; i < this->curDir->NrOfFiles; i++) {
		if (name == this->curDir->files[i].name) {
			ret = i;
		}
	}


	return ret;
}

void FileSystem::save(const std::string &filename) const {
	//This is for cross-platform 
	#if (defined  _WIN32)
	if (!CreateDirectory(filename.c_str(), NULL)) {
		std::string msg("The location directory did not exists, can't be created\n");
		throw std::runtime_error(msg);
	}
	#else
	if (!mkdir(filename.c_str(), NULL)) {
		std::string msg("The location directory did not exists, can't be created\n");
		throw std::runtime_error(msg);
	}
	#endif


	std::ofstream structure;
	std::ofstream files;
	std::ofstream directory;
	directory.open(filename + "/files.txt");
	files.open(filename + "/information.txt");
	structure.open(filename + "/structure.txt");
	

	dir* walker = this->root;
	inorderTraverse(walker, structure, directory, files);
	structure << '?';
	structure.close();
	directory.close();
	files.close();

}

void FileSystem::read(const std::string &filename) {
	std::ifstream structure;
	std::ifstream files;
	std::ifstream directory;

	directory.open(filename + "/directory.txt");
	files.open(filename + "/information.txt");
	structure.open(filename + "/structure.txt");
	
	
	dir* walker = this->root;
	deSerialize(walker, structure, directory, files);
	structure.close();
	directory.close();
	files.close();
	
	
}

void FileSystem::inorderTraverse(dir* &paraPtr, std::ofstream &structure, std::ofstream &directories, std::ofstream &files) const {
	if (root == nullptr) return;

	structure << 'D';
	directories << paraPtr->dirName << "\n";
	
	
	for (int i = 0; i < paraPtr->NrOfDirs; i++) { //for the subdirs
		inorderTraverse(paraPtr->subDirs[i], structure, directories, files);
	}
	for (int j = 0; j < paraPtr->NrOfFiles; j++) { //for the subfiles
		structure << 'F';
		//files << paraPtr->files[j].name << "\n" << mMemblockDevice.getContent(root->files[j].startingBlock) << "\n";
	}

	
	structure << '#';
	
}

int FileSystem::deSerialize(dir* &paraPtr, std::ifstream &structure, std::ifstream &directories, std::ifstream &files) {
	
	char val = structure.get();

	if (val == '#' || val == EOF)	return 1;

	if (val == 'D') {
		paraPtr->mkdir("DIR", paraPtr);
	}
	else {
		create("FILE", "yo", paraPtr);
	}

	while (1) {
		if(deSerialize(paraPtr->subDirs[paraPtr->NrOfDirs - 1], structure, directories, files)) break; //-1 is just the offset
	}

	/*else if (val == 'D') {
		paraPtr->mkdir("DIR", paraPtr);
		while (structure.peek() == 'D') {
			deSerialize(paraPtr->subDirs[paraPtr->NrOfDirs-1], structure, directories, files); //-1 is just the offset
		}
	}
	
	while (val == 'F') {
		std::vector<std::string> tmp;
		tmp.push_back("FILE");
		std::string stmp = "CONTENT";
		create(tmp, stmp);
		val = structure.get();
	}*/
	
	
	return 0;

}

bool FileSystem::tmpPath(dir* &walker, std::vector<std::string> &filePath, std::string whereToStop) { //this one FINALLY works

	int index = -1;
	bool found = false;
	int VectorSize = filePath.size() - 1;

	if (filePath.size() == 0) { //safetycheck, incase of sameone writing only . or ..
		return true;
	}

	while (!found) {
		//hitta platsen i vectorn, och flytta dit.
		for (int i = 0; i < walker->NrOfDirs; i++) {
			if (walker->subDirs[i]->dirName == filePath[VectorSize] + '/') {
				index = i;
			}
		}

		if (index == -1) {
			return false;
		}
		else {
			walker = walker->subDirs[index];
			VectorSize--;
		}


		if (walker->dirName == whereToStop + '/') {
			return true;
		}
		


	}


	return true;

} 

