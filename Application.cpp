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

	mContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

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

	if (!createSamplerState())
		return false;

	if (!initShaders())
		return false;

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

bool Application::createSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the Sample State
	HRESULT hr = mDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateSamplerState() failed.\n");
		return false;
	}
	return true;
}

bool Application::initShaders()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> skinnedIEs = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48,
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 64,
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 1, DXGI_FORMAT_R8G8B8A8_UINT, 0, 68,
			D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11Utils::CreateVertexShaderAndInputLayout(mDevice, L"VertexShader.hlsl", skinnedIEs, mVertexShader, mInputLayout);
	D3D11Utils::CreatePixelShader(mDevice, L"PixelShader.hlsl", mPixelShader);

	return true;
}

void Application::setViewport()
{
	// Set the viewport
	ZeroMemory(&mViewport, sizeof(D3D11_VIEWPORT));
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = float(mResolution.width);
	mViewport.Height = float(mResolution.height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mContext->RSSetViewports(1, &mViewport);
}

void Application::Run()
{
	SkinnedMeshModel larva(mDevice, mContext, "C:/Users/Soon/Desktop/ROR2_Resources/Animator/AcidLarva/", "AcidLarva.fbx", {});
	larva.UpdateWorldRow(DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, 30.0f));

	mModelList.push_back(larva);

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
		}
	}
}

void Application::update()
{
	if (frameCount == mModelList[0].GetClipKeySize())
		frameCount = 0;
	else
		++frameCount;

	mModelList[0].UpdateAnimation(mContext, 0, frameCount);

	UpdateGlobalConstants(0.01,
		mCamera.GetEyePos(),
		mCamera.GetViewRow(),
		mCamera.GetProjRow());

	mContext->VSSetConstantBuffers(0, 1, mGlobalConstantsBuffer.GetAddressOf());
}

void Application::render()
{
	setViewport();

	mContext->VSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	mContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mContext->ClearRenderTargetView(mRenderTargetView.Get(), clearColor);
	mContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mGlobalConstantsBuffer.Upload(mContext);

	mContext->VSSetShader(mVertexShader.Get(), 0, 0);
	mContext->IASetInputLayout(mInputLayout.Get());
	mContext->PSSetShader(mPixelShader.Get(), 0, 0);
	mContext->RSSetState(mRasterizerState.Get());
	mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	// setRenderTargetView in init func
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO :: implement Model::Render(), SkinnedMeshModel::Render().
	mModelList[0].Render(mContext);

	mSwapChain->Present(1, 0);
}
