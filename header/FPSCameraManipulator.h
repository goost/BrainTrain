#pragma once
#include <osgGA/FirstPersonManipulator>
namespace brtr {
    /**
    *  @brief       A FPS style CameraManipulator with ground clamping and intersection 
    *  @details     @par <b> Controls: </b>
    *               <pre>
    *                     W       = Move forward.
    *                   A S D     = (A)=Move Left, (S)=Move backward, (D)=Move Right.
    *                     F       = Toggle FlightMode on/off
    *                   Q / E     = Up/Down (in FlightMode)
    *                     G       = Attach/Detach Body
    *                     X       = Crouch
    *                   SPACE     = Jump (if not flying)
    *                   SHIFT     = Sprint
    *                   CTRL      = Walk
    *               </pre>
    *               Inspiration for Intersection and Clamping Testing: <br/>
    *               Official OSG Source (mostly DriveManipulator)<br/>
    *               GameManipulator and Pod by 	Viggo Løvli, http://markmail.org/message/e6magjobl7fywbe6 , visited 26/05/2014<br/>
    *               For Nodes, which should be passable regardless of FlightMode (e.g. FakeWalls) one must set the <br/>
    *               NodeMask to ~brtr::collisionMask <br/>
    *               Body Code not used anymore (we did not like it).
    *               Not deleted, because it works.
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        06/2014
    *  @pre         Need to be attached to a viewer, so, create a viewer first
    *  @bug         Jumping forward if there is no ground is not working<br/>
    *            
    *  @copyright   GNU Public License.
    */
    class FPSCameraManipulator :
        public osgGA::FirstPersonManipulator {
    public:
        /**
         * @brief Constructor
         * @param  movementSpeed the player "movement" speed
         * @param  zHeight      the camera height
         * @param  root         root node for attaching the body to, not used anymore
         * @param  flightMode   flightmoe true or false in the beginning
         */
        FPSCameraManipulator(double movementSpeed, double zHeight, osg::Node* root, bool flightMode = false);
        double getMovementSpeed() const;
        FPSCameraManipulator& setMovementSpeed(double val);
        double getZHeight() const;
        FPSCameraManipulator& setZHeight(double val);
        double getJumpHeight() const;
        FPSCameraManipulator& setJumpHeight(double val);

    protected:
        ~FPSCameraManipulator();
        /**
         * @brief Handles the movement of the mouse
         *
         * Warp the mouse back to the center, adds the mouse movement to the event stack
         * and calls performMovement(). If it was succesfull, request redraw.
         *
         * @param   ea the GUIEventAdapter
         * @param   us the GUIActionAdapter
         * @return  always false otherwise the method would block other mouse movement handle methods 
         */
        virtual bool handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) ;
        /**
         * @brief Handles, what happens every frame
         *
         * Every frame there is a movement check, if the player holds one of the move buttons down
         * the camera moves.
         *
         * @param   ea the GUIEventAdapter
         * @param   us the GUIActionAdapter
         * @return  always false otherwise the method would block other frame handle methods 
         */
        virtual bool handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) ;
        /**
         * @brief Handle key down presses. For supported keys see the class desc
         *
         *
         * @param ea 
         * @param us 
         * @return true, if the keypress is handled, false otherwise
         */
        virtual bool handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) ;
        /**
         * @brief Handle key up presses. For supported keys see the class desc
         *
         *
         * @param ea  ea the GUIEventAdapter
         * @param us  us the GUIActionAdapter
         * @return    true, if the key-de-press is handled, false otherwise
         */
        virtual bool handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) ;       
        /**
        * @brief MouseLook is implemented in this method.
        *
        * Because the mouse is always at the center, it is enough to get the previous
        * mouse position (_ga_t0, the current position is always the center = _ga_t1)
        * and rotate the camera to that position.
        *
        * @return true, if there was any movement, false otherwise
        */
        virtual bool performMovement() ;
        

        //Just  to kill the implementation
        virtual bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy) ;
       // virtual bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy) ; //remove?
       // virtual bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy) ; //remove?
        virtual bool handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) ;

    private:
        /**
        * @brief moves the cameraEye, checking various conditions
        *
        *    directions + speed depends on the pressed keys.
        *    if flightmode is off, clamping to ground is performed, _zHeight is height of eye
        *    also, if flightmode is off, LineIntersection from the eye is performed
        *    so no walking trough walls
        *    again, with flightmode off, jumping request is also handled
        *    if flightmode is on, then just moves the cameraEye
        *
        * @return true, if there was any movement, false otherwise
        */
        bool performEyeMovement();
        /**
         * @brief Finds the distance between start and end intersection, if there is any
         *
         * performs a LineIntersectionTest in Model coordinates, from intersect::start to intersect::end
         * the distance to the nearest intersection point, if any exist, is stored in intersect::distance
         *
         * @param start     the start point of the line 
         * @param end       the end point of the line
         * @param distance  var, which will hold the distance to the nearest intersection point, if any
         * @return          true, if there is at least one intersection, false otherwise
         */
        bool intersect(const osg::Vec3d start, const osg::Vec3d end, double& distance);
        /**
         * @brief checks, whether the newEye is still clamped to ground
         *
         * Performs a LineIntersectionTest from newEye to -Z_AXIS * FPSCameraManipulator::_maxFallHeight
         * if a ground is found (any geometry within _maxFallHeight), the z-Value of newEye will be corrected
         * to be _zHeight above ground.
         * some smoothing is applied for not-so-abrupt jumps
         *
         * @param newEye the wannabe new cameraEye Position 
         * @return true, if Position is valid, false otherwise 
         */
        bool groundIntersection(osg::Vec3d& newEye);

        osg::ref_ptr<osg::PositionAttitudeTransform> _body;
        bool _flightMode;
        bool _forwardMovement; 
        bool _backwardMovement;
        bool _leftMovement;
        bool _rightMovement;
        bool _upMovement;
        bool _downMovement;
        bool _attachBody;
        bool _shift;
        bool _ctrl;
        bool _jumpingUp;
        bool _jumpingDown;
        bool _crouch;
        double _maxFallHeight;
        double _movementSpeed;
        double _zHeight;
        double _savedzHeight;
        double _intensity;
        double _frameFactor;
        double _bodyLength;
        double _jumpHeight;
        double _savedzHeightCrouch;
        };
}

