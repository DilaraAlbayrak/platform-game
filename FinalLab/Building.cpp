#include "Building.h"
#include "DDSTextureLoader.h"

//--------------------------------------------------------------------------------------
// NOT being used in the final project
//--------------------------------------------------------------------------------------

void Building::init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

	for (const auto& model : _geometry)
	{
        UINT byteWidthV = sizeof(TexturedVertex) * model.texturedVertices.size();
        D3D11_SUBRESOURCE_DATA InitDataV = {};
        InitDataV.pSysMem = model.texturedVertices.data();
        const auto stride = static_cast<UINT>(sizeof(TexturedVertex));

        UINT byteWidthI = sizeof(WORD) * model.indices.size();
        D3D11_SUBRESOURCE_DATA InitDataI = {};
        InitDataI.pSysMem = model.indices.data();

		initResources(device, context, InitDataV, InitDataI, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, layout, ARRAYSIZE(layout), byteWidthV, byteWidthI, stride, L"Building.fx");
	}

    initTextureResources(device, context);
}

void Building::render(ID3D11DeviceContext* context, float time)
{
    const ConstantBuffer cbPlatform{
        DirectX::XMMatrixTranspose(getWorldMatrix())
    };

    ID3D11Buffer* const constantBuffer = getConstantBuffer();
    context->UpdateSubresource(constantBuffer, 0, nullptr, &cbPlatform, 0, 0);
    context->VSSetConstantBuffers(4, 1, &constantBuffer); // register b4

    // Set the primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    context->VSSetShader(getVertexShader(), nullptr, 0);
    context->PSSetShader(getPixelShader(), nullptr, 0);
    bindBuffers(context);
    std::vector<UINT> textureSlots = { 5, 6, 7 };
    updateTextureResources(context, textureSlots, 3);
    context->DrawIndexed(getIndexCount(), 0, 0);
}

void Building::bindBuffers(ID3D11DeviceContext* context) const
{
	const UINT stride = sizeof(TexturedVertex);
	bind(context, stride);
}

Building::~Building() = default;
