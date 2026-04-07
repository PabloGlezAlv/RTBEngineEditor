#include <Windows.h>
#include <RTBEngine/Reflection/TypeInfo.h>
#include <RTBEngine/Scripting/ScriptBridgeABI.h>
#include <cstring>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

// ABI-safe descriptor structs — only POD types, no STL across the module boundary.
// The EXE and DLL may have separate CRT heaps (/MT), so std::string / std::vector
// must never be passed by value or reference between them.

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
struct ScriptPropertyEntry {
    const char*     ownerType;
    RTBPropertyDesc desc;
};
static ScriptPropertyEntry s_scriptProps[4096];
static int s_scriptPropCount = 0;

extern "C" void RTBScripts_RegisterLocalType(const char* typeName, const RTBEngine::Reflection::TypeInfo* info)
{
    if (s_scriptTypeCount < 256) {
        s_scriptTypes[s_scriptTypeCount].typeName = typeName;
        s_scriptTypes[s_scriptTypeCount].typeInfo  = info;
        ++s_scriptTypeCount;
    }
}

extern "C" void RTBScripts_RegisterLocalProperty(const char* ownerType, const RTBPropertyDesc* desc)
{
    if (!ownerType || !desc) return;
    if (s_scriptPropCount >= 4096) return;

    s_scriptProps[s_scriptPropCount].ownerType = ownerType;
    s_scriptProps[s_scriptPropCount].desc = *desc;
    ++s_scriptPropCount;
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

    for (int i = 0; i < s_scriptPropCount; ++i) {
        const ScriptPropertyEntry& prop = s_scriptProps[i];
        if (!prop.ownerType || !entry.typeName) continue;
        if (std::strcmp(prop.ownerType, entry.typeName) != 0) continue;
        cb->propCallback(&prop.desc);
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
