#include "../header/AddPortalGunInteractionCallback.h"
#include "../header/WeaponHUD.h"

namespace brtr{
    AddPortalGunInteractionCallback::AddPortalGunInteractionCallback(osg::Node* weaponHUD, osg::Camera* hudCam, osg::Switch* switcher, int width, int height):
    BaseInteractionCallback(weaponHUD,hudCam,width,height),
    _switcher(switcher){}

    void AddPortalGunInteractionCallback::setText() {
        _text->setText("Defect Portal Gun. \nLeft Click to pick it up anyway.");
    }

    void AddPortalGunInteractionCallback::interact(osg::Node* node, osg::NodeVisitor* nv) {
        _switcher->setAllChildrenOff();
        brtr::WeaponHUD* weaponHUD = static_cast<brtr::WeaponHUD*>(node);
        weaponHUD->addPortalGun();
        OSG_ALWAYS << "after add method" << std:: endl;
        _done = true;
    }

}
