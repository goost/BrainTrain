#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include "../header/ProgramSwitcherCallback.h"

namespace brtr {
  

    ProgramSwitcherCallback::ProgramSwitcherCallback(osg::Node* postprocessCam, osg::Camera* hudCam, int width, int height, std::vector<osg::ref_ptr<osg::Program>> programs):
        BaseInteractionCallback(postprocessCam,hudCam,width,height),
        _programs(programs),
        _curProg(0){}

    void ProgramSwitcherCallback::setText() {
        _text->setText("You feel a mysterious power\nfrom this strange device.\nA click will change the world...");
    }

    void ProgramSwitcherCallback::interact(osg::Node*, osg::NodeVisitor*) {
        _done = true;
        _attachTo->getOrCreateStateSet()->removeAttribute(_programs[_curProg]);
        _curProg++;
        _curProg = _curProg % _programs.size();
        _attachTo->getOrCreateStateSet()->setAttributeAndModes(_programs[_curProg], osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    }



}