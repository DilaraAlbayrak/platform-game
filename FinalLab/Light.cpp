#include "Light.h"

void Light::init(ID3D11Device* device)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
    bufferDesc.ByteWidth = sizeof(LightBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 

    const HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &_constantBuffer);
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to create constant buffer for Light\n");
    }
}

void Light::render(ID3D11DeviceContext* context, UINT registerSlot) const
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    const HRESULT hr = context->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        memcpy(mappedResource.pData, &_buffer, sizeof(LightBuffer));
        context->Unmap(_constantBuffer, 0); 
    }
    else
    {
        OutputDebugStringA("Failed to map constant buffer in Light::render\n");
    }

    context->VSSetConstantBuffers(registerSlot, 1, &_constantBuffer.p);
    context->PSSetConstantBuffers(registerSlot, 1, &_constantBuffer.p);
}
