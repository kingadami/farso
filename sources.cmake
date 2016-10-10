set(FARSO_SOURCES
src/button.cpp
src/checkbox.cpp
src/colors.cpp
src/container.cpp
src/controller.cpp
src/cursor.cpp
src/draw.cpp
src/event.cpp
src/font.cpp
src/label.cpp
src/picture.cpp
src/progressbar.cpp
src/rect.cpp
src/scrollbar.cpp
src/scrolltext.cpp
src/skin.cpp
src/stacktab.cpp
src/surface.cpp
src/textentry.cpp
src/textselector.cpp
src/widget.cpp
src/widgetrenderer.cpp
src/window.cpp
)

set(FARSO_HEADERS
src/button.h
src/checkbox.h
src/colors.h
src/container.h
src/controller.h
src/cursor.h
src/draw.h
src/event.h
src/font.h
src/label.h
src/picture.h
src/progressbar.h
src/rect.h
src/scrollbar.h
src/scrolltext.h
src/skin.h
src/stacktab.h
src/surface.h
src/textentry.h
src/textselector.h
src/widget.h
src/widgetrenderer.h
src/window.h
)

set(FARSO_FULL_SOURCES ${FARSO_SOURCES})
set(FARSO_FULL_HEADERS ${FARSO_HEADERS})

set(FARSO_OPENGL_SOURCES
src/opengl/opengldraw.cpp
src/opengl/opengljunction.cpp
src/opengl/openglsurface.cpp
src/opengl/openglwidgetrenderer.cpp
)

set(FARSO_OPENGL_HEADERS 
src/opengl/opengldraw.h
src/opengl/opengljunction.h
src/opengl/openglsurface.h
src/opengl/openglwidgetrenderer.h
)

if(${OPENGL_FOUND})
   set(FARSO_FULL_SOURCES ${FARSO_FULL_SOURCES} ${FARSO_OPENGL_SOURCES})
   set(FARSO_FULL_HEADERS ${FARSO_FULL_HEADERS} ${FARSO_OPENGL_HEADERS})
endif(${OPENGL_FOUND})

set(FARSO_OGRE_SOURCES
src/ogre3d/ogredraw.cpp
src/ogre3d/ogrejunction.cpp
src/ogre3d/ogresurface.cpp
src/ogre3d/ogrewidgetrenderer.cpp
)

set(FARSO_OGRE_HEADERS
src/ogre3d/ogredraw.h
src/ogre3d/ogrejunction.h
src/ogre3d/ogresurface.h
src/ogre3d/ogrewidgetrenderer.h
)

if(${OGRE_FOUND})
   set(FARSO_FULL_SOURCES ${FARSO_FULL_SOURCES} ${FARSO_OGRE_SOURCES})
   set(FARSO_FULL_HEADERS ${FARSO_FULL_HEADERS} ${FARSO_OGRE_HEADERS})
endif(${OGRE_FOUND})

set(FARSO_COMMON_EXAMPLE_SOURCES
examples/src/common.cpp
)

set(FARSO_COMMON_EXAMPLE_HEADERS
examples/src/common.h
)

set(FARSO_OGRE3D_EXAMPLE_SOURCES
examples/src/ogre3d/ogre3d_example.cpp
examples/src/ogre3d/ogre3d_example_main.cpp
)
set(FARSO_OGRE3D_EXAMPLE_HEADERS
examples/src/ogre3d/ogre3d_example.h
)

set(FARSO_OPENGL_EXAMPLE_SOURCES
examples/src/opengl/opengl_example.cpp
)
set(FARSO_OPENGL_EXAMPLE_HEADERS
examples/src/opengl/opengl_example.h
)

set(FARSO_HAS_EXAMPLE 0)

if(${OGRE_FOUND})
   set(FARSO_EXAMPLE_SOURCES ${FARSO_COMMON_EXAMPLE_SOURCES}
                             ${FARSO_OGRE3D_EXAMPLE_SOURCES})
   set(FARSO_EXAMPLE_HEADERS ${FARSO_COMMON_EXAMPLE_HEADERS}
                             ${FARSO_OGRE3D_EXAMPLE_HEADERS})
   add_executable(farso_ogre3d_example WIN32 
                        ${FARSO_EXAMPLE_SOURCES} 
                        ${FARSO_COMMON_EXAMPLE_HEADERS})
   target_link_libraries(farso_ogre3d_example farso
                        ${GOBLIN_LIBRARY} ${KOBOLD_LIBRARY}
                        ${OGRE_LIBRARIES} 
                        ${OGRE_RTShaderSystem_LIBRARIES}
                        ${OGRE_Overlay_LIBRARIES}
                        ${OGRE_Terrain_LIBRARIES}
                        ${FREETYPE_LIBRARIES}
                        ${OPENGL_LIBRARY}
                        ${SDL2_IMAGE_LIBRARY}
                        ${SDL2_LIBRARY} ${OPENAL_LIBRARY} 
                        ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY}
                        ${OGG_LIBRARY} m
                        ${LIBINTL_LIBRARIES} pthread)
   add_custom_command(TARGET farso_ogre3d_example PRE_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy_directory
                      ${CMAKE_SOURCE_DIR}/examples/data 
                      $<TARGET_FILE_DIR:farso_ogre3d_example>/data)
   set(FARSO_HAS_EXAMPLE 1)
endif(${OGRE_FOUND})

if(${OPENGL_FOUND})
   set(FARSO_EXAMPLE_SOURCES ${FARSO_COMMON_EXAMPLE_SOURCES}
                             ${FARSO_OPENGL_EXAMPLE_SOURCES})
   set(FARSO_EXAMPLE_HEADERS ${FARSO_COMMON_EXAMPLE_HEADERS}
                             ${FARSO_OPENGL_EXAMPLE_HEADERS})
   add_executable(farso_opengl_example WIN32 
                        ${FARSO_EXAMPLE_SOURCES} 
                        ${FARSO_COMMON_EXAMPLE_HEADERS})
   target_link_libraries(farso_opengl_example farso
                        ${GOBLIN_LIBRARY} ${KOBOLD_LIBRARY}
                        ${OGRE_LIBRARIES} 
                        ${OGRE_RTShaderSystem_LIBRARIES}
                        ${OGRE_Overlay_LIBRARIES}
                        ${OGRE_Terrain_LIBRARIES}
                        ${FREETYPE_LIBRARIES}
                        ${OPENGL_LIBRARY}
                        ${SDL2_IMAGE_LIBRARY}
                        ${SDL2_LIBRARY} ${OPENAL_LIBRARY} 
                        ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY}
                        ${OGG_LIBRARY} m
                        ${LIBINTL_LIBRARIES} pthread)
  
   add_custom_command(TARGET farso_opengl_example PRE_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy_directory
                      ${CMAKE_SOURCE_DIR}/examples/data 
                      $<TARGET_FILE_DIR:farso_opengl_example>/data)
   set(FARSO_HAS_EXAMPLE 1)

endif(${OPENGL_FOUND})
   


