#pragma once
#include "GameObject.h"

class Tree : public GameObject
{
private:
	OBJModel _geometry;

	void bindBuffers(ID3D11DeviceContext* context) const override;

public:
	explicit Tree(const OBJModel& geometry)
		: _geometry(geometry)
	{
		convertIndices(_geometry.faces);
		_geometry.indices = getIndices();
		ModelLoader::randomizeModelColors(_geometry);
	}

	const OBJModel& getGeometry() const { return _geometry; }

	void init(ID3D11Device* device, ID3D11DeviceContext* context) override;

	void render(ID3D11DeviceContext* context, float time = 0.0f) override;

	~Tree();
};