// parasoft-begin-suppress ALL "suppress all violations"

#include <directxcolors.h>
#include "Resource.h"
#include "D3DFramework.h"
#include "DDSTextureLoader.h"
// parasoft-end-suppress ALL "suppress all violations"

std::unique_ptr<D3DFramework> D3DFramework::_instance = std::make_unique<D3DFramework>();
float D3DFramework::time = 0.0f;

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK D3DFramework::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	
	std::string msg;
	auto& app = D3DFramework::getInstance();

	switch (message)
	{
		PAINTSTRUCT ps;
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	{
		const auto player = app._player;
		player->setLookVertical(false);
		switch (wParam) {
		case 'A':
			//msg = "A pressed";
			if (player->getState() != PlayerState::DEAD) {
				if (player->getLookDirection() != LookDirection::LEFT) {
					player->setRotation(XMFLOAT3(player->getRotation().x, -0.3f, player->getRotation().z));
				}
				player->setLookDirection(LookDirection::LEFT);
				//player->incrementJumpX(-increment);
				player->setState(PlayerState::MOVE);
				if (app._playerPosition.x - player.get()->getPosition().x > 5.0f)
				{
					app._playerPosition = player.get()->getPosition();
					app._platformsMinIndex = (std::max)(0, app._platformsMinIndex - 1);
					app._platformsMaxIndex = (std::max)(0, app._platformsMaxIndex - 1);
					app._treesMinIndex = (std::max)(0, app._treesMinIndex - 1);
					app._treesMaxIndex = (std::max)(0, app._treesMaxIndex - 1);
				}
			}
			break;
		case 'D':
			//msg = "D pressed";
			if (player->getState() != PlayerState::DEAD) {
				if (player->getLookDirection() != LookDirection::RIGHT) {
					player->setRotation(XMFLOAT3(player->getRotation().x, 3.14f, player->getRotation().z));
					//player->setInitialJumpX(0.0f);
				}
				player->setState(PlayerState::MOVE);
				player->setLookDirection(LookDirection::RIGHT);
				//player->incrementJumpX(increment);
				if (app._playerPosition.x - player.get()->getPosition().x < -5.0f)
				{
					app._playerPosition = player.get()->getPosition();
					app._platformsMaxIndex = (std::min)(app._platformsMaxIndex + 1, static_cast<int>(app._platforms.size()));
					app._platformsMinIndex = (std::min)(app._platformsMinIndex + 1, static_cast<int>(app._platforms.size()));
					app._treesMaxIndex = (std::min)(app._treesMaxIndex + 1, static_cast<int>(app._trees.size()));
					app._treesMinIndex = (std::min)(app._treesMinIndex + 1, static_cast<int>(app._trees.size()));
				}
			}
			break;
		case 'W':
			if (player->getState() != PlayerState::DEAD) {
				if (player->getState() == PlayerState::IDLE)
				{
					player->setVerticalAngle(DirectX::XMConvertToRadians(-45));
					player->setLookVertical(true);
					player->setLookDirection(LookDirection::UP);
				}
				else if (player->getLookDirection() == LookDirection::RIGHT)
				{
					const float increment = app.deltaTime * player->getMoveSpeed() * 2.0f;
					//player->incrementJumpX(increment);
					player->incrementJumpY(increment);
				}
				else if (player->getLookDirection() == LookDirection::RIGHT)
				{
					const float increment = app.deltaTime * player->getMoveSpeed() * 2.0f;
					//player->incrementJumpX(-increment);
					player->incrementJumpY(-increment);
				}
			}
			break;
		case 'S':
			if (player->getState() != PlayerState::DEAD) {
				if (player->getState() == PlayerState::IDLE)
				{
					player->setVerticalAngle(DirectX::XMConvertToRadians(45));
					player->setLookVertical(true);
					player->setLookDirection(LookDirection::DOWN);
				}
			}
			break;
			// case space
		case VK_SPACE:
			if (player->getState() != PlayerState::DEAD) {
				player->jump();
			}
			break;
		case VK_LEFT:
			msg = "Left cursor pressed";
			break;
		case VK_F1:
			msg = "F1 pressed";
			break;
		default:
			break;
		}
		//OutputDebugStringA((msg + "\n").c_str());
		break;
	}
	case WM_KEYUP:
		switch (wParam) {
		case 'A':
			msg = "!!!!!!!!! A released";
			if (app._player) {
				app._player->setState(PlayerState::IDLE);
			}
			break;
		case 'D':
			msg = "!!!!!!!!! D released";
			if (app._player) {
				app._player->setState(PlayerState::IDLE);
			}
			break;
		case 'W':
			msg = "!!!!!!!!! W released";
			if (auto player = app._player) {
				player->setState(PlayerState::IDLE);
				player->setVerticalAngle(DirectX::XMConvertToRadians(0));
				player->setLookVertical(true);
			}
			break;
		case 'S':
			msg = "!!!!!!!!! S released";
			if (auto player = app._player) {
				player->setState(PlayerState::IDLE);
				player->setVerticalAngle(DirectX::XMConvertToRadians(0));
				player->setLookVertical(true);
			}
			break;
		case 'T':
			msg = "!!!!!!!!! T released";
			app._player->toggleTorch();
			break;
		case VK_SPACE:
			msg = "!!!!!!!!! quit jump";
			break;
		case VK_OEM_PLUS:
			app._camera.zoom += 0.02f;
			break;
		case VK_OEM_MINUS:
			app._camera.zoom -= 0.02f;
			break;
		case VK_OEM_COMMA:
			app.deltaTimeFactor = app.deltaTimeFactor > 1.0f ? 1.0 : 0.5f;
			break;
		case VK_OEM_PERIOD:
			app.deltaTimeFactor = app.deltaTimeFactor < 1.0f ? 1.0 : 2.0f;;
			break;
		}
        //OutputDebugStringA((std::to_string(app.deltaTime) + "\n").c_str());
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT D3DFramework::initWindow(HINSTANCE hInstance, int nCmdShow) {
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMULATION));
	wcex.hCursor = LoadCursor(nullptr, MAKEINTRESOURCE(IDC_ARROW)); // Fixed line
	wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(COLOR_WINDOW + 1));
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"Starter Template";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SIMULATION));

	if (!RegisterClassEx(&wcex))
		//return static_cast<HRESULT>(E_FAIL);
		return E_FAIL;

	// Create window
	_hInst = hInstance;
	RECT rc = { 0, 0, _windowWidth, _windowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(wcex.lpszClassName, L"Final Lab",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!_hWnd)
		static_cast<HRESULT>(E_FAIL);

	ShowWindow(_hWnd, nCmdShow);

	return static_cast<HRESULT>(S_OK);
}

void D3DFramework::createSkybox()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	_pd3dDevice->CreateDepthStencilState(&dsDesc, &_pDepthStencilStateSky);
	dsDesc.DepthEnable = true;
	_pd3dDevice->CreateDepthStencilState(&dsDesc, &_pDepthStencilStateCube);

	// Set the rasterizer state
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.ScissorEnable = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	_pd3dDevice->CreateRasterizerState(&rasterDesc, &_pRasterStateSky);

	_pd3dDevice->CreateRasterizerState(&rasterDesc, &_pRasterStateCube);

	// Skybox Shaders
	CComPtr<ID3DBlob> pVSBlob, pPSBlob;
	ShaderManager::getInstance(_pd3dDevice)->compileShaderFromFile(L"Skybox.fx", "VS", "vs_5_0", &pVSBlob);
	_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShaderSky);

	// Define the input layout
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = GameObject::createLayout();
	const UINT numElements = layout.size();

	// Create the input layout
	_pd3dDevice->CreateInputLayout(layout.data(), numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &_pVertexLayout);

	// Set the input layout
	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	ShaderManager::getInstance(_pd3dDevice)->compileShaderFromFile(L"Skybox.fx", "PS", "ps_5_0", &pPSBlob);
	_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShaderSky);

	// for cubemapping
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f)},
		{ XMFLOAT3(1.0f, 1.0f, -1.0f)},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f)},
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f)},
		{ XMFLOAT3(1.0f, -1.0f, -1.0f)},
		{ XMFLOAT3(1.0f, -1.0f, 1.0f)},
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f)},
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	HRESULT hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_skyboxVertexBuffer);
	assert(SUCCEEDED(hr) && "Failed to create vertex buffer!");
	// Set vertex buffer
	const UINT stride = sizeof(SimpleVertex);
	const UINT offset = 0;
	_pImmediateContext->IASetVertexBuffers(0, 1, &_skyboxVertexBuffer.p, &stride, &offset);

	// for cubemapping
	WORD indices[] =
	{
		0, 1, 2,
		0, 2, 3,

		4, 6, 5,
		4, 7, 6,

		4, 0, 3,
		4, 3, 7,

		1, 5, 6,
		1, 6, 2,

		0, 4, 5,
		0, 5, 1,

		3, 2, 6,
		3, 6, 7
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_skyboxIndexBuffer);
	assert(SUCCEEDED(hr) && "Failed to create index buffer!");
	// Set index buffer
	_pImmediateContext->IASetIndexBuffer(_skyboxIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	// Create the constant buffers
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBufferSkybox);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_pd3dDevice->CreateBuffer(&bd, nullptr, &_skyboxConstantBuffer);

	// parasoft-begin-suppress ALL "suppress all violations"
	// Load the Texture
	CreateDDSTextureFromFile(_pd3dDevice, L"textures/skybox.dds", nullptr, &sky_TextureRV);
	// parasoft-end-suppress ALL "suppress all violations"

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc = {};
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	_pd3dDevice->CreateSamplerState(&sampDesc, &sky_Sampler);

	// Initialize the world matrix for the skybox
	_cubemapWorld = XMMatrixIdentity();
	_cubemapWorld = XMMatrixScaling(200.0f, 200.0f, 200.0f);
	_cubemapWorld = XMMatrixRotationZ(XMConvertToRadians(180.0f)) * _cubemapWorld;

	// skybox constant buffer
	const ConstantBufferSkybox cbSkybox{
		DirectX::XMMatrixTranspose(_camera.view),
		DirectX::XMMatrixTranspose(_camera.projection),
		_camera.eye,
		DirectX::XMMatrixTranspose(_cubemapWorld),
	};

	_pImmediateContext->UpdateSubresource(_skyboxConstantBuffer, 0, nullptr, &cbSkybox, 0, 0);
	_pImmediateContext->VSSetConstantBuffers(2, 1, &_skyboxConstantBuffer.p); // register b2
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT D3DFramework::initDevice()
{
	HRESULT hr = static_cast<HRESULT>(S_OK);

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Supported driver types
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	auto numDriverTypes = ARRAYSIZE(driverTypes);

	// Supported feature levels
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	auto numFeatureLevels = static_cast<UINT>(ARRAYSIZE(featureLevels));

	// Attempt to create the device and context
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex) {
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(
			nullptr, _driverType, nullptr, D3D11_CREATE_DEVICE_DEBUG,
			featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
			&_pd3dDevice, &_featureLevel, &_pImmediateContext);

		if (hr == static_cast<HRESULT>(E_INVALIDARG)) {
			// Retry without D3D_FEATURE_LEVEL_11_1 if unsupported
			hr = D3D11CreateDevice(
				nullptr, _driverType, nullptr, D3D11_CREATE_DEVICE_DEBUG,
				&featureLevels[1], numFeatureLevels - 1, D3D11_SDK_VERSION,
				&_pd3dDevice, &_featureLevel, &_pImmediateContext);
		}

		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Obtain DXGI factory
	CComPtr<IDXGIFactory1> dxgiFactory;
	{
		CComPtr<IDXGIDevice> dxgiDevice;
		hr = _pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr)) {
			CComPtr<IDXGIAdapter> adapter;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr)) {
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
			}
		}
	}
	if (FAILED(hr))
		return hr;

	// Create swap chain
	CComPtr<IDXGIFactory2> dxgiFactory2;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));

	// DirectX 11.1 or later
	hr = _pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&_pd3dDevice1));
	if (SUCCEEDED(hr)) {
		static_cast<void>(_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&_pImmediateContext1)));
	}

	DXGI_SWAP_CHAIN_DESC1 sd{};
	sd.Width = _windowWidth;
	sd.Height = _windowHeight;
	sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;

	hr = dxgiFactory2->CreateSwapChainForHwnd(_pd3dDevice, _hWnd, &sd, nullptr, nullptr, &_swapChain1);
	if (SUCCEEDED(hr)) {
		hr = _swapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&_swapChain));
	}

	// Disable Alt+Enter fullscreen shortcut
	dxgiFactory->MakeWindowAssociation(_hWnd, DXGI_MWA_NO_ALT_ENTER);

	if (FAILED(hr))
		return hr;

	// Create render target view
	CComPtr<ID3D11Texture2D> pBackBuffer;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
	if (FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = _windowWidth;
	descDepth.Height = _windowHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &_pDepthStencil);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	_pd3dDevice->CreateDepthStencilView(_pDepthStencil, &descDSV, &_pDepthStencilView);

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView.p, _pDepthStencilView);

	// Setup viewport
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(_windowWidth);
	vp.Height = static_cast<FLOAT>(_windowHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	_pImmediateContext->RSSetViewports(1, &vp);

	// scene and scene elements
	_scene = std::make_unique<Scene>(_pd3dDevice, _pImmediateContext);
	_scene->init();

	_player = _scene->getPlayer();
	_platforms = _scene->getPlatforms();
	_trees = _scene->getTrees();
	_rain = _scene->getRain();

	// additional init arrangements
	initSceneElements();

	// introduce lights
	_directionalLight = _scene->getDirectionalLight();
	_ambientLight = _scene->getAmbientLight();
	_spotLight = _scene->getSpotLight();

	// Create constant buffer for camera matrices
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.ByteWidth = sizeof(ConstantBufferCamera);  
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = _pd3dDevice->CreateBuffer(&cbDesc, nullptr, &_cameraConstantBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	// Initialize the camera
	initCamera();

	// skybox
	createSkybox();

	// GUI
	initIMGUI();

	return static_cast<HRESULT>(S_OK);
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
D3DFramework::~D3DFramework()
{
	try {
		if (_pImmediateContext)
			_pImmediateContext->ClearState();
	}
	catch (...) {

	}
}

// render skybox
void D3DFramework::renderSkybox()
{
	// skybox constant buffer
	const ConstantBufferSkybox cbSkybox{
		DirectX::XMMatrixTranspose(_camera.view),
		DirectX::XMMatrixTranspose(_camera.projection),
		_camera.eye,
		DirectX::XMMatrixTranspose(_cubemapWorld),

	};

	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_pImmediateContext->UpdateSubresource(_skyboxConstantBuffer, 0, nullptr, &cbSkybox, 0, 0);

	_pImmediateContext->VSSetShader(_pVertexShaderSky, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(2, 1, &_skyboxConstantBuffer.p); // register b2
	_pImmediateContext->PSSetShader(_pPixelShaderSky, nullptr, 0);
	_pImmediateContext->PSSetConstantBuffers(2, 1, &_skyboxConstantBuffer.p); // register b2

	_pImmediateContext->IASetIndexBuffer(_skyboxIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	_pImmediateContext->OMSetDepthStencilState(_pDepthStencilStateSky, 1);
	_pImmediateContext->RSSetState(_pRasterStateSky);
	_pImmediateContext->PSSetShaderResources(1, 1, &sky_TextureRV.p);
	_pImmediateContext->PSSetSamplers(1, 1, &sky_Sampler.p);
	_pImmediateContext->DrawIndexed(36, 0, 0);

	// render the cube
	_pImmediateContext->OMSetDepthStencilState(_pDepthStencilStateCube, 1);
	_pImmediateContext->RSSetState(_pRasterStateCube);
	_pImmediateContext->DrawIndexed(36, 0, 0);
}

void D3DFramework::renderLights() const
{
	// add condition based on daylight or torch mode
	_directionalLight->render(_pImmediateContext, 5);
	_ambientLight->render(_pImmediateContext, 6);

	if (_player->isTorchEnabled())
		_scene->updateSpotLight();

	_spotLight->render(_pImmediateContext,7);
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void D3DFramework::render()
{
	_drawCalls = 0;
	_numVertices = 0;

	static float currentTime = 0.0f;
	// Update our time	
	if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		time += static_cast<float>(XM_PI) * 0.0125f;
	}
	else
	{
		static ULONGLONG timeStart = 0;
		const ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		time = (timeCur - timeStart) / 1000.0f;
	}
	deltaTime = (time - currentTime) * deltaTimeFactor;
	currentTime = time;
	//OutputDebugStringA((std::to_string(deltaTime) + "\n").c_str());
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView.p, _pDepthStencilView);
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, DirectX::Colors::Black);
	// clear depth buffer
	_pImmediateContext->ClearDepthStencilView(_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	// Render skybox
	renderSkybox();
	_drawCalls += 2;
	// to display other objects
	_pImmediateContext->ClearDepthStencilView(_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	if (_player->getState() != PlayerState::DEAD) {		
		_camera.followPlayer(_player->getPosition());
	}
	for (size_t i = _treesMinIndex; i < _treesMaxIndex; ++i) {
		_trees[i]->render(_pImmediateContext, deltaTime);
		_drawCalls++;
		_numVertices += _trees[i]->getVertexCount();
	}

	// Render player
	if (_player->getRenderable()) {
		_player->render(_pImmediateContext, deltaTime);
		_drawCalls++;
		_numVertices += _player->getVertexCount();
	}
	if (_player->getState() != PlayerState::DEAD) {
		_camera.followPlayer(_player->getPosition());
	}

	if (_player->isInGap(_scene->getGaps())) {
		_player->setState(PlayerState::DEAD);
	}

	// Pass camera matrices to the constant buffer
	const ConstantBufferCamera cbc{
		DirectX::XMMatrixTranspose(_camera.view),
		DirectX::XMMatrixTranspose(_camera.projection),
		{
			DirectX::XMVectorGetX(_camera.eye),
			DirectX::XMVectorGetY(_camera.eye),
			DirectX::XMVectorGetZ(_camera.eye),
			time
		}
	};
	_pImmediateContext->UpdateSubresource(_cameraConstantBuffer, 0, nullptr, &cbc, 0, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_cameraConstantBuffer.p); // register b0

	// Render platforms
	for (size_t i = _platformsMinIndex; i < _platformsMaxIndex; ++i) {
		_platforms[i]->render(_pImmediateContext, deltaTime);
		_drawCalls++;
		_numVertices += _platforms[i]->getVertexCount();
	}

	renderLights();

	//_rain->render(_pd3dDevice, _pImmediateContext, deltaTime);
	// Render GUI
	renderIMGUI();

	// Present the frame
	_swapChain->Present(1, 0);
}

void D3DFramework::initCamera()
{
	auto transformData = ConfigParser::getInstance()->getCameraMatrix();
	assert(transformData.size() == 3 && "Camera Transform must include eye, at, and up!");
	_camera.eye = XMVectorSet(transformData[0].x, transformData[0].y, transformData[0].z, 0.0f);
	_camera.at = XMVectorSet(transformData[1].x, transformData[1].y, transformData[1].z, 0.0f);
	_camera.up = XMVectorSet(transformData[2].x, transformData[2].y, transformData[2].z, 0.0f);
	_camera.zoom = 1.0f;
	_camera.updateViewProjection();
}

void D3DFramework::initSceneElements()
{
	_numAllVertices = 0;
	_player->setState(PlayerState::IDLE);
	_player->reset();
	_playerPosition = _player->getPosition();
	_numAllVertices += _player->getVertexCount();

	_platformsMinIndex = _platforms.size() / 2 - _platformsRange / 2;
	_platformsMaxIndex = _platforms.size() / 2 + _platformsRange / 2;

	for (const auto& platform : _platforms) {
		_numAllVertices += platform->getVertexCount();
	}

	_treesMinIndex = _trees.size() / 2 - _treesRange / 2;
	_treesMaxIndex = _trees.size() / 2 + _treesRange / 2;

	for (const auto& tree : _trees) {
		_numAllVertices += tree->getVertexCount();
	}
}

void D3DFramework::reset()
{
	//time = 0.0f;
	deltaTime = 0.0f;
	deltaTimeFactor = 1.0f;

	initCamera();
	initSceneElements();
}

void D3DFramework::initIMGUI() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplDX11_Init(_pd3dDevice, _pImmediateContext);
	ImGui_ImplWin32_Init(_hWnd);
	_initGui = true;
}

void D3DFramework::renderIMGUI() const {
	if (_initGui) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(620, 480)); 
		ImGui::SetNextWindowSize(ImVec2(180, 120)); 

		ImGui::Begin("ImGui");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("#DrawCalls: %d", _drawCalls);
		ImGui::Text("#RenderedVertices: %d", _numVertices);
		ImGui::Text("#AllVertices: %d", _numAllVertices);
		//ImGui::Text("#PlayerVertices: %d", _player->getVertexCount());
		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}
