#include "../header/ModifyMaterialVisitor.h"
#include <osg/Material>
namespace brtr{

    ModifyMaterialVisitor::ModifyMaterialVisitor() :
        _ambientFlag(false),
        _diffuseFlag(false), 
        _specularFlag(false), 
        _shininessFlag(false) {
        setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);

    }

    void ModifyMaterialVisitor::apply(osg::Geode& geode) {
        for (unsigned int i = 0; i < geode.getNumDrawables(); ++i) {
            osg::Drawable* drawable = geode.getDrawable(i);
            osg::Material* materialPtr = dynamic_cast<osg::Material*>(drawable->getOrCreateStateSet()->getAttribute(osg::StateAttribute::Type::MATERIAL));
            if (materialPtr) {
                osg::Material& material = *materialPtr;
                if (_ambientFlag)
                    material.setAmbient(osg::Material::FRONT_AND_BACK, _ambient);
                if (_specularFlag)
                    material.setSpecular(osg::Material::FRONT_AND_BACK, _specular);
                if (_shininessFlag)
                    material.setShininess(osg::Material::FRONT_AND_BACK, _shininess);
                if (_diffuseFlag)
                    material.setDiffuse(osg::Material::FRONT_AND_BACK, _diffuse);
            }
        }

    }

    osg::Vec4 ModifyMaterialVisitor::getDiffuse() const {
        return _diffuse;
    }

    ModifyMaterialVisitor& ModifyMaterialVisitor::setDiffuse(osg::Vec4 val) {
        _diffuseFlag = true;
        _diffuse = val; 
        return *this;
    }

    osg::Vec4 ModifyMaterialVisitor::getSpecular() const {
        return _specular;
    }

    ModifyMaterialVisitor& ModifyMaterialVisitor::setSpecular(osg::Vec4 val) {
        _specular = val; 
        _specularFlag = true;
        return *this;
    }

    osg::Vec4 ModifyMaterialVisitor::getAmbient() const {
        return _ambient;
    }

    ModifyMaterialVisitor& ModifyMaterialVisitor::setAmbient(osg::Vec4 val) {
        _ambient = val;
        _ambientFlag = true;
        return *this;
    }

    double ModifyMaterialVisitor::getShininess() const {
        return _shininess;
    }

    ModifyMaterialVisitor& ModifyMaterialVisitor::setShininess(double val) {
        _shininess = val;
        _shininessFlag = true;
        return *this;
    }

}


