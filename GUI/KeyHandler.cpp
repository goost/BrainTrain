#include "../header/KeyHandler.h"
#include "../header/UtilFunctions.h"
#include <osgUtil/CullVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osg/ValueObject>

namespace brtr {
    KeyHandler::KeyHandler(osg::Node* rootNode, osg::Camera* postProcessCam, std::vector<osg::ref_ptr<osg::Program>> programs) :
        _programs(programs),
        _postProcessCam(postProcessCam),
        _rootNode(rootNode),
        _isWireFrame(false),
        _curProg(0){
        _wireFrameMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        _normaleMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
    }


    KeyHandler::~KeyHandler() {}

    bool KeyHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
        switch (ea.getEventType()) {
        case osgGA::GUIEventAdapter::FRAME:
            mouseIntersection(aa);
            //return false;
        case osgGA::GUIEventAdapter::MOVE:
        case osgGA::GUIEventAdapter::DRAG:
        case osgGA::GUIEventAdapter::RELEASE:
            _mouseEvent = &ea;
            return false;
        case osgGA::GUIEventAdapter::PUSH:
            _mouseEvent = &ea;
            if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
                brtr::BaseInteractionCallback* callback = modifyText(false);
                if (callback) {
                    OSG_NOTICE << "Attaching callback" << std::endl;
                    if (callback->getNode()->getUpdateCallback() == callback)
                        callback->reactivate();
                    else
                        callback->getNode()->addUpdateCallback(callback);
                }// callback
                
                return true;
            }
            return false;
        case osgGA::GUIEventAdapter::KEYDOWN:
            return handleKeyDown(ea, aa);
        default:
            return false;
        }
    }//hanlde()

    bool KeyHandler::handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
        switch (ea.getUnmodifiedKey()) {
            //Turn PolygonMode on/off
        case osgGA::GUIEventAdapter::KEY_C: {
            osg::PolygonMode* curMode = _isWireFrame ? _normaleMode : _wireFrameMode;
            _rootNode->getOrCreateStateSet()->setAttributeAndModes(curMode);
            _isWireFrame = !_isWireFrame;
            return true;
        }//case KEY_C
        case osgGA::GUIEventAdapter::KEY_1:{
            if (ea.getModKeyMask() == osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT) {
                _postProcessCam->getOrCreateStateSet()->removeAttribute(_programs[_curProg]);
                _curProg++;
                _curProg = _curProg % _programs.size();
                _postProcessCam->getOrCreateStateSet()->setAttributeAndModes(_programs[_curProg], osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
                return true;
            }
        }   //KEY_1
        default:
            return false;
        }//switch  
    }//if(KEYDOWN)

    void KeyHandler::mouseIntersection(osgGA::GUIActionAdapter& aa) {
        osg::ref_ptr<osg::Camera> camera = aa.asView()->getCamera();
        if (!_mouseEvent || !camera)
            return;

        osg::Vec3d eyeInWorld = osg::Vec3d() *osg::Matrixd::inverse(camera->getViewMatrix());
        osg::ref_ptr<osgUtil::LineSegmentIntersector> lIntersector =
            new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, _mouseEvent->getX(), _mouseEvent->getY());
        lIntersector->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
        osgUtil::IntersectionVisitor iv(lIntersector);
        iv.setTraversalMask(interactionMask);
        camera->accept(iv);
        if (lIntersector->containsIntersections()) {
            auto intersection = lIntersector->getIntersections().begin();           
            double curDistance = (eyeInWorld - intersection->getWorldIntersectPoint()).length();
            if (curDistance < 4.5 ) {
                _curDrawable = intersection->drawable;
                modifyText(true);
            }//if (curDistance < distance)
            else {//not the right distance, remove Text & drawable, if any were present
                modifyText(false);
            }//else
        }//if (intersector->containsIntersections()
    }//intersect()

    brtr::BaseInteractionCallback* KeyHandler::modifyText(bool show) {
        brtr::BaseInteractionCallback* callback =nullptr;
        if (_curDrawable) {
            OSG_NOTICE << "Checking Container..." << std::endl;
            osg::UserDataContainer* container = _curDrawable->getUserDataContainer();
            if (container) {
                OSG_NOTICE << "Container True" << std::endl;
                callback = dynamic_cast<brtr::BaseInteractionCallback*>(container->getUserObject(0));
                if (callback) {
                    if (show) callback->setText();
                    else callback->clearText();
                }// callback
            }//container
        }//_curDrawable
        if (!show)
            _curDrawable = nullptr;
        return callback;
    }//modifyText()

}//namespace
