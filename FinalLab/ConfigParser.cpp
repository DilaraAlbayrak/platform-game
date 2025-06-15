#include "ConfigParser.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cassert>
#include <vector> 

std::unordered_map<std::string, std::string> ConfigParser::_config;
std::unique_ptr<ConfigParser> ConfigParser::_instance = nullptr;

ConfigParser* ConfigParser::getInstance() {
    if (!_instance) {
        _instance = std::unique_ptr<ConfigParser>(new ConfigParser());
        _instance->loadConfig();
    }
    return _instance.get();
}

void ConfigParser::loadConfig(const std::string& filePath) const {
    _config.clear();

    std::ifstream file(filePath);
    assert(file.is_open() && "Failed to open configuration file!");

    std::string line;
    std::string currentNode;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '[' && line.back() == ']') {
            currentNode = line.substr(1, line.size() - 2); // Extract node name
            continue;
        }

        std::istringstream lineStream(line);
        std::string key, value;
        if (std::getline(lineStream, key, '=') && std::getline(lineStream, value)) {
            if (!currentNode.empty()) {
                _config[currentNode + "." + key] = value;
            }
        }
    }
}

std::string ConfigParser::getModelPath(const std::string& baseKey) const
{
    const auto it = _config.find(baseKey);
    assert(it != _config.end() && "Key not found in configuration for ");
    return it->second;
    return std::string();
}

std::vector<std::wstring> ConfigParser::getTexturePaths(const std::string& baseKey) const
{
    std::vector<std::string> orderedKeys = { "color_map", "normal_map", "height_map" };
    std::vector<std::wstring> texturePaths;

    for (const auto& key : orderedKeys) {
        const std::string fullKey = baseKey + "." + key;
        const auto it = _config.find(fullKey);
        if (it != _config.end()) {
            texturePaths.push_back(std::wstring(it->second.begin(), it->second.end()));
        }
    }
    return texturePaths;
}

std::vector<XMFLOAT3> ConfigParser::getTransform(const std::string& baseKey) const
{
    const std::vector<std::string> keys = { "position", "rotation", "scale" };

    return parseNode(baseKey, keys);
}

std::vector<XMFLOAT3> ConfigParser::getCameraMatrix(const std::string& baseKey) const
{
    const std::vector<std::string> keys = { "eye", "at", "up" };

    return parseNode(baseKey, keys);
}

LightBuffer& ConfigParser::getLightData(const std::string& baseKey) const
{
    LightBuffer lightData = {};

    // Parse color
    const auto colorIt = _config.find(baseKey + ".color");
    if (colorIt != _config.end()) {
        std::istringstream colorStream(colorIt->second);
        std::string component;
        std::vector<float> colorValues;
        while (std::getline(colorStream, component, ',')) {
            colorValues.push_back(std::stof(component));
        }
        if (colorValues.size() == 4) {
            lightData.color = { colorValues[0], colorValues[1], colorValues[2], colorValues[3] };
        }
    }

    // Parse direction
    const auto directionIt = _config.find(baseKey + ".direction");
    if (directionIt != _config.end()) {
        std::istringstream directionStream(directionIt->second);
        std::string component;
        std::vector<float> directionValues;
        while (std::getline(directionStream, component, ',')) {
            directionValues.push_back(std::stof(component));
        }
        if (directionValues.size() == 3) {
            lightData.direction = { directionValues[0], directionValues[1], directionValues[2] };
        }
    }

    // Parse position
    const auto positionIt = _config.find(baseKey + ".position");
    if (positionIt != _config.end()) {
        std::istringstream positionStream(positionIt->second);
        std::string component;
        std::vector<float> positionValues;
        while (std::getline(positionStream, component, ',')) {
            positionValues.push_back(std::stof(component));
        }
        if (positionValues.size() == 3) {
            lightData.position = { positionValues[0], positionValues[1], positionValues[2] };
        }
    }

    // Parse attenuation
    const auto attenuationIt = _config.find(baseKey + ".attenuation");
    if (attenuationIt != _config.end()) {
        std::istringstream attenuationStream(attenuationIt->second);
        std::string component;
        std::vector<float> attenuationValues;
        while (std::getline(attenuationStream, component, ',')) {
            attenuationValues.push_back(std::stof(component));
        }
        if (attenuationValues.size() == 3) {
            lightData.attenuation = { attenuationValues[0], attenuationValues[1], attenuationValues[2] };
        }
    }

    // Parse intensity
    const auto intensityIt = _config.find(baseKey + ".intensity");
    if (intensityIt != _config.end()) {
        lightData.intensity = std::stof(intensityIt->second);
    }

    // Parse range
    const auto rangeIt = _config.find(baseKey + ".range");
    if (rangeIt != _config.end()) {
        lightData.range = std::stof(rangeIt->second);
    }

    LightBuffer* lightBuffer = &lightData;
    return *lightBuffer;
}

std::vector<XMFLOAT3> ConfigParser::parseNode(const std::string& baseKey, const std::vector<std::string>& keys)
{
    std::vector<XMFLOAT3> transformData;

    for (const auto& subKey : keys) {
        const std::string fullKey = baseKey + "." + subKey;
        const auto it = _config.find(fullKey);
        if (it != _config.end()) {
            const std::string& value = it->second; // Reference to value
            std::istringstream valueStream(value);
            std::string component;
            std::vector<float> values;

            // Parse comma-separated floats
            while (std::getline(valueStream, component, ','))
            {
                values.push_back(std::stof(component));
            }

            const XMFLOAT3 transform = { values[0], values[1], values[2] };

            // Add parsed floats to transform data
            transformData.push_back(transform);
        }
    }

    return transformData;
}

ConfigParser::~ConfigParser() = default;