#include "../header/UtilFunctions.h"

#include "../header/Bench.h"
using namespace osg;

namespace brtr{
	Bench::Bench(const Vec3& pcenter, const double plength){
	  if(plength < 2 || plength > 30) length = 8;
		    else length = plength;
		
		initBench(length); //initalize the bench and saves it into a private attribute
		center = pcenter;

		addChild(bench.get()); 
		setPosition(center);
	}

	Bench::Bench(const Bench& copy, const CopyOp& copyop)
		: PositionAttitudeTransform(copy, copyop){}

	Bench::~Bench() {
	}	

	ref_ptr<PositionAttitudeTransform> Bench::getHitbox(const double alpha, double height){		
		Vec3 size = getDimensionOfNode(bench);
		
		if (height < 0) height = size.z();
		ref_ptr<Group> hitbox = brtr::createCuboid(size.x(), size.y(), height, 0);
		
		//needed to make the hitbox transparent
		ref_ptr<BlendFunc> blendFunc = new BlendFunc;
		blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		StateSet* stateset = hitbox->getOrCreateStateSet();
		Material * newmaterial = new Material();
		newmaterial->setEmission(Material::FRONT, Vec4(0, 0, 0, alpha));
		newmaterial->setDiffuse(Material::FRONT, Vec4(0, 0, 0, alpha));
		newmaterial->setAmbient(Material::FRONT, Vec4(0, 0, 0, alpha));
		stateset->setAttributeAndModes(newmaterial, StateAttribute::OVERRIDE | StateAttribute::ON);
		stateset->setRenderingHint(StateSet::TRANSPARENT_BIN);

		stateset->setAttributeAndModes(blendFunc);
		
		//set the position of the hitbox
		ref_ptr<PositionAttitudeTransform> benchpos = new PositionAttitudeTransform;
		benchpos->setPosition(this->center);
		benchpos->addChild(hitbox);


		return benchpos;
	}
	
	ref_ptr<Material> Bench::createIronMaterial(){
		ref_ptr<Material> mat = createSimpleMaterial(	osg::Material::FRONT_AND_BACK,	
								osg::Vec4(.9f, .9f, .9f, 1.0f), //diffuse
								osg::Vec4(.7f, .7f, .7f, 1.0f), //ambient
								osg::Vec4(.3f, .3f, .3f, 1.0f), //specular
								128); //Shininess
		return mat;
	}
	
	ref_ptr<Material> Bench::createWoodMaterial(){
		ref_ptr<Material> mat = createSimpleMaterial(osg::Material::FRONT_AND_BACK,	
								osg::Vec4(.3f, .3f, .3f, 1.0f), //diffuse
								osg::Vec4(.7f, .7f, .7f, 1.0f), //ambient
								osg::Vec4(.1f, .1f, .1f, 1.0f), //specular
								3); //Shininess
		return mat;
	}

	ref_ptr<DrawElementsUInt> Bench::getPrimitiveSetforARectangle(int lsteps, int wsteps){
		ref_ptr<DrawElementsUInt> indices = new DrawElementsUInt(GL_TRIANGLE_STRIP);
		for (int i = 0; i < lsteps; i++) {
			for (int j = 0; j <= wsteps; j++) {
				indices->push_back(i*(wsteps + 1) + j);
				indices->push_back((i + 1)*(wsteps + 1) + j);
			}
			indices->push_back((i + 1)*(wsteps + 1) + wsteps);
			indices->push_back((i + 1)*(wsteps + 1));
		}
		return indices;
	}

	ref_ptr<Geometry> Bench::createArmrestSidesLeftRight(double radius, double radius2, int lsteps, int wsteps, bool flip){
		ref_ptr<Geometry> side = new Geometry;
		ref_ptr<Vec3Array> vertices = new Vec3Array();
		ref_ptr<Vec3Array> normals = new Vec3Array();		

		double xstep = (radius2 - radius) / lsteps;
		double ystep = radius / wsteps;
		// current vertex coordinates
		double x = 0.0;
		double y = 0.0;
		//  current normal coordinates
		double nx = 0.0;
		double ny = 1.0;
		double nz = 0.0;
		
		double curRad = radius; //the current radius
		double radSteps = (radius2 - radius) / wsteps; 
		double xstepalpha = DegreesToRadians(90.0 / wsteps); //steps for the radius

		double alpha = DegreesToRadians(180.0);
		if (flip)  alpha -= DegreesToRadians(90.0); // when this should be the other side, the start radius is changed 
		
		ref_ptr<Vec2Array> texcoords = new Vec2Array;
		// set vertices and normals
		for (int i = 0; i <= lsteps; i++) {
			y = 0.0;
			curRad = radius;
			for (int j = 0; j <= wsteps; j++) {
				vertices->push_back(Vec3d(cos(alpha)*curRad, 0, sin(alpha)*curRad));
				
				texcoords->push_back(Vec2(x / (radius2 - radius), y / radius));
				
				normals->push_back(Vec3d(nx, ny, nz));
				y += ystep;
				curRad += radSteps;
			}
			alpha += xstepalpha;

			x += xstep;
		}

		
		//normalize the normals
		for (auto cnt = 0; cnt < normals->size(); cnt++) {
			normals->at(cnt).normalize();
		}

		side->setVertexArray(vertices.get());
		side->addPrimitiveSet(getPrimitiveSetforARectangle(lsteps,wsteps));
		side->setNormalArray(normals.get());
		side->setTexCoordArray(0, texcoords.get());
		side->setNormalBinding(Geometry::BIND_PER_VERTEX);

		return side;
	}	

	ref_ptr<Geometry> Bench::createArmrestSidesFrontBack(double radius, double width, int lsteps, int wsteps,bool flip){
		ref_ptr<Geometry> side = new Geometry;
		ref_ptr<Vec3Array> vertices = new Vec3Array();
		ref_ptr<Vec3Array> normals = new Vec3Array();
		ref_ptr<DrawElementsUInt> indices = new DrawElementsUInt(GL_TRIANGLE_STRIP);

		double ystep = width / wsteps;
		
		double xstepalpha = DegreesToRadians(90.0 / wsteps); //steps for the radius

		// current vertex coordinates		
		double y = 0.0;
		//  current normal coordinates		
		double ny = 0.0;

		double alpha = DegreesToRadians(180.0);
		if (flip)  alpha -= DegreesToRadians(90.0); // when this should be the other side, the start radius is changed 
		// set vertices and normals
		ref_ptr<Vec2Array> texcoords = new Vec2Array;
		for (int i = 0; i <= lsteps; i++) {
			y = 0.0;
			for (int j = 0; j <= wsteps; j++) {

				if (!flip) {
					vertices->push_back(Vec3d(cos(alpha)*radius, y, sin(alpha)*radius));
					normals->push_back(Vec3d(-cos(alpha)*radius, ny, -sin(alpha)*radius));
				}else{ // flip the vertices and normales to fit
					vertices->push_back(Vec3d(cos(alpha)*radius, y, -sin(alpha)*radius));
					normals->push_back(Vec3d(cos(alpha)*radius, ny, -sin(alpha)*radius));
				}


				texcoords->push_back(Vec2(cos(alpha) / 1, y / width)); //calculate the texture coordinates
				y += ystep;
			}
			alpha += xstepalpha;

		}
	
		//normalize the normals
		for (auto cnt = 0; cnt < normals->size(); cnt++) {
			normals->at(cnt).normalize();
		}
		side->setVertexArray(vertices.get());
		side->addPrimitiveSet(getPrimitiveSetforARectangle(lsteps,wsteps));
		side->setNormalArray(normals.get());
		side->setNormalBinding(Geometry::BIND_PER_VERTEX);
		side->setTexCoordArray(0, texcoords.get());
		return side;
	}
	ref_ptr<Group> Bench::createLeg(){

		double length = 0.5;
		double width = length;
		double height = 4 * length;

		ref_ptr<Group> leg = createCuboid(length, width, height);

		//set Textures
		ref_ptr<Texture2D> texture = new Texture2D;
		ref_ptr<Image> image = osgDB::readImageFile("../BlenderFiles/Texturen/iron.jpg");
		texture->setImage(image.get());
		texture->setWrap(Texture::WRAP_S, Texture::MIRROR);
		texture->setWrap(Texture::WRAP_T, Texture::MIRROR);
		osg::ref_ptr<osg::StateSet> legs(leg->getOrCreateStateSet());

		legs->setAttribute(createIronMaterial());
		legs->setTextureAttributeAndModes(0, texture.get());

		return leg;
	}

	ref_ptr<Group> Bench::createSeat(const double width){
		ref_ptr<Group> seat = new Group;

		double length = width; //ratio of the seat is 1:1
		double height = 0.1; 

		ref_ptr<Group> base = createCuboid(length, width, height);
      
		 //create the texture
		ref_ptr<Texture2D> texturewood = new Texture2D;
		ref_ptr<Image> imagewood = osgDB::readImageFile("../BlenderFiles/Texturen/wood.jpg");		
		texturewood->setImage(imagewood.get());
		texturewood->setWrap(Texture::WRAP_S, Texture::MIRROR);
		texturewood->setWrap(Texture::WRAP_T, Texture::MIRROR);


		
		base->getChild(0)->getOrCreateStateSet()->setAttribute(createWoodMaterial());
		base->getChild(0)->getOrCreateStateSet()->setTextureAttributeAndModes(0, texturewood.get()); //Front

		base->getChild(1)->getOrCreateStateSet()->setAttribute(createWoodMaterial());				
		base->getChild(1)->getOrCreateStateSet()->setTextureAttributeAndModes(0, texturewood.get()); //Back
		
		
		//rotate the base to get the back of the seat
		ref_ptr<MatrixTransform> seatback = new MatrixTransform();
		seatback->setMatrix(Matrix::rotate(PI / 2 * 1.1, 1, 0, 0));
		seatback->addChild(base.get());

		//group the components
		seat->addChild(base);
		seat->addChild(seatback);

		//change the position
		ref_ptr<PositionAttitudeTransform> seatpos = new PositionAttitudeTransform;
		seatpos = wrapInPositionAttitudeTransform(seat,Vec3d(0, -(width - getDimensionOfNode(seat).y()), 0.0));

		return seatpos;

	}
	ref_ptr<Group> Bench::createArmrest(double radius, double width, double length, double totalwidth){
		ref_ptr<Group> armrest = new Group;

		ref_ptr<Group> armrest_arch = new Group;
		//creates the components for the armrest_arch
		ref_ptr<Geode> front = new Geode;
		front->addDrawable(createArmrestSidesFrontBack(radius, width, 10, 10, false));
		ref_ptr<Geode> back = new Geode;
		back->addDrawable(createArmrestSidesFrontBack(radius + length, width, 10, 10, true));
		ref_ptr<Geode> leftside = new Geode;
		leftside->addDrawable(createArmrestSidesLeftRight(radius, radius + length, 10, 10, true));
		ref_ptr<Geode> rigthside = new Geode;
		rigthside->addDrawable(createArmrestSidesLeftRight(radius, radius + length, 10, 10, false));

		ref_ptr<PositionAttitudeTransform> leftsiderotated = new PositionAttitudeTransform;
		leftsiderotated->setAttitude(Quat(DegreesToRadians(180.0), X_AXIS));
		leftsiderotated->addChild(leftside);
		
		ref_ptr<PositionAttitudeTransform> rigthsiderotated = new PositionAttitudeTransform;
		rigthsiderotated->setPosition(Vec3d(0, width, 0));
		rigthsiderotated->addChild(rigthside);

		
		
		ref_ptr<Geode> topshape = new Geode;
		topshape->addDrawable(brtr::createRectangleWithTexcoords(length, width, 20, 20));
		ref_ptr<PositionAttitudeTransform> top = wrapInPositionAttitudeTransform(topshape,Vec3d(-(radius + length), 0, 0));


		ref_ptr <MatrixTransform> bottom = new MatrixTransform;
		bottom->setMatrix(Matrix::rotate(PI / 2, 0, 1, 0)*Matrix::translate(0, 0, -radius));
		bottom->addChild(topshape);


		

		armrest_arch->addChild(front);
		armrest_arch->addChild(back);
		armrest_arch->addChild(leftsiderotated);
		armrest_arch->addChild(rigthsiderotated);
		armrest_arch->addChild(top);
		armrest_arch->addChild(bottom);
		
		//Positioning the armrest_arch
		armrest_arch = wrapInPositionAttitudeTransform(armrest_arch, Vec3d(radius + length, 0, radius + length));
		
		double height = 0.5;
		ref_ptr<Group> bar_up = brtr::createCuboid(length, width, height);
		ref_ptr<Group> barontop = brtr::createCuboid(width, totalwidth, width / 2);
		
		bar_up = wrapInPositionAttitudeTransform(bar_up, Vec3d(0.0, 0.0, radius + length));
		barontop = wrapInPositionAttitudeTransform(barontop, Vec3d(-width / 4, -(totalwidth) / 2 + width / 2, radius + length + height));

		ref_ptr<Group> bars = new Group;
		bars->addChild(barontop);
		bars->addChild(bar_up);



		armrest->addChild(armrest_arch);
		armrest->addChild(bars);


		ref_ptr<Texture2D> texture = new Texture2D;
		ref_ptr<Image> image = osgDB::readImageFile("../BlenderFiles/Texturen/iron.jpg");
		texture->setImage(image.get());
		texture->setWrap(Texture::WRAP_S, Texture::MIRROR);
		texture->setWrap(Texture::WRAP_T, Texture::MIRROR);

		osg::ref_ptr<osg::StateSet> armrest_state(armrest->getOrCreateStateSet());

		armrest_state->setAttribute(createIronMaterial());
		armrest_state->setTextureAttributeAndModes(0, texture.get());

		return armrest;
	}



	ref_ptr<Group> Bench::createBar(){

		double width = 0.5;
		double height = width;
		double length = (this->length);


		ref_ptr<Group> bar = brtr::createCuboid(length, width, height);


		//set Textures
		ref_ptr<Texture2D> texture = new Texture2D;
		ref_ptr<Image> image =
			osgDB::readImageFile("../BlenderFiles/Texturen/iron.jpg");
		texture->setImage(image.get());
		texture->setWrap(Texture::WRAP_S, Texture::MIRROR);
		texture->setWrap(Texture::WRAP_T, Texture::MIRROR);

		osg::ref_ptr<osg::StateSet> nodess(bar->getOrCreateStateSet());


		nodess->setAttribute(createIronMaterial());
		nodess->setTextureAttributeAndModes(0, texture.get());


		return bar;
	}


	void Bench::initBench(const double plength){
		ref_ptr<Group> leg = createLeg();

		double legdistance = 0.05;

		//create the legs
		ref_ptr<PositionAttitudeTransform> leg1 = new PositionAttitudeTransform;
		ref_ptr<PositionAttitudeTransform> leg2 = new PositionAttitudeTransform;
		leg1->addChild(leg.get());
		leg2->addChild(leg.get());

		ref_ptr<Group> legs = new Group;
		legs->addChild(leg1.get());
		legs->addChild(leg2.get());

		//save the dimension
		Vec3 legssize = getDimensionOfNode(legs);


		//create the bar
		ref_ptr<Group> bar = createBar();
		ref_ptr<PositionAttitudeTransform> bar1 = new PositionAttitudeTransform;
		bar1->addChild(bar.get());
		ref_ptr<Group> bars = new Group;
		ref_ptr<PositionAttitudeTransform> armrest1 = new PositionAttitudeTransform;
		
		
		Vec3 barssize = getDimensionOfNode(bar);


	  
		int anzahl_sitze = (int)(0.5*plength); // caluclate the number of seats
		double sitzspacebetween = 0.9;
		double seatwidth = (barssize.x()*sitzspacebetween) / anzahl_sitze;




		double radiusarmrest = 0.2;

		ref_ptr<Group> armrest = createArmrest(radiusarmrest, barssize.y() / 2, barssize.z() / 4, seatwidth);



		armrest1->addChild(armrest);
		
		armrest1->setPosition(Vec3d(-(radiusarmrest + barssize.z() / 4), (barssize.y() / 4), (barssize.z() / 2) - (barssize.z() / 8)));
		

		ref_ptr <MatrixTransform> armrest2 = new MatrixTransform;
		armrest2->setMatrix(Matrix::translate(-(radiusarmrest + (barssize.z() / 2) / 2), 0, 0)*Matrix::rotate(PI, 0, 0, 1));
		armrest2->addChild(armrest);
		
		ref_ptr<PositionAttitudeTransform> armrest22 = new PositionAttitudeTransform;
		armrest22->setPosition(Vec3d(barssize.x(), (barssize.y() / 4) + barssize.y() / 2, (barssize.z() / 2) - (barssize.z() / 8)));
		armrest22->addChild(armrest2);

		bar1->addChild(armrest1);
		bar1->addChild(armrest22);

		bars->addChild(bar1.get());
		Vec3 armrestsize = getDimensionOfNode(armrest);




		//creating the seats
		ref_ptr<Group> seat = createSeat(seatwidth);

		Vec3 seatsize = getDimensionOfNode(seat);
		double middle = (seatsize.y() - seatwidth) + (seatwidth / 2) - barssize.y() / 2;
		//Position the stuff
		leg1->setPosition(Vec3d(legdistance*plength, middle, 0.0));
		leg2->setPosition(Vec3d((1 - legdistance)*plength - getDimensionOfNode(leg).x(), middle, 0.0));
		bar1->setPosition(Vec3d(0, middle, legssize.z()));

		//duplicate the seats
		PositionAttitudeTransform *sitze[20]; //maximum of 20 seats
		ref_ptr<Group> sitze_all = new Group;
		for (int i = 0; i < anzahl_sitze; i++){
			sitze[i] = new PositionAttitudeTransform;
			double posx = (plength / anzahl_sitze) * i + ((plength / anzahl_sitze) - (seatsize.x())) / 2; //calcute the position for each seat
			sitze[i]->setPosition(Vec3d(posx, 0, legssize.z() + barssize.z()));
			sitze[i]->addChild(seat.get());
			sitze_all->addChild(sitze[i]);
		}

		//bundle all components together into one group
		ref_ptr<Group> bench = new Group;
		bench->addChild(legs.get());
		bench->addChild(bars.get());
		bench->addChild(sitze_all.get());
		
		//position
		ref_ptr<PositionAttitudeTransform> benchpos = new PositionAttitudeTransform;
		benchpos = wrapInPositionAttitudeTransform(bench,Vec3d(armrestsize.x(), 0, 0));
	  
		
		this->bench = benchpos;
	}



}

