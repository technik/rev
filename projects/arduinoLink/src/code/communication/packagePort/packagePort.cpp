//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 22nd, 2012
//----------------------------------------------------------------------------------------------------------------------
// Arduino link's communication port

#include <cstdint>

#include "packagePort.h"

#include <revCore/stream/byteStream/byteStream.h>

//----------------------------------------------------------------------------------------------------------------------
PackagePort::PackagePort(rev::IOByteStream* _communicationStream)
	:mIncomingPackage(nullptr)
	,mComStream(_communicationStream)
{
}

//----------------------------------------------------------------------------------------------------------------------
PackagePort::~PackagePort()
{
	// Discard any half-recieved package
	if(nullptr != mIncomingPackage)
		delete mIncomingPackage;
}

//----------------------------------------------------------------------------------------------------------------------
bool PackagePort::sendPackage(const Package& _pkg)
{
	bool transmissionOk = true; // Used to determine wether the packet has been properly transmitted
	// Send header
	transmissionOk |= mComStream->write('#');
	transmissionOk |= mComStream->write('!');
	// Send packet type
	sendPakageType(_pkg.type());
	// Send data size
	uint8_t dataSize = _pkg.dataSize();
	transmissionOk |= mComStream->write(dataSize);
	// Send actual data
	const uint8_t* data = _pkg.data();
	for(unsigned i = 0; i < dataSize; ++i)
	{
		transmissionOk |= mComStream->write(data[i]);
	}
	// Send checksum
	uint16_t checksum = _pkg.checksum();
	uint8_t checksumL = checksum % 256;	// This two lines of code separate checksum into the most significant part byte and
	uint8_t checksumH = checksum / 256;	// less significant byte independently of platform endianness
	transmissionOk |= mComStream->write(checksumL);
	transmissionOk |= mComStream->write(checksumH);
	return transmissionOk;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned PackagePort::pendingPackages()
{
	// TODO: Implement package reading
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
Package* PackagePort::readPackage()
{
	// TODO: Implement package reading
	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
void PackagePort::sendPakageType(Package::Type _type)
{
	switch(_type)
	{
	case Package::Type::data:
		mComStream->write(0x01);
		break;
	case Package::Type::debug:
		mComStream->write(0x02);
		break;
	default:
		mComStream->write(0x00); // Invalid type
		break;
	}
}