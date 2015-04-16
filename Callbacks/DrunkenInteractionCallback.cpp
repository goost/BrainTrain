#include "../header/DrunkenInteractionCallback.h"


namespace brtr {
    DrunkenInteractionCallback::DrunkenInteractionCallback(osg::Node* camera,osg::Camera* hudCam, osg::Switch* geometrySwitch, int width, int height) :
    BaseInteractionCallback(camera, hudCam, width, height),
    _geometrySwitch(geometrySwitch),
    _startTime(0),
    _backwards(false){
        _motion = new osgAnimation::LinearMotion(70, 70, 70, osgAnimation::Motion::LOOP);
    }

    void DrunkenInteractionCallback::interact(osg::Node* node, osg::NodeVisitor* nv) {
        //save starttime and switch off the bottle
        if (_startTime == 0) {
            _geometrySwitch->setAllChildrenOff();
            _startTime = nv->getFrameStamp()->getReferenceTime();
        }
        //linear motion for the effect
        _motion->update(1);
        osg::Camera* camera = static_cast<osg::Camera*>(node);
        if (camera) {
            OSG_NOTICE << "Motion Value " << _motion->getValue() << std::endl;
            //backwards after up to FOV 125
            if (_motion->getValue() >= 125)
                _backwards = !_backwards;
            camera->setProjectionMatrixAsPerspective(_backwards ? 195 - _motion->getValue() : _motion->getValue(), 2, 0.01, 100000);
        }//camera
        
        if (nv->getFrameStamp()->getReferenceTime() - _startTime >= 20) {
            if (camera) {
                camera->setProjectionMatrixAsPerspective(70, 1.778, 0.01, 100000);
                _done = true;
                _geometrySwitch = nullptr;
                _hudCam = nullptr;
                _attachTo = nullptr;
                _motion = nullptr;
                _text = nullptr;
                OSG_NOTICE << "DrunkenInteractionCallback: Should now be removed" << std::endl;
            }//camera
        }//30 sec over
    }

    void DrunkenInteractionCallback::setText() {
        _text->setText("Click Left Mouse for a drink!");
    }

}

