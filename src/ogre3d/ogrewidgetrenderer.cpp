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

#include "ogrewidgetrenderer.h"

#include "ogresurface.h"
#include "ogrejunction.h"
#include "../controller.h"

#include <OGRE/OgreStringConverter.h>
#include <OGRE/OgreDataStream.h>
#include <OGRE/OgreMath.h>

#if OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR >= 2
   #include <OGRE/OgreTextureGpu.h>
   #include <OGRE/OgreTextureGpuManager.h>
   #include <OGRE/OgreGpuResource.h>
   #include <OGRE/OgrePixelFormatGpuUtils.h>
   #include <OGRE/OgreStagingTexture.h>
#else
   #include <OGRE/OgreTextureManager.h>
#endif

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   #include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#else
   #include <OGRE/OgreRoot.h>
   #include <OGRE/OgreHlmsManager.h>
   #include <OGRE/OgreHlmsDatablock.h>
   #include <OGRE/Hlms/Unlit/OgreHlmsUnlit.h>
   #include <OGRE/Hlms/Pbs/OgreHlmsPbsPrerequisites.h>
#endif

#if FARSO_USE_OGRE_OVERLAY == 1
   #include <OGRE/Overlay/OgreOverlayContainer.h>
   #include <OGRE/Overlay/OgreOverlayManager.h>
#endif

#include <OGRE/OgreHardwarePixelBuffer.h>

using namespace Farso;

/***********************************************************************
 *                             OgreWidgetRenderer                          *
 ***********************************************************************/
OgreWidgetRenderer::OgreWidgetRenderer(int width, int height,
            ControllerRendererJunction* junction) 
                   :WidgetRenderer(width, height, junction)
{
   /* Set junction to use */
   this->ogreJunction = (OgreJunction*) junction;
#if FARSO_USE_OGRE_OVERLAY == 1
   container = NULL;
#else
   renderable = NULL;
   movable = NULL;
   sceneNode = NULL;
#endif

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   texState = NULL;
#else
   datablock = NULL;
#endif

}

/***********************************************************************
 *                            ~OgreWidgetRenderer                      *
 ***********************************************************************/
OgreWidgetRenderer::~OgreWidgetRenderer()
{
   deleteSurface();
}

/***********************************************************************
 *                              createSurface                          *
 ***********************************************************************/
void OgreWidgetRenderer::createSurface()
{
   /* Create the drawable surface */ 
   this->surface = new OgreSurface(name, realWidth, realHeight);

   /* Create the ogre texture to render the widget */
#if OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR >= 2
   Ogre::TextureGpuManager* textureMgr = 
      ogreJunction->getRenderSystem()->getTextureGpuManager();
   
   this->texture = textureMgr->createOrRetrieveTexture(name, name,
         Ogre::GpuPageOutStrategy::Discard,
         Ogre::TextureFlags::ManualTexture,
         Ogre::TextureTypes::Type2D);

   texture->setResolution(realWidth, realHeight);
   texture->setNumMipmaps(1u);
   texture->setPixelFormat(Ogre::PFG_RGBA8_UNORM_SRGB);

#else 
   this->texture = Ogre::TextureManager::getSingleton().createManual(
         name, 
         Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
         Ogre::TEX_TYPE_2D, realWidth, realHeight, 0, Ogre::PF_A8R8G8B8,
         Ogre::TU_DYNAMIC_WRITE_ONLY);

   /* Map the drawable surface contents to a PixelBox to make easer the 
    *  update-to-texture proccess */
   pixelBox = Ogre::PixelBox(realWidth, realHeight,
         1, Ogre::PF_A8B8G8R8, surface->getSurface()->pixels);
#endif

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)

   /* Nullify things */
   this->texState = NULL;

   /* Create the material to use */
   Ogre::ResourceManager::ResourceCreateOrRetrieveResult matRes;
   matRes = Ogre::MaterialManager::getSingleton().createOrRetrieve(name,
         "gui");
   material = Ogre::static_pointer_cast<Ogre::Material>(matRes.first);
#endif

   uploadSurface();
   defineTexture();

#if FARSO_USE_OGRE_OVERLAY == 1

   /* Define the container and set it to the overlay */
   #if OGRE_VERSION_MAJOR == 1 || \
       (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   this->container = static_cast<Ogre::OverlayContainer*>(
         Ogre::OverlayManager::getSingletonPtr()->createOverlayElement(
            "Panel", name));
   #else
   this->container = static_cast<Ogre::v1::OverlayContainer*>(
         Ogre::v1::OverlayManager::getSingletonPtr()->createOverlayElement(
            "Panel", name));
   #endif
   ogreJunction->getOverlay()->add2D(this->container);

   /* Set container dimensions */
   container->setWidth(((float)realWidth) / 
         Controller::getWidth());
   container->setHeight(((float)realHeight) / 
         Controller::getHeight());

   if(!isVisible())
   {
      container->hide();
   }
   container->setPosition(targetX.getValue() / Controller::getWidth(),
         targetY.getValue() / Controller::getHeight());

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   container->setMaterialName(material->getName());
#else
   /* Note: we still need to set the material name, as at 
    * OgreOverlayElement::_update it will check if the datablock name is
    * equal to the defined material name, trying to get a datablock
    * with the name if they aren't equal. */
   container->setDatablock(datablock);
   container->setMaterialName(name);
#endif

   /* Note:  the Overlay::add2d will reassign all the internal zorder (which
    * isn't what we expected). A workaround to not let the mouse behind is
    * to force cursor Z order again after this call.*/
   ogreJunction->getOverlay()->setZOrder(0);
   if(Cursor::getRenderer())
   {
      Cursor::getRenderer()->setRenderQueueSubGroup(650);
   }


#else
   Ogre::SceneManager* sceneManager = ogreJunction->getSceneManager();

   /* Create our renderable and set its datablock */
   renderable = new OgreWidgetRenderable(name, realWidth, realHeight);

   /* Define our movable and attach our renderable to it */
   movable = static_cast<OgreWidgetMovable*>(
         sceneManager->createMovableObject(
            OgreWidgetMovableFactory::FACTORY_TYPE_NAME,
            &sceneManager->_getEntityMemoryManager(Ogre::SCENE_DYNAMIC)));

   movable->attachOgreWidgetRenderable(renderable); 
   renderable->setDatablock(datablock);

   /* Finally, add them to a SceneNode */
   sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();
   sceneNode->attachObject(movable);
   sceneNode->setPosition(0.0f, 0.0f, 0.0f);
   if(!isVisible())
   {
      sceneNode->setVisible(false);
   }
   setPosition(targetX.getValue(), targetY.getValue());
#endif
}

/***********************************************************************
 *                             deleteSurface                           *
 ***********************************************************************/
void OgreWidgetRenderer::deleteSurface()
{
#if FARSO_USE_OGRE_OVERLAY == 1
   if(container != NULL)
   {
      /* Clear the container of the overlay */
      OgreJunction* ogreJunction = (OgreJunction*) junction;
      ogreJunction->getOverlay()->remove2D(container);
      #if OGRE_VERSION_MAJOR == 1 || \
         (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
         Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(
               container);
      #else
         Ogre::v1::OverlayManager::getSingletonPtr()->destroyOverlayElement(
               container);
      #endif
      container = NULL;
   }
#else
   if(sceneNode)
   {
      sceneNode->detachObject(movable);
      movable->detachOgreWidgetRenderable(renderable);
      OgreJunction* ogreJunction = (OgreJunction*) junction;
      ogreJunction->getSceneManager()->destroySceneNode(sceneNode);
      ogreJunction->getSceneManager()->destroyMovableObject(movable);
      delete renderable;
   }
#endif

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   if(surface)
   {
      /* Remove Shaders from material */
      Ogre::RTShader::ShaderGenerator::getSingleton().
         removeAllShaderBasedTechniques(material->getName(), 
               material->getGroup());

      /* Free the material used */
      Ogre::MaterialManager::getSingleton().remove(material->getName(), 
            material->getGroup());
   }
#endif

   /* Free the surface and its texture */
   if(surface)
   {
      delete surface;
      surface = NULL;
#if OGRE_VERSION_MAJOR == 1
      Ogre::TextureManager::getSingleton().remove(texture->getName(), 
            texture->getGroup());
#elif OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR <= 1
      Ogre::TextureManager::getSingleton().remove(texture->getName()); 
#else
      Ogre::TextureGpuManager* textureMgr = 
         ogreJunction->getRenderSystem()->getTextureGpuManager();
      textureMgr->destroyTexture(texture);
#endif
   }
}

/***********************************************************************
 *                         setRenderQueueSubGroup                      *
 ***********************************************************************/
void OgreWidgetRenderer::setRenderQueueSubGroup(int renderQueueId)
{
#if FARSO_USE_OGRE_OVERLAY == 1
   if(container)
   {
   #if OGRE_VERSION_MAJOR > 1
      container->setRenderQueueSubGroup(
            static_cast<Ogre::uint8>(renderQueueId));
   #else
      container->_notifyZOrder(renderQueueId);
   #endif
   }
#else
   if(renderable)
   {
      renderable->setRenderQueueSubGroup(
            static_cast<Ogre::uint8>(renderQueueId));
   }
#endif
}

/***********************************************************************
 *                            redefineTexture                          *
 ***********************************************************************/
void OgreWidgetRenderer::defineTexture()
{
#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   /* Using 1.x materials */
   Ogre::Technique* tech = material->getBestTechnique();

   if(!tech)
   {
      /* Let's use the first one. */
      tech = material->getTechnique(0);
   }
   /* As we are using a shader, let's set the technique as RTSS to avoid our
    * Goblin listener try to recreate a technique for it. */
#if OGRE_VERSION_MAJOR == 1   
   tech->setSchemeName(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
#endif
   Ogre::RTShader::ShaderGenerator::getSingleton().validateMaterial(
         Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, 
         material->getName(), material->getGroup());

   /* Verify if need to create a pass */
   int numPasses = tech->getNumPasses();
   if(numPasses == 0)
   {
      tech->createPass();
   }
   /* Get or create new texture */
   int numTextures = tech->getPass(0)->getNumTextureUnitStates();
   if(numTextures != 0)
   {
      texState = tech->getPass(0)->getTextureUnitState(0);
   }
   else
   {
      texState = tech->getPass(0)->createTextureUnitState(texture->getName());
   }
   texState->setTexture(texture);
   texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
   
   tech->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
   tech->getPass(0)->setDepthWriteEnabled(false);
   tech->getPass(0)->setDepthCheckEnabled(false);
   tech->getPass(0)->setLightingEnabled(false);
   tech->getPass(0)->setCullingMode(Ogre::CULL_NONE);
   tech->getPass(0)->setColourWriteEnabled(true);

   tech->getPass(0)->setVertexProgram(ogreJunction->getVertexProgramName());
   tech->getPass(0)->setFragmentProgram(ogreJunction->getFragmentProgramName());
#else
   /* Using Ogre's Unlit HLMS */
   Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingleton().getHlmsManager();
   Ogre::HlmsUnlit* hlmsUnlit = static_cast<Ogre::HlmsUnlit*>(
         hlmsManager->getHlms(Ogre::HLMS_UNLIT));

   /* Macroblock with depth write, depth check and culling disabled */
   Ogre::HlmsMacroblock macroBlock;
   macroBlock.mDepthCheck = false;
   macroBlock.mDepthWrite = false;
   macroBlock.mCullMode = Ogre::CULL_NONE;

   Ogre::HlmsBlendblock blendBlock;
   blendBlock.mDestBlendFactor = Ogre::SBF_ONE_MINUS_SOURCE_ALPHA;
   
   /* Let's create and define our datablock */
   datablock = static_cast<Ogre::HlmsUnlitDatablock*>(
         hlmsUnlit->createDatablock(name, name,
            macroBlock, blendBlock, Ogre::HlmsParamVec()));
   #if (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR >= 2)
      datablock->setTexture(0, texture);
   #else
      datablock->setTexture(Ogre::PBSM_DIFFUSE, 0, texture);
   #endif
#endif
}

/***********************************************************************
 *                              setPosition                            *
 ***********************************************************************/
void OgreWidgetRenderer::doSetPosition(Ogre::Real x, Ogre::Real y)
{
#if FARSO_USE_OGRE_OVERLAY == 1
   if(container)
   {
      container->setPosition(x / Controller::getWidth(),
            y / Controller::getHeight());
   }
#else
   if(sceneNode)
   {
      /* Redefine the coordinates to -1,1 space. */
      float x1 = -1.0f + 
         (x / static_cast<float>(Controller::getWidth()) * 2.0f);
      float y1 = 1.0f - 
         (y / static_cast<float>(Controller::getHeight()) * 2.0f);

      /* Set our node */
      sceneNode->setPosition(x1, y1, 0.0f);
   }
#endif
}
 
/***********************************************************************
 *                                doShow                               *
 ***********************************************************************/
void OgreWidgetRenderer::doShow()
{
#if FARSO_USE_OGRE_OVERLAY == 1
   if(container)
   {
      container->show();
   }
#else
   if(sceneNode)
   {
      sceneNode->setVisible(true);
   }
#endif
}

/***********************************************************************
 *                                 doHide                              *
 ***********************************************************************/
void OgreWidgetRenderer::doHide()
{
#if FARSO_USE_OGRE_OVERLAY == 1
   if(container)
   {
      container->hide();
   }
#else
   if(sceneNode)
   {
      sceneNode->setVisible(false);
   }
#endif
}

/***********************************************************************
 *                                doRender                             *
 ***********************************************************************/
void OgreWidgetRenderer::doRender()
{
}

/***********************************************************************
 *                             uploadSurface                           *
 ***********************************************************************/
void OgreWidgetRenderer::uploadSurface()
{
   /* The idea here is similar to the opengl implementation: we just update
    * the texture with the contents of the rendering surface (represented
    * by it PixelBox bellow). */
   OgreSurface* ogreSurface = static_cast<OgreSurface*>(surface);

   ogreSurface->lock();
#if OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR >= 2

   /* As Ogre 2.2+ uses a parallell upload to GPU proccess, the work
    * is a bit more complicated. */
   Ogre::TextureGpuManager* textureManager = 
      ogreJunction->getRenderSystem()->getTextureGpuManager();

   /* Define our sizes */
   const size_t bytesPerRow = texture->_getSysRamCopyBytesPerRow(0);

   /* Tell texture we are going resident, if not already */
   if(texture->getResidencyStatus() != Ogre::GpuResidency::Resident)
   {
      texture->_transitionTo(Ogre::GpuResidency::Resident, NULL);
      texture->_setNextResidencyStatus(Ogre::GpuResidency::Resident);
   }

   Ogre::StagingTexture* stagingTexture = textureManager->getStagingTexture(
         texture->getWidth(), texture->getHeight(), texture->getDepth(),
         texture->getNumSlices(), texture->getPixelFormat());

   /* Let's map it, for the whole texture */
   stagingTexture->startMapRegion();
   Ogre::TextureBox texBox = stagingTexture->mapRegion(texture->getWidth(), 
         texture->getHeight(), texture->getDepth(), texture->getNumSlices(),
         texture->getPixelFormat());

   /* Copy the contents */
   texBox.copyFrom(ogreSurface->getSurface()->pixels, 
         texture->getWidth(), texture->getHeight(), bytesPerRow);

   /* Done with map */
   stagingTexture->stopMapRegion();

   /* Now we should upload it. */
   stagingTexture->upload(texBox, texture, 0, 0, true);

   /* No more needed the staging texture */
   textureManager->removeStagingTexture( stagingTexture );
   stagingTexture = 0;

   /* Notify data is ready to display */
   texture->notifyDataIsReady();

#else
   /* Previously to 2.2, Ogre should just lock the rendering pipeline */
   texture->getBuffer()->blitFromMemory(pixelBox);
#endif
   ogreSurface->unlock();
}

