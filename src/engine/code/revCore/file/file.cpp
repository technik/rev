////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File system

// Standard headers
#include <fstream>

// Engine headers
#include "file.h"

#include "revCore/codeTools/assert/assert.h"

using namespace rev::codeTools;
using namespace std;

namespace rev
{

char * bufferFromFile(const char * _fileName)
{
	fstream file;
	// Open the file
	file.open(_fileName, ios_base::binary|ios_base::in);
	revAssert(file.is_open());
	// Meassure it's size
	file.seekg(0, ios::end);
	int size = file.tellg();
	file.seekg(0, ios::beg);
	int begin = file.tellg();
	size -= begin;
	// Allocate the buffer
	char * buffer = new char[size+1];
	// Fill the buffer with the contents of the file
	file.read(buffer, size);
	buffer[size] = '\0';
	// return the buffer
	return buffer;
}

}	// namespace rev
