#pragma once
#include <osg/NodeCallback>
#include <osgViewer/Viewer>

namespace brtr {
     /**
    *  @brief       Callback for switching the "trains"
    *  @details	    every ~36 secs the "train" on the rails switched
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @pre         needs to be attached to a switch node
    *  @copyright   GNU Public License.
    */
    class TrainSwitcherCallback : public osg::NodeCallback {
    public:

        TrainSwitcherCallback();
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
    private:
        int _curActiveTrain;
        int _deltaTime;
    };
}
