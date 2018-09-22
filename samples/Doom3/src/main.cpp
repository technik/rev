//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// A doom 3 map loader
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>
#include <graphics/backend/Windows/windowsPlatform.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/scene/renderGeom.h>

#include <iostream>
#include <string>
#include <vector>

#include <fstream>

using namespace std;
using namespace rev::core;
using namespace rev::gfx;
using namespace rev::math;

#define SwapBytes( x, y )		{ uint8_t t = (x); (x) = (y); (y) = t; }

template<class type> static void SwapBig( type &c ) {
	if ( sizeof( type ) == 1 ) {
	} else if ( sizeof( type ) == 2 ) {
		uint8_t *b = (uint8_t *)&c;
		SwapBytes( b[0], b[1] );
	} else if ( sizeof( type ) == 4 ) {
		uint8_t *b = (uint8_t *)&c;
		SwapBytes( b[0], b[3] );
		SwapBytes( b[1], b[2] );
	} else if ( sizeof( type ) == 8 ) {
		uint8_t * b = (uint8_t *)&c;
		SwapBytes( b[0], b[7] );
		SwapBytes( b[1], b[6]);
		SwapBytes( b[2], b[5] );
		SwapBytes( b[3], b[4] );
	} else {
		assert( false );
	}
}

//--------------------------------------------------------------------------------------------------------------
struct idFile
{
	template<class type> size_t ReadBig( type &c ) {
		size_t r = Read( &c, sizeof( c ) );
		SwapBig( c );
		return r;
	}
	
	template<class type> size_t ReadInt( type &c ) {
		size_t r = Read( &c, sizeof( c ) );
		return r;
	}

	size_t ReadString(string& dst)
	{
		uint32_t len;
		ReadInt(len);
		dst.resize(len);
		Read(dst.data(), len);
		return len;
	}

	virtual size_t Read( void*c , size_t sz) = 0;
};

//--------------------------------------------------------------------------------------------------------------
struct MemoryFile : idFile
{
	MemoryFile(const char* buffer) : filePtr(buffer), curPtr(buffer) {}

	size_t Read( void*c , size_t sz) override {
		char* dst = (char*)c;
		for(int i = 0; i < sz; ++i)
			dst[i] = *curPtr++;
		return sz;
	}

	const char* filePtr;
	const char* curPtr;
};

//--------------------------------------------------------------------------------------------------------------
struct DiskFile : idFile
{
	DiskFile(const char* fileName)
	{
		inFile.open(fileName, ios::binary);
	}

	bool is_open() const { return inFile.is_open(); }

	size_t Read( void*c , size_t sz) override
	{
		inFile.read((char*)c, sz);
		return sz;
	}

	ifstream inFile;
};

class idResourceCacheEntry {
public:
	idResourceCacheEntry() {
		Clear();
	}
	void Clear() {
		filename.clear();
		//filename = NULL;
		offset = 0;
		length = 0;
		containerIndex = 0;
	}
	size_t Read( MemoryFile *f ) {
		size_t sz = f->ReadString( filename );
		sz += f->ReadBig( offset );
		sz += f->ReadBig( length );
		return sz;
	}
	string filename;
	int	 offset;							// into the resource file
	int length;
	uint8_t	containerIndex;
};

string extractPath(const string& fileName)
{
	auto end = fileName.find_last_of("/");
	auto path = fileName.substr(0, end);
	for(auto& c : path)
		if(c == '/')
			c = '\\';
	return path;
}

constexpr uint32_t RESOURCE_FILE_MAGIC = 0xD000000D;
//--------------------------------------------------------------------------------------------------------------
void ExtractResourceFile ( const char * _fileName, const char * _outPath, bool _copyWavs ) {
	DiskFile inFile(_fileName);

	if (!inFile.is_open()) {
		cout << "Unable to open resource file " << _fileName << "\n";
		return;
	}

	uint32_t magic;
	inFile.ReadBig(magic);
	if ( magic != RESOURCE_FILE_MAGIC ) {
		return;
	}

	uint32_t _tableOffset = 0;
	uint32_t _tableLength;
	inFile.ReadBig(_tableOffset);
	inFile.ReadBig(_tableLength);

	vector<char> buf(_tableLength);
	inFile.inFile.seekg(_tableOffset);
	inFile.inFile.read(buf.data(), _tableLength);
	// read this into a memory buffer with a single read
	MemoryFile memFile(buf.data());

	int _numFileResources;
	memFile.ReadBig( _numFileResources );

	for ( int i = 0; i < _numFileResources; i++ ) {
		idResourceCacheEntry rt;
		rt.Read( &memFile );
		//rt.filename.BackSlashesToSlashes();
		//rt.filename.ToLower();
		std::vector<uint8_t> fbuf;
		if ( _copyWavs && ( rt.filename.find( ".idwav" ) != string::npos ||  rt.filename.find( ".idxma" ) != string::npos ||  rt.filename.find( ".idmsf" ) != string::npos ) ) {
			//rt.filename.SetFileExtension( "wav" );
			//rt.filename.Replace( "generated/", "" );
			//int len = fileSystem->GetFileLength( rt.filename );
			//fbuf =  (byte *)Mem_Alloc( len, TAG_RESOURCE );
			//fileSystem->ReadFile( rt.filename, (void**)&fbuf, NULL );
			cout << "Ignoring generated file " << rt.filename << "\n";
		} else {
			inFile.inFile.seekg( rt.offset);
			fbuf.resize(rt.length);
			inFile.Read( fbuf.data(), rt.length );
		}

		auto path = extractPath(rt.filename);
		auto command = string("mkdir ") + path;
		system(command.c_str());

		ofstream outFile(rt.filename, ofstream::binary);
		if ( outFile.is_open()) {
			outFile.write((const char*)fbuf.data(), fbuf.size());
		}
	}
}

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Init engine core systems
	OSHandler::startUp();
	FileSystem::init();

	// Parse arguments
	vector<string> arguments(_argc);
	for(int i = 0; i < _argc; ++i)
		arguments[i] = _argv[i];
	string inFile;
	bool extractResources = false;
	for(int i = 0; i < _argc; ++i)
	{
		if(arguments[i] == "-extract")
			extractResources = true;
		if(arguments[i] == "-i" && (i+1)<_argc)
			inFile = arguments[++i];
	}

	if(inFile.empty())
		return -1;

	if(extractResources)
	{
		cout << "Load resource file \"" << inFile <<"\"\n";
		ExtractResourceFile ( inFile.c_str(), "./", true );
	}
	else
	{
	}

	// Clean up
	FileSystem::end();
	return 0;
}
