//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// A doom 3 map loader
#include "mapFile.h"
#include "MapEntity.h"
#include "lexer.h"

#include <fstream>

using namespace std;

namespace id {

	bool MapFile::Parse( const char *fileName )
	{
		// Open file
		Lexer parser(fileName);
		if(parser.empty())
			return false;

		parser.skipLine(); // Skip version
		if(!parser.readToken<'{'>())
			return false;

		// Process entities
		for(;;)
		{
			if(parser.readToken<'}'>()) // End of file
				return true;

			if(parser.empty())
				return false; // End of file without '}'

			auto entity = MapEntity::Parse(parser);
			if(!entity)
				return false;

			entities.push_back(entity);
		}

		return false; // Shouldn't be getting here
	}

}