#pragma once
#include <osgViewer/Viewer>
namespace brtr {
    /**
    *  @brief       NodeVisitor for batch replacing all Geometry in all visited Geodes
    *  @details     Takes a geometry as argument and replaces every geometry in the sub scene
    *               Useful for batch replacing a bunch of geometrys which were placed as dummys
    *               in Blender and then imported. Rotation and Scaling of the Geometry will persist.
    *  @author     Gleb Ostrowski
    *  @version     1.0
    *  @date        2014
    *  @pre         needs a Node which will accept it. Should have some Geode's for this to work
    *  @copyright   GNU Public License.
    */
    class GeometryPlacerVisitor : public osg::NodeVisitor {
    public:
        /**
         * @brief Constructor
         *
         * @param  geometryToPlace geometry to replace the found drawables
         * @return
         */
        GeometryPlacerVisitor(osg::Geometry* geometryToPlace);

        /**
         * @brief Change the Geometry of this Geode
         * @param geode the Geode which will be alternate  
         */
        virtual void apply(osg::Geode& geode);

        osg::ref_ptr<osg::Geometry> getGeometryToPlace() const;
        void setGeometryToPlace(osg::ref_ptr<osg::Geometry> val);
    private:
        osg::ref_ptr<osg::Geometry> _geometryToPlace;
    };
}