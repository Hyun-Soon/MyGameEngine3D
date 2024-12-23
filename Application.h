#pragma once

#include <directxmath.h>

#include "SkinnedMeshModel.h"
#include "WindowManager.h"
#include "Camera.h"

class Application
{
public:
	Application(UINT width, UINT height);

	bool Initialize();

	void UpdateGlobalConstants(const float& dt,
		const DirectX::SimpleMath::Vector3& eyeWorld,
		const DirectX::SimpleMath::Matrix&	viewRow,
		const DirectX::SimpleMath::Matrix&	projRow);

	void Run();

private:
	void update();
	void render();

	bool InitializeWindow();
	bool InitializeDirectX();
	bool createDeviceContextAndSwapChain();
	bool createRenderTargetView();
	bool createRasterizerState();
	bool createDepthStencilBuffer();
	bool createSamplerState();
	bool initShaders();

	void setViewport();

	struct Resolution
	{
		UINT width;
		UINT height;
		Resolution(UINT w, UINT h) { this->width = w, this->height = h; }
	};

	Microsoft::WRL::ComPtr<ID3D11Device>		mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>		mSwapChain;
	HWND										mWindow;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			mBackBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			mDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	mDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			mVertexConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		mSamplerState;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  mInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  mPixelShader;

	ConstantBuffer<GlobalConstants> mGlobalConstantsBuffer;

	D3D11_VIEWPORT mViewport;

	UINT			  mNumQualityLevel = 0;
	Resolution		  mResolution;
	D3D_FEATURE_LEVEL mFeatureLevel;

	Camera mCamera;

	std::vector<SkinnedMeshModel> mModelList;
	int							  frameCount = 0;
};