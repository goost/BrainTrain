#pragma once
#include <osg/NodeVisitor>
#include <../header/BaseInteractionCallback.h>
#include <osg/ValueObject>

namespace brtr{
    /**
    *  @brief       NodeVisitor for batch replacing all UserDataContainer of all Drawables.               
    *  @details     New Container contains the provided InteractionCallback.
                    Mainly used for making imported objects (e.g. from blender) interact-able.
    *  @author     Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @pre         needs a Node which will accept it. Should have some Geode's for this to work
    *  @copyright   GNU Public License.
    */
    class AddInteractionCallbackToDrawableVisitor : public osg::NodeVisitor {
    public:
        /**
         * @brief Constructor

         * @param  callbackToAdd the callback which should be add to all drawables in the object
         * @return
         */
        AddInteractionCallbackToDrawableVisitor(brtr::BaseInteractionCallback* callbackToAdd);
        virtual void apply(osg::Geode& geode);
    private:
        osg::ref_ptr<osg::DefaultUserDataContainer> _containerToAdd;
    };
}