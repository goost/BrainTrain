#include <osgViewer/Viewer>
#include <osg/AnimationPath>


    class AnimationCreator {
		/**
		* @brief Creator of the Animation Path for the Train-Simulation
		* 
		* @author      Philip Sauer
		* @version     1.0
		* @date        2014
		*/
    public:
		/**
		* @brief  Calculate the angle between two Vectors
		*
		* The Method calculates the dotproduct between two vectors and devides it with the length of both vectors.
		* ( vectorA * vectorB ) / ( |vectorA| * |vectorB| )
		* 
		* @param pointA the starting Vector
		* @param pointB the end Vector
		* @return the angle in radian
		*/
        double getAngleRad(osg::Vec3 pointA, osg::Vec3 pointB);
		/**
		* @brief Creates the animation path
		*
		* The Method creates the Train AnimationPath. Each vector will be included in the AnimationPath, together with the correct rotation between two points.
		* 
		* @param time is the time that the train will take between two vectors, low time = fast train.
		* @return the complete AnimationPath for the train
		*/
        osg::AnimationPath* createAnimationPath(float time);
    private:
    };