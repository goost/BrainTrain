#pragma once
#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include <osgText/Text>

namespace brtr {
    /**
    *  @brief        This is the TemplateClass for InteractionCallbacks
    *  @details     InteractionCallbacks are set as an UserObject in an UserDataContainer of a Geometry.<br/>
                    Furthermore, the right NodeMask (brtr::interactionMask) must be set. <br/>
                    Every subclass must override the setText() and interact() method. <br/>
                    After the child is finished with its work, it must set the done-flag to the value true <br/>
                    The client must check if there is a valid Geometry with a valid InteractionCallback and call <br/>
                    the setText Method to set the text on screen. If the user interacts (e.g by clicking a mouse button) <br/>
                    the client must attach the callback to the node with getNode()->addUpdateCallback(), if its not already attached. <br/>
                    In this case the client must call reactivate() to reactivate the callback. (which basicly sets the done flag back to false) <br/>
                    clearText() should be called, if the clients wants to remove the message from the screen (e.g. if the player no longer looks at the geometry).   

    *  @author      Gleb Ostrowski                                                                                            
    *  @version     1.0
    *  @date        2014
    *  @copyright   GNU Public License.
    */
    class BaseInteractionCallback: public osg::NodeCallback {
    public:

        /**
         * @brief Constructor
         *
         * @param  attachTo the node the Callback will be attached to upon interaction
         * @param  hudCam  the HUDCam, where the text will appear
         * @param  width   screenWidth
         * @param  height  screenHeight
         */
        BaseInteractionCallback(osg::Node* attachTo, osg::Camera* hudCam, int width, int height);
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

        /**
         * @brief sets the text on screen. Subclasses must override to set its own (info)text
         */
        virtual void setText() = 0;
        void clearText();
        void reactivate();
        osg::ref_ptr<osg::Node> getNode() const;
        void setNode(osg::ref_ptr<osg::Node> val);
    protected:
        /**
         * @brief the interaction logic must be implemented be the children in this method
         */
        virtual void interact(osg::Node*, osg::NodeVisitor*)=0;
        osg::ref_ptr<osg::Node> _attachTo;
        osg::ref_ptr<osg::Camera> _hudCam;
        bool _done;
        osg::ref_ptr<osgText::Text> _text;
        
        
    private:
    };
}
