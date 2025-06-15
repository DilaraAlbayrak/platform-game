#pragma once
#include "GameObject.h"
//#include "Building.h"

class Platform : public GameObject
{
private:
	std::vector<TexturedVertex> _vertices;
	static const float _width;
	//std::shared_ptr<Building> _building;
	void initialiseGeometry();
	void initPlatform(ID3D11Device* device, ID3D11DeviceContext* context);
	void bindBuffers(ID3D11DeviceContext* context) const override;

public:
	explicit Platform(const std::vector<std::wstring>& texturePaths)
		: GameObject(texturePaths)
	{
		initialiseGeometry();
		setScale(XMFLOAT3(3.0f, 3.2f, 0.25f));
	}
	void init(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void render(ID3D11DeviceContext* context, float time = 0.0f) override;

	static const float getWidth() { return _width; }

	~Platform();
};