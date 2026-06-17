#include "EditorWindowPrefs.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace {

    std::string Trim(const std::string& value)
    {
        const auto first = std::find_if_not(value.begin(), value.end(),
            [](unsigned char character) {
                return std::isspace(character) != 0;
            });

        if (first == value.end()) {
            return {};
        }

        const auto last = std::find_if_not(value.rbegin(), value.rend(),
            [](unsigned char character) {
                return std::isspace(character) != 0;
            }).base();

        return std::string(first, last);
    }

    std::filesystem::path GetLocalAppDataDirectory()
    {
        char* value = nullptr;
        size_t valueSize = 0;
        if (_dupenv_s(&value, &valueSize, "LOCALAPPDATA") != 0 || !value) {
            return std::filesystem::current_path();
        }

        std::string result(value);
        std::free(value);
        return std::filesystem::path(result);
    }

    bool ParseBool(const std::string& value, bool defaultValue)
    {
        const std::string normalized = Trim(value);
        if (normalized == "1" || normalized == "true" || normalized == "True") {
            return true;
        }

        if (normalized == "0" || normalized == "false" || normalized == "False") {
            return false;
        }

        return defaultValue;
    }

    int ParseInt(const std::string& value, int defaultValue)
    {
        if (value.empty()) {
            return defaultValue;
        }

        try {
            return std::stoi(value);
        }
        catch (...) {
            return defaultValue;
        }
    }

    float ParseFloat(const std::string& value, float defaultValue)
    {
        if (value.empty()) {
            return defaultValue;
        }

        try {
            return std::stof(value);
        }
        catch (...) {
            return defaultValue;
        }
    }

    std::string ExtractJsonStringValue(const std::string& json, const std::string& key)
    {
        const std::string needle = "\"" + key + "\"";
        const size_t keyPos = json.find(needle);
        if (keyPos == std::string::npos) {
            return {};
        }

        const size_t colonPos = json.find(':', keyPos + needle.size());
        if (colonPos == std::string::npos) {
            return {};
        }

        const size_t valueStart = json.find_first_not_of(" \t\r\n", colonPos + 1);
        if (valueStart == std::string::npos) {
            return {};
        }

        if (json[valueStart] == '"') {
            const size_t valueEnd = json.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) {
                return {};
            }
            return json.substr(valueStart + 1, valueEnd - valueStart - 1);
        }

        const size_t valueEnd = json.find_first_of(",}\r\n", valueStart);
        if (valueEnd == std::string::npos) {
            return json.substr(valueStart);
        }

        return Trim(json.substr(valueStart, valueEnd - valueStart));
    }

    std::string ExtractJsonObject(const std::string& json, const std::string& key)
    {
        const std::string needle = "\"" + key + "\"";
        const size_t keyPos = json.find(needle);
        if (keyPos == std::string::npos) {
            return {};
        }

        const size_t objectStart = json.find('{', keyPos + needle.size());
        if (objectStart == std::string::npos) {
            return {};
        }

        int depth = 0;
        for (size_t index = objectStart; index < json.size(); ++index) {
            if (json[index] == '{') {
                ++depth;
            } else if (json[index] == '}') {
                --depth;
                if (depth == 0) {
                    return json.substr(objectStart, index - objectStart + 1);
                }
            }
        }

        return {};
    }

    void WriteBool(std::ostream& out, const char* key, bool value, bool trailingComma)
    {
        out << "  \"" << key << "\": " << (value ? "true" : "false");
        if (trailingComma) {
            out << ',';
        }
        out << '\n';
    }

}

namespace RTBEditor {

    std::filesystem::path EditorWindowPrefs::GetPrefsPath()
    {
        return GetLocalAppDataDirectory() / "RTBEngineEditor" / "EditorWindowPrefs.json";
    }

    void EditorWindowPrefs::LoadInto(EditorContext& context)
    {
        const std::filesystem::path prefsPath = GetPrefsPath();
        std::ifstream file(prefsPath);
        if (!file.is_open()) {
            return;
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();
        const std::string json = buffer.str();
        if (json.empty()) {
            return;
        }

        OptionalWindowState& windows = context.optionalWindows;
        windows.online = ParseBool(ExtractJsonStringValue(json, "online"), windows.online);
        windows.physicsLayers = ParseBool(ExtractJsonStringValue(json, "physicsLayers"), windows.physicsLayers);
        windows.navigationDebug =
            ParseBool(ExtractJsonStringValue(json, "navigationDebug"), windows.navigationDebug);

        const std::string navDebugJson = ExtractJsonObject(json, "navDebug");
        if (navDebugJson.empty()) {
            return;
        }

        NavDebugSettings& navDebug = context.navDebug;
        navDebug.enabled = ParseBool(ExtractJsonStringValue(navDebugJson, "enabled"), navDebug.enabled);
        navDebug.showBounds = ParseBool(ExtractJsonStringValue(navDebugJson, "showBounds"), navDebug.showBounds);
        navDebug.showWalkableCells =
            ParseBool(ExtractJsonStringValue(navDebugJson, "showWalkableCells"), navDebug.showWalkableCells);
        navDebug.showBlockedCells =
            ParseBool(ExtractJsonStringValue(navDebugJson, "showBlockedCells"), navDebug.showBlockedCells);
        navDebug.showAgentPaths =
            ParseBool(ExtractJsonStringValue(navDebugJson, "showAgentPaths"), navDebug.showAgentPaths);
        navDebug.gridCellStep = ParseInt(ExtractJsonStringValue(navDebugJson, "gridCellStep"), navDebug.gridCellStep);
        navDebug.yOffset = ParseFloat(ExtractJsonStringValue(navDebugJson, "yOffset"), navDebug.yOffset);
    }

    void EditorWindowPrefs::SaveFrom(const EditorContext& context)
    {
        const std::filesystem::path prefsPath = GetPrefsPath();
        std::error_code errorCode;
        std::filesystem::create_directories(prefsPath.parent_path(), errorCode);

        std::ofstream file(prefsPath, std::ios::trunc);
        if (!file.is_open()) {
            return;
        }

        const OptionalWindowState& windows = context.optionalWindows;
        const NavDebugSettings& navDebug = context.navDebug;

        file << "{\n";
        WriteBool(file, "online", windows.online, true);
        WriteBool(file, "physicsLayers", windows.physicsLayers, true);
        WriteBool(file, "navigationDebug", windows.navigationDebug, true);
        file << "  \"navDebug\": {\n";
        WriteBool(file, "enabled", navDebug.enabled, true);
        WriteBool(file, "showBounds", navDebug.showBounds, true);
        WriteBool(file, "showWalkableCells", navDebug.showWalkableCells, true);
        WriteBool(file, "showBlockedCells", navDebug.showBlockedCells, true);
        WriteBool(file, "showAgentPaths", navDebug.showAgentPaths, true);
        file << "    \"gridCellStep\": " << navDebug.gridCellStep << ",\n";
        file << "    \"yOffset\": " << navDebug.yOffset << "\n";
        file << "  }\n";
        file << "}\n";
    }

}
