#pragma once
#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include <osg/Material>
#include "../header/ToonTexSwitcherCallback.h"
#include "../header/ProgramSwitcherCallback.h"

namespace brtr{
    /**
    *  @brief       Control Room Class, derived from PositionAttitudeTransform, set ups the whole room as its own children
    *  @details	    sets always a light as light0, client should not use this light number any more
    *               the chess figures alongside with the provided interactioncallbacks are also set up
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @copyright   GNU Public License.
    */
    class ControlRoom : public osg::PositionAttitudeTransform {
    public:
        /**
         * @brief Constructor

         * @param  roomSize         size of the room, height is roomsize/2
         * @param  lod              level of detail, the higher the more triangles are created
         * @param  toonCallback     ToonTexSwitcherCallback, will be attached to first chess figure
         * @param  programCallback  ProgramSwitcherCallback, will be attached to third chess figure
         */
        ControlRoom(double roomSize, int lod, brtr::ToonTexSwitcherCallback& toonCallback, brtr::ProgramSwitcherCallback& programCallback );

    protected:
        ~ControlRoom() {}
    private:
        osg::ref_ptr<osg::Group> createRoomSurrounding(double roomSize, int lod);
        osg::ref_ptr<osg::Group> createChessFigures(brtr::ToonTexSwitcherCallback& toonCallback, brtr::ProgramSwitcherCallback& programCallback);
        osg::ref_ptr<osg::Material> createMaterial(osg::Vec4 diffuse, osg::Vec4 ambient, osg::Vec4 specular = osg::Vec4(0.7,0.7,0.7,1), double shininess = 42.0);
    };
}