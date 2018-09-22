//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// A doom 3 map loader
#include "MapEntity.h"
#include "lexer.h"

#include <fstream>

using namespace std;

namespace id {

	MapEntity* MapEntity::Parse( Lexer& lexer )
	{
		if(!lexer.readToken<'{'>())
			return nullptr;

		MapEntity* entity = new MapEntity();

		for(;;)
		{
			//
		}


		if(!lexer.readToken<'}'>())
			return nullptr;
		return nullptr;
	}

}