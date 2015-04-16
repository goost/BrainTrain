#include "../header/TrainSwitcherCallback.h"

namespace brtr{
    TrainSwitcherCallback::TrainSwitcherCallback():
        _curActiveTrain(0),
        _deltaTime(0){}

    void TrainSwitcherCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
        //deltatime == 0? So wee need a new timestamp
        if (_deltaTime == 0) {
            _deltaTime = nv->getFrameStamp()->getReferenceTime();
        }
        //it is attached to a switch, so the node is a switch
        osg::Switch* switcher = static_cast<osg::Switch*>(node);
        if (nv->getFrameStamp()->getReferenceTime() - _deltaTime > 36) {
            _curActiveTrain++;
            _curActiveTrain = _curActiveTrain % switcher->getNumChildren();
            switcher->setAllChildrenOff();
            switcher->setValue(_curActiveTrain, true);
            _deltaTime = 0;
        }
        traverse(node, nv);
    }

}

