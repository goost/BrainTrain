#include "../header/ControlRoom.h"
#include "../header/UtilFunctions.h"
#include "../header/CelShading.h"

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/ValueObject>

using namespace osg;

namespace brtr{

    ControlRoom::ControlRoom(double roomSize, int lod, brtr::ToonTexSwitcherCallback& toonCallback, brtr::ProgramSwitcherCallback& programCallback) {
        ref_ptr<Group> roomRoot = createRoomSurrounding(roomSize, lod);
        ref_ptr<Group> chessFigureRoot = createChessFigures(toonCallback,programCallback);

        ref_ptr<brtr::CelShading> celShader = new brtr::CelShading(false);
        ref_ptr<LightSource> roomLight = brtr::createLight(Vec3(0, 0, roomSize / 2.0 /2.0 +2), 0);

        celShader->addChild(roomLight);
        celShader->addChild(roomRoot);
        celShader->addChild(chessFigureRoot);
        addChild(celShader);
    }

    ref_ptr<Group> ControlRoom::createRoomSurrounding(double roomSize, int lod) {
        ref_ptr<Geode> floorGeode = new Geode;
        floorGeode->addDrawable(brtr::createRectangle(roomSize, roomSize, lod, lod));
        ref_ptr<MatrixTransform> floor = new MatrixTransform;
        floor->setMatrix(Matrix::translate(-roomSize / 2, -roomSize / 2, 0));
        floor->addChild(floorGeode);

        ref_ptr<Geode> ceilingGeode = new Geode;
        ceilingGeode->addDrawable(brtr::createRectangle(roomSize, roomSize, lod, lod));
        ref_ptr<MatrixTransform> ceiling = new MatrixTransform;
        ceiling->setMatrix(Matrix::translate(-roomSize / 2, -roomSize / 2, -roomSize / 2)
            * Matrix::rotate(DegreesToRadians(180.0), X_AXIS));
        ceiling->addChild(ceilingGeode);

        ref_ptr<Geode> firstWallGeode = new Geode;
        firstWallGeode->addDrawable(brtr::createRectangle(roomSize / 2, roomSize, lod, lod));
        ref_ptr<MatrixTransform> firstWall = new MatrixTransform;
        firstWall->setMatrix(
            Matrix::translate(-roomSize / 2, -roomSize / 2, 0)
            *Matrix::rotate(DegreesToRadians(90.0), Y_AXIS)
            *Matrix::translate(-roomSize / 2, 0, 0)
            );
        firstWall->addChild(firstWallGeode);

        ref_ptr<Geode> secondWallGeode = new Geode;
        secondWallGeode->addDrawable(brtr::createRectangle(roomSize / 2, roomSize, lod, lod));
        ref_ptr<MatrixTransform> secondWall = new MatrixTransform;
        secondWall->setMatrix(
            Matrix::translate(-roomSize / 2, -roomSize / 2, 0)
            *Matrix::rotate(DegreesToRadians(-90.0), Y_AXIS)
            *Matrix::translate(roomSize / 2, 0, roomSize / 2)
            );
        secondWall->addChild(secondWallGeode);

        ref_ptr<Geode> thirdWallGeode = new Geode;
        thirdWallGeode->addDrawable(brtr::createRectangle(roomSize, roomSize / 2, lod, lod));
        ref_ptr<MatrixTransform> thirdWall = new MatrixTransform;
        thirdWall->setMatrix(
            Matrix::translate(-roomSize / 2, -roomSize / 2, 0)
            *Matrix::rotate(DegreesToRadians(-90.0), X_AXIS)
            *Matrix::translate(0, -roomSize / 2, 0)
            );
        thirdWall->addChild(thirdWallGeode);

        ref_ptr<Geode> fourthWallGeode = new Geode;
        fourthWallGeode->addDrawable(brtr::createRectangle(roomSize, roomSize / 2, lod, lod));
        ref_ptr<MatrixTransform> fourthWall = new MatrixTransform;
        fourthWall->setMatrix(
            Matrix::translate(-roomSize / 2, -roomSize / 2, 0)
            *Matrix::rotate(DegreesToRadians(90.0), X_AXIS)
            *Matrix::translate(0, roomSize / 2, roomSize / 2)
            );
        fourthWall->addChild(thirdWallGeode);

        ref_ptr<Group> roomRoot = new Group;
        roomRoot->addChild(floor);
        roomRoot->addChild(ceiling);
        roomRoot->addChild(firstWall);
        roomRoot->addChild(secondWall);
        roomRoot->addChild(thirdWall);
        roomRoot->addChild(fourthWall);
        roomRoot->setNodeMask(brtr::collisionMask);
        //this is the fakewall
        thirdWall->setNodeMask(~brtr::interactionAndCollisionMask);

        //material for the whole room
        ref_ptr<Material> roomMaterial = createMaterial(Vec4(0.3, 0.3, 0.3, 1.0), Vec4(0.4, 0.4, 0.4, 1.0), Vec4(0.9, 0.9, 0.9, 1.0), 42);     
        roomRoot->getOrCreateStateSet()->setAttributeAndModes(roomMaterial, StateAttribute::ON);
        //shader should know that there is no texture
        roomRoot->getOrCreateStateSet()->addUniform(new Uniform("tex", false), StateAttribute::ON | StateAttribute::OVERRIDE);

        return roomRoot;
    }

    ref_ptr<Group> ControlRoom::createChessFigures(brtr::ToonTexSwitcherCallback& toonCallback, brtr::ProgramSwitcherCallback& programCallback)
{
        ref_ptr<Geometry> chessFigure1Geometry = brtr::createChessFigure();
        ref_ptr<Geode> chessFigure1Source = new Geode;
        chessFigure1Source->addDrawable(chessFigure1Geometry);

        ref_ptr<MatrixTransform> chessFigure1 = new MatrixTransform;
        chessFigure1->setMatrix(
            Matrix::translate(-5, 0, 0)
            );
        chessFigure1->addChild(chessFigure1Source);
        chessFigure1->getOrCreateStateSet()->addUniform(new Uniform("xAnimation", true), StateAttribute::ON | StateAttribute::OVERRIDE);

        ref_ptr<Geode> chessFigure2Source = new Geode;
        chessFigure2Source->addDrawable(brtr::createChessFigure());
        ref_ptr<MatrixTransform> chessFigure2 = new MatrixTransform;
        chessFigure2->setMatrix(
            Matrix::translate(0, 5, 0)
            );
        chessFigure2->addChild(chessFigure2Source);
        chessFigure2->getOrCreateStateSet()->addUniform(new Uniform("zAnimation", true), StateAttribute::ON | StateAttribute::OVERRIDE);
        chessFigure2->getOrCreateStateSet()->setAttributeAndModes(createMaterial(Vec4(0.4583, 0.35, 1, 1), Vec4(0.4583, 0.35, 1, 1)), StateAttribute::ON | StateAttribute::OVERRIDE);

        ref_ptr<Geometry> chessFigure3Geometry = brtr::createChessFigure();
        ref_ptr<Geode> chessFigure3Source = new Geode;
        chessFigure3Source->addDrawable(chessFigure3Geometry);
        ref_ptr<MatrixTransform> chessFigure3 = new MatrixTransform;
        chessFigure3->setMatrix(
            Matrix::translate(5, 0, 0)
            );
        chessFigure3->addChild(chessFigure3Source);
        chessFigure3->getOrCreateStateSet()->addUniform(new Uniform("yAnimation", true), StateAttribute::ON | StateAttribute::OVERRIDE);
        chessFigure3->getOrCreateStateSet()->setAttributeAndModes(createMaterial(Vec4(0, 0.63, 0.084, 1), Vec4(0, 0.63, 0.084, 1)), StateAttribute::ON | StateAttribute::OVERRIDE);

        
        chessFigure1Geometry->getOrCreateUserDataContainer()->addUserObject(&toonCallback);
        chessFigure3Geometry->getOrCreateUserDataContainer()->addUserObject(&programCallback);
        ref_ptr<Group> chessFigureRoot = new Group;
        chessFigureRoot->addChild(chessFigure1);
        chessFigureRoot->addChild(chessFigure2);
        chessFigureRoot->addChild(chessFigure3);

        return chessFigureRoot;
    }

    ref_ptr<Material> ControlRoom::createMaterial(Vec4 diffuse, Vec4 ambient, Vec4 specular, double shininess) {
        ref_ptr<Material> mat = new Material;
        mat->setAmbient(Material::FRONT_AND_BACK, ambient);
        mat->setDiffuse(Material::FRONT_AND_BACK, diffuse);
        mat->setSpecular(Material::FRONT_AND_BACK, specular);
        mat->setShininess(Material::FRONT_AND_BACK, shininess);
        return mat;
    }

}
