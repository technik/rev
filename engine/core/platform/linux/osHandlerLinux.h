//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Interface with Windows operating system
#ifndef _REV_CORE_PLATFORM_LINUX_OSHANDLERLINUX_H_
#define _REV_CORE_PLATFORM_LINUX_OSHANDLERLINUX_H_

#ifdef __linux__

namespace rev {
	namespace core {

		class OSHandlerBaseLinux {
		public:
			//typedef std::function<bool(MSG)>	OSDelegate;

			//void operator+= (OSDelegate);
		protected:
			bool update() { return true; }
		private:
			//vector<OSDelegate>	mMsgProcessors;
		};

		typedef OSHandlerBaseLinux OSHandlerBase;

	}	// namespace core
}	// namespace rev

#endif // __linux__

#endif // _REV_CORE_PLATFORM_LINUX_OSHANDLERLINUX_H_