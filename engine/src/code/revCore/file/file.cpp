////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File system

// Standard headers
#ifdef ANDROID
#include <jni.h> // Java native interface
#include <zip.h> // Zip library for apk decompression
#else // !ANDROID
#include <fstream>
#endif // !ANDROID

// Engine headers
#include "file.h"

#include "revCore/codeTools/assert/assert.h"
#include "revCore/codeTools/log/log.h"

using namespace rev::codeTools;
using namespace std;

#ifdef ANDROID
// JNI Interface and apk management
extern "C" {
	JNIEXPORT void JNICALL Java_com_rev_gameclient_revFileManager_initFileSystem(JNIEnv* _env, jclass _class, jstring _apkPath);
}

zip * gAPKArchive;

JNIEXPORT void JNICALL Java_com_rev_gameclient_revFileManager_initFileSystem(JNIEnv* _env, jclass /*_class*/, jstring _apkPath)
{
	const char* str;
	jboolean isCopied;
	str = _env->GetStringUTFChars(_apkPath, &isCopied);

	LOG_ANDROID("------------ APK Path:%s", str);
	gAPKArchive = zip_open(str, 0, NULL);
#ifdef _DEBUG
	// Check APK is open
	if (gAPKArchive == 0)
	{
		LOG_ANDROID("Error loading APK");
		return;
	}
	else
	{
		LOG_ANDROID("Success loading APK");
		// Print APK contents
		int numFiles = zip_get_num_files(gAPKArchive);
		for (int i=0; i<numFiles; i++)
		{
			const char* name = zip_get_name(gAPKArchive, i, 0);
			if (name == 0)
			{
				LOG_ANDROID("Error reading zip file name at index %i : %s", i, zip_strerror(gAPKArchive));
				return;
			}
			else
			{
				LOG_ANDROID("File %i : %s\n", i, name);
			}
		}
	}
#endif // _DEBUG
}
#endif // ANDROID
namespace rev
{

int stringSize(const char * _string)
{
	int size = 0;
	while(0 != _string[size])
	{
		++size;
	}
	return size;
}

char * bufferFromFile(const char * _fileName)
{
#ifdef ANDROID
	// Add the "assets/prefix"
	// Measure filename size
	int fileNameSize = rev::stringSize(_fileName);
	char * fullName = new char[fileNameSize + 8];
	strcpy(fullName, "assets/");
	strcpy(&fullName[7], _fileName);
	LOG_ANDROID("------------OPEN FILE: %s", fullName);
	// Open the zip file inside the apk
	zip_file * file = zip_fopen(gAPKArchive, fullName, 0);
	if(!file)
		LOG_ANDROID("Error opening file");
	else
		LOG_ANDROID("Open file. Success");
	// Get the file size
	struct zip_stat fileStat;
	zip_stat(gAPKArchive, fullName, 0, &fileStat);
	// Read the file into a buffer
	unsigned long int fileSize = fileStat.size;
	LOG_ANDROID("file size:%u", unsigned(fileSize));
	char * buffer = new char[fileSize+1];
	zip_fread(file, buffer, fileSize);
	// Close zip file
	zip_fclose(file);
	delete fullName;
	return buffer;
#else // ! ANDROID
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
#endif // !ANDROID
}

}	// namespace rev
