//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Serial port communications

#ifndef _REV_COMM_SERIAL_SERIALPORT_H_
#define _REV_COMM_SERIAL_SERIALPORT_H_

// Platform dependent configuration
#if defined(WIN32)
#define REV_ENABLE_SERIAL_PORT
#endif // WIN32

#ifdef REV_ENABLE_SERIAL_PORT

namespace rev { namespace comm
{

}	// namespace comm
}	// namespace rev

#endif // REV_ENABLE_SERIAL_PORT

#endif // _REV_COMM_SERIAL_SERIALPORT_H_
