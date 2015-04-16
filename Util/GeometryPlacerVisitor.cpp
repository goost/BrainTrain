#include "../header/GeometryPlacerVisitor.h"

using namespace osg;

namespace brtr {
    GeometryPlacerVisitor::GeometryPlacerVisitor(osg::Geometry* geometryToPlace) :
        _geometryToPlace(geometryToPlace) {
        setTraversalMode(NodeVisitor::TRAVERSE_ALL_CHILDREN);
    }

    void GeometryPlacerVisitor::apply(osg::Geode& geode) {
        geode.removeDrawables(0, geode.getNumDrawables());
        geode.addDrawable(_geometryToPlace);
    }

    osg::ref_ptr<osg::Geometry> GeometryPlacerVisitor::getGeometryToPlace() const {
        return _geometryToPlace;
    }

    void GeometryPlacerVisitor::setGeometryToPlace(osg::ref_ptr<osg::Geometry> val) {
        _geometryToPlace = val;
    }

}
