////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 12th, 2012
////////////////////////////////////////////////////////////////////////////////
// Collada loader

#ifndef _REV_GAME_COLLADA_COLLADALOADER_H_
#define _REV_GAME_COLLADA_COLLADALOADER_H_

namespace rev { namespace game
{
	class SColladaLoader
	{
	public:
		static void load(const char * _filename);
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_COLLADA_COLLADALOADER_H_
