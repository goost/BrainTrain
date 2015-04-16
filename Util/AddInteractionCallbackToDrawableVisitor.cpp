#include "../header/AddInteractionCallbackToDrawableVisitor.h"

namespace brtr{

    AddInteractionCallbackToDrawableVisitor::AddInteractionCallbackToDrawableVisitor(brtr::BaseInteractionCallback* callbackToAdd) {
        setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
        _containerToAdd = new osg::DefaultUserDataContainer;
        _containerToAdd->addUserObject(callbackToAdd);
    }

    void AddInteractionCallbackToDrawableVisitor::apply(osg::Geode& geode) {
        for (int i = 0; i < geode.getNumDrawables(); ++i) {
            geode.getDrawable(i)->setUserDataContainer(_containerToAdd);
        }
    }

}
