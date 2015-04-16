#include "../header/UtilFunctions.h"
#include "../header/CelShading.h"
#include <osgText/Text>
#include <osg/PolygonMode>
#include <osg/LightSource>
#include <osg/BlendFunc>
#include <osg/ComputeBoundsVisitor>
#include <osg/Point>
#include <osg/PointSprite>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/Particle>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/AccelOperator>

using namespace osg;

namespace brtr{
  
    ref_ptr<osg::Camera> createRTTCamera(osg::Camera::BufferComponent buffer, osg::Texture* tex, bool isAbsolute) {
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setClearColor(osg::Vec4());
        camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
        camera->setRenderOrder(osg::Camera::PRE_RENDER);
        if (tex) {
            tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
            tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
            camera->setViewport(0, 0, tex->getTextureWidth(), tex->getTextureHeight());
            camera->attach(buffer, tex);
        }

        if (isAbsolute) {
            camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
            camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0));
            camera->setViewMatrix(osg::Matrix::identity());
            camera->addChild(createScreenQuad(1.0f, 1.0f,1.0f));
        }
        return camera;
    }
    
    ref_ptr<osg::Geode> createScreenQuad(float width, float height, float scale) {
        osg::Geometry* geom = osg::createTexturedQuadGeometry(
            osg::Vec3(), osg::Vec3(width, 0.0f, 0.0f), osg::Vec3(0.0f, height, 0.0f),
            0.0f, 0.0f, width*scale, height*scale);
        osg::ref_ptr<osg::Geode> quad = new osg::Geode;
        quad->addDrawable(geom);

        int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
        quad->getOrCreateStateSet()->setAttribute(
            new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL), values);
        quad->getOrCreateStateSet()->setMode(GL_LIGHTING, values);
        return quad;
    }

    ref_ptr<Camera> createHUDCamera(double left, double right, double bottom, double top) {
        osg::ref_ptr<Camera> camera = new Camera;
        camera->setReferenceFrame(Transform::ABSOLUTE_RF);
        camera->setClearMask(GL_DEPTH_BUFFER_BIT);
        camera->setRenderOrder(Camera::POST_RENDER);
        camera->setAllowEventFocus(false);
        camera->setProjectionMatrix(Matrix::ortho2D(left, right, bottom, top));
        camera->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF);
        return camera;
    }

    ref_ptr<Geometry> createRectangle(double length, double width, int lsteps, int wsteps) {
        ref_ptr<Geometry> rect = new Geometry;
        ref_ptr<Vec3Array> vertices = new Vec3Array();
        ref_ptr<Vec3Array> normals = new Vec3Array();
        ref_ptr<DrawElementsUInt> indices = new DrawElementsUInt(GL_TRIANGLE_STRIP);

        double xstep = length / lsteps;
        double ystep = width / wsteps;
        // current vertex coordinates
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        //  current normal coordinates
        double nx = 0.0;
        double ny = 0.0;
        double nz = 1.0;

        // set vertices and normals
        for (int i = 0; i <= lsteps; i++) {
            y = 0.0;
            for (int j = 0; j <= wsteps; j++) {
                vertices->push_back(Vec3d(x, y, z));
                normals->push_back(Vec3d(nx, ny, nz));
                y += ystep;
            }
            x += xstep;
        }

        for (int i = 0; i < lsteps; i++) {

            for (int j = 0; j <= wsteps; j++) {
                indices->push_back(i*(wsteps + 1) + j);
                indices->push_back((i + 1)*(wsteps + 1) + j);
            }
            indices->push_back((i + 1)*(wsteps + 1) + wsteps);
            indices->push_back((i + 1)*(wsteps + 1));
        }

        rect->setVertexArray(vertices.get());
        rect->addPrimitiveSet(indices.get());
        rect->setNormalArray(normals.get());
        rect->setNormalBinding(Geometry::BIND_PER_VERTEX);
         
        return rect;
    }

	ref_ptr<Geometry> createRectangleWithTexcoords(double length, double width, int lsteps, int wsteps) {
		ref_ptr<Geometry> rect = createRectangle(length,  width,  lsteps,  wsteps);

		osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
		double xstep = length / lsteps;
		double ystep = width / wsteps;
		// current vertex coordinates
		double x = 0.0;
		double y = 0.0;
		double z = 0.0;
		//  current normal coordinates
		double nx = 0.0;
		double ny = 0.0;
		double nz = 1.0;
		double coordlen = length;
		double coordwid = width;
		if (width / length < 0.25) coordlen = 1;
		else coordlen = length;
		if (length / width < 0.25) coordwid = 1;
		else coordwid = width;
		// set vertices and normals
		for (int i = 0; i <= lsteps; i++) {
			y = 0.0;
			for (int j = 0; j <= wsteps; j++) {
				texcoords->push_back(Vec2(x / coordlen, y / coordwid));
				y += ystep;
			}
			x += xstep;
		}
		rect->setTexCoordArray(0, texcoords.get());
		return rect;
	}

    osg::ref_ptr<osg::Geometry> createBodyOfRotation(double height, int hsteps, int rsteps, const BodyOfRotationFunction& function) {
        ref_ptr<Geometry> rect = new Geometry;
        ref_ptr<Vec3Array> vertices = new Vec3Array();
        ref_ptr<Vec3Array> normals = new Vec3Array();
        ref_ptr<DrawElementsUInt> indices = new DrawElementsUInt(GL_TRIANGLE_STRIP);
        const BodyOfRotationFunction* curFunc = &function;

        double zstep = height / hsteps;
        double alphastep = DegreesToRadians(360.0 / rsteps);
        // current vertex coordinates
        double z = 0.0;
        double alpha = 2*PI;
        //normal coordinates are calculated on the fly

        // set vertices and normals
        for (int i = 0; i <= hsteps; i++) {
            alpha = 0;
            double curRadius = curFunc->func(z);
            for (int j = 0; j <= rsteps; j++) {
                vertices->push_back(Vec3d(
                    curRadius * cos(alpha), //X 
                    curRadius * sin(alpha),//Y 
                    z));                   //Z
                normals->push_back(Vec3d(
                    curRadius * cos(alpha), //X
                    curRadius * sin(alpha), //Y
                    -curRadius * curFunc->derivation(z))); //Z
                alpha -= alphastep;
            }
            z += zstep;
            if (curFunc->end < z - 1e-8)
                curFunc = curFunc->nextFunc;
        }

        //set strip connections
        for (int i = 0; i < hsteps; i++) {

            for (int j = 0; j <= rsteps; j++) {
                indices->push_back(i*(rsteps + 1) + j);
                indices->push_back((i + 1)*(rsteps + 1) + j);
            }
            indices->push_back((i + 1)*(rsteps + 1) + rsteps);
            indices->push_back((i + 1)*(rsteps + 1));
        }

        //normalize the normals
        for (auto cnt = 0; cnt < normals->size(); cnt++) {
            normals->at(cnt).normalize();
        }

        rect->setVertexArray(vertices);
        rect->addPrimitiveSet(indices);
        rect->setNormalArray(normals);
        //i know, deprecated, but osg 3.0.1
        rect->setNormalBinding(Geometry::BIND_PER_VERTEX);

        return rect;
    }

    osg::ref_ptr<osgText::Text> createText(const osg::Vec3& pos, const std::string& content, float size) {
        osg::ref_ptr<osgText::Font> g_font = osgText::readFontFile("../fonts/dirtydoz.ttf");
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        text->setDataVariance(osg::Object::DYNAMIC);
        text->setFont(g_font);
        text->setCharacterSize(size);
        text->setAxisAlignment(osgText::TextBase::XY_PLANE);
        text->setPosition(pos);
        text->setText(content);
        return text;
    }

    ref_ptr<LightSource> createLight(const Vec3 &pos, int lightNum, int point, double spotCutoff, double spotExponent) {
        ref_ptr<LightSource> light = new LightSource;
        light->getLight()->setPosition(Vec4(pos.x(), pos.y(), pos.z(), point));
        light->getLight()->setAmbient(Vec4(.2, .2, .2, 1));
        light->getLight()->setDiffuse(Vec4(.7, .7, .7, 1));
        light->getLight()->setSpecular(Vec4(0.7, 0.7, 0.7, 1));
        light->getLight()->setLightNum(lightNum);
        light->getLight()->setLinearAttenuation(0);
        light->getLight()->setQuadraticAttenuation(0.0008);
        light->getLight()->setConstantAttenuation(0.000);
        light->getLight()->setSpotCutoff(spotCutoff);
        light->getLight()->setSpotExponent(spotExponent);
        return light;
    }


  osg::ref_ptr<osg::PositionAttitudeTransform> wrapInPositionAttitudeTransform(osg::Node * srcNode, const osg::Vec3d& pos){
	osg::ref_ptr<osg::PositionAttitudeTransform> newpos = new osg::PositionAttitudeTransform;
	newpos->setPosition(pos);
	newpos->addChild(srcNode);
	return newpos;	  
  }

	ref_ptr<osg::Group> createCuboid(const double length, const double width, const double height, const double faktor){

		ref_ptr<Group> cube = new Group();
		Geode *cubeSides[6];
		MatrixTransform *cubeTrans[6];
		for (int i = 0; i < 6; i++){
			cubeSides[i] = new Geode();
			cubeTrans[i] = new MatrixTransform;
			
		}
		cubeSides[0]->addDrawable(createRectangleWithTexcoords(length, width, (int)(length * faktor) + 1, (int)(width * faktor) + 1));
		cubeSides[1]->addDrawable(createRectangleWithTexcoords(length, width, (int)(length * faktor) + 1, (int)(width * faktor) + 1));

		cubeSides[2]->addDrawable(createRectangleWithTexcoords(height, width, (int)(height * faktor) + 1, (int)(width * faktor) + 1));
		cubeSides[3]->addDrawable(createRectangleWithTexcoords(height, width, (int)(height * faktor) + 1, (int)(width * faktor) + 1));

		cubeSides[4]->addDrawable(createRectangleWithTexcoords(height, length, (int)(height * faktor) + 1, (int)(length * faktor) + 1));
		cubeSides[5]->addDrawable(createRectangleWithTexcoords(height, length, (int)(height * faktor) + 1, (int)(length * faktor) + 1));

		cubeTrans[0]->setMatrix(osg::Matrix::translate(0, 0, height));
		cubeTrans[1]->setMatrix(osg::Matrix::rotate((3 / 2)*PI, 1, 0, 0)*osg::Matrix::translate(0.0f, width, 0));

		cubeTrans[2]->setMatrix(osg::Matrix::rotate(PI / 2, 0, 1, 0)*osg::Matrix::translate(length, 0, height));
		cubeTrans[3]->setMatrix(osg::Matrix::rotate((PI / 2) * 3, 0, 1, 0));

		cubeTrans[4]->setMatrix(osg::Matrix::rotate((PI / 2), 0, 0, 1)*osg::Matrix::rotate((PI / 2), 1, 0, 0)*osg::Matrix::translate(length, 0, 0));
		cubeTrans[5]->setMatrix(osg::Matrix::rotate((PI / 2) * 3, 0, 0, 1)*osg::Matrix::rotate((PI / 2) * 3, 1, 0, 0)*osg::Matrix::translate(0, width, 0));

		for (int i = 0; i < 6; i++){
			cubeTrans[i]->addChild(cubeSides[i]);			
			cube->addChild(cubeTrans[i]);
		}

		return cube.release();

	}


    void createRenderingPipeline(unsigned int width, unsigned int height, osg::Node& rootForToon, osgViewer::Viewer &viewer, RenderingPipeline& pipe, Vec3f& fogColor) {
        osg::ref_ptr<brtr::CelShading> toonRoot = new brtr::CelShading;
        toonRoot->addChild(&rootForToon);

        osg::ref_ptr<osg::Texture2D> toonAndOutline = new osg::Texture2D;
        toonAndOutline->setTextureSize(width, height);
        toonAndOutline->setInternalFormat(GL_RGBA);
        osg::ref_ptr<osg::Camera> rttCamToon = brtr::createRTTCamera(osg::Camera::COLOR_BUFFER, toonAndOutline);
        rttCamToon->addChild(toonRoot);

        //taken from the OSG Beginners Guide
        osg::ref_ptr<osg::Texture2D> deepth = new osg::Texture2D;
        deepth->setTextureSize(width, height);
        deepth->setInternalFormat(GL_DEPTH_COMPONENT24);
        deepth->setSourceFormat(GL_DEPTH_COMPONENT);
        deepth->setSourceType(GL_FLOAT);

        osg::ref_ptr<osg::Camera> rttCamDepth = brtr::createRTTCamera(osg::Camera::DEPTH_BUFFER, deepth);
        rttCamDepth->addChild(toonRoot);

        osg::ref_ptr<Camera> postProcessCam = brtr::createHUDCamera(0, 1, 0, 1);
        postProcessCam->addChild(brtr::createScreenQuad(width, height));

        osg::ref_ptr<osg::Shader> fogFrag = osgDB::readShaderFile("../Shader/fogShader.frag");
        osg::ref_ptr<osg::Shader> fogVert = osgDB::readShaderFile("../Shader/fogShader.vert");
        osg::ref_ptr<osg::Program> fogProgram = new osg::Program;
        fogProgram->addShader(fogFrag);
        fogProgram->addShader(fogVert);

        osg::ref_ptr<osg::Shader> sepiaFogFrag = osgDB::readShaderFile("../Shader/sepiaFogShader.frag");
        osg::ref_ptr<osg::Program> sepiaFogProgram = new osg::Program;
        sepiaFogProgram->addShader(sepiaFogFrag);
        sepiaFogProgram->addShader(fogVert);

        osg::ref_ptr<osg::Shader> wavesFrag = osgDB::readShaderFile("../Shader/sinShader.frag");
        osg::ref_ptr<osg::Program> wavesProgram = new osg::Program;
        wavesProgram->addShader(wavesFrag);
        wavesProgram->addShader(fogVert);

        //creating Program vector, element 0 should be the active one
        std::vector<osg::ref_ptr<osg::Program>> programVector;
        programVector.push_back(fogProgram);
        programVector.push_back(sepiaFogProgram);
        programVector.push_back(wavesProgram);

        //postprocess Attributs and Mods
        postProcessCam->getOrCreateStateSet()->setAttributeAndModes(programVector[0], osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
        postProcessCam->getOrCreateStateSet()->setTextureAttributeAndModes(0, toonAndOutline, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
        postProcessCam->getOrCreateStateSet()->addUniform(new osg::Uniform("texture0", 0), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
        postProcessCam->getOrCreateStateSet()->setTextureAttributeAndModes(1, deepth, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
        postProcessCam->getOrCreateStateSet()->addUniform(new osg::Uniform("deepth", 1), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
        postProcessCam->getOrCreateStateSet()->addUniform(new osg::Uniform("fogColor", fogColor), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);

        //setting Clipping Pane
        float zNear = 0.01, zFar = 100000;
        osg::ref_ptr<osg::Uniform> zNearUniform = new osg::Uniform("zNear", zNear);
        osg::ref_ptr<osg::Uniform> zFarUniform = new osg::Uniform("zFar", zFar);
        postProcessCam->getOrCreateStateSet()->addUniform(zNearUniform);
        postProcessCam->getOrCreateStateSet()->addUniform(zFarUniform);
        viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
        viewer.getCamera()->setProjectionMatrixAsPerspective(70, 1.778, zNear, zFar);

        //Setting Pipeline
        pipe.pass_0_color = rttCamToon;
        pipe.pass_0_depth = rttCamDepth;
        pipe.pass_PostProcess = postProcessCam;
        pipe.programs = programVector;
    }

    osg::ref_ptr<osg::Geometry> createBeerBottle() {
        brtr::BodyOfRotationFunction seventh = { [](double x) {
            return -18.4*x + 17.02;
        }, 0.925, nullptr };
        brtr::BodyOfRotationFunction sixth = { [](double x) {
            return 0.19* exp(-0.77 * x);
        }, 0.92, &seventh };
        brtr::BodyOfRotationFunction fifth = { [](double x) {
            return 0.8*x - 0.6;
        }, 0.865, &sixth };
        brtr::BodyOfRotationFunction fourth = { [](double x) {
            return 0.092;
        }, 0.86, &fifth };
        brtr::BodyOfRotationFunction third = { [](double x) {
            return 0.22*exp(-1.49 * x);
        }, 0.6, &fourth };
        brtr::BodyOfRotationFunction second = { [](double x) {
            return 0.11;
        }, 0.48, &third };
        brtr::BodyOfRotationFunction first = { [](double x) {
            return 55 * x;
        }, 0.002, &second };
        return brtr::createBodyOfRotation(0.925, 1000, 50, first);
    }

    osg::ref_ptr<osg::Geometry> createRealBottle() {
        brtr::BodyOfRotationFunction six = { [](double x) {
            return -1e3*x + 1280.06;
        }, 1.28002, nullptr };
        brtr::BodyOfRotationFunction fifth = { [](double x) {
            return 0.06;
        }, 1.28, &six };
        brtr::BodyOfRotationFunction fourth = { [](double x) {
            return 0.054;
        }, 1.22, &fifth };
        brtr::BodyOfRotationFunction third = { [](double x) {
            return -9594.216687 *x*x*x*x*x + 41734.84259*x*x*x*x
                - 72461.92057 * x*x*x + 62769.20326*x*x - 27127.83957*x + 4679.866301;
        }, 0.94, &fourth };
        brtr::BodyOfRotationFunction second = { [](double x) {
            return 0.14;
        }, 0.8, &third };
        brtr::BodyOfRotationFunction first = { [](double x) {
            return 1e3 * x;
        }, 0.14e-3, &second };


        ref_ptr<Geometry> body = brtr::createBodyOfRotation(1.28002, 50, 25, first);
        ref_ptr<Material> bodyMat = new Material;
        bodyMat->setDiffuse(Material::FRONT, Vec4(0.33, 0.23, 0.15, 1));
        bodyMat->setAmbient(Material::FRONT, Vec4(0.33, 0.23, 0.15, 1));
        bodyMat->setSpecular(Material::FRONT, Vec4(0.7, 0.7, 0.7, 1));
        bodyMat->setShininess(Material::FRONT, 42.0);

        
        body->getOrCreateStateSet()->setAttributeAndModes(bodyMat, StateAttribute::ON);
        body->getOrCreateStateSet()->addUniform(new Uniform("tex",false), StateAttribute::ON | StateAttribute::OVERRIDE);
        return body;
    }

    osg::ref_ptr<osg::Geometry> createVase() {
        brtr::BodyOfRotationFunction six = { [](double x) {
            return  13.14800902*x*x*x - 29.67693464*x*x + 22.22013524*x - 5.442163787;
        }, 0.92, nullptr };
        brtr::BodyOfRotationFunction fifth = { [](double x) {
            return -7723.99737*pow(x, 6) + 34421.33266*pow(x, 5) - 63445.68073*pow(x, 4)
                + 61911.20624*x*x*x - 33732.12852*x*x + 9729.204284*x - 1160.328634;
        }, 0.7, &six };
        brtr::BodyOfRotationFunction fourth = { [](double x) {
            return -6.74046e-4*pow(x, 7) + 22500.00265*pow(x, 6) - 67500.00438*pow(x, 5)
                + 83825.00396*pow(x, 4) - 55150.00212*x*x*x + 20268.00067*x*x - 3943.000117*x + 317.1600086;
        }, 0.6, &fifth };
        brtr::BodyOfRotationFunction third = { [](double x) {
            return -2.46772e-4*pow(x, 7) - 22499.99949*pow(x, 6) + 40499.99956*pow(x, 5)
                - 29824.99979*pow(x, 4) + 11489.99994*x*x*x - 2435.99999*x*x + 268.1999991*x - 11.79999997;
        }, 0.4, &fourth };
        brtr::BodyOfRotationFunction second = { [](double x) {
            return 91.2477433*pow(x, 7) + 22479.62608*pow(x, 6) - 13517.78749*pow(x, 5) + 2833.488659*pow(x, 4)
                - 231.3692301*x*x*x + 0.0830483429*x*x + 0.999990044*x + 0.1198800004;
        }, 0.2, &third };
        brtr::BodyOfRotationFunction first = { [](double x) {
            return 1e3 * x;
        }, 0.12e-3, &second };


        ref_ptr<Geometry> body = brtr::createBodyOfRotation(0.92, 50, 25, first);
        ref_ptr<Material> bodyMat = new Material;
        bodyMat->setDiffuse(Material::FRONT, Vec4(0.0754, 0.3529, 0.58, 1));
        bodyMat->setAmbient(Material::FRONT, Vec4(0.0754, 0.3529, 0.58, 1));
        bodyMat->setSpecular(Material::FRONT, Vec4(0.7, 0.7, 0.7, 1));
        bodyMat->setShininess(Material::FRONT, 42.0);


        body->getOrCreateStateSet()->setAttributeAndModes(bodyMat, StateAttribute::ON);
        body->getOrCreateStateSet()->addUniform(new Uniform("tex", false), StateAttribute::ON | StateAttribute::OVERRIDE);

        return body;
    }

    osg::ref_ptr<osg::Geometry> createStalk() {
        brtr::BodyOfRotationFunction third = { [](double x) {
            return -1e3*x + 1200.02;
        }, 1.20002, nullptr };
        brtr::BodyOfRotationFunction second = { [](double x) {
            return 0.015;
        }, 1.2, &third };
        brtr::BodyOfRotationFunction first = { [](double x) {
            return 1e3 * x;
        }, 0.02e-3, &second };


        ref_ptr<Geometry> body = brtr::createBodyOfRotation(1.20002, 50, 25, first);
        ref_ptr<Material> bodyMat = new Material;
        bodyMat->setDiffuse(Material::FRONT, Vec4(0, 0.43, 0.0215, 1));
        bodyMat->setAmbient(Material::FRONT, Vec4(0, 0.43, 0.0215, 1));
        bodyMat->setSpecular(Material::FRONT, Vec4(0.1, 0.1, 0.1, 1));
        bodyMat->setShininess(Material::FRONT, 100.0);


        body->getOrCreateStateSet()->setAttributeAndModes(bodyMat, StateAttribute::ON);
        body->getOrCreateStateSet()->addUniform(new Uniform("tex", false), StateAttribute::ON | StateAttribute::OVERRIDE);

        return body;
    }

    osg::ref_ptr<osg::Geometry> createBud() {
        brtr::BodyOfRotationFunction second = { [](double x) {
            return  -1.5625 * x*x + 0.25 * x + 0.03;
        }, 0.24, nullptr };
        brtr::BodyOfRotationFunction first = { [](double x) {
            return -6.25 * x *x + x;
        }, 0.08, &second };


        ref_ptr<Geometry> body = brtr::createBodyOfRotation(0.24, 50, 25, first);
        ref_ptr<Material> bodyMat = new Material;
        bodyMat->setDiffuse(Material::FRONT, Vec4(0.8, 0.008, 0.4304, 1));
        bodyMat->setAmbient(Material::FRONT, Vec4(0.8, 0.008, 0.4304, 1)); 
        bodyMat->setSpecular(Material::FRONT, Vec4(0.1, 0.1, 0.1, 1));
        bodyMat->setShininess(Material::FRONT, 100.0);


        body->getOrCreateStateSet()->setAttributeAndModes(bodyMat, StateAttribute::ON);
        body->getOrCreateStateSet()->addUniform(new Uniform("tex", false), StateAttribute::ON | StateAttribute::OVERRIDE);

        return body;
    }

    osg::ref_ptr<osg::Geometry> createChessFigure() {
        brtr::BodyOfRotationFunction thirtheen = { [](double x) {
            return  -1e3*x + 8000.44;
        }, 8.00044, nullptr };
        brtr::BodyOfRotationFunction twelve = { [](double x) {
            return sqrt(0.375 *0.375 - (x - 7.625)*(x - 7.625)) + 0.44;
        }, 8, &thirtheen };
        brtr::BodyOfRotationFunction eleventh = { [](double x) {
            return 0.44;
        }, 7.25, &twelve };
        brtr::BodyOfRotationFunction tenth = { [](double x) {
            return 0.4*x - 2.2;
        }, 7, &eleventh };
        brtr::BodyOfRotationFunction ninth = { [](double x) {
            return  -(1 + 1 / 3)*x*x + (17.6 + 1 / 30)*x - 57.5;
        }, 7, &eleventh };
        brtr::BodyOfRotationFunction eight = { [](double x) {
            return   -2.25*x*x + 29.7*x - 97.2;
        }, 6.6, &tenth };
        brtr::BodyOfRotationFunction seventh = { [](double x) {
            return 0.44;
        }, 6.2, &eight };
        brtr::BodyOfRotationFunction six = { [](double x) {
            return  -1e3*x + 4200.96;
        }, 4.20052, &seventh };
        brtr::BodyOfRotationFunction fifth = { [](double x) {
            return  0.96;
        }, 4.2, &six };
        brtr::BodyOfRotationFunction fourth = { [](double x) {
            return 3.59724403e-58*exp(32.90396023*x) + 0.44;
        }, 4, &fifth };
        brtr::BodyOfRotationFunction third = { [](double x) {
            return 0.44;
        }, 3.6, &fourth };
        brtr::BodyOfRotationFunction second = { [](double x) {
            return sqrt(1.44*1.44 - x*x) + 0.44;
        }, 1.44, &third };
        brtr::BodyOfRotationFunction first = { [](double x) {
            return 1e3 * x;
        }, 0.00188, &second };

        ref_ptr<Geometry> body = brtr::createBodyOfRotation(8.00044, 1000, 50, first);
        ref_ptr<Material> bodyMat = new Material;
        bodyMat->setDiffuse(Material::FRONT, Vec4(0.84, 0.238, 0.0, 1));
        bodyMat->setAmbient(Material::FRONT, Vec4(0.84, 0.238, 0.0, 1));
        bodyMat->setSpecular(Material::FRONT, Vec4(0.7, 0.7, 0.7, 1));
        bodyMat->setShininess(Material::FRONT, 42.0);


        body->getOrCreateStateSet()->setAttributeAndModes(bodyMat, StateAttribute::ON);
        body->getOrCreateStateSet()->addUniform(new Uniform("tex", false), StateAttribute::ON | StateAttribute::OVERRIDE);

        return body;
    }

    extern osg::ref_ptr<osg::Geode> createCrosshair(unsigned int width, unsigned int height) {
        ref_ptr<osg::Geode>         geode = new osg::Geode;
        ref_ptr<osg::Geometry>    geom = new Geometry;;
        ref_ptr<osg::Vec3Array>   vertices =new osg::Vec3Array;
        geode->addDrawable(geom);

        vertices->push_back(osg::Vec3(width / 2.0 - 10, height / 2.0 - 1, 0.0));
        vertices->push_back(osg::Vec3(width / 2.0 - 10, height / 2.0 + 1, 0.0));
        vertices->push_back(osg::Vec3(width / 2.0 + 10, height / 2.0 + 1, 0.0));
        vertices->push_back(osg::Vec3(width / 2.0 + 10, height / 2.0 - 1, 0.0));

        vertices->push_back(osg::Vec3(width / 2.0 - 1, height / 2.0 - 10, 0.0));
        vertices->push_back(osg::Vec3(width / 2.0 - 1, height / 2.0 + 10, 0.0));
        vertices->push_back(osg::Vec3(width / 2.0 + 1, height / 2.0 + 10, 0.0));
        vertices->push_back(osg::Vec3(width / 2.0 + 1, height / 2.0 - 10, 0.0));
        geom->setVertexArray(vertices);

        // set colors
        ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(0.55, 0.55, 0.55, 1.0));
        geom->setColorArray(colors);
        geom->setColorBinding(Geometry::BIND_OVERALL);
        geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 8));
        return geode;
    }

    osg::ref_ptr<osg::PositionAttitudeTransform> createVaseWithFlower() {
        ref_ptr<Geode> bottom = new Geode;
        bottom->addDrawable(brtr::createVase());
        ref_ptr<Geode> inner = new Geode;
        inner->addDrawable(brtr::createStalk());
        ref_ptr<Geode> knospe = new Geode;
        knospe->addDrawable(brtr::createBud());
        ref_ptr<PositionAttitudeTransform> budTranslate = new PositionAttitudeTransform;
        budTranslate->addChild(knospe);
        budTranslate->setPosition(Vec3(0, 0, 1.0));
        ref_ptr<PositionAttitudeTransform> vaseFlower = new PositionAttitudeTransform;
        vaseFlower->addChild(bottom);
        vaseFlower->addChild(inner);
        vaseFlower->addChild(budTranslate);
        return vaseFlower;
    }
    osg::ref_ptr<osg::Material> createSimpleMaterial(osg::Material::Face face, const osg::Vec4& diffuse, const osg::Vec4& ambient, const osg::Vec4& specular, const double shininess) {
        ref_ptr<Material> mat = new osg::Material;
        mat->setDiffuse(face, osg::Vec4(.7f, .7f, .7f, 1.0f));
        mat->setAmbient(face, osg::Vec4(.3f, .3f, .3f, 1.0f));
        mat->setSpecular(face, osg::Vec4(.9f, .9f, .9f, 1.0f));
        mat->setShininess(face, shininess);
        return mat;
    }

    osg::Vec3 getDimensionOfNode(Node * source) {
        ComputeBoundsVisitor cbbv;
        source->accept(cbbv);
        BoundingBox bb = cbbv.getBoundingBox();
        Vec3 size = bb._max - bb._min;
        return size;
    }

    osg::ref_ptr<osg::Texture2D> createToonTex(std::string toonTex) {
        osg::ref_ptr<osg::Texture2D> toonTexture = new osg::Texture2D;
        toonTexture->setImage(osgDB::readImageFile("../BlenderFiles/Texturen/toons/" + toonTex));
        toonTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
        toonTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
        return toonTexture;
    }

}