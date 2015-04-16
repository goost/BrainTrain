#pragma once
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osg/PolygonMode>
#include <osg/Program>
#include "../header/FPSCameraManipulator.h"
#include "../header/BaseInteractionCallback.h"
namespace brtr {
    /**
    *  @brief       Key Handler Class, handles all of our KeyFunctions, which do not belong
    *               to camera control (this are handled by FPSCameraManipulator)
    *  @details     \par <b> Controls: </b>
    *               <pre>
    *                     C       = Toggle WireFrame Mode On/Off
    *                   LClick    = Interact
    *                   Shift+1   = Toggle programs
    *               </pre>
    *  @author     Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @copyright   GNU Public License.
    */
    class KeyHandler :
        public osgGA::GUIEventHandler {
    public:
        /**
         * @brief Constructor
         *
         * @param  rootnode rootnode of the scene, polygonmode will be activatd on all children 
         * @param  postProcessCam   node containing the postprocess programs
         * @param  programs         vector with postprocess programs
         */
        KeyHandler(osg::Node*, osg::Camera* postProcessCam, std::vector<osg::ref_ptr<osg::Program>> programs);
        virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    protected:
        ~KeyHandler();
    private:
        bool handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
        /**
         * @brief Checks, if under the mouse (e.a center of screen) is an interact-able object (e.a geometry)
         *
         * @param  aa GUIActionAdapter for getting the camera , to whom the LineIntersectionVisitor will be attached to
         */
        void mouseIntersection(osgGA::GUIActionAdapter& aa);
        /**
         * @brief Shows the InteractionMessage on screen, if there is an InteractionObject beneath the mouse (e.a center of screen)
         *
         * @param  to show or not show the text, that is the question
         * @return the BaseInteractionCallback associated with the interactionObject, if any, null else;
         */
        brtr::BaseInteractionCallback* modifyText(bool show);
        osg::ref_ptr<osg::Drawable> _curDrawable;
        osg::ref_ptr<osg::Node> _rootNode;
        osg::ref_ptr<osg::PolygonMode> _wireFrameMode;
        osg::ref_ptr<osg::PolygonMode> _normaleMode;
        osg::ref_ptr<osg::Camera> _postProcessCam;
        std::vector<osg::ref_ptr<osg::Program>> _programs;
        osg::ref_ptr< const osgGA::GUIEventAdapter > _mouseEvent;
        bool _isWireFrame;
        unsigned int _curProg;
    };
}

