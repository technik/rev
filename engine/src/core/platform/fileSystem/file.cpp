//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// File implementation for platforms conforming to C++ standard

#include <cassert>
#include <cstring>
#include <fstream>

#include "file.h"


using namespace std;

namespace rev { namespace core {

	// Static data definitions
#ifdef ANDROID
	AAssetManager* File::sAssetMgr = nullptr;
#endif // ANDROID
	template<>
	NamedResource<File>::Mgr* NamedResource<File>::sInstance = nullptr;

	//--------------------------------------------------------------------------------------------------------------
	void File::setAssetMgr(AAssetManager* _mgr) {
		sAssetMgr = _mgr;
	}

	//--------------------------------------------------------------------------------------------------------------
	File::File(const string& _path) {
		mPath = _path; 
#ifdef ANDROID
		assert(sAssetMgr != nullptr);
		AAsset* srcAsset = AAssetManager_open(sAssetMgr, _path.c_str(), AASSET_MODE_STREAMING);
		if(!srcAsset)
			return;
		mSize = (size_t)AAsset_getLength(srcAsset);
		mBuffer = new char[mSize+1];
		memcpy(mBuffer, AAsset_getBuffer(srcAsset), mSize);
		((char*)mBuffer)[mSize] = '\0';
		AAsset_close(srcAsset);
#else // !ANDROID
		ifstream srcFile(_path.c_str(), ios_base::binary);
		if (srcFile.good()) {
			fstream srcFile;
			// Open the file
			// Meassure it's size
			srcFile.seekg(0, ios::end);
			mSize = int(srcFile.tellg());
			srcFile.seekg(0, ios::beg);
			int begin = int(srcFile.tellg());
			mSize -= begin;
			// Allocate the buffer
			mBuffer = new char[mSize + 1];
			// Fill the buffer with the contents of the file
			srcFile.read((char*)mBuffer, mSize);
			((char*)mBuffer)[mSize] = '\0';

			srcFile.close();
		}
#endif
	}

	//--------------------------------------------------------------------------------------------------------------
	File::~File() {
		if (mBuffer)
			delete[] static_cast<const char*>(mBuffer); // Static cast prevents undefined behavior deleting void*
	}
}}