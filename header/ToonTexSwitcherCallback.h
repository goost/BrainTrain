#pragma once
#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include "../header/BaseInteractionCallback.h"

namespace brtr {
    /**
    *  @brief       Callback for switching the ToonTextures
    *  @details	    Every click the next texture in the vector is choosen
    *               scenedata is the node which stateset holds the textures
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @copyright   GNU Public License.
    */
    class ToonTexSwitcherCallback : public BaseInteractionCallback {
    public:
        /**
        * @brief Constructor
        *
        * @param  scenedata  node which stateset contains the ToonTextures
        * @param  hudCam
        * @param  width      screenWidth
        * @param  height     screenHeight
        * @param  toonTexs   vector with ToonTextures
        */
        ToonTexSwitcherCallback(osg::Node* scenedata, osg::Camera* hudCam, int width, int height, std::vector<osg::ref_ptr<osg::Texture2D>> toonTexs);
        //docu in parent
        virtual void setText();
    protected:
        /**
         * @brief each interact sets the next texture
         *
         * @param  not used
         * @param  not used
         */
        virtual void interact(osg::Node* node, osg::NodeVisitor*);
    private:
        int _curTex;
        std::vector<osg::ref_ptr<osg::Texture2D>> _toonTexs;
    };
}