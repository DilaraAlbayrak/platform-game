#include "Rain.h"

void Rain::init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    // Vertex Buffer Setup
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(Particle) * _maxParticles;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &_vertexBuffer);

    // Shader Setup
    CComPtr<ID3DBlob> vsBlob, psBlob;
    ShaderManager::getInstance(device)->compileShaderFromFile(L"RainParticle.fx", "VS", "vs_5_0", &vsBlob);
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &_vertexShader);

    ShaderManager::getInstance(device)->compileShaderFromFile(L"RainParticle.fx", "PS", "ps_5_0", &psBlob);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &_pixelShader);

    // Input Layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &_inputLayout);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    // Depth Stencil State
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = false; // No depth testing for particles
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    device->CreateDepthStencilState(&dsDesc, &_depthStencilState);

    // Setup Vertex Buffer
    UINT stride = sizeof(Particle);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &_vertexBuffer.p, &stride, &offset);
    context->IASetInputLayout(_inputLayout);
}

void Rain::spawnParticle()
{
    if (_particles.size() < _maxParticles)
    {
        Particle particle = {};
        particle.position = _emitterPosition;
        particle.velocity = XMFLOAT3(0.0f, -5.0f, 0.0f);
        particle.lifetime = 5.0f; // Lifetime in seconds

        _particles.push_back(particle);
    }
}

void Rain::update(ID3D11Device* device, ID3D11DeviceContext* context, float deltaTime)
{
    // Spawn new particles
    _timeAccumulator += deltaTime;
    while (_timeAccumulator > _spawnRate)
    {
        spawnParticle();
        _timeAccumulator -= _spawnRate;
    }

    // Update particle positions
    for (auto& particle : _particles)
    {
        particle.position.x += particle.velocity.x * deltaTime;
        particle.position.y += particle.velocity.y * deltaTime;
        particle.position.z += particle.velocity.z * deltaTime;
        particle.lifetime -= deltaTime;
    }

    // Remove dead particles
    _particles.erase(std::remove_if(_particles.begin(), _particles.end(), [](const Particle& p) { return p.lifetime <= 0.0f; }), _particles.end());

    // Update vertex buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, _particles.data(), sizeof(Particle) * _particles.size());
    context->Unmap(_vertexBuffer, 0);
}

void Rain::render(ID3D11Device* device, ID3D11DeviceContext* context, float deltaTime) {
   
    update(device, context, deltaTime);


    UINT stride = sizeof(Particle);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &_vertexBuffer.p, &stride, &offset);
    context->IASetInputLayout(_inputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    context->VSSetShader(_vertexShader, nullptr, 0);
    context->PSSetShader(_pixelShader, nullptr, 0);

    context->Draw(static_cast<UINT>(_particles.size()), 0);
}
