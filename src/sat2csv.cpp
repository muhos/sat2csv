
#include "fileio.h"
#include "parser.h"
#include "options.h"

// global flags
bool help = false, quiet = false, verbose = false;

int main(int argc, char** argv)
{
	/* handle commands */
	if (argc == 1) {
		cerr << "directory path not specified" << endl;
		cerr << "use -h for help" << endl;
		exit(EXIT_FAILURE);
	}
	vector<string> options;
	readOptionsTo(options);
	vector<string> args;
	parseArguments(argc, argv, options, args);
	/* scan given directory */
	string directory(argv[1]);
	ifstream inputFile;
	ofstream outputFile;
	vector<string> tablerows;
	for (auto& arg : args) {
		tablerows.clear();
		string input = getCharsUntil(arg, ' ');
		eatSpaces(arg);
		string outname = arg;
		if (input == "-s") {
			if (verbose) cout << "solver mode is set" << endl;
			createFile(outputFile, directory, outname);
			string outline = "CNF";
			ae(outline, "Simplify time (s)"), ae(outline, "Solve time (s)"), ae(outline, "Sat.");
			outputFile << outline << endl;
			double total_time = 0;
			int nSAT = 0, nUNSAT = 0;
			if (verbose) cout << "reading directory \"" << directory << "\" contents:" << endl;
			for (auto& p : fs::directory_iterator(directory)) {
				string fileStr(p.path().string()), fileName(p.path().filename().string());
				if (validFile(fileStr, fileName, inputFile)) continue;
				if (verbose) cout << " parsing file " << fileName << "..";
				string line, sat = "I", verified = "";
				double solve_time = 0.0, simp_time = 0.0;
				while (getline(inputFile, line))
					parse_time(line, sat, verified, solve_time, simp_time);
				inputFile.close();
				if (sat == "I") solve_time = -1;
				else {
					if (sat == "S") nSAT++;
					else nUNSAT++;
					total_time += simp_time + solve_time;
				}
				outline = fileName;
				ae(outline, simp_time), ae(outline, solve_time), ae(outline, sat), ae(outline, verified);
				tablerows.push_back(outline);
				if (verbose) cout << " done" << endl;
			}
			write2file(outputFile, tablerows);
			outline = "Total time:", ae(outline, total_time);
			ae(outline, "SAT:"), ae(outline, nSAT);
			ae(outline, "UNSAT:"), ae(outline, nUNSAT);
			outputFile << outline << endl;
			outputFile.close();
		}
		else if (input == "-cnf") {
			if (verbose) cout << "cnf mode is set" << endl;
			createFile(outputFile, directory, outname, "cnf_");
			outputFile << "CNF" << endl;
			if (verbose) cout << "reading directory \"" << directory << "\" contents:" << endl;
			for (auto& path : fs::directory_iterator(directory)) {
				string file = path.path().string();
				if (file.find(".cnf") != -1) {
					string fileName = path.path().filename().string();
					if (verbose) cout << " found file " << fileName << endl;
					size_t pos;
					if ((pos = fileName.find(CNFEXT)) != -1) fileName.erase(pos, strlen(CNFEXT));
					if ((pos = fileName.find(BZ2EXT)) != -1) fileName.erase(pos, strlen(BZ2EXT));
					if ((pos = fileName.find(XZEXT)) != -1) fileName.erase(pos, strlen(XZEXT));
					tablerows.push_back(fileName);
				}
			}
			write2file(outputFile, tablerows);
			outputFile.close();
		}
		else if (input == "-bmc") {
			if (verbose) cout << "bmc mode is set" << endl;
			createFile(outputFile, directory, outname, "bmc_");
			string outline = "CNF";
			ae(outline, "Variables"), ae(outline, "Clauses"), ae(outline, "Simplify time (s)"), ae(outline, "Solve time (s)");
			ae(outline, "BMC time (s)"), ae(outline, "BMC result"), ae(outline, "Sat."), ae(outline, "Sat. Model");
			ae(outline, "MDM Calls"), ae(outline, "MDs"), ae(outline, "MDs Assumed"), ae(outline, "Timeout");
			outputFile << outline << endl;
			double total_time = 0;
			if (verbose) cout << "reading directory \"" << directory << "\" contents:" << endl;
			for (auto& p : fs::directory_iterator(directory)) {
				string fileStr(p.path().string()), fileName(p.path().filename().string());
				if (validFile(fileStr, fileName, inputFile)) continue;
				if (verbose) cout << " parsing file " << fileName << "..";
				string line, sat = "I", verified = "", bmc_result = "";
				string in_vars = "0", in_cls = "0", mdmcalls = "0", mds = "0", md_assumed = "0";
				double solve_time = 0, simp_time = 0, bmc_time = 0, time_out = 0;
				while (getline(inputFile, line)) {
					parse_bmc(line, sat, verified, bmc_result, in_vars, in_cls, mdmcalls, mds, md_assumed, bmc_time, solve_time, simp_time, time_out);
				}
				inputFile.close();
				if (bmc_result.empty()) bmc_result = "NONE";
				if (bmc_result != "ERROR") {
					if (sat == "I") solve_time = 0;
					total_time += bmc_time;
					outline = fileName;
					ae(outline, in_vars), ae(outline, in_cls), ae(outline, simp_time), ae(outline, solve_time);
					ae(outline, bmc_time), ae(outline, bmc_result), ae(outline, sat), ae(outline, verified);
					ae(outline, mdmcalls), ae(outline, mds), ae(outline, md_assumed), ae(outline, time_out);
					tablerows.push_back(outline);
				}
				if (verbose) cout << " done" << endl;
			}
			write2file(outputFile, tablerows);
			outline = "Total Verify time :", ae(outline, total_time);
			outputFile << outline << endl;
			outputFile.close();
		}
		else if (input == "-red") {
			if (verbose) cout << "simplifer statistics mode is set" << endl;
			createFile(outputFile, directory, outname, "reductions_");
			string outline = "CNF";
			ae(outline, "V (org)"), ae(outline, "C(org)"), ae(outline, "V (rem)"), ae(outline, "C (rem)"), ae(outline, "%V"), ae(outline, "%C");
			ae(outline, "Tried reduns"), ae(outline, "Original reduns"), ae(outline, "Learnt Reduns"), ae(outline, "All Reduns");
			ae(outline, "%original"), ae(outline, "%learnt");
			outputFile << outline << endl;
			if (verbose) cout << "reading directory \"" << directory << "\" contents:" << endl;
			for (auto& p : fs::directory_iterator(directory)) {
				string fileStr(p.path().string()), fileName(p.path().filename().string());
				if (validFile(fileStr, fileName, inputFile)) continue;
				if (verbose) cout << " parsing file " << fileName << "..";
				string line, in_vars, in_cls, rem_vars, rem_cls, forced_units;
				string triedReduns = "0", orgReduns = "0", lrnReduns = "0", remReduns = "0";
				double vars_perc = 0, cls_perc = 0, orgReduns_perc = 0, lrnReduns_perc = 0, remReduns_perc = 0;
				while (getline(inputFile, line))
					parse_reds(line, in_vars, in_cls, rem_vars, rem_cls, forced_units, triedReduns, orgReduns, lrnReduns, remReduns);
				long long in_vars_val = atoll(in_vars.c_str());
				long long in_cls_val = atoll(in_cls.c_str());
				long long rem_vars_val = atoll(rem_vars.c_str()) + atoll(forced_units.c_str());
				long long rem_cls_val = atoll(rem_cls.c_str());
				vars_perc = ceil(percent(rem_vars_val, in_vars_val));
				cls_perc = ceil(percent(rem_cls_val, in_cls_val));
				orgReduns_perc = ceil(percent(atoll(orgReduns.c_str()), atoll(triedReduns.c_str())));
				lrnReduns_perc = ceil(percent(atoll(lrnReduns.c_str()), atoll(triedReduns.c_str())));
				inputFile.close();
				outline = fileName;
				ae(outline, in_vars), ae(outline, in_cls);
				ae(outline, rem_vars_val), ae(outline, rem_cls_val);
				ae(outline, vars_perc), ae(outline, cls_perc);
				ae(outline, triedReduns), ae(outline, orgReduns), ae(outline, lrnReduns);
				ae(outline, remReduns), ae(outline, orgReduns_perc), ae(outline, lrnReduns_perc);
				tablerows.push_back(outline);
				if (verbose) cout << " done" << endl;
			}
			write2file(outputFile, tablerows);
			outputFile.close();
		}
		else if (input == "-pro") {
			if (verbose) cout << "simplifer profiler mode is set" << endl;
			createFile(outputFile, directory, outname, "profile_");
			string outline = "CNF,";
			SIG_TIME sigTime;
			sigTime.toCSVHeader(outline);
			outputFile << outline << endl;
			if (verbose) cout << "reading directory \"" << directory << "\" contents:" << endl;
			for (auto& p : fs::directory_iterator(directory)) {
				string fileStr(p.path().string()), fileName(p.path().filename().string());
				if (validFile(fileStr, fileName, inputFile)) continue;
				if (verbose) cout << " parsing file " << fileName << "..";
				string line;
				while (getline(inputFile, line))
					parse_time(line, sigTime);
				inputFile.close();
				outline = fileName + ",";
				sigTime.toCSVLine(outline);
				tablerows.push_back(outline);
				if (verbose) cout << " done" << endl;
			}
			write2file(outputFile, tablerows);
			outputFile.close();
		}
		else if (input == "-drat") {
			if (verbose) cout << "verify mode is set" << endl;
			createFile(outputFile, directory, outname, "verify_");
			string outline = "CNF";
			ae(outline, "verify time (s)"), ae(outline, "proof bytes"), ae(outline, "outcome");
			outputFile << outline << endl;
			if (verbose) cout << "reading directory \"" << directory << "\" contents:" << endl;
			for (auto& p : fs::directory_iterator(directory)) {
				string fileStr(p.path().string()), fileName(p.path().filename().string());
				if (validFile(fileStr, fileName, inputFile)) continue;
				if (verbose) cout << " parsing file " << fileName << "..";
				string line, verified = "";
				double verify_time = 0.0;
				long long bytes = 0;
				while (getline(inputFile, line))
					parse_time(line, verified, verify_time, bytes);
				if (verified.empty()) verified = "UNKNOWN";
				inputFile.close();
				outline = fileName;
				ae(outline, verify_time), ae(outline, bytes), ae(outline, verified);
				tablerows.push_back(outline);
				if (verbose) cout << " done" << endl;
			}
			write2file(outputFile, tablerows);
			outputFile.close();
		}
		else if (input == "-stats") {
			if (verbose) cout << "solver statistics mode is set" << endl;
			createFile(outputFile, directory, outname, "stats_");
			string outline = "CNF";
			ae(outline, "C2V Ratio"), ae(outline, "Conflicts"), ae(outline, "Propagations");
			ae(outline, "Single Decisions"), ae(outline, "Multiple Decisions"), ae(outline, "MDM Calls");
			ae(outline, "Solve time (s)");
			outputFile << outline << endl;
			if (verbose) cout << "reading directory \"" << directory << "\" contents:" << endl;
			for (auto& p : fs::directory_iterator(directory)) {
				string fileStr(p.path().string()), fileName(p.path().filename().string());
				if (validFile(fileStr, fileName, inputFile)) continue;
				if (verbose) cout << " parsing file " << fileName << "..";
				double time = 0;
				string line, c2v, conflicts, propagations, single, multiple, calls;
				while (getline(inputFile, line)) {
					parse_stats(line, c2v, conflicts, propagations, single, multiple, calls, time);
				}
				inputFile.close();
				outline = fileName;
				ae(outline, c2v), ae(outline, conflicts), ae(outline, propagations);
				ae(outline, single), ae(outline, multiple), ae(outline, calls);
				ae(outline, time);
				tablerows.push_back(outline);
				if (verbose) cout << " done" << endl;
			}
			write2file(outputFile, tablerows);
			outputFile.close();
		}
	}
}

void SIG_TIME::toCSVHeader(string& csv_header) {
	csv_header.append("vo (ms),");
	csv_header.append("cot (ms),");
	csv_header.append("sot (ms),");
	csv_header.append("rot (ms),");
	csv_header.append("sig (ms),");
	csv_header.append("gc (ms),");
	csv_header.append("ve (ms),");
	csv_header.append("sub (ms),");
	csv_header.append("bce (ms),");
	csv_header.append("ere (ms),");
	csv_header.append("io (ms),");
	csv_header.append("mem (MB)");
}

void SIG_TIME::toCSVLine(string& csv_line) {
	csv_line += to_string(vo) + "," +
		to_string(cot) + "," +
		to_string(sot) + "," +
		to_string(rot) + "," +
		to_string(sig) + "," +
		to_string(gc) + "," +
		to_string(ve) + "," +
		to_string(sub) + "," +
		to_string(bce) + "," +
		to_string(ere) + "," +
		to_string(io) + "," +
		to_string(mem);
}

void parseArguments(const int& argc, char** argv, const vector<string>& options, vector<string>& args)
{
	if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') {
		printHelp(options);
		exit(EXIT_SUCCESS);
	}
	if (!exists(argv[1])) {
		cerr << "cannot find directory: " << argv[1] << endl;
		cerr << "use -h for help" << endl;
		exit(EXIT_FAILURE);
	}
	for (int i = 2; i < argc; i++) {
		if (validOption(argv[i], options)) {
			string opt(argv[i]), outname = "";
			if (opt[0] == '-') {
				if ((opt[1] != 'h' && opt[1] != 'v' && opt[1] != 'q') && ((i + 1) < argc && argv[i + 1][0] != '-'))
					outname = " " + string(argv[i + 1]);
				args.push_back(opt + outname);
			}
		}
		else {
			cerr << "unknown option: " << argv[i] << endl;
			cerr << "use -h for help" << endl;
			exit(EXIT_FAILURE);
		}
	}
	if (args.empty()) {
		cerr << "nothing to be done" << endl;
		exit(EXIT_SUCCESS);
	}
	for (const auto& arg : args) {
		if (arg == "-h") {
			printHelp(options);
			exit(EXIT_SUCCESS);
		}
		else if (arg == "-v") verbose = true;
		else if (arg == "-q") quiet = true, verbose = false;
	}
}

void parse_bmc(const string& line, string& sat, string& verified, string& bmc_result,
	string& in_vars, string& in_cls, string& MDMCalls, string& MDs, string& MD_assumed,
	double& bmc_time, double& time, double& simp_time, double& time_out)
{
	size_t vars_pos = line.find("variables");
	size_t cls_pos = line.find("clauses");
	if (vars_pos != -1 && cls_pos != -1) {
		string tmp(line);
		if (eq(tmp.c_str(), ".")) eatDigitsUntil(tmp, ' ');
		eatSpaces(tmp);
		in_vars = getDigitsUntil(tmp, ' ');
		eatNondigits(tmp);
		in_cls = getDigitsUntil(tmp, ' ');
	}
	else if (eq(line.c_str(), "Solver time")) { // parafrost
		size_t offset = line.find(CREPORTVAL);
		if (offset != -1) offset += strlen(CREPORTVAL);
		else offset = line.find(":") + 1;
		const char* n = line.c_str() + offset;
		time = atof(n);
	}
	else if (eq(line.c_str(), "Simplifier time")) { // parafrost
		size_t offset = line.find(CREPORTVAL);
		if (offset != -1) offset += strlen(CREPORTVAL);
		else offset = line.find(":") + 1;
		const char* n = line.c_str() + offset;
		simp_time = atof(n);
	}
	else if (eq(line.c_str(), "MDM calls")) { // parafrost
		size_t offset = line.find(CREPORTVAL);
		if (offset != -1) offset += strlen(CREPORTVAL);
		else offset = line.find(":") + 1;
		string n = line.c_str() + offset;
		MDMCalls = getDigitsUntil(n, ' ');
	}
	else if (eq(line.c_str(), "Assumed") && eq(line.c_str(), "Decisions")) { // parafrost
		size_t offset = line.find(CREPORTVAL);
		if (offset != -1) offset += strlen(CREPORTVAL);
		else offset = line.find(":") + 1;
		string n = line.c_str() + offset;
		MD_assumed = getDigitsUntil(n, ' ');
	}
	else if (!eq(line.c_str(), "Search") && eq(line.c_str(), "Decisions")) { // parafrost
		size_t offset = line.find(CREPORTVAL);
		if (offset != -1) offset += strlen(CREPORTVAL);
		else offset = line.find(":") + 1;
		string n = line.c_str() + offset;
		MDs = getDigitsUntil(n, ' ');
	}
	else if (eq(line.c_str(), "Runtime decision procedure")) { // solver time
		const char* n = line.c_str() + line.find(":") + 1;
		bmc_time = atof(n);
	}
	else if (eq(line.c_str(), "TIMEOUT")) { // timeout
		const char* n = line.c_str() + line.find(":") + 1;
		time_out = atof(n);
	}
	else if (eq(line.c_str(), "simplification time")) { // simp time in minisat
		const char* n = line.c_str() + line.find_last_of(":") + 1;
		simp_time = atof(n);
	}
	else if (line[0] == 'c' && eq(line.c_str(), "simplify")) { // cadical or kissat
		const char* n = line.c_str() + 1;
		simp_time = atof(n);
	}
	else if (bmc_result.empty() && eq(line.c_str(), "VERIFICATION SUCCESSFUL")) { // cbmc
		bmc_result = "SUCCESSFUL";
	}
	else if (bmc_result.empty() && eq(line.c_str(), "VERIFICATION FAILED")) { // cbmc
		bmc_result = "FAILED";
	}
	else if (eq(line.c_str(), "UNKNOWN") || eq(line.c_str(), "VERIFICATION ERROR")) { // cbmc
		bmc_result = "ERROR";
	}
	else if (eq(line.c_str(), "UNSATISFIABLE")) sat = "U";
	else if (eq(line.c_str(), "SATISFIABLE")) sat = "S";
	else if (eq(line.c_str(), "VERIFIED")) verified = "VERIFIED"; // parafrost
	else if (eq(line.c_str(), "NOT VERIFIED")) verified = "NOT VERIFIED"; // parafrost
}

void parse_time(const string& line, string& sat, string& verified, double& solve_time, double& simp_time)
{
	if (eq(line.c_str(), "Solver time")) { // parafrost
		size_t offset = line.find(CREPORTVAL);
		if (offset != -1) offset += strlen(CREPORTVAL);
		else offset = line.find(":") + 1;
		const char* n = line.c_str() + offset;
		solve_time = atof(n);
	}
	else if (eq(line.c_str(), "Simplifier time")) { // parafrost
		size_t offset = line.find(CREPORTVAL);
		if (offset != -1) offset += strlen(CREPORTVAL);
		else offset = line.find(":") + 1;
		const char* n = line.c_str() + offset;
		simp_time = atof(n);
	}
	else if (eq(line.c_str(), "total") &&
		!eq(line.c_str(), "process time") &&
		!eq(line.c_str(), "real time")) { // kissat
		const char* n = line.c_str() + 1;
		solve_time = atof(n);
	}
	else if (!eq(line.c_str(),  "percentage") && eq(line.c_str(), "total process time")) { // cadical
		const char* n = line.c_str() + line.find(":") + 1;
		solve_time = atof(n);
	}
	else if (eq(line.c_str(), "simplify")) { // cadical or kissat
		const char* n = line.c_str() + 1;
		simp_time = atof(n);
	}
	else if (eq(line.c_str(), "Simplification time")) { // minisat or glucose
		const char* n = line.c_str() + line.find(":") + 1;
		simp_time += atof(n);
	}
	else if (eq(line.c_str(), "CPU time")) { // minisat or glucose
		const char* n = line.c_str() + line.find(":") + 1;
		solve_time += atof(n);
	}
	else if (eq(line.c_str(), "UNSATISFIABLE")) 
		sat = "U";
	else if (eq(line.c_str(), "SATISFIABLE")) 
		sat = "S";
	else if (verified.empty() && eq(line.c_str(), "NOT VERIFIED")) 
		verified = "NOT VERIFIED";
	else if (verified.empty() && eq(line.c_str(), "VERIFIED"))
		verified = "VERIFIED";
}

void parse_time(const string& line, string& verified, double& verify_time, long long& bytes)
{
	if (eq(line.c_str(), "finished parsing")) {
		size_t offset = line.find("read") + 4;
		const char* n = line.c_str() + offset;
		bytes = atoll(n);
	}
	else if (eq(line.c_str(), "verification time")) {
		size_t offset = line.find(":") + 1;
		const char* n = line.c_str() + offset;
		verify_time = atof(n);
	}
	else if (eq(line.c_str(), "NOT VERIFIED")) 
		verified = "NOT VERIFIED";
	else if (eq(line.c_str(), "VERIFIED"))
		verified = "VERIFIED";
}

void parse_time(const string& line, SIG_TIME& sigtime)
{
	size_t offset = line.find(CREPORTVAL);
	if (offset != -1) offset += strlen(CREPORTVAL);
	else offset = line.find(":") + 1;
	string n = line.substr(offset);
	if (line.find("Var ordering") != -1) {
		sigtime.vo = atof(n.c_str());
	}
	else if (line.find("prepare") != -1) {
		sigtime.sig = atof(n.c_str());
	}
	else if (line.find("compact") != -1) {
		sigtime.gc = atof(n.c_str());
	}
	else if (line.find("transfer") != -1) {
		sigtime.io = atof(n.c_str());
	}
	else if (line.find("creation") != -1) {
		sigtime.cot = atof(n.c_str());
	}
	else if (line.find("sorting") != -1) {
		sigtime.sot = atof(n.c_str());
	}
	else if (line.find("reduction") != -1) {
		sigtime.rot = atof(n.c_str());
	}
	else if (line.find("BVE") != -1) {
		sigtime.ve = atof(n.c_str());
	}
	else if (line.find("SUB") != -1 || line.find("HSE") != -1) {
		sigtime.sub = atof(n.c_str());
	}
	else if (line.find("BCE") != -1) {
		sigtime.bce = atof(n.c_str());
	}
	else if (line.find("ERE") != -1) {
		sigtime.ere = atof(n.c_str());
	}
	else if (line.find("Device memory") != -1) {
		sigtime.mem = atof(n.c_str());
	}
}

void parse_stats(const string& line, string& c2v, string& conflicts, string& propagations,
	string& single, string& multiple, string& calls, double& time)
{
	size_t offset = line.find(CREPORTVAL);
	if (offset != -1) offset += strlen(CREPORTVAL);
	else offset = line.find(":") + 1;
	if (eq(line.c_str(), "Solver time") || eq(line.c_str(), "CPU time")) {
		const char* n = line.c_str() + offset;
		time = atof(n);
	}
	else if (eq(line.c_str(), "C2V ratio")) {
		c2v = line.substr(offset);
		eatString(c2v, CNORMAL);
	}
	else if (eq(line.c_str(), "Conflicts") || eq(line.c_str(), "c conflicts")) { 
		string val = line.substr(offset);
		eatString(val, CNORMAL);
		eatSpaces(val);
		conflicts = getDigitsUntil(val, ' ');
	}
	else if (eq(line.c_str(), "Propagations") || eq(line.c_str(), "c propagations")) {
		string val = line.substr(offset);
		eatString(val, CNORMAL);
		eatSpaces(val);
		propagations = getDigitsUntil(val, ' ');
	}
	else if (eq(line.c_str(), "Search decisions") || eq(line.c_str(), "c decisions")) { 
		string val = line.substr(offset);
		eatString(val, CNORMAL);
		eatSpaces(val);
		single = getDigitsUntil(val, ' ');
	}
	else if (eq(line.c_str(), "All decisions") || eq(line.c_str(), "parallel decisions")) { 
		string val = line.substr(offset);
		eatString(val, CNORMAL);
		eatSpaces(val);
		multiple = getDigitsUntil(val, ' ');
	}
	else if (eq(line.c_str(), "MDM calls") || eq(line.c_str(), "PDM calls")) {
		string val = line.substr(offset);
		eatString(val, CNORMAL);
		eatSpaces(val);
		calls = getDigitsUntil(val, ' ');
	}
}

void parse_reds(string& line, string& in_vars, string& in_cls,
	string& rem_vars, string& rem_cls, string& forced_units,
	string& triedReduns, string& orgReduns, string& lrnReduns, string& remReduns)
{
	while (eatString(line, CREPORTVAL));
	while (eatString(line, CNORMAL));
	if (eq(line.c_str(), "c  Read")) {
		string tmp(line);
		eatNondigits(tmp);
		in_vars = getDigitsUntil(tmp, ' ');
		eatNondigits(tmp);
		in_cls = getDigitsUntil(tmp, ' ');
	}
	else if (forced_units.empty() && eq(line.c_str(), "Forced units")) {
		string n = line.substr(line.find(":") + 1);
		eatSpaces(n);
		forced_units = getDigitsUntil(n, ' ');
	}
	else if (rem_vars.empty() && eq(line.c_str(), "Removed variables")) {
		string n = line.substr(line.find(":") + 1);
		eatSpaces(n);
		rem_vars = getDigitsUntil(n, ' ');
	}
	else if (rem_cls.empty() && eq(line.c_str(), "Removed clauses")) {
		string n = line.substr(line.find(":") + 1);
		eatSpaces(n);
		rem_cls = getDigitsUntil(n, ' ');
	}
	else if (eq(line.c_str(), "Removed redundancies")) {
		string n = line.substr(line.find(":") + 1);
		eatSpaces(n);
		remReduns = getDigitsUntil(n, ' ');
	}
	else if (eq(line.c_str(), "Tried redundancies")) {
		triedReduns.clear();
		string n = line.substr(line.find(":") + 1);
		eatSpaces(n);
		triedReduns = getDigitsUntil(n, ' ');
	}
	else if (eq(line.c_str(), "Original redundancies")) {
		orgReduns.clear();
		string n = line.substr(line.find(":") + 1);
		eatSpaces(n);
		orgReduns = getDigitsUntil(n, ' ');
	}
	else if (eq(line.c_str(), "Learnt redundancies")) {
		lrnReduns.clear();
		string n = line.substr(line.find(":") + 1);
		eatSpaces(n);
		lrnReduns = getDigitsUntil(n, ' ');
	}
	else {
		// cbmc
		size_t vars_pos = line.find("variables");
		size_t cls_pos = line.find("clauses");
		if (vars_pos != -1 && cls_pos != -1) {
			string tmp(line);
			if (eq(tmp.c_str(), ".")) eatDigitsUntil(tmp, ' ');
			eatSpaces(tmp);
			in_vars = getDigitsUntil(tmp, ' ');
			eatNondigits(tmp);
			in_cls = getDigitsUntil(tmp, ' ');
		}
	}
}

bool eatString(string& line, const char* str)
{
	const size_t pos = line.find(str);
	if (pos != -1) line.erase(pos, strlen(str));
	else return false;
	return true;
}

void eatSpaces(string& line)
{
	const size_t len = line.size();
	size_t i = 0;
	while (i < len && isspace(line[i])) i++;
	if (i) line.erase(0, i);
}

void eatNondigits(string& line)
{
	const size_t len = line.size();
	size_t i = 0;
	while (i < len && !isdigit(line[i])) i++;
	if (i) line.erase(0, i);
}

string getDigits(string& line)
{
	const size_t len = line.size();
	size_t i = 0;
	string numbers;
	while (i < len && (isdigit(line[i]) || line[i] == '.'))
		numbers += line[i++];
	if (i) line.erase(0, i);
	return numbers;
}

void eatDigitsUntil(string& line, const char& prefix)
{
	const size_t len = line.size();
	size_t i = 0;
	while (i < len && (isdigit(line[i]) || line[i] == '.') && line[i] != prefix) i++;
	if (i) line.erase(0, i);
}

string getDigitsUntil(string& line, const char& prefix)
{
	const size_t len = line.size();
	size_t i = 0;
	string numbers;
	while (i < len && (isdigit(line[i]) || line[i] == '.') && line[i] != prefix) numbers += line[i++];
	if (i) line.erase(0, i);
	return numbers;
}

string getCharsUntil(string& line, const char& prefix)
{
	const size_t len = line.size();
	size_t i = 0;
	string str;
	while (i < len && line[i] != prefix) str += line[i++];
	if (i) line.erase(0, i);
	return str;
}

void write2file(ofstream& file, vector<string>& lines)
{
	std::sort(lines.begin(), lines.end());
	for (auto s : lines) file << s << endl;
}