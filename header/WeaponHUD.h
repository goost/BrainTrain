#pragma once
#include <osg/Camera>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgGA/GUIEventHandler>
#include <osg/Switch>

using namespace osg;
namespace brtr {
  
     /**
    *  @brief       WeaponHUD class, provides the functions to add
    *	a HUD camera to the scene.
    *  @details	    Use the mouse wheel to shift between weapons 
    * 		    after picking up a second one
    *  @author      Jonathan Spielvogel
    *  @version     1.0
    *  @date        2014
    *  @pre         create a root node and attach the scene to it,
    * 		    then add the HUD to root
    *  @copyright   GNU Public License.
    */
class WeaponHUD : public Camera {
    /**
    *  @brief       EventHandler for WeaponSwitching
    *  @details     can only be obtained trough WeaponHUD::getWeaponHandler()
    *  @author      Jonathan Spielvogel
    *  @version     1.0
    *  @date        2014   
    *  @copyright   GNU Public License.
    */
    class WeaponSwitchHandler : public osgGA::GUIEventHandler {
    public:
	/**
	 * @brief initializes a switch handler to switch through weapons
	 * @param switchNode pointer to Switch
	 * @return WeaponSwitchHandler
	 */
	WeaponSwitchHandler(Switch* switchNode);
	/**
	 * @brief When a mouse event is triggered, this function is called to switch between weapons
	 * @param ea GuiEventAdapter
	 * @param aa GuiActionAdapter
	 * @return true, if the event was handled, otherwise false
	 */
        virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    protected:

        ~WeaponSwitchHandler() {}
    private:
        ref_ptr<Switch> _switch;
        int _curWeapon;
        unsigned int _frameNumber;

    };

public:
    WeaponHUD();
    WeaponHUD(const WeaponHUD&,const CopyOp& copyop=CopyOp::SHALLOW_COPY);
    ref_ptr<WeaponSwitchHandler> getWeaponHandler();
    /**
     * @brief a portal gun is added to the weapon switch
     */
    void addPortalGun();
    ~WeaponHUD();
protected:
private:
    /**
     * @brief creates a weapon hud with the default weapon crowbar
     */
	void createWeaponHUD();
    ref_ptr<Switch> _switcher;
    ref_ptr<WeaponSwitchHandler> _handler;
};
}
