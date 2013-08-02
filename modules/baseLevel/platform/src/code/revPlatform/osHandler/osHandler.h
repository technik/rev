//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Windows handler for Operating System call

#ifndef _REV_PLATFORM_OSHANDLER_OSHANDLER_H_
#define _REV_PLATFORM_OSHANDLER_OSHANDLER_H_


namespace rev { namespace platform {

	class OSHandler
	{
	public:
		static OSHandler* get();

		virtual bool update() = 0;

	protected:
		OSHandler(){}
	};

}	// namespace platform
}	// namespace rev

#endif // _REV_PLATFORM_OSHANDLER_OSHANDLER_H_