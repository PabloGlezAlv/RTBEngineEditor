#include "TargetHolder.h"

namespace RTBEditor {

    using ThisClass = TargetHolder;

    RTB_REGISTER_COMPONENT(TargetHolder)
        RTB_PROPERTY_GAMEOBJECT(targetRef)
    RTB_END_REGISTER(TargetHolder)

    TargetHolder::TargetHolder() {}
    TargetHolder::~TargetHolder() {}

}
