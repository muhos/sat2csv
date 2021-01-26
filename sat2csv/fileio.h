#ifndef __FS_
#define __FS_

#include "utils.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#define PREFIX1 "result_"
#define PREFIX2 "simp_"
#define TABLEEXT ".csv"
#define RESULTEXT ".txt"
#define CNFEXT ".cnf"
#define BZ2EXT ".bz2"
#define XZEXT ".xz"

inline bool validFile(const string& fileStr, string& fileName, ifstream& file)
{
	if (fileStr.find(TABLEEXT) != -1) return true;
	size_t pos = -1;
	if ((pos = fileName.find(PREFIX1)) != -1) fileName.erase(pos, strlen(PREFIX1));
	else if ((pos = fileName.find(PREFIX2)) != -1) fileName.erase(pos, strlen(PREFIX2));
	else return true;
	fileName.erase(fileName.find(RESULTEXT), strlen(RESULTEXT));
	file.open(fileStr, ifstream::in);
	if (!file.is_open()) {
		cerr << "Cannot read the input path: " << fileStr << endl;
		exit(EXIT_FAILURE);
	}
	return false;
}

inline bool exists(const char* path)
{
	struct stat info;
	if (stat(path, &info) == -1) return false;
	return (info.st_mode & S_IFDIR) ? true : false;
}

inline void createFile(ofstream& outfile, const string& directory, const string& outname, const string& prefix = "")
{
	if (outname.empty()) {
		cout << "output file name is not specified" << endl;
		exit(EXIT_FAILURE);
	}
	if (verbose) cout << "creating output file \"";
	string path = directory + DIRLIMITER;
	if (!prefix.empty()) path += prefix;
	path += outname + TABLEEXT;
	if (verbose) cout << path << "\"..";
	outfile.open(path, ofstream::out);
	if (!outfile.is_open()) {
		cout << "cannot open output file" << endl;
		exit(EXIT_FAILURE);
	}
	if (verbose) cout << " done." << endl;
}

#endif
