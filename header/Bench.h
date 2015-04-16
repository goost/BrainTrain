#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/ComputeBoundsVisitor>

using namespace osg;

namespace brtr {
   /**
    *  @brief       Bench class, creates a bench Object
    *  @details	    creates a bench with a given length at a given position.
    * 		    The length has to be between 2 and 30    		   
    *  @author      Marcel Felix
    *  @version     1.0
    *  @date        2014
    * 
    *  @copyright   GNU Public License.
    */
	class Bench : public PositionAttitudeTransform{
	
		  
	  public:
		  
		  Bench(const Vec3& pcenter = Vec3(0, 0, 0), const double plength = 8);

		  /**
		   * @brief	return the Hitbox of the Bench
		   * 
		   * @param alpha 
		   * @param height the height of the hitbox. height < 0 will use the height of the bench
		   * @return the hitbox as a PositionAttitudeTransform with the given alpha value
		   **/
		  ref_ptr<PositionAttitudeTransform> getHitbox(const double alpha, double height = 8);
	  
		  Bench(const Bench&, const CopyOp& copyop = CopyOp::SHALLOW_COPY);
		  ~Bench();
	  
	  private:
	  
	  /**
	   * @brief initialize the bench
	   * 
	   * @param plength the length of the bench 
	   **/
	  void initBench(const double plength);
	  
	   /**
	   * @brief create the material for iron objects
	   **/
	  ref_ptr<Material> createIronMaterial();
	  
	   /**
	   * @brief create the material for wood objects
	   **/
	  ref_ptr<Material> createWoodMaterial();
	  
	  ref_ptr<Group> createLeg();
	  
	  ref_ptr<Group> createBar();
	  
	  
	  /**
	   * @brief creates the seat
	   * 
	   * @param width the width/length of the Seat
	   **/
	  ref_ptr<Group> createSeat(const double width);
	  
	   /**
	   * @brief creates the armrest
	   * 
	   * @param radius the distance between bench and armrest
	   * @param width width of the armrest
	   * @param length length of the armrest
	   * @param totalwidth width of the bar on the armrest
	   **/
	  ref_ptr<Group> createArmrest(double radius, double width, double length, double totalwidth);
	  
	   /**
	   * @brief creates the front/back for the armrest
	   * 
	   * @param radius the distance between bench and armrest
	   * @param width width of the armrest
	   * @param length length of the armrest
	   * @param flip switch between front and back creation
	   **/
	  ref_ptr<Geometry> createArmrestSidesFrontBack(double radius, double width, int lsteps, int wsteps, bool flip = true);
	  
	    /**
	   * @brief creates the left/rigth for the armrest
	   * 
	   * @param radius the distance between bench and armrest
	   * @param width width of the armrest
	   * @param length length of the armrest
	   * @param flip switch between front and back creation
	   **/
	  ref_ptr<Geometry> createArmrestSidesLeftRight(double length, double width, int lsteps, int wsteps, bool flip = true);


	  Vec3 center;

	  double length;
	  ref_ptr<Group>  bench;
	  /**
	   * @brief creates a primitives set for the getRectangle function
	   *
	   * parts of the function are copy/pasted from Chapter 7, CG1 Lecture Script by Frauke Sprengel
	   * 
	   * @param lsteps 
	   * @param wsteps
	   * @return a ref_ptr<DrawElementsUInt> containing the primitives set
	   **/
	  ref_ptr<DrawElementsUInt> getPrimitiveSetforARectangle(int lsteps, int wsteps);
	};



	
	
}