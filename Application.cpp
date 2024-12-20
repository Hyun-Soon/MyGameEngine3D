#include "Application.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// switch (msg)
	//{
	//	case WM_SIZE:
	//		break;
	//	case WM_LBUTTONDOWN:
	//		std::cout << "LBUTTONDOWN" << std::endl;
	//		break;
	// }
	return DefWindowProc(hwnd, msg, wParam, lParam); // Default Window Procedure
}

Application::Application(UINT width, UINT height)
	: mResolution(width, height)
{
}

bool Application::Initialize()
{

	InitializeWindow();
	InitializeDirectX();
	mGlobalConstantsBuffer.Initialize(mDevice);

	return true;
}

void Application::UpdateGlobalConstants(const float& dt,
	const DirectX::SimpleMath::Vector3&				 eyeWorld,
	const DirectX::SimpleMath::Matrix&				 viewRow,
	const DirectX::SimpleMath::Matrix&				 projRow)
{
	GlobalConstants& constantBufferCpu = mGlobalConstantsBuffer.GetCpu();
	constantBufferCpu.globalTime += dt;
	constantBufferCpu.eyeWorld = eyeWorld;
	constantBufferCpu.view = viewRow.Transpose();
	constantBufferCpu.proj = projRow.Transpose();

	mGlobalConstantsBuffer.Upload(mContext);
}

bool Application::InitializeWindow()
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), // size of structure
		CS_CLASSDC,						  // style of window
		WndProc,						  // points to window procedure
		0L,
		0L,
		GetModuleHandle(NULL), // because used main function, not WinMain
		NULL,
		NULL,
		NULL,
		NULL,
		L"Application", // name of window class
		NULL };

	if (!RegisterClassEx(&wc))
	{
		OutputDebugString(L"RegisterClassEx() failed.\n");
		return 1;
	}

	RECT wr = { 0, 0, mResolution.width, mResolution.height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	mWindow = CreateWindow(wc.lpszClassName,
		L"Application",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		wc.hInstance,
		NULL);

	if (!mWindow)
	{
		OutputDebugString(L"CreateWindow() failed.\n");
		return false;
	}
	mResolution.width = wr.right - wr.left;
	mResolution.height = wr.bottom - wr.top;

	ShowWindow(mWindow, SW_SHOWDEFAULT);
	UpdateWindow(mWindow);
	SetForegroundWindow(mWindow);

	return true;
}

bool Application::InitializeDirectX()
{
	if (!createDeviceContextAndSwapChain())
		return false;

	if (!createRenderTargetView())
		return false;

	if (!createRasterizerState())
		return false;

	if (!createDepthStencilBuffer())
		return false;

	// if (!createVertexConstantBuffer())
	//	return false;

	// if (!createSamplerState())
	//	return false;

	// setViewport(res);

	return true;
}

bool Application::createDeviceContextAndSwapChain()
{
	const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG) // not executed in release mode
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[3] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_9_3
	};

	// check if support 4X MSAA
	// UINT numQualityLevels;
	// device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);
	// if (numQualityLevels <= 0)
	//{
	//	std::cout << "MSAA not supported." << std::endl;
	// }
	// numQualityLevels = 0; // MSAA turn off

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc.Width = mResolution.width;
	swapChainDesc.BufferDesc.Height = mResolution.height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferCount = 2; // Double-buffering
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // DXGI_USAGE_SHADER_INPUT : to use input for shader
	swapChainDesc.OutputWindow = mWindow;
	swapChainDesc.Windowed = TRUE;								  // windowed/full-screen mode
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (mNumQualityLevel > 0)
	{
		swapChainDesc.SampleDesc.Count = 4; // how many multisamples
		swapChainDesc.SampleDesc.Quality = mNumQualityLevel - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	if (FAILED(D3D11CreateDeviceAndSwapChain(
			0, // Default adapter
			driverType,
			0, // No software device
			createDeviceFlags,
			featureLevels,
			1,
			D3D11_SDK_VERSION,
			&swapChainDesc,			   // [in, optional]
			mSwapChain.GetAddressOf(), // [out, optional]
			mDevice.GetAddressOf(),	   // [out, optional]
			&mFeatureLevel,			   // [out, optional]
			mContext.GetAddressOf()))) // [out, optional]
	{
		OutputDebugString(L"D3D11CreateDeviceAndSwapChain() failed.\n");
		return false;
	}

	if (mFeatureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		OutputDebugString(L"D3D Feature Level 11.1 unsupported.\n");
		return false;
	}
	return true;
}

bool Application::createRenderTargetView()
{
	mSwapChain->GetBuffer(0, IID_PPV_ARGS(mBackBuffer.GetAddressOf())); // 0 : index
	if (mBackBuffer)
	{
		HRESULT hr = mDevice->CreateRenderTargetView(mBackBuffer.Get(), nullptr, mRenderTargetView.GetAddressOf());

		if (FAILED(hr))
		{
			OutputDebugString(L"CreateRenderTargetView() failed.\n");
			return false;
		}
	}
	else
	{
		OutputDebugString(L"backBuffer is nullptr.\n");
		return false;
	}
	return true;
}

bool Application::createRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID; // D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;

	HRESULT hr = mDevice->CreateRasterizerState(&rasterizerDesc, mRasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateRasterizerState() failed.\n");
		return false;
	}
	return true;
}

bool Application::createDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = { 0 };
	depthStencilBufferDesc.Width = mResolution.width;
	depthStencilBufferDesc.Height = mResolution.height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth, 8 bits for stencil
	if (mNumQualityLevel > 0)
	{
		depthStencilBufferDesc.SampleDesc.Count = 4;
		depthStencilBufferDesc.SampleDesc.Quality = mNumQualityLevel - 1;
	}
	else
	{
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
	}
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	// Create Depth Stencil Buffer
	if (FAILED(mDevice->CreateTexture2D(&depthStencilBufferDesc, 0, mDepthStencilBuffer.GetAddressOf())))
	{
		OutputDebugString(L"CreateTexture2D() failed.\n");
		return false;
	}

	// Create Depth Stencil View
	if (FAILED(mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), 0, mDepthStencilView.GetAddressOf())))
	{
		OutputDebugString(L"CreateDepthStencilView() failed.\n");
		return false;
	}

	// Create Depth Stencil State
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	if (FAILED(mDevice->CreateDepthStencilState(&depthStencilDesc, mDepthStencilState.GetAddressOf())))
	{
		OutputDebugString(L"CreateDepthStencilState() failed.\n");
		return false;
	}
	return true;
}

void Application::Run()
{

	// Object golfBat("C:/Users/Soon/Desktop/Application/Assets/", "golf_bat_bin.fbx", "GradientPalette.png");
	// Object golfBat("C:/Users/Soon/Desktop/Application/Assets/", "AgarthanBody.fbx"); //, "nemlemtemptex.png");
	// Object golfBat("C:/Users/Soon/Desktop/HonglabGraphics/Part4/Assets/Characters/Mixamo", "character.fbx", "Ch03_1001_Diffuse.png");
	SkinnedMeshModel swat(mDevice, mContext, "C:/Users/Soon/Desktop/Application/Assets/", "Swat.fbx", { "CatwalkIdle.fbx" });
	swat.UpdateWorldRow(DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, 3.0f));

	const HWND window = mWindow;
	MSG		   msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			update();
			render();
			//// Render
			// r += 2.0f;
			// swat.RotateY(DirectX::XMConvertToRadians(r));

			// mDxManager.Render(swat);
		}
	}
}

void Application::update()
{
}

void Application::render()
{
}

// const HWND& Application::GetWindow() const
//{
//	return this->mWindow;
// }

// const Microsoft::WRL::ComPtr<ID3D11Device>& Application::GetDevice() const
//{
//	return this->mDxData.device;
// }
//
// const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& Application::GetContext() const
//{
//	return this->mDxData.context;
// }
//
// void Application::SwitchBackBuffer()
//{
//	mDxData.swapChain->Present(1, 0);
// }
//
// void Application::Render()
//{
// }
//
// DX11Data& Application::getDxData()
//{
//	return mDxData;
// }
