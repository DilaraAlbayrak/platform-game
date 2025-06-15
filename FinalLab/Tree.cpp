#include "Tree.h"
#include <iterator>


void Tree::init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    initNoTextureObject(device, context, _geometry, L"Tree.fx");
}

void Tree::render(ID3D11DeviceContext* context, float time)
{
    const ConstantBuffer cbTree{
        DirectX::XMMatrixTranspose(getWorldMatrix())
    };

    ID3D11Buffer* const constantBuffer = getConstantBuffer();
    context->UpdateSubresource(constantBuffer, 0, nullptr, &cbTree, 0, 0);
    context->VSSetConstantBuffers(4, 1, &constantBuffer); // register b4

    // Set the primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(getVertexShader(), nullptr, 0);
    context->PSSetShader(getPixelShader(), nullptr, 0);
    bindBuffers(context);
    context->DrawIndexed(getIndexCount(), 0, 0);
}

void Tree::bindBuffers(ID3D11DeviceContext* context) const
{
    const UINT stride = sizeof(SimpleVertex);
    bind(context, stride);
}

Tree::~Tree() = default;