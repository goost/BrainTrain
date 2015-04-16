#include "../header/WeaponHUD.h"
#include "../header/CelShading.h"
#include "../header/ModifyMaterialVisitor.h"
#include "../header/UtilFunctions.h"


using namespace osg;

namespace brtr {

	WeaponHUD::WeaponHUD(const WeaponHUD& copy,const CopyOp& copyop)
	: Camera(copy, copyop)
	{}

	/*
	 * The constructor initializes the WeaponHUD with a standard weapon (in this case it's a crowbar) 
	 */
	WeaponHUD::WeaponHUD(){
		createWeaponHUD();
	}

	void WeaponHUD::createWeaponHUD() {
	
		ref_ptr<Node> crow = osgDB::readNodeFile("../BlenderFiles/exports/Brecheisen.ive");
	/*
	 * here a transformation is applied to the crowbar to show 
	 * it at the lower right of the screen as if it is held in
	 * the right hand
	 */
		ref_ptr<MatrixTransform> crowTransform = new MatrixTransform;
        crowTransform->setMatrix(
         Matrix::rotate(osg::DegreesToRadians(3.0f), Y_AXIS)
       * Matrix::rotate(DegreesToRadians(190.0f), Z_AXIS)
       * Matrix::translate(osg::Vec3(1.0f, 1.5f, -1.5f))
                                );
		crowTransform->addChild(crow);
	/*
	 * adjusting the shininess to give the crowbar a good appearance 
	 */
        brtr::ModifyMaterialVisitor mmv;
        mmv.setShininess(42).setSpecular(Vec4(0.4, 0.4, 0.4, 1));
        crow->accept(mmv);
	/*
	 * Adding a weapon switch to enable multiple weapon use
	 */
        ref_ptr<Switch> switcher = new Switch;
        switcher->addChild(crowTransform, true);
        _switcher = switcher;
        _handler = new WeaponSwitchHandler(switcher);

        ref_ptr<brtr::CelShading> celshade = new brtr::CelShading;
        celshade->addChild(switcher);
        celshade->setNodeMask(~brtr::interactionAndCollisionMask);

		setClearMask(GL_DEPTH_BUFFER_BIT);
		setRenderOrder(Camera::POST_RENDER);
        setReferenceFrame(Camera::ABSOLUTE_RF);
        setProjectionMatrixAsPerspective(100, 1, 0.001, 5);
		
        setViewMatrixAsLookAt(Vec3(), Vec3(0,1,0), Z_AXIS);
        getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF | StateAttribute::PROTECTED | StateAttribute::OVERRIDE);
        addChild(celshade);
	}

	WeaponHUD::~WeaponHUD() {

	}

    ref_ptr<WeaponHUD::WeaponSwitchHandler> WeaponHUD::getWeaponHandler() {
        return _handler;
    }

    void WeaponHUD::addPortalGun() {
        ref_ptr<Node> portalGun = osgDB::readNodeFile("../BlenderFiles/exports/Portalgun.ive");
	/*
	 * rotating and translating the portal gun to the lower
	 * right of the screen
	 */
        ref_ptr<MatrixTransform> portalGunTransform = new MatrixTransform;
        portalGunTransform->setMatrix(
            Matrix::rotate(osg::DegreesToRadians(0.0), X_AXIS)
            *Matrix::rotate(DegreesToRadians(0.0), Y_AXIS)
            *Matrix::rotate(DegreesToRadians(90.0), Z_AXIS)
            *Matrix::translate(osg::Vec3(0.7f, 1.5f, -1.3f))
            );
        portalGunTransform->addChild(portalGun);
        /*
	 * Set proper lighting and reflection
	 */
	brtr::ModifyMaterialVisitor mmv;
        mmv.setAmbient(Vec4(1.3, 1.3, 1.3, 1)).setShininess(42).setSpecular(Vec4(0.4, 0.4, 0.4, 1));
        portalGun->accept(mmv);
        _switcher->setAllChildrenOff();
        _switcher->addChild(portalGunTransform, true);
        

    }

//WEAPON_SWITCH_HANDLER

    WeaponHUD::WeaponSwitchHandler::WeaponSwitchHandler(Switch* switchNode) :
        _switch(switchNode),
        _curWeapon(0),
        _frameNumber(0){}

    bool WeaponHUD::WeaponSwitchHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
        switch (ea.getEventType()) {
        case osgGA::GUIEventAdapter::SCROLL:
        {
	  /*
	   * catches multiple activations of the weapon switching
	   * per frame because linux triggers keys twice
	   */
            if (_frameNumber == aa.asView()->getFrameStamp()->getFrameNumber()) {
                return false;
            }
            _frameNumber = aa.asView()->getFrameStamp()->getFrameNumber();
	    //Debug message
            OSG_ALWAYS << "SCROLL: " << aa.asView()->getFrameStamp()->getFrameNumber() << std::endl;
            osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();
            switch (sm) {
            /*
	     * switches backwards through weapon list when
	     * the mouse wheel is scrolled down
	     */
            case osgGA::GUIEventAdapter::SCROLL_DOWN:
	    //Debug message
	      OSG_ALWAYS << "SCROLL DOWN" << std::endl;
		_switch->setValue(_curWeapon, false);
                _curWeapon--;
                /*
		 * if the first weapon is selected and another
		 * scroll backwards is triggered, the last weapon
		 * gets selected
		 */
		if (_curWeapon < 0)
                    _curWeapon = _switch->getNumChildren() - 1;
                _curWeapon = _curWeapon % _switch->getNumChildren();
                _switch->setValue(_curWeapon, true);
                return true;
	    /*
	     * switches forward through weapon list when
	     * the mouse wheel is scrolled up
	     */
	    case osgGA::GUIEventAdapter::SCROLL_UP:
	      //Debug message
                OSG_ALWAYS << "SCROLL UP" << std::endl;
                _switch->setValue(_curWeapon, false);
                _curWeapon++;
                _curWeapon = _curWeapon % _switch->getNumChildren();
                _switch->setValue(_curWeapon, true);
                return true;
            default:
                return false;
            }
        }
        default:
            return false;
        }
    }

}
