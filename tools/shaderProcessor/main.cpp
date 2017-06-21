//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "shaderProcessor.hpp"

using namespace std;

struct Params {
	const char* in = nullptr;
	const char* out = nullptr;
	vector<const char*>	defines;

	bool parseArguments(int _argc, const char** _argv) {
		for (int i = 0; i < _argc; ++i) {
			const char* arg = _argv[i];
			if (0 == strcmp(arg, "-i")) { // Input file
				++i;
				if (i < _argc) {
					cout << "Error: missing input filename after -i\n";
					return false;
				}
				in = _argv[i];
			}
			else if (0 == strcmp(arg, "-o")) { // Output file
				++i;
				if (i < _argc) {
					cout << "Error: missing output filename after -o\n";
					return false;
				}
				out = _argv[i];
			}
			else if (0 == strncmp(arg, "-D", 2)) {
				if (arg[2] == '\0') {
					cout << "Error: empty define (found -D without anything after it)\n";
					return false;
				}
				defines.push_back(&arg[2]);
			}
			else {
				cout << "Error: unknown argument " << arg << "\n";
				return false;
			}
		} // for
		return checkParamsAreValid();
	}

private:
	bool checkParamsAreValid() {
		if (in == nullptr)
			return false;
		if (out == nullptr)
			return false;
		return true;
	}
};

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Parse arguments
	Params params;
	if (!params.parseArguments(_argc, _argv))
		return -1;
	// Process shader
	ifstream inFile(params.in);
	ofstream outFile(params.out);
	if (!preprocessIncludes(inFile, outFile)) {
		cout << "Error processing shader file " << params.in << "\n";
		return - 1;
	}
	return 0;
}
