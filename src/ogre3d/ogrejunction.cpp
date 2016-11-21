/* 
  Farso: a simple GUI.
  Copyright (C) DNTeam <dnt@dnteam.org>
 
  This file is part of Farso.
 
  Farso is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Farso is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with Farso.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ogrejunction.h"
#include <OGRE/Overlay/OgreOverlayManager.h>

using namespace Farso;

/*************************************************************************
 *                            OgreJunction                               *
 *************************************************************************/
OgreJunction::OgreJunction(Kobold::String name)
{
#if FARSO_USE_OGRE_OVERLAY == 1
   overlay = Ogre::OverlayManager::getSingletonPtr()->create(name);
   overlay->show();
#endif
   sceneManager = NULL;
}

/*************************************************************************
 *                           ~OgreJunction                               *
 *************************************************************************/
OgreJunction::~OgreJunction()
{
#if FARSO_USE_OGRE_OVERLAY == 1
   Ogre::OverlayManager::getSingletonPtr()->destroy(overlay);
#endif
}

#if FARSO_USE_OGRE_OVERLAY == 1
/*************************************************************************
 *                             getOverlay                                *
 *************************************************************************/
Ogre::Overlay* OgreJunction::getOverlay()
{
   return overlay;
}
#endif

/*************************************************************************
 *                          setSceneManager                              *
 *************************************************************************/
void OgreJunction::setSceneManager(Ogre::SceneManager* sceneManager)
{
   this->sceneManager = sceneManager;
}

/*************************************************************************
 *                          getSceneManager                              *
 *************************************************************************/
Ogre::SceneManager* OgreJunction::getSceneManager()
{
   return sceneManager;
}


