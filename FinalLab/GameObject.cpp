#include "GameObject.h"
#include "DDSTextureLoader.h"
#include <iterator>

void GameObject::initResources(ID3D11Device* device, ID3D11DeviceContext* context, const D3D11_SUBRESOURCE_DATA& verticesInitData, const D3D11_SUBRESOURCE_DATA& indicesInitData, D3D_PRIMITIVE_TOPOLOGY topology, const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutSize, UINT byteWidthVertices, UINT byteWidthIndices, const UINT stride, const std::wstring& shaderFile)
{
	const auto numElements = layoutSize;

	CComPtr <ID3DBlob> pVSBlob;
	ShaderManager::getInstance(device)->compileShaderFromFile(shaderFile.c_str(), "VS", "vs_5_0", &pVSBlob);

	CComPtr <ID3D11VertexShader> _pVertexShader;
	device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	CComPtr <ID3D11InputLayout> _pVertexLayout;
	device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &_pVertexLayout);

	context->IASetInputLayout(_pVertexLayout);

	CComPtr <ID3DBlob> pPSBlob;
	ShaderManager::getInstance(device)->compileShaderFromFile(shaderFile.c_str(), "PS", "ps_5_0", &pPSBlob);

	CComPtr <ID3D11PixelShader> _pPixelShader;
	device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);

	CComPtr <ID3D11Buffer> _pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = byteWidthVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	device->CreateBuffer(&bd, &verticesInitData, &_pVertexBuffer);

	// Set vertex buffer
	const auto offset = static_cast<UINT>(0);
	context->IASetVertexBuffers(0, 1, &_pVertexBuffer.p, &stride, &offset);

	CComPtr <ID3D11Buffer> _pIndexBuffer;
	bd.Usage = D3D11_USAGE_DEFAULT;

	bd.ByteWidth = byteWidthIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, &indicesInitData, &_pIndexBuffer);
	context->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Create constant buffer for world matrix
	// updated every frame for every scene object
	CComPtr <ID3D11Buffer> _pConstantBuffer;
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.ByteWidth = sizeof(ConstantBuffer);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	device->CreateBuffer(&cbDesc, nullptr, &_pConstantBuffer);

	// Set primitive topology
	context->IASetPrimitiveTopology(topology);

	setVertexShader(_pVertexShader);
	setPixelShader(_pPixelShader);
	setVertexLayout(_pVertexLayout);
	setVertexBuffer(_pVertexBuffer);
	setIndexBuffer(_pIndexBuffer, _indices.size());
	setConstantBuffer(_pConstantBuffer);
}

void GameObject::initNoTextureObject(ID3D11Device* device, ID3D11DeviceContext* context, const OBJModel& geometry, const std::wstring& shaderFile)
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = createLayout();

    const auto numElements = layout.size();
    const UINT byteWidthVertices = sizeof(SimpleVertex) * geometry.vertices.size();
    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = geometry.vertices.data();
    const auto stride = static_cast<UINT>(sizeof(SimpleVertex));

    const UINT byteWidthI = sizeof(WORD) * getIndices().size();
    D3D11_SUBRESOURCE_DATA InitDataI = {};
    InitDataI.pSysMem = getIndices().data();

    initResources(device, context, InitData, InitDataI, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, layout.data(), numElements, byteWidthVertices, byteWidthI, stride, shaderFile);
    setVertexCount(geometry.vertices.size());
}

void GameObject::initTextureResources(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Load the Textures
	CreateDDSTextureFromFile(device, context, _texturePaths[0].c_str(), nullptr, &_pColorTextureRV);
	CreateDDSTextureFromFile(device, context, _texturePaths[1].c_str(), nullptr, &_pNormalTextureRV);
	CreateDDSTextureFromFile(device, context, _texturePaths[2].c_str(), nullptr, &_pHeightTextureRV);

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, &_pTextureSampler);
}

void GameObject::updateTextureResources(ID3D11DeviceContext* context, std::vector<UINT>& textureSlots, UINT samplerSlot) const
{
	context->PSSetShaderResources(textureSlots[0], 1, &_pColorTextureRV.p);
	context->PSSetShaderResources(textureSlots[1], 1, &_pNormalTextureRV.p);
	context->PSSetShaderResources(textureSlots[2], 1, &_pHeightTextureRV.p);
	context->PSSetSamplers(samplerSlot, 1, &_pTextureSampler.p);
}

void GameObject::bind(ID3D11DeviceContext* context, const UINT stride) const
{
	context->IASetInputLayout(getVertexLayout());

	// Bind the vertex buffer // Adjust according to the size of your vertex structure
	const UINT offset = 0;
	ID3D11Buffer* const vertexBuffer = getVertexBuffer();
	assert(vertexBuffer && "vertex buffer is not initialised!");

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Bind the index buffer
	context->IASetIndexBuffer(getIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);
}

void GameObject::convertIndices(const std::vector<Face>& faces)
{
	std::vector<WORD> convertedIndices;
	for (const auto& face : faces) {
		std::copy(face.vertexIndices.begin(), face.vertexIndices.end(), std::back_inserter(convertedIndices));
	}
	setIndices(convertedIndices);
}

// parasoft doesn't accept inline or =default(in header)
GameObject::~GameObject() = default;