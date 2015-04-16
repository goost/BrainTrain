cmake_minimum_required( VERSION 2.6 )
project( BrainTrain )

find_package( OpenThreads )
find_package( osg )
find_package( osgDB )
find_package( osgUtil )
find_package( osgViewer )
find_package( osgGA )
find_package( osgShadow )
find_package( osgText )
find_package( osgAnimation )

find_package( osgFX )
find_package( osgManipulator )
find_package( osgParticle )
find_package( osgSim )
find_package( osgTerrain )
find_package( osgVolume )
find_package( osgWidget )

macro( config_project PROJNAME LIBNAME )
    include_directories( ${${LIBNAME}_INCLUDE_DIR} )
    target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
endmacro()

#C++11 Checker taken from http://www.guyrutenberg.com/2014/01/05/enabling-c11-c0x-in-cmake/
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(source
    Util/UtilFunctions.cpp
    Objects/Bench.cpp
    Animation/AnimationCreater.cpp
    Camera/FPSCameraManipulator.cpp
    Camera/WeaponHUD.cpp
    GUI/KeyHandler.cpp
    Shader/CelShading.cpp
    Util/GeometryPlacerVisitor.cpp
    Callbacks/DrunkenInteractionCallback.cpp
    Callbacks/BaseInteractionCallback.cpp
    Util/ModifyMaterialVisitor.cpp
    Callbacks/AddPortalGunInteractionCallback.cpp
    Util/AddInteractionCallbackToDrawableVisitor.cpp
    Objects/ControlRoom.cpp
    Callbacks/ProgramSwitcherCallback.cpp
    Callbacks/ToonTexSwitcherCallback.cpp
    Callbacks/TrainSwitcherCallback.cpp
 
	)
set(headerPath ${PROJECT_SOURCE_DIR}/header)
set(header
	${headerPath}/FPSCameraManipulator.h
    ${headerPath}/KeyHandler.h
    ${headerPath}/CelShading.h
    ${headerPath}/WeaponHUD.h
    ${headerPath}/UtilFunctions.h
    ${headerPath}/GeometryPlacerVisitor.h
    ${headerPath}/BaseInteractionCallback.h
    ${headerPath}/DrunkenInteractionCallback.h
	${headerPath}/Bench.h
    ${headerPath}/ModifyMaterialVisitor.h
    ${headerPath}/AnimationCreater.h
    ${headerPath}/AddPortalGunInteractionCallback.h
    ${headerPath}/AddInteractionCallbackToDrawableVisitor.h
    ${headerPath}/ControlRoom.h
    ${headerPath}/ProgramSwitcherCallback.h
    ${headerPath}/ToonTexSwitcherCallback.h
    ${headerPath}/TrainSwitcherCallback.h
    )

add_executable( BrainTrain Main/Main.cpp ${source} ${header} )
include_directories("${headerPath}")
config_project( BrainTrain OPENTHREADS )
config_project( BrainTrain OSG )
config_project( BrainTrain OSGDB )
config_project( BrainTrain OSGUTIL )
config_project( BrainTrain OSGVIEWER )
config_project( BrainTrain OSGGA )
config_project( BrainTrain OSGSHADOW )
config_project( BrainTrain OSGTEXT )
config_project( BrainTrain OSGANIMATION )
config_project( BrainTrain OSGFX )
config_project( BrainTrain OSGMANIPULATOR )
config_project( BrainTrain OSGPARTICLE )
config_project( BrainTrain OSGSIM )
config_project( BrainTrain OSGTERRAIN )
config_project( BrainTrain OSGVOLUME )
config_project( BrainTrain OSGWIDGET )

