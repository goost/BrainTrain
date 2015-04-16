#include "CelShading.h"

#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/Program>
#include <osg/Shader>
#include <osg/PolygonOffset>
#include "osg/TexEnv"
#include "osg/PolygonMode"
#include "osg/CullFace"

namespace brtr{
    /**
    *  @brief       The Technique for the cel-shading effect
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @copyright   GNU Public License.
    */
    class CelShadingTechnique : public osgFX::Technique {
    public:
        CelShadingTechnique(osg::Material* material, osg::LineWidth *lineWidth, bool secondPass, std::string vertSource)
            : Technique(), 
            _material(material),
            _lineWidth(lineWidth),
            _secondPass(secondPass),
            _vertSource(vertSource){}

    protected:

        void define_passes() {
            // implement pass #1 (solid surfaces)
                {
                    osg::ref_ptr<osg::Shader> toonFrag = osgDB::readShaderFile("../Shader/celShader.frag");
                    osg::ref_ptr<osg::Shader> toonVert = osgDB::readShaderFile("../Shader/" +  _vertSource);
                    osg::ref_ptr<osg::Program> celShadingProgram = new osg::Program;
                    celShadingProgram->addShader(toonFrag);
                    celShadingProgram->addShader(toonVert);

                    osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;

                    ss->addUniform(new osg::Uniform("toonTex", 1));
                    ss->setAttributeAndModes(celShadingProgram, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

                    ss->addUniform(new osg::Uniform("tex", true));
                    ss->addUniform(new osg::Uniform("zAnimation", false));
                    ss->addUniform(new osg::Uniform("xAnimation", false));
                    ss->addUniform(new osg::Uniform("yAnimation", false));
                    addPass(ss);
                }

            // implement pass #2 (outlines) copy/paste from osgFX::Cartoon 
            if(_secondPass){
                osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;
                osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
                polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
                ss->setAttributeAndModes(polymode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

                osg::ref_ptr<osg::CullFace> cf = new osg::CullFace;
                cf->setMode(osg::CullFace::FRONT);
                ss->setAttributeAndModes(cf.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

                ss->setAttributeAndModes(_lineWidth.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

                _material->setColorMode(osg::Material::OFF);
                _material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));
                _material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));
                _material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));

                // set by outline colour so no need to set here.
                _material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0, 0, 1));

                ss->setAttributeAndModes(_material.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

                ss->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
                ss->setTextureMode(0, GL_TEXTURE_1D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
                ss->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
                ss->setTextureMode(1, GL_TEXTURE_1D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
                ss->setTextureMode(1, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

                 addPass(ss.get());
            }
        }

    private:
        osg::ref_ptr<osg::Material> _material;
        osg::ref_ptr<osg::LineWidth> _lineWidth;
        std::string _toonTex;
        bool _secondPass;
        std::string _vertSource;
    };

    ///////////////////////////////////////////////////////////////////////////

    CelShading::CelShading(bool secondPass, std::string vertSource)
        : Effect(),
        _material(new osg::Material),
        _lineWidth(new osg::LineWidth(3.0f)),
        _secondPass(secondPass),
        _vertSource(vertSource){}

    CelShading::CelShading(const CelShading& copy, const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/): 
        osgFX::Effect(copy, copyop),
        _material(static_cast<osg::Material*>(copyop(copy._material.get()))),
        _lineWidth(static_cast<osg::LineWidth *>(copyop(copy._lineWidth.get()))) {}


    bool CelShading::define_techniques() {
        addTechnique(new CelShadingTechnique(_material, _lineWidth, _secondPass, _vertSource));
        return true;
    }
}