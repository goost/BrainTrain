#pragma once
#include <osg/NodeCallback>
#include <osgViewer/Viewer>
#include "../header/BaseInteractionCallback.h"

namespace brtr {
    /**
    *  @brief       InteractionCallback for adding the portal gun to the players inventar
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @copyright   GNU Public License.
    */
    class AddPortalGunInteractionCallback : public BaseInteractionCallback {
    public:
        /**
         * @brief Constructor
         *
         * @param  weaponHUD  weaponHUD which provides the method to add the portalgun to it
         * @param  hudCam     
         * @param  switcher   switch-node which contains the portalGun object, will be switched to off upon interaction, removing the portal Gun from the world
         * @param  width      screenWidth
         * @param  height     screenHeight
         * @return
         */
        AddPortalGunInteractionCallback(osg::Node* weaponHUD, osg::Camera* hudCam, osg::Switch* switcher, int width, int height);
        //doc in parent
        virtual void setText();
    protected:
        virtual void interact(osg::Node*, osg::NodeVisitor*);
    private:
        osg::ref_ptr<osg::Switch> _switcher;
    };
}