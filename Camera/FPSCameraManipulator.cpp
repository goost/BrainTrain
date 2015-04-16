#include "../header/FPSCameraManipulator.h"
#include "../header/UtilFunctions.h"
#include <osgGA/GUIEventAdapter>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>

using namespace osg;
using namespace osgGA;

namespace brtr {
    FPSCameraManipulator::FPSCameraManipulator(double movementSpeed, double zHeight, Node* root, bool flightMode)
        :FirstPersonManipulator(),
        _forwardMovement(false),
        _backwardMovement(false),
        _leftMovement(false),
        _rightMovement(false),
        _upMovement(false),
        _downMovement(false),
        _attachBody(true),
        _shift(false),
        _ctrl(false),
        _jumpingUp(false),
        _jumpingDown(false),
        _crouch(false),
        _movementSpeed(movementSpeed),
        _zHeight(zHeight),
        _flightMode(flightMode),
        _savedzHeight(zHeight),
        _maxFallHeight(50),
        _frameFactor(30),
        _intensity(1.0),
        _bodyLength(0.0),
        _savedzHeightCrouch(0.0),
        _jumpHeight(4)
    {
        setNode(root);
        ref_ptr<Node> evaBody= osgDB::readNodeFile("../BlenderFiles/exports/BodyEva.ive");
        _body = new PositionAttitudeTransform();
        setHomePosition(Vec3(0, 134, 35 + zHeight + 5), Vec3(-1, 0, 26 + zHeight + 5), Z_AXIS);
        //_body->addChild(evaBody); body not used anymore
        _body->getOrCreateStateSet()->setAttribute(nullptr);
        Vec3d bodyPos = _homeEye;
        auto forward = _rotation * osg::Vec3d(0.0, 0.0, -1.0);
        bodyPos._v[2] =_homeEye._v[2]-1;
        _body->setPosition(bodyPos);
        _body->setNodeMask(~brtr::interactionAndCollisionMask);
        _body->getOrCreateStateSet()->addUniform(new Uniform("tex", false), StateAttribute::ON | StateAttribute::OVERRIDE);
        getNode()->asGroup()->addChild(_body);     
        home(0);
        
    }

    FPSCameraManipulator::~FPSCameraManipulator() {}

    bool FPSCameraManipulator::handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
        addMouseEvent(ea);
        if (performMovement())
            us.requestRedraw();
        return false;
    }

    bool FPSCameraManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
        FirstPersonManipulator::handleFrame(ea, us);
        if (performEyeMovement())
            us.requestRedraw();
        centerMousePointer(ea, us);
        return false;
    }

    bool FPSCameraManipulator::handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
        //FirstPersonManipulator::handleKeyDown(ea, us);
        switch (ea.getUnmodifiedKey()) {
        case GUIEventAdapter::KEY_W:
            _forwardMovement = true;
            return true;
        case GUIEventAdapter::KEY_A:
            _leftMovement = true;
            return true;
        case GUIEventAdapter::KEY_S:
            _backwardMovement = true;
            return true;
        case GUIEventAdapter::KEY_D:
            _rightMovement = true;
            return true;
        case GUIEventAdapter::KEY_E:
            if (_flightMode)
                _upMovement = true;
            return true;
        case GUIEventAdapter::KEY_Q:
            if (_flightMode)
                _downMovement = true;
            return true;
        case GUIEventAdapter::KEY_X:
            _crouch = !_crouch;
            _savedzHeightCrouch = _crouch ? _zHeight : _savedzHeightCrouch;
            _zHeight = _crouch ? _zHeight -3 : _savedzHeightCrouch;
            return true;
        case GUIEventAdapter::KEY_F:
            _flightMode = !_flightMode;
            return true;
        case GUIEventAdapter::KEY_G:
            _attachBody = !_attachBody;
            return true;
        case GUIEventAdapter::KEY_Space:
            if (!_jumpingUp && !_jumpingDown && !_flightMode) {
                _savedzHeight = _zHeight;
                _jumpingUp = true;
            }
            return true;
        case GUIEventAdapter::KEY_Shift_L:
           _shift = true;
            return true;
        case GUIEventAdapter::KEY_Control_L:
            _ctrl = true;
            return true;
        default:
            return false;
        }

    }

    bool FPSCameraManipulator::handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
        switch (ea.getUnmodifiedKey()) {
        case GUIEventAdapter::KEY_W:
            _forwardMovement = false;
            return true;     
        case GUIEventAdapter::KEY_A:
            _leftMovement = false;
            return true;           
        case GUIEventAdapter::KEY_S:
            _backwardMovement = false;
            return true;            
        case GUIEventAdapter::KEY_D:
            _rightMovement = false;
            return true;
        case GUIEventAdapter::KEY_E:
            _upMovement = false;
            return true;
        case GUIEventAdapter::KEY_Q:
            _downMovement = false;
            return true;
        case GUIEventAdapter::KEY_Shift_L:
            _shift = false;
            return true;
        case GUIEventAdapter::KEY_Control_L:
            _ctrl = false;
            return true;
        default:
            return false;           
        }
    }

    bool FPSCameraManipulator::performMovement() {
        if (_ga_t0.get() == NULL || _ga_t1.get() == NULL) return false;
        auto dy = _ga_t0->getYnormalized();
        auto dx = _ga_t0->getXnormalized();
        auto dt = _ga_t0->getTime() - _ga_t1->getTime();

        // return if there is no movement.
        if (dx == 0. && dy == 0.)
            return false;

        //rotate center
        Vec3d localUp = getUpVector(getCoordinateFrame(_eye));
        rotateYawPitch(_rotation, dx, dy, localUp);
        return true;
    }

    bool FPSCameraManipulator::performEyeMovement() {
        double intensity = _intensity * _shift ? 2.0 : 1.0 * _ctrl ? 0.5 : 1;
        OSG_DEBUG << "Intensity= " << intensity << std::endl;
        
        /*
        *  COLLISION DETECTION AND CLAMPING INSPIRED BY                 *
        *  Viggo Løvli (http://markmail.org/message/e6magjobl7fywbe6)   *
        *  AND DRIVEMANIPULATOR                                         *                            
        */

        //newEye tmp 'cos we do not know, if the new position is valid
        auto newEye = _eye;
        //up, right, forward dependent on the view
        auto up = _rotation * osg::Vec3d(0.0, 1.0, 0.0);
        auto forward = _rotation * osg::Vec3d(0.0, 0.0, -1.0);
        auto right = _rotation* osg::Vec3d(1.0, 0.0, 0.0);
        osg::Vec3d movement;

        //where to move
        if (_rightMovement)
            movement += right;
        if (_leftMovement)
            movement += -right;
        if (_forwardMovement)
            movement += forward * 1.2;
        if (_backwardMovement)
            movement += -forward * 0.8;
        if (_upMovement)
            movement += up;
        if (_downMovement)
            movement += -up;

        //_delta_frame_time -> not FPS depended, _frameFactor->'cos _deltaTime is so small
        movement *= _movementSpeed * intensity * _delta_frame_time * _frameFactor;

        if (_flightMode) {
            newEye += movement;
            _eye = newEye;
        }//if(_flightMode)
        else {
            double eyeIntersectionDistance = 1000;
            if (intersect(newEye, newEye + movement * 10, eyeIntersectionDistance)) {
                if (eyeIntersectionDistance < 1.75)
                    movement = Vec3d();
            }//if(intersect())
      
            if (_jumpingUp) {
                if (_zHeight < _savedzHeight + _jumpHeight)
                    _zHeight += 0.4 * _delta_frame_time * _frameFactor * 2;
                else {
                    _jumpingDown = true;
                    _jumpingUp = false;
                } //else  
            }//if (_jumpingUp)

            if (_jumpingDown) {
                if (_zHeight > _savedzHeight )
                    _zHeight -= 0.2 * _delta_frame_time * _frameFactor * 2;
                else {
                    _jumpingDown = false;
                    _jumpingUp = false;
                    _zHeight = _savedzHeight;
                }//else
            }// if (_jumpingDown)

            newEye += movement;
            if (groundIntersection(newEye))
                _eye = newEye;
            else return false;
        }
        OSG_DEBUG << "eyeZ: " << _eye._v[2]<< std::endl;
        if (_attachBody)
            _body->setPosition(Vec3d(_eye._v[0], _eye._v[1], _eye._v[2] -1));
        return true;
    }//performEyeMovement

    bool FPSCameraManipulator::groundIntersection(osg::Vec3d& newEye) {
        double dist = 0.0;

        if (!intersect(newEye, newEye + Vec3(0, 0, -1) * _maxFallHeight, dist))
            return false;       
        dist -= _zHeight;
        OSG_DEBUG << "dDist: " << dist << std::endl;
        if (-dist > _zHeight -2.5 || dist > _maxFallHeight) {
             return false;
        }

       
        dist /=2;  //slide effect, smooth up/down
        newEye._v[2] -=  dist; 
        return true;
    }//groundIntersection()

    bool FPSCameraManipulator::intersect(const osg::Vec3d start, const osg::Vec3d end, double& distance) {
        if ((start - end).length() < 1e-8) return false; //avoids termination of program, if start == end
        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(start, end);
        intersector->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
        osgUtil::IntersectionVisitor iv(intersector);
        
        iv.setTraversalMask(collisionMask); 
        _node->accept(iv);

        if (intersector->containsIntersections()) {
            auto intersection = intersector->getIntersections().begin();
            distance = (start - intersection->getWorldIntersectPoint()).length();
            return true;
        }//if (intersector->containsIntersections()
        return false;
    }//intersect()

    double FPSCameraManipulator::getMovementSpeed() const {
        return _movementSpeed;
    }

    FPSCameraManipulator& FPSCameraManipulator::setMovementSpeed(double val) {
        _movementSpeed = val;
        return *this;
    }

    double FPSCameraManipulator::getZHeight() const {
        return _zHeight;
    }

    FPSCameraManipulator& FPSCameraManipulator::setZHeight(double val) {
        _zHeight = val;
        return *this;
    }

    FPSCameraManipulator& FPSCameraManipulator::setJumpHeight(double val) {
        _jumpHeight = val;
        return *this;
    }

    double FPSCameraManipulator::getJumpHeight() const {
        return _jumpHeight;
    }

    bool FPSCameraManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy) {
        return false;
    }

    bool FPSCameraManipulator::handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
        return false;
    }

}
