//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 22nd, 2012
//----------------------------------------------------------------------------------------------------------------------
// Arduino link's communication port interface

#ifndef _ARDUINOLINK_COMMUNICATION_PACKAGEPORT_PACKAGEPORT_H_
#define _ARDUINOLINK_COMMUNICATION_PACKAGEPORT_PACKAGEPORT_H_

// Forward declarations
namespace rev { class IOByteStream; }

#include "../package/package.h"

class PackagePort
{
public:
	PackagePort(rev::IOByteStream* _communicationStream);
	~PackagePort();

	// Package port interface
	virtual bool		sendPackage		(const Package& _pkg) = 0;	// Returns true on success
	virtual unsigned	pendingPackages	() const = 0;				// Returns the number of stored packages pending to be read
	virtual Package*	readPackage		() = 0;						// Returns the first stored package, if any. If there is no package
																	// Available, the port will wait until one is recived.
																	// Returns nullptr on failure
};

#endif // _ARDUINOLINK_COMMUNICATION_PACKAGEPORT_PACKAGEPORT_H_
