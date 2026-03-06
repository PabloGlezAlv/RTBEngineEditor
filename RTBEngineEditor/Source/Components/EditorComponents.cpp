#include "EditorComponents.h"
#include <RTBEngine/Reflection/TypeInfo.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEditor {

    void RegisterEditorComponents() {
        // Register editor-side components here using RTB_PROPERTY_* macros
        // Example:
        //   using ThisClass = MyComponent;
        //   RTBEngine::Reflection::TypeInfo& info = MyComponent::MutableTypeInfo();
        //   RTB_PROPERTY(myValue)
        //   RTBEngine::Reflection::TypeRegistry::GetInstance().RegisterType(info.GetTypeName(), info);
    }

}
