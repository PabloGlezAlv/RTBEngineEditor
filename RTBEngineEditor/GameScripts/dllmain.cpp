#include <Windows.h>
#include <RTBEngine/Reflection/TypeInfo.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

// ABI-safe descriptor structs — only POD types, no STL across the module boundary.
// The EXE and DLL may have separate CRT heaps (/MT), so std::string / std::vector
// must never be passed by value or reference between them.

struct RTBPropertyDesc {
    const char* name;           // property name (points into DLL string literals)
    const char* displayName;
    int         type;           // cast of RTBEngine::Reflection::PropertyType
    size_t      offset;
    size_t      size;
    int         flags;          // cast of RTBEngine::Reflection::PropertyFlags
    float       rangeMin;
    float       rangeMax;
    int         hasRange;
    const char* componentTypeName; // for ComponentRef properties
};

struct RTBTypeDesc {
    const char* typeName;
    int         propertyCount;
};

// Script-local registry: stores each script component's type name and TypeInfo pointer.
// Populated by RTB_END_REGISTER via RTBScripts_RegisterLocalType() during DLL load.
// Uses fixed-size arrays — no heap allocation, no STL, no cross-heap issues.
struct ScriptTypeEntry {
    const char*                                    typeName; // points to #ClassName string literal
    const RTBEngine::Reflection::TypeInfo*         typeInfo;
};
static ScriptTypeEntry s_scriptTypes[256];
static int s_scriptTypeCount = 0;

extern "C" void RTBScripts_RegisterLocalType(const char* typeName, const RTBEngine::Reflection::TypeInfo* info)
{
    if (s_scriptTypeCount < 256) {
        s_scriptTypes[s_scriptTypeCount].typeName = typeName;
        s_scriptTypes[s_scriptTypeCount].typeInfo  = info;
        ++s_scriptTypeCount;
    }
}

// Called by the editor after loading this DLL.
// For each registered component type, calls beginType, then propCallback for
// each property, then endType. All data is POD — no STL crosses the boundary.
struct BridgeCallbacks {
    void(*beginType)(const char* typeName, void* factory);
    void(*propCallback)(const RTBPropertyDesc* desc);
    void(*endType)();
};

static void BridgeTypeInfo(const ScriptTypeEntry& entry, BridgeCallbacks* cb)
{
    cb->beginType(entry.typeName, reinterpret_cast<void*>(const_cast<RTBEngine::Reflection::TypeInfo*>(entry.typeInfo)));

    for (const auto& prop : entry.typeInfo->GetProperties()) {
        RTBPropertyDesc desc{};
        desc.name              = prop.name.c_str();
        desc.displayName       = prop.displayName.c_str();
        desc.type              = static_cast<int>(prop.type);
        desc.offset            = prop.offset;
        desc.size              = prop.size;
        desc.flags             = static_cast<int>(prop.flags);
        desc.rangeMin          = prop.range ? prop.range->minValue : 0.0f;
        desc.rangeMax          = prop.range ? prop.range->maxValue : 1.0f;
        desc.hasRange          = prop.range ? 1 : 0;
        desc.componentTypeName = prop.componentTypeName.c_str();
        cb->propCallback(&desc);
    }

    cb->endType();
}

extern "C" __declspec(dllexport)
void RTBScripts_RegisterAll(
    void(*beginType)(const char* typeName, void* factory),
    void(*propCallback)(const RTBPropertyDesc* desc),
    void(*endType)())
{
    BridgeCallbacks cb{ beginType, propCallback, endType };
    for (int i = 0; i < s_scriptTypeCount; ++i)
        BridgeTypeInfo(s_scriptTypes[i], &cb);
}
