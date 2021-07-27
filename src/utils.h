#ifndef __UTILS_
#define __UTILS_

#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <cmath>
#include <sys/stat.h>

#ifdef __linux__ 
#include <cstring>
#define DIRLIMITER "/";
#elif _WIN32
#include <string>
#define DIRLIMITER "\\";
#endif

using namespace std;

extern bool help;
extern bool quiet;
extern bool verbose;

inline double lratio(const double& x, const double& y) { return y ? x / y : 0; }

inline double percent(const long long& x, const long long& y) { return lratio(100.0 * x, double(y)); }

inline void ae(string& line, const long long& value) { line += "," + to_string(value); }

inline void ae(string& line, const int& value) { line += "," + to_string(value); }

inline void ae(string& line, const double& value) { line += "," + to_string(value); }

inline void ae(string& line, const string& value) { line += "," + value; }

inline bool eq(const char* in, const char* ref)
{
	size_t count = 0;
	const size_t reflen = strlen(ref);
	while (*in) {
		if (ref[count] != *in)
			count = 0;
		else
			count++;
		in++;
		if (count == reflen)
			return true;
	}
	return false;
}

inline bool eql(const char* in, const char* ref)
{
	size_t count = 0;
	const size_t reflen = strlen(ref);
	while (*in) {
		if (tolower(ref[count]) != *in)
			count = 0;
		else
			count++;
		in++;
		if (count == reflen)
			return true;
	}
	return false;
}


#endif
