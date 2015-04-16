#pragma once
#include <osg/NodeCallback>
#include <osgViewer/Viewer>

namespace brtr {
    /**
    *  @brief       Visitor for altering the material attributes, mainly used for objects craeted with blender
    *  @details	    before applying one must set the desired changes (setDiffuse, setAmbient, setSpecular oder setShininess)
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @copyright   GNU Public License.
    */
    class ModifyMaterialVisitor : public osg::NodeVisitor {
    public:

        ModifyMaterialVisitor();
        void apply(osg::Geode& geode);

        osg::Vec4 getDiffuse() const;
        ModifyMaterialVisitor& setDiffuse(osg::Vec4 val);
        osg::Vec4 getSpecular() const;
        ModifyMaterialVisitor& setSpecular(osg::Vec4 val);
        osg::Vec4 getAmbient() const;
        ModifyMaterialVisitor& setAmbient(osg::Vec4 val);
        double getShininess() const;
        ModifyMaterialVisitor& setShininess(double val);
    private:
        osg::Vec4 _diffuse;
        osg::Vec4 _specular;
        osg::Vec4 _ambient;
        double _shininess;
        bool _ambientFlag;
        bool _specularFlag;
        bool _shininessFlag;
        bool _diffuseFlag;
    };
}



