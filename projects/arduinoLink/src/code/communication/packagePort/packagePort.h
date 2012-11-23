//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 22nd, 2012
//----------------------------------------------------------------------------------------------------------------------
// Arduino link's communication port

#ifndef _ARDUINOLINK_COMMUNICATION_PACKAGEPORT_PACKAGEPORT_H_
#define _ARDUINOLINK_COMMUNICATION_PACKAGEPORT_PACKAGEPORT_H_

#include <list>
#include "../package/package.h"

// Forward declarations
namespace rev { class IOByteStream; }

class PackagePort
{
public:
	PackagePort(rev::IOByteStream* _communicationStream);
	~PackagePort();

	// Package port interface
	bool		sendPackage		(const Package& _pkg);	// Returns true on success
	unsigned	pendingPackages	();						// Returns the number of stored packages pending to be read
	Package*	readPackage		();						// Returns the first stored package, if any. If there is no package
														// Available, the port will wait until one is recived.
														// Returns nullptr on failure

private:
	void sendPakageType (Package::Type _type);

	std::list<Package*>	mReadPackages;
	Package*			mIncomingPackage;
	rev::IOByteStream*	mComStream;	// Communications stream used by this port
};

#endif // _ARDUINOLINK_COMMUNICATION_PACKAGEPORT_PACKAGEPORT_H_
