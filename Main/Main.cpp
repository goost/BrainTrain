#include <osgViewer/Viewer>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/BlendFunc>
#include <osg/ValueObject>
#include <osgUtil/Optimizer>
#include <string>
#include <sstream>
#include <iostream>

#include "../header/UtilFunctions.h"
#include "../header/WeaponHUD.h"
#include "../header/FPSCameraManipulator.h"
#include "../header/GeometryPlacerVisitor.h"
#include "../header/DrunkenInteractionCallback.h"
#include "../header/ModifyMaterialVisitor.h"
#include "../header/Bench.h"
#include "../header/CelShading.h"
#include "../header/BaseInteractionCallback.h"
#include "../header/KeyHandler.h"
#include "../header/AnimationCreater.h"
#include "../header/AddPortalGunInteractionCallback.h"
#include "../header/AddInteractionCallbackToDrawableVisitor.h"
#include "../header/ControlRoom.h"
#include "../header/TrainSwitcherCallback.h"

/**
* @file
* @brief Main file which loads and set ups everything.
*/

using namespace osg;

int main(void){
    //some vars
    osg::setNotifyLevel(FATAL);
    Vec3f fogColor(.3219, 0.37, 0.3564);
    unsigned int width, height;
    unsigned int oldWidth, oldHeight;
    int screen = 0;  //for easy multimonitor switching while debugging
    std::string inputLine = "";
    int choose = 0;
    std::vector<ref_ptr<Texture2D>> toonTexs;
    toonTexs.push_back(brtr::createToonTex("2d_toons_brown.png"));
    toonTexs.push_back(brtr::createToonTex("2d_toons_blue.png"));
    toonTexs.push_back(brtr::createToonTex("2d_toons_red.png"));
    toonTexs.push_back(brtr::createToonTex("2d_toons_violet.png"));
    toonTexs.push_back(brtr::createToonTex("2d_toons_yellow.png"));
    ref_ptr<GraphicsContext::WindowingSystemInterface> wsi = GraphicsContext::getWindowingSystemInterface();

    OSG_ALWAYS << "Please choose the desired Display Resolution:" << std::endl;
    OSG_ALWAYS << "\t(1): Full HD 1920x1080 (only with a decent Graphic Card!)" << std::endl;
    OSG_ALWAYS << "\t(2): HD+ 1366x768 (should work with most Cards)" << std::endl;
    OSG_ALWAYS << "\t(3): HD 1280x720 (choose this for best performance, but worst quality)" <<std::endl;
    OSG_ALWAYS << "\t(4): Use Screen Resolution" <<std::endl; 
    OSG_ALWAYS << "\t(5): quit the program without experiencing the forsaken station =(" << std::endl;
    std::getline(std::cin, inputLine);
    std::stringstream(inputLine) >> choose;
    while (!(choose == 1 || choose == 2 || choose == 3 || choose == 4 || choose == 5)) {
        OSG_ALWAYS << "Only (1), (2), (3), (4) or (5) are valid options!" <<std::endl;
        OSG_ALWAYS << choose << std::endl;
        std::getline(std::cin, inputLine);
        std::stringstream(inputLine) >> choose;
    } 
    switch (choose) {
    case 1:
        width = 1920;
        height = 1080;
        break;
    case 2:
        width = 1366;
        height = 768;
        break;
    case 3:
        width = 1280;
        height = 720;
        break;
    case 4:
        wsi->getScreenResolution(GraphicsContext::ScreenIdentifier(screen), width, height);
        break;
    case 5:
        return EXIT_SUCCESS;
    }
      
    
    OSG_ALWAYS << "Setting some options which should help with performance (but probably do not)" << std::endl;
    //this viewer will display our graph
    osgViewer::Viewer viewer;
    //Faster Intersection, hell yeah!
    osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::Options::BUILD_KDTREES);
    //Get/Set Screen Resolution 
    wsi->getScreenResolution(GraphicsContext::ScreenIdentifier(screen), oldWidth, oldHeight);
    OSG_ALWAYS << "This DisplaySettings will be used:" << std::endl;
    OSG_ALWAYS << width << "x" << height << std::endl;
    wsi->setScreenResolution(GraphicsContext::ScreenIdentifier(screen), width, height);
    //to make sure, we are using the right resolution, even if the set fails
    wsi->getScreenResolution(GraphicsContext::ScreenIdentifier(screen), width, height);

    //Read IVEs, set Masks
    OSG_ALWAYS << "Reading IVE's, making cookies." << std::endl;
    ref_ptr<Node> trainStation = osgDB::readNodeFile("../BlenderFiles/exports/BrainTrain6_1p25E_Lights.ive");
    trainStation->setNodeMask(brtr::collisionMask);
    ref_ptr<Node> trainStationHitbox = osgDB::readNodeFile("../BlenderFiles/exports/BrainTrain6_1p25E_Lights_Hitbox.osgt");
    trainStationHitbox->setNodeMask(brtr::collisionMask);
    ref_ptr<Node> bottleEmitter = osgDB::readNodeFile("../BlenderFiles/exports/BrainTrain_BottleParticles.osgt");
    bottleEmitter->setNodeMask(~brtr::interactionAndCollisionMask);
    ref_ptr<Node> drinkablebottleEmitter = osgDB::readNodeFile("../BlenderFiles/exports/BrainTrain_BottleParticlesDrinkable.osgt");
    drinkablebottleEmitter->setNodeMask(brtr::interactionMask);
    ref_ptr<Node> trainModel = osgDB::readNodeFile("../BlenderFiles/exports/Train.ive.0,0,-48.rot");
    ref_ptr<Node> portalGunTrain = osgDB::readNodeFile("../BlenderFiles/exports/Portalgun_Big.ive.0,0,-48.rot");
    //Position "Trains" 
    ref_ptr<PositionAttitudeTransform> trainPosition = new PositionAttitudeTransform;
    trainPosition->setNodeMask(brtr::collisionMask);
    trainPosition->setPosition(Vec3(0, 0, -20));
    trainPosition->addChild(trainModel);
    trainPosition->setDataVariance(Object::DYNAMIC);
    ref_ptr<PositionAttitudeTransform> portalGuntrainPosition = new PositionAttitudeTransform;
    portalGuntrainPosition->setNodeMask(~brtr::interactionAndCollisionMask);
    portalGuntrainPosition->setPosition(Vec3(0, 0, -20));
    portalGuntrainPosition->addChild(portalGunTrain);
    portalGuntrainPosition->setDataVariance(Object::DYNAMIC);

    //Animation for Train   
    ref_ptr<AnimationPath> trainPath = AnimationCreator().createAnimationPath(0.1f);
    osg::ref_ptr<osg::AnimationPathCallback> trainAniCallback = new  osg::AnimationPathCallback;
    trainAniCallback->setAnimationPath(trainPath);
    trainPosition->setUpdateCallback(trainAniCallback);
    portalGuntrainPosition->setUpdateCallback(trainAniCallback);

    //Switch for trains
    ref_ptr<Switch> train = new Switch;
    train->addChild(trainPosition, true);
    train->addChild(portalGuntrainPosition, false);
    train->addUpdateCallback(new brtr::TrainSwitcherCallback);

    ref_ptr<Node> ponyFlagSourceNode = osgDB::readNodeFile("../BlenderFiles/exports/BrainTrain_Flag.ive");
    ref_ptr<brtr::CelShading> ponyFlag = new brtr::CelShading(false);
    ponyFlag->addChild(ponyFlagSourceNode);
    //let the flag move!
    ponyFlag->getOrCreateStateSet()->addUniform(new Uniform("zAnimation",true), StateAttribute::ON | StateAttribute::OVERRIDE);
    ref_ptr<Node> portalGunSource = osgDB::readNodeFile("../BlenderFiles/exports/Portalgun.ive");
    ref_ptr<PositionAttitudeTransform> portalGunPlacer = new PositionAttitudeTransform;
    portalGunPlacer->addChild(portalGunSource);
    portalGunPlacer->setPosition(Vec3(-76.54, 5.28, 3.82));
    //vase on top of the ticketcorner
    ref_ptr<PositionAttitudeTransform> vase = brtr::createVaseWithFlower();
    vase->setPosition(Vec3(-27.9, 17.4, 9.7));
    
    OSG_ALWAYS << "Placing bottles (and making some them drinkable)" << std::endl;
    OSG_ALWAYS << "Do not drink and drive" << std::endl;
    OSG_ALWAYS << "Actually, this drink is bad, so do not drink it at all." << std::endl;
    //Create and make alpha Bottle

    ref_ptr<Geometry> bottle = brtr::createRealBottle();
    
    //Drinkable bottles
    ref_ptr<Geometry> drinkablebottle = brtr::createRealBottle();
   
    //drunk one bottle, disable all! It's a Feature, not a bug ;)
    ref_ptr<Switch> drinableBottleSwitch = new Switch;
    drinableBottleSwitch->addChild(drinkablebottleEmitter, true);
    drinableBottleSwitch->setNodeMask(brtr::interactionMask);

    //portalGunPicker
    ref_ptr<Switch> portalGunSwitch = new Switch;
    portalGunSwitch->addChild(portalGunPlacer, true);
    portalGunSwitch->setNodeMask(brtr::interactionMask);

    //place bottle
    brtr::GeometryPlacerVisitor bottlePlacer(bottle);
    bottleEmitter->accept(bottlePlacer);
    //place drinkablebottle
    brtr::GeometryPlacerVisitor drinkablebottlePlacer(drinkablebottle);
    drinkablebottleEmitter->accept(drinkablebottlePlacer);

    //Placing Benches
    OSG_ALWAYS << "Placing (uncomfortable) benches." << std::endl;
    OSG_ALWAYS << "Lying, they are great!" << std::endl;
    OSG_ALWAYS << "Na, that was a lie." << std::endl;

	ref_ptr<PositionAttitudeTransform> leftBench = new brtr::Bench(Vec3(49.5, -2.3, -0.6), 8);
    leftBench->setAttitude(Quat(DegreesToRadians(167.0), Z_AXIS));
    leftBench->setNodeMask(brtr::collisionMask);

	ref_ptr<PositionAttitudeTransform> rightBench = new brtr::Bench(Vec3(-50, 14.3, -0.6), 14);
    rightBench->setAttitude(Quat(DegreesToRadians(192.7), Z_AXIS));
    rightBench->setNodeMask(brtr::collisionMask);

    //a group for the whole station
    //needed for the createPipeLine Function
    ref_ptr<Group> rootForToon = new Group;
    rootForToon->addChild(trainStation);
    rootForToon->addChild(leftBench);
    rootForToon->addChild(rightBench);
    rootForToon->addChild(train);
    rootForToon->addChild(bottleEmitter);
    rootForToon->addChild(drinableBottleSwitch);
    rootForToon->addChild(portalGunSwitch);
    rootForToon->addChild(vase);
    //just to make sure
    rootForToon->setDataVariance(Object::STATIC);


    OSG_ALWAYS << "Creating Lights. Nobody wants a creepy, dark station." << std::endl;
    OSG_ALWAYS << "Except for the creators." << std::endl;
    ref_ptr<LightSource> light1 = brtr::createLight(Vec3(-76.88403, -8.27441, 20.63965), 1);
    ref_ptr<LightSource> light2 = brtr::createLight(Vec3(-26.8972, 1.97552, 20.02043), 2);
    ref_ptr<LightSource> light3 = brtr::createLight(Vec3(24.33239, 2.49185, 21.58063), 3);
    ref_ptr<LightSource> light4 = brtr::createLight(Vec3(74.73347, -8.83866, 21.33362), 4);
    ref_ptr<LightSource> staircaseLight = brtr::createLight(Vec3(0, 110, 38), 5);
    staircaseLight->getLight()->setQuadraticAttenuation(0.005);


    rootForToon->addChild(light1);
    rootForToon->addChild(light2);
    rootForToon->addChild(light3);
    rootForToon->addChild(light4); 
    rootForToon->addChild(staircaseLight);


    OSG_ALWAYS << "Creating RenderingPipeline. ToonyLoony!" << std::endl;
    brtr::RenderingPipeline pipe;
    brtr::createRenderingPipeline(width, height, *rootForToon, viewer, pipe,fogColor);


    //HUD Cams
    ref_ptr<brtr::WeaponHUD> weaponHUD = new brtr::WeaponHUD;
    ref_ptr<Camera> textHUD = brtr::createHUDCamera(0, width, 0, height);
    textHUD->addChild(brtr::createCrosshair(width, height));
    textHUD->getOrCreateStateSet()->setTextureMode(1, GL_TEXTURE_2D, StateAttribute::OFF);
    //making bottles drinkable
    drinkablebottle->getOrCreateUserDataContainer()->addUserObject(new brtr::DrunkenInteractionCallback(viewer.getCamera(), textHUD, drinableBottleSwitch, width, height));
    ref_ptr<brtr::AddPortalGunInteractionCallback> portalGunCallback = new brtr::AddPortalGunInteractionCallback(weaponHUD, textHUD, portalGunSwitch, width, height);
    brtr::AddInteractionCallbackToDrawableVisitor portalGunCallbackVisitor(portalGunCallback);
    portalGunSource->accept(portalGunCallbackVisitor);

    OSG_ALWAYS << "Making coffee." << std::endl;
    brtr::ModifyMaterialVisitor mmv;
    mmv.setAmbient(Vec4(0.4, 0.4, 0.4, 4)).setDiffuse(Vec4(0.7,0.7,0.7,1.0)).setShininess(42*3); //.setShininess(42 * 3).setSpecular(Vec4(0.7, 0.7, 0.7, 1));
    //weaponHUD->accept(imv);
    trainStation->accept(mmv);
    trainPosition->accept(mmv);
    portalGunSource->accept(mmv);

    //the root node, which holds the cams (pass and HUDs) as siblings
    ref_ptr<Group> sceneData = new Group;
    //add elements to sceneData
    OSG_ALWAYS << "Adding elements to scene root." << std::endl;
    OSG_ALWAYS << "I am soooo excited, we are nearly done!." << std::endl;
    sceneData->addChild(pipe.pass_0_color);
    sceneData->addChild(pipe.pass_0_depth);
    sceneData->addChild(pipe.pass_PostProcess);  
    sceneData->addChild(trainStationHitbox);
    sceneData->addChild(weaponHUD);
    sceneData->addChild(textHUD);
    //safety
    //sceneData->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF | StateAttribute::OVERRIDE);

    //Set toonTex
    sceneData->getOrCreateStateSet()->setTextureAttributeAndModes(1, toonTexs[0], osg::StateAttribute::ON);

    //Control Room
    ref_ptr<brtr::ToonTexSwitcherCallback> toonCallback = new brtr::ToonTexSwitcherCallback(sceneData, textHUD, width, height, toonTexs);
    ref_ptr<brtr::ProgramSwitcherCallback> programCallback = new brtr::ProgramSwitcherCallback(pipe.pass_PostProcess, textHUD, width, height, pipe.programs);
    ref_ptr<brtr::ControlRoom> controlRoom = new brtr::ControlRoom(40, 50, *toonCallback, *programCallback);
    controlRoom->setPosition(Vec3(0, 170.3, 23.2));

    //Adding "special-treatment nodes" (mainly no outlines) to first pass
    pipe.pass_0_color->addChild(ponyFlag);
    pipe.pass_0_depth->addChild(ponyFlag);
    pipe.pass_0_color->addChild(controlRoom);
    pipe.pass_0_depth->addChild(controlRoom);

    viewer.setSceneData(sceneData);
    osgUtil::Optimizer optimizer;
    optimizer.optimize(sceneData,osgUtil::Optimizer::STATIC_OBJECT_DETECTION);
    

    //Manipulator and KeyHandler
    OSG_ALWAYS << "Adding Manipulator and KeyHandler. What could possible go wrong?." << std::endl;
    viewer.setCameraManipulator(new brtr::FPSCameraManipulator(0.25, 7, rootForToon));
    osg::ref_ptr<brtr::KeyHandler> keyHandler = new brtr::KeyHandler(sceneData, pipe.pass_PostProcess, pipe.programs);
    viewer.addEventHandler(weaponHUD->getWeaponHandler());
    viewer.addEventHandler(keyHandler);

    OSG_ALWAYS << "Potato." << std::endl;
    OSG_ALWAYS << "Finished! Press Enter to start the fun!" <<std::endl;
 
    getchar();
    OSG_ALWAYS << "The cake is a lie." << std::endl;
    viewer.setUpViewOnSingleScreen(screen);
    osgViewer::GraphicsWindow* window = dynamic_cast<osgViewer::GraphicsWindow*>(viewer.getCamera()->getGraphicsContext());
    if (window) {
        window->useCursor(false);
    }
    else {
        OSG_ALWAYS << "WARNING: COULD NOT HIDE MOUSE CURSOR" << std::endl;
    }

    while (!viewer.done())
        viewer.frame();
 
    wsi->setScreenResolution(GraphicsContext::ScreenIdentifier(screen), oldWidth, oldHeight);
    return EXIT_SUCCESS;
}
