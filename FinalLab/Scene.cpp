#include "Scene.h"
#include <random>

OBJModel Scene::_treeModel = OBJModel();

void Scene::createPlatforms()
{
	std::vector<std::wstring> texturePaths = ConfigParser::getInstance()->getTexturePaths("PlatformTextures");

	const float scaleX = Platform::getWidth();

	// middle platforms (scene opening) - no gap
	_platforms.push_back(std::make_shared<Platform>(texturePaths));
	_platforms.back().get()->setPosition(XMFLOAT3(scaleX * -7.0f, -5.2f, 0.2f));

	for (int i = -5; i < 7; i += 2)
	{
		_platforms.push_back(std::make_shared<Platform>(texturePaths));
		_platforms.back().get()->setPosition(XMFLOAT3(scaleX * i, -5.2f, 0.2f));
	}

	// left platforms
	for (int i = -9; i > -100; i -= 2)
	{
		_platforms.insert(_platforms.begin(), std::make_shared<Platform>(texturePaths));
		_platforms.front().get()->setPosition(XMFLOAT3(scaleX * i + ((-i % 3) * -3.0f), -5.2f, 0.2f));
		if ((-i % 3) > 1)
		{
			float left = _platforms.at(1).get()->getPosition().x - scaleX;
			float right = _platforms.front().get()->getPosition().x + scaleX;
			_gaps.push_back(std::make_shared<Gap>(left, right));
		}
	}
	std::reverse(_gaps.begin(), _gaps.end());
	_gaps.push_back(std::make_shared<Gap>(18.0f, 21.0f));
	// right platforms
	for (int i = 7; i < 100; i += 2)
	{
		if ((i % 3) > 1)
		{
			float left = _platforms.back().get()->getPosition().x + scaleX;
			float right = scaleX * i + ((i % 3) * 3.0f) - scaleX;
			_gaps.push_back(std::make_shared<Gap>(left, right));
		}
		_platforms.push_back(std::make_shared<Platform>(texturePaths));
		_platforms.back().get()->setPosition(XMFLOAT3(scaleX * i + ((i % 3) * 3.0f), -5.2f, 0.2f));

	}

	_leftBoundX = _platforms.front().get()->getPosition().x - scaleX;
	_rightBoundX = _platforms.back().get()->getPosition().x + scaleX;

	/*for (const auto& gap : _gaps)
	{
		OutputDebugStringA(("Gap: " + std::to_string(gap->left) + " - " + std::to_string(gap->right) + "\n").c_str());
	}*/
}

void Scene::createTrees()
{
	if (_treeModel.vertices.empty()) {
		_treeModel = ModelLoader::loadTreeModel();
	}

	// Random number generators
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> gapDist(2.0f, 7.0f); // Random gap between trees
	std::uniform_real_distribution<float> yDist(-18.0f, -15.0f);
	std::uniform_real_distribution<float> zDist(8.0f, 10.0f); // Random z position
	std::uniform_real_distribution<float> scaleDist(2.0f, 4.5f); // Random uniform scale
	std::uniform_real_distribution<float> rotationYDist(0.0f, DirectX::XM_2PI); // Random rotation around y-axis

	float currentX = _leftBoundX;

	while (currentX < _rightBoundX) {
		const std::shared_ptr<Tree> tree = std::make_shared<Tree>(_treeModel);

		// Random properties
		const float randomGap = gapDist(gen);
		const float randomY = yDist(gen);
		const float randomZ = zDist(gen);
		const float randomScale = scaleDist(gen);
		const float randomRotationY = rotationYDist(gen);

		// Set tree properties
		tree->setPosition(XMFLOAT3(currentX, randomY, randomZ));
		tree->setScale(XMFLOAT3(randomScale, randomScale, randomScale));
		tree->setRotation(XMFLOAT3(0.0f, randomRotationY, 0.0f));
		tree->updateWorldMatrix();

		// Add to vector
		_trees.push_back(tree);

		// Move currentX forward by the scale and gap
		currentX += randomScale + randomGap;
	}
}

void Scene::createLights()
{
	_ambientLight = std::make_shared<Light>(LightType::Ambient, ConfigParser::getInstance()->getLightData("AmbientLight"));
	_directionalLight = std::make_shared<Light>(LightType::Directional, ConfigParser::getInstance()->getLightData("DirectionalLight"));
	_spotLight = std::make_shared<Light>(LightType::Spot, ConfigParser::getInstance()->getLightData("SpotLight"));
}

void Scene::init() const
{
	_directionalLight->init(_device);
	_ambientLight->init(_device);
	_spotLight->init(_device);

	_player->init(_device, _context);
	for (auto& platform : _platforms)
	{
		platform->init(_device, _context);
	}
	for (auto& tree : _trees)
	{
		tree->init(_device, _context);
	}
	_rain->init(_device, _context);
}

void Scene::updateSpotLight() const
{
	LightBuffer buffer = _spotLight->getBuffer();

	// Update the spot light position to follow the player
	const XMFLOAT3 playerPosition = _player->getPosition();
	const float directionX = _player->getLookDirection() == LookDirection::RIGHT ? 1.0f : -1.0f;
	buffer.direction = XMFLOAT3(directionX, -1.0f, 0.0f);
	buffer.position = XMFLOAT3(playerPosition.x * directionX, playerPosition.y + 3.0, playerPosition.z);

	_spotLight->setBuffer(buffer);
}