#pragma once
#include <osgFX/Export>
#include <osgFX/Effect>
#include <osg/Material>
#include <osg/LineWidth>
namespace brtr {
    /**
    *  @brief       CelSading Effect, every child of this node will get the effect
    *  @details      This effect implements a technique called 'Cel-Shading' to produce a cartoon-style (non photorealistic) rendering.<br/>
    *                Two passes are required:<br/>
    *                the first one draws solid surfaces, the second one draws the outlines.
    *  @author      Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @pre         In Texture Layer 1 (stateset) must be the ToonTexture set
    *  @copyright   GNU Public License.
    */
    class CelShading : public osgFX::Effect {
    public:
        /**
         * @brief  Constructor
         *
         * @param  secondPass if false, no outlines are being drawn
         * @param  vertSource one can set explicitly the vertex shader
         */
        CelShading(bool secondPass = true, std::string vertSource = "celShader.vert");
        CelShading(const CelShading& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

        META_Effect(
            null,
            CelShading,

            "CelShading",

            "This effect implements a technique called 'Cel-Shading' to produce a "
            "cartoon-style (non photorealistic) rendering. Two passes are required: "
            "the first one draws solid surfaces, the second one draws the outlines. "
            "Vertices Shader, Toon Texture pass can be customize upon creating."
             ,
            "Marco Jez; OGLSL port by Mike Weiblen, adaptions by Gleb Ostrowski ");

    protected:
        virtual ~CelShading() {}

        bool define_techniques();

    private:
        osg::ref_ptr<osg::Material> _material;
        osg::ref_ptr<osg::LineWidth> _lineWidth;
        bool _secondPass;
        std::string _vertSource;
    };
}

