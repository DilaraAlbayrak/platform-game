#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "ShaderManager.h"

using namespace DirectX;

struct Particle
{
    XMFLOAT3 position;
    XMFLOAT3 velocity;
    float lifetime;
};

class Rain final
{
private:
    std::vector<Particle> _particles;
    CComPtr<ID3D11Buffer> _vertexBuffer;
    CComPtr<ID3D11Buffer> _constantBuffer;
    CComPtr<ID3D11VertexShader> _vertexShader;
    CComPtr<ID3D11PixelShader> _pixelShader;
    CComPtr<ID3D11InputLayout> _inputLayout;
    CComPtr <ID3D11DepthStencilState> _depthStencilState;

    XMFLOAT3 _emitterPosition;
    UINT _maxParticles = 1000;
    float _spawnRate = 0.01f;
    float _timeAccumulator = 0.0f;

	void spawnParticle();
    void update(ID3D11Device* device, ID3D11DeviceContext* context, float deltaTime);

public:
    explicit Rain(const XMFLOAT3& emitterPosition= XMFLOAT3(0.0f, 10.0f, 0.2f))
        : _emitterPosition(emitterPosition) { }

    void init(ID3D11Device* device, ID3D11DeviceContext* context);
	void render(ID3D11Device* device, ID3D11DeviceContext* context, float deltaTime);

	~Rain() = default;
};

