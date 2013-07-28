//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Windows handler for Operating System call

#ifndef _REV_GAME_APPLICATION_PLATFORM_OSHANDLER_H_
#define _REV_GAME_APPLICATION_PLATFORM_OSHANDLER_H_


namespace rev { namespace game {

	class OSHandler
	{
	public:
		static OSHandler* get() {
			if(!sHandler) createHandler();
			return sHandler;
		}

		virtual bool update() = 0;
	private:
		static void createHandler();	// Define this two along with the proper derived class.

	protected:
		static OSHandler* sHandler;			// That ensures one and only one class derives this at link time.
		OSHandler(){}
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_APPLICATION_PLATFORM_OSHANDLER_H_