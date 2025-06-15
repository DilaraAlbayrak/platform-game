#pragma once
#include "GameObject.h"

//--------------------------------------------------------------------------------------
// NOT being used in the final project
//--------------------------------------------------------------------------------------

class Building : public GameObject
{
private:
	std::vector<OBJModel> _geometry;

	std::vector<TexturedVertex> _vertices;

	void bindBuffers(ID3D11DeviceContext* context) const override;

public:
	explicit Building(const std::vector<OBJModel>& model, const std::vector<std::wstring>& texturePaths)
		: _geometry(model)
	{
		setScale(XMFLOAT3(0.3f, 0.3f, 0.3f));
		setRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		setTexturePaths(texturePaths);
	}

	void setVertices(std::vector<TexturedVertex>& vertices) { _vertices = vertices; }
	
	void init(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void render(ID3D11DeviceContext* context, float time = 0.0f) override;

	~Building();
};