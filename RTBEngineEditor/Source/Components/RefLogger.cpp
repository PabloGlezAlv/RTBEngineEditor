#include "RefLogger.h"

namespace RTBEditor {

    using ThisClass = RefLogger;

    RTB_REGISTER_COMPONENT(RefLogger)
        RTB_PROPERTY_COMPONENT(holderRef, TargetHolder)
    RTB_END_REGISTER(RefLogger)

    RefLogger::RefLogger() {}
    RefLogger::~RefLogger() {}

}
