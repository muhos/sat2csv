#ifndef __PARSER_
#define __PARSER_

#include "utils.h"

// parafrost-specific colors
#define CREPORT		"\x1B[38;5;187m"
#define CREPORTVAL	"\x1B[38;5;106m"
#define CPRE1		"\x1B[38;5;78m"
#define CPRE2		"\x1B[38;5;168m"
#define CPOST1		"\x1B[32m"
#define CPOST2		"\x1B[38;5;125m"
#define CNORMAL		"\x1B[0m"

struct SIG_TIME {
	double vo, ve, hse, bce, hre, cot, rot, sot, sig, gc, io, mem;
	SIG_TIME() { memset(this, 0, sizeof(*this)); }
	void reset() { memset(this, 0, sizeof(*this)); }
	void toCSVHeader(string& csv_header);
	void toCSVLine(string& csv_line);
};

bool eatString(string& line, const char* str);

void eatSpaces(string& line);

string getDigits(string& line);

void eatNondigits(string& line);

void eatDigitsUntil(string& line, const char& prefix);

string getCharsUntil(string& line, const char& prefix);

string getDigitsUntil(string& line, const char& prefix);

void write2file(ofstream& file, vector<string>& lines);

void parseArguments(const int& argc, char** argv, const vector<string>& options, vector<string>& args);

void parse_bmc(const string& line, string& sat, string& verified, string& bmc_result,
	string& in_vars, string& in_cls, string& MDMCalls, string& MDs, string& MD_assumed,
	double& bmc_time, double& solve_time, double& simp_time);

void parse_time(const string& line, SIG_TIME& sigtime);

void parse_time(const string& line, string& sat, string& verified, double& solve_time, double& simp_time);

void parse_preLine(const string& line, string& orgVars, string& orgCls, string& lrnCls);

void parse_postLine(const string& line, string& afterVars, string& afterCls, string& lrnCls);

void parse_reds(string& line, string& in_vars, string& in_cls, string& triedReduns, string& orgReduns, string& lrnReduns);

#endif
