//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_set>

using namespace std;

//----------------------------------------------------------------------------------------------------------------------
/// Assumes all folders finish with a '/'
/// \return folder path without filename. Empty string for unsupported paths (like those with windows style '\\'), or 
/// ./ for files without folder
string getFolder(const string& _path) {
	for (size_t i = _path.size(); i > 0; --i) {
		if (_path[i] == '\\') // Windows style paths not supported
			return "";
		if (_path[i] == '/')
			return _path.substr(0, i+1);
	}
	return "./"; // Return current path for folderless paths
}

static constexpr char includeDirective[] = "#include ";
static constexpr size_t directiveSize = sizeof(includeDirective) - 1;
//----------------------------------------------------------------------------------------------------------------------
string extractIncludeFileName(const string& _line) {
	if (_line.size() < directiveSize + 2) {
		cout << "Error parsing include directive: " << _line << "\n";
		return "";
	}
	constexpr size_t startPos = directiveSize + 1;
	size_t closePos = _line.substr(startPos).find('"');
	if (closePos == string::npos) {
		cout << "Error, incomplete include directive: " << _line << "\n";
		return "";
	}
	else {
		return _line.substr(startPos, closePos);
	}
}

//----------------------------------------------------------------------------------------------------------------------
string locateFile(const string& _localPath, const vector<string>& _includePaths) {
	for (auto pathIter = _includePaths.rbegin(); pathIter != _includePaths.rend(); ++pathIter) {
		string fullPath = *pathIter + _localPath;
		ifstream file(fullPath);
		if (file.is_open()) {
			file.close();
			return fullPath;
		}
	}
	return ""; // Nothing found -> empty string
}

//----------------------------------------------------------------------------------------------------------------------
/// \param _lineBuffer a temporary buffer to store lines. Must have sufficient size preallocated
bool preprocessIncludes(istream& _in, ostream& _out, vector<string>& includePaths, vector<char>& _lineBuffer, unordered_set<string>& _alreadyIncluded) {
	assert(_lineBuffer.size() > 0);
	char* buffer = _lineBuffer.data();
	size_t line = 0;
	while (!_in.eof()) {
		++line;
		_in.getline(buffer, _lineBuffer.size());
		if (0 == strncmp(buffer, includeDirective, directiveSize)) // #include line detected
		{
			string localPath = extractIncludeFileName(buffer);
			if (localPath.empty()) {
				cout << "in line " << line;
				// In line ...
				return false;
			}
			// Locate include file
			string fullPath = locateFile(localPath, includePaths);
			if (fullPath.empty()) {
				cout << "Error: Unable to locate file: " << localPath << "\n"
					<< "included in line " << line;
				return false;
			}
			// process file
			if (_alreadyIncluded.find(fullPath) != _alreadyIncluded.end())
				continue; // Redundant include
			ifstream includeFile(fullPath);
			if (!includeFile.is_open()) {
				cout << "Error: Can't open file: " << localPath << "\n"
					<< "included in line " << line;
				return false;
			}
			_alreadyIncluded.insert(fullPath);
			includePaths.push_back(getFolder(fullPath));
			bool ok = preprocessIncludes(includeFile, _out, includePaths, _lineBuffer, _alreadyIncluded);
			includePaths.pop_back();
			if (!ok) {
				cout << " of file " << fullPath << "\n";
				cout << "included in line " << line;
				return false;
			}
		} // if
		else {
			_out << buffer << "\n"; // No include line? Just stream the contents
		}
	} // while
	return true;
}