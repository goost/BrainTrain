#pragma once
#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include <osg/Program>
#include "../header/BaseInteractionCallback.h"

namespace brtr {
    /**
    *  @brief       Callback for switching the postprocess programs
    *  @details	    Every click the next program in the vector is choosen
    *               postprocessCam is the node which stateset holds the programs
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @copyright   GNU Public License.
    */
    class ProgramSwitcherCallback : public BaseInteractionCallback {
    public:
        /**
        * @brief Constructor
        *
        * @param  postprocessCam  node which stateset contains the programs
        * @param  hudCam
        * @param  width      screenWidth
        * @param  height     screenHeight
        * @param  programs   vector with postProcessprograms
        */
        ProgramSwitcherCallback(osg::Node* postprocessCam, osg::Camera* hudCam, int width, int height, std::vector<osg::ref_ptr<osg::Program>> programs);
        //Doc in parent
        virtual void setText();
    protected:
        /**
        * @brief each interact sets the next program
        *
        * @param  not used
        * @param  not used
        */
        virtual void interact(osg::Node*, osg::NodeVisitor*);
    private:
        std::vector<osg::ref_ptr<osg::Program>> _programs;
        unsigned int _curProg;
    };
}