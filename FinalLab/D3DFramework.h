#pragma once
// Simulation template, based on the Microsoft DX11 tutorial 04
// parasoft-begin-suppress ALL "suppress all violations"

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include <vector>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
// parasoft-end-suppress ALL "suppress all violations"
#include "Scene.h"

using namespace DirectX;

#define COMPILE_CSO

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

struct ConstantBufferCamera
{
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMVECTOR mEyePos;
};

struct ConstantBufferSkybox
{
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMVECTOR mEyePos;
	XMMATRIX mCubemap;
};

struct Camera
{
	XMVECTOR eye;
	XMVECTOR at;
	XMVECTOR up;
	XMMATRIX view = {};
	XMMATRIX projection = {};
	float zoom = 1.0f; // Default zoom level

	void updateViewProjection() {
		// Adjust field of view (FOV) based on zoom level
		if (zoom < 0.9f) zoom = 0.9f;
		if (zoom > 1.6f) zoom = 1.6f;

		const float fov = XM_PIDIV2 / zoom; // Zoom scales the FOV
		projection = XMMatrixPerspectiveFovLH(fov, 800.0f / 600.0f, 0.01f, 1000.0f);
		view = XMMatrixLookAtLH(eye, at, up);
	}

	void followPlayer(const XMFLOAT3& playerPosition)
	{
		XMFLOAT3 eyePosition;
		XMStoreFloat3(&eyePosition, eye);

		// Update only the x-coordinate of the camera to follow the player's x position
		eyePosition.x = playerPosition.x;

		// Keep the camera's current y position intact
		eye = XMVectorSet(eyePosition.x, eyePosition.y, eyePosition.z, 0.0f);

		// Update only the x-coordinate of the camera's focus point
		at = XMVectorSet(playerPosition.x, XMVectorGetY(at), playerPosition.z, 0.0f);

		updateViewProjection();
	}
};

class D3DFramework final {
	HINSTANCE _hInst = nullptr;
	HWND _hWnd = nullptr;
	D3D_DRIVER_TYPE _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL _featureLevel = D3D_FEATURE_LEVEL_11_1;
	// CComPtr is a smart pointer class that manages the lifetime of COM objects
	// It automatically calls the Release method when the object goes out of scope
	CComPtr <ID3D11Device> _pd3dDevice;
	CComPtr <ID3D11Device1> _pd3dDevice1;
	CComPtr <ID3D11DeviceContext> _pImmediateContext;
	CComPtr <ID3D11DeviceContext1> _pImmediateContext1;
	CComPtr <IDXGISwapChain1> _swapChain;
	CComPtr <IDXGISwapChain1> _swapChain1;
	CComPtr <ID3D11RenderTargetView> _pRenderTargetView;
	CComPtr <ID3D11Buffer> _skyboxVertexBuffer;
	CComPtr <ID3D11Buffer> _skyboxIndexBuffer;
	CComPtr <ID3D11Buffer> _cameraConstantBuffer;
	CComPtr <ID3D11Buffer> _skyboxConstantBuffer;
	CComPtr <ID3D11ShaderResourceView> sky_TextureRV;
	CComPtr <ID3D11SamplerState> sky_Sampler;
	CComPtr <ID3D11VertexShader> _pVertexShaderSky;
	CComPtr <ID3D11PixelShader> _pPixelShaderSky;
	CComPtr <ID3D11Texture2D> _pDepthStencil;
	CComPtr <ID3D11DepthStencilView> _pDepthStencilView;
	CComPtr <ID3D11InputLayout> _pVertexLayout;
	CComPtr <ID3D11DepthStencilState> _pDepthStencilStateSky;
	CComPtr <ID3D11DepthStencilState> _pDepthStencilStateCube;
	CComPtr <ID3D11RasterizerState> _pRasterStateSky;
	CComPtr <ID3D11RasterizerState> _pRasterStateCube;
	XMMATRIX _World = {};
	XMMATRIX _cubemapWorld = {};
	Camera _camera;
	UINT _windowWidth = 800;
	UINT _windowHeight = 600;
	static float time;
	float deltaTime = 0.0f;
	float deltaTimeFactor = 1.0f;

	// IMGUI related stuffs
	bool _initGui = false;
	int _drawCalls = 0u;
	int _numVertices = 0u;
	int _numAllVertices = 0u;

	// to display possibly visible objects
	int _platformsMinIndex = 0u;
	int _platformsMaxIndex = 0u;
	int _platformsRange = 20u;
	int _treesMinIndex = 0u;
	int _treesMaxIndex = 0u;
	int _treesRange = 16u;

	XMFLOAT3 _playerPosition;
	
	std::unique_ptr<Scene> _scene;
	std::shared_ptr<Player> _player;
	std::shared_ptr<Light> _directionalLight;
	std::shared_ptr<Light> _ambientLight;
	std::shared_ptr<Light> _spotLight;
	std::shared_ptr<Rain> _rain;

	std::vector<std::shared_ptr<Platform>> _platforms;
	std::vector<std::shared_ptr<Tree>> _trees;
	static std::unique_ptr<D3DFramework> _instance;

	void createSkybox();
	void renderSkybox();
	void renderLights() const; 
	
	void initCamera();
	void initSceneElements();

	void initIMGUI();
	void renderIMGUI() const;

public:

	D3DFramework() = default;
	D3DFramework(D3DFramework&) = delete;
	D3DFramework(D3DFramework&&) = delete;
	D3DFramework& operator=(const D3DFramework&) = delete;
	D3DFramework& operator=(D3DFramework&&) = delete;
	~D3DFramework();

	static D3DFramework& getInstance() { return *_instance; }

	// callback function that Windows calls whenever an event occurs for the window (e.g., mouse clicks, key presses)
	// Windows expects this function to have a specific signature and does not pass an instance of the class to it
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT initWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT initDevice();
	void render();
	void reset();

	ID3D11Device* getDevice() const { return _pd3dDevice; }
	ID3D11DeviceContext* getContext() const { return _pImmediateContext; }
	HWND getWindowHandle() const { return _hWnd; }

	const DirectX::XMMATRIX& getCameraView() const { return _camera.view; }
	const DirectX::XMMATRIX& getCameraProjection() const { return _camera.projection; }

	ID3D11RenderTargetView* getRenderTargetView() const { return _pRenderTargetView.p; }
	ID3D11DepthStencilView* getDepthStencilView() const { return _pDepthStencilView.p; }

	const int getDeltaTime() const { return deltaTime; }
};
