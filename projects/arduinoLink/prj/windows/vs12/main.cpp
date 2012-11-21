//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 21st, 2012
//----------------------------------------------------------------------------------------------------------------------
// Main

#include <revCore/codeTools/assert/assert.h>
#include <revCore/stream/byteStream/byteStream.h>
#include <revCore/stream/byteStream/dummyByteStream.h>
#include <iostream>

using namespace std;

int main(int _argC, const char** _argV)
{
	rev::DummyByteStream dummy;
	dummy.write('H');
	const char* phrase = "ello World";
	unsigned written = dummy.write(11, phrase);
	rev::revAssert(written == 11);
	char character = dummy.read();
	rev::revAssert('H' == character);
	return 0;
}