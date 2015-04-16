#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include "../header/ToonTexSwitcherCallback.h"

namespace brtr {
   

    ToonTexSwitcherCallback::ToonTexSwitcherCallback(osg::Node* sceneData, osg::Camera* hudCam, int width, int height, std::vector<osg::ref_ptr<osg::Texture2D>> toonTexs) :
    BaseInteractionCallback(sceneData,hudCam,width,height),
    _curTex(0),
    _toonTexs(toonTexs){ }

    void ToonTexSwitcherCallback::setText() {
        _text->setText("The colors of the world\nare hidden here.\nTouch them, if you dare.");
    }

    void ToonTexSwitcherCallback::interact(osg::Node*, osg::NodeVisitor*) {
        _done = true;
        _attachTo->getOrCreateStateSet()->removeTextureAttribute(1, _toonTexs[_curTex]);
        _curTex++;
        _curTex = _curTex % _toonTexs.size();
        _attachTo->getOrCreateStateSet()->setTextureAttribute(1, _toonTexs[_curTex], osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    }



}