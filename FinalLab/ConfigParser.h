#pragma once
#include <string>
#include <unordered_map>
#include <cassert>
#include "OBJModel.h"
#include <memory>
#include <atlbase.h>

struct LightBuffer
{
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT3 direction;
    float intensity;
    DirectX::XMFLOAT3 position;
    float range;
    DirectX::XMFLOAT3 attenuation;
    float padding = 0.0f;
};

class ConfigParser final
{
private:
    static std::unique_ptr<ConfigParser> _instance;
    static std::unordered_map<std::string, std::string> _config;
    static std::vector<XMFLOAT3> parseNode(const std::string& baseKey, const std::vector<std::string>& keys);
    void loadConfig(const std::string& filePath = "scene.config") const;
    ConfigParser() = default;

public:
    ConfigParser(const ConfigParser&) = delete;
    ConfigParser& operator=(const ConfigParser&) = delete;
    ~ConfigParser();
    static ConfigParser* getInstance();

    std::string getModelPath(const std::string& baseKey) const;
    std::vector<std::wstring> getTexturePaths(const std::string& baseKey) const;
    std::vector<XMFLOAT3> getTransform(const std::string& baseKey = "PlayerTransform") const;
    std::vector<XMFLOAT3> getCameraMatrix(const std::string& baseKey = "CameraTransform") const;
    LightBuffer& getLightData(const std::string& baseKey = "DirectionalLight") const;
};