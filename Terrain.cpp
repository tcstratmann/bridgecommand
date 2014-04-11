#include "irrlicht.h"

#include "Terrain.hpp"

using namespace irr;

Terrain::Terrain(irr::scene::ISceneManager* smgr, irr::video::IVideoDriver* driver)
{
    terrain = smgr->addTerrainSceneNode(
                       "World/SimpleEstuary/height.bmp",
                       0,					// parent node
                       -1,					// node id
		               core::vector3df(0.f, -44.07f, 0.f),		// position
		               core::vector3df(0.f, 180.f, 0.f),		// rotation (NOTE 180 deg rotation)
		               core::vector3df(6.97705f, 0.56498f, 8.6871f),	// scale
		               video::SColor ( 255, 255, 255, 255 ),	// vertexColor
		               5,					// maxLOD
		               scene::ETPS_17,		// patchSize
		               4					// smoothFactoespr
                       );
    terrain->setMaterialFlag(video::EMF_LIGHTING, false);
    terrain->setMaterialFlag(video::EMF_FOG_ENABLE, true);
    terrain->setMaterialTexture(0, driver->getTexture("World/SimpleEstuary/texture.bmp"));
}

Terrain::~Terrain()
{
    //dtor
}
