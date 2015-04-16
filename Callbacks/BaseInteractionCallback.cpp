#include "../header/BaseInteractionCallback.h"
#include <osg/Camera>
#include "../header/UtilFunctions.h"
namespace brtr {

    BaseInteractionCallback::BaseInteractionCallback(osg::Node* attachTo, osg::Camera* hudCam, int width, int height) :
        _attachTo(attachTo),
        _hudCam(hudCam),
        _done(false){
        _text = brtr::createText(osg::Vec3d(width / 2.0 - 320, height / 2.0 - 110, 0), "", width * 0.02);
        osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
        textGeode->addDrawable(_text);    
        _hudCam->addChild(textGeode);
    }

    void BaseInteractionCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
        if (!_done) {
            interact(node, nv);
        }
        traverse(node, nv);
    }

    void BaseInteractionCallback::setNode(osg::ref_ptr<osg::Node> val) {
        _attachTo = val;
    }

    osg::ref_ptr<osg::Node> BaseInteractionCallback::getNode() const {
        return _attachTo;
    }

    void BaseInteractionCallback::clearText() {
        _text->setText("");
    }

    void BaseInteractionCallback::reactivate() {
        _done = false;
    }

}

