//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// A doom 3 map loader
#pragma once

#include <string>
#include <vector>

namespace id
{
	class MapEntity;

	class MapFile {
	public:
		// filename does not require an extension
		// normally this will use a .reg file instead of a .map file if it exists,
		// which is what the game and dmap want, but the editor will want to always
		// load a .map file
		bool					Parse( const char *filename );
		//bool					Write( const char *fileName, const char *ext, bool fromBasePath = true );
		// get the number of entities in the map
		//int						GetNumEntities() const { return entities.Num(); }
		// get the specified entity
		//idMapEntity *			GetEntity( int i ) const { return entities[i]; }
		// get the name without file extension
		const char *			GetName() const { return name.c_str(); }
		// get CRC for the map geometry

		bool					HasPrimitiveData() { return hasPrimitiveData; }

	protected:
		std::vector<MapEntity *>	entities;
		std::string				name;
		bool					hasPrimitiveData = false;
};
}