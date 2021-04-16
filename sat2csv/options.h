#ifndef __OPTS_
#define __OPTS_

#include "utils.h"

/* fixed options */
#define TOOLUSAGE "Usage: sat2csv <directory> [options]"
#define OPTHELP   "-h               print help messages for all options"
#define OPTVERB   "-v               be verbose"
#define OPTQUIET  "-q               be quiet"

/* configurable options */
#define OPTSOLVER "-s     <output>    produce a <output>.csv table for SAT solver logs"
#define OPTREDS   "-p     <output>    produce a <output>.csv table for SAT solver/simplifier reductions"
#define OPTCNF    "-cnf   <output>    produce a <output>.csv table for CNF formulas"
#define OPTBMC    "-bmc   <output>    produce a <output>.csv table for model checker logs"
#define OPTSTATS  "-stats <output>    produce a <output>.csv table for SAT solver search statistics"
#define SOLVERS  "minisat, glucose, cadical, sigma, parafrost"
#define CHECKERS "CBMC"

inline void readOptionsTo(vector<string>& options)
{
	options.push_back(OPTHELP);
	options.push_back(OPTVERB);
	options.push_back(OPTQUIET);
	options.push_back(OPTSOLVER);
	options.push_back(OPTREDS);
	options.push_back(OPTCNF);
	options.push_back(OPTBMC);
	options.push_back(OPTSTATS);
}

inline bool validOption(const char* arg, const vector<string>& options)
{
	if (arg[0] == '-') {
		for (auto& opt : options) {
			if (opt.find(arg) != -1) return true;
		}
		return false;
	}
	return true;
}

inline void printHelp(const vector<string>& options)
{
	cout << "sat2csv: data collector for SAT applications" << endl << endl;
	cout << TOOLUSAGE << endl << endl;
	for (auto& opt : options) {
		cout << " " << opt << endl;
		if (opt == OPTSOLVER) cout << "\t\t  supported: " << SOLVERS << endl;
		if (opt == OPTBMC) cout << "\t\t  supported: " << CHECKERS << endl;
	}
}

#endif


