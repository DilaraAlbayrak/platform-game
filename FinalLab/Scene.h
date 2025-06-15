#pragma once
#include <memory>
#include "Player.h"
#include "Platform.h"
#include "Tree.h"
#include "Light.h"
#include "Rain.h"

class Scene final {
private:
    ID3D11Device* _device = nullptr;
    ID3D11DeviceContext* _context = nullptr;
    std::shared_ptr<Player> _player;
    std::shared_ptr<Light> _ambientLight;
    std::shared_ptr<Light> _directionalLight;
    std::shared_ptr<Light> _spotLight;
	std::shared_ptr<Rain> _rain;

    std::vector<std::shared_ptr<Platform>> _platforms;
    std::vector<std::shared_ptr<Tree>> _trees;
    std::vector<std::shared_ptr<Gap>> _gaps; // a helper type to apply player fall

    // to avoid loading the same model multiple times (it's a huge model)
    // if I had multiple buildings
    // I would have used multiple models
    //static std::vector<OBJModel> _buildingModel;
    static OBJModel _treeModel;

    float _leftBoundX = 0.0f;
    float _rightBoundX = 0.0f;

    void createPlatforms();
    //std::shared_ptr<Building> createBuilding(const XMFLOAT3& position);
    void createTrees();
    void createLights();

public:
    Scene(ID3D11Device* device, ID3D11DeviceContext* context)
        : _device(device),
        _context(context),
        _player(std::make_shared<Player>())
    {
		_rain = std::make_shared<Rain>();
        createLights();
        createPlatforms();
        createTrees();
    }

    Scene(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene& operator=(Scene&&) = delete;

    void init() const;

    void setDevice(ID3D11Device* device) { _device = device; }
    void setContext(ID3D11DeviceContext* context) { _context = context; }

    const std::shared_ptr<Light>& getAmbientLight() const { return _ambientLight; }
    const std::shared_ptr<Light>& getDirectionalLight() const { return _directionalLight; }
    const std::shared_ptr<Light>& getSpotLight() const { return _spotLight; }

    std::shared_ptr<Player> getPlayer() const { return _player; }
	std::shared_ptr<Rain> getRain() const { return _rain; }
    const std::vector<std::shared_ptr<Platform>>& getPlatforms() const { return _platforms; }
    const std::vector<std::shared_ptr<Tree>>& getTrees() const { return _trees; }
    const std::vector<std::shared_ptr<Gap>>& getGaps() const { return _gaps; }

    void updateSpotLight() const;

    ~Scene() = default;
};