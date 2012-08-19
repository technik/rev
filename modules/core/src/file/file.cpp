//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Ag�era Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Basic interaction with files

#include "file.h"

#include <fstream>

#include <codeTools/log/log.h>

using namespace std;

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	File * File::open(const char * _fileName, bool ) // Second parameter is unused, writeable files are unsupported
	{
		fstream srcFile;
		// Open the file
		srcFile.open(_fileName, ios_base::binary|ios_base::in);
		if(!srcFile.is_open())
		{
			revLog() << "Couldn't open file \"" << _fileName << "\"\n";
			revLog().flush();
			return nullptr;
		}

		unsigned size;
		void * buffer;
		// Meassure it's size
		srcFile.seekg(0, ios::end);
		size = int(srcFile.tellg());
		srcFile.seekg(0, ios::beg);
		int begin = int(srcFile.tellg());
		size -= begin;
		// Allocate the buffer
		buffer = new char[size+1];
		// Fill the buffer with the contents of the file
		srcFile.read((char*)buffer, size);
		((char*)buffer)[size] = '\0';

		srcFile.close();
		
		File * file = new File();
		file->mSize = size;
		file->mBuffer = buffer;

		return file;
	}

	//------------------------------------------------------------------------------------------------------------------
	File::~File()
	{
		delete (static_cast<char*>(mBuffer));
	}

	//------------------------------------------------------------------------------------------------------------------
	File::File()
		:mBuffer(0)
		,mSize(0)
	{
	}

}	// namespace rev