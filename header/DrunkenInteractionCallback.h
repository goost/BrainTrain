#pragma once
#include <osg/NodeCallback>
#include <osgAnimation/EaseMotion>
#include <osgViewer/Viewer>
#include "../header/BaseInteractionCallback.h"

namespace brtr {
    /**
    *  @brief       Callback for the drunk effect
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @copyright   GNU Public License.
    */
    class DrunkenInteractionCallback : public BaseInteractionCallback {
    public:

        /**
         * @brief  Constructor
         *
         * @param  camera the camera, whichs projection matrix will be manipulated
         * @param  hudCam
         * @param  geometrySwitch   switch containing the bottle, for removing it after the interaction
         * @param  width    screenwidth
         * @param  height   screenheight
         */
        DrunkenInteractionCallback(osg::Node* camera, osg::Camera* hudCam, osg::Switch* geometrySwitch, int width, int height);

        virtual void setText();
    protected:
        /**
         * @brief Drunk effect is simulated by changing the FOV of the projection matrix
         *
         * @param  not needed
         * @param  not needed
         */
        virtual void interact(osg::Node*, osg::NodeVisitor*);
    private:        
        int _startTime;
        osg::ref_ptr<osg::Switch> _geometrySwitch;
        osg::ref_ptr<osgAnimation::LinearMotion> _motion;
        bool _backwards;
    };
}