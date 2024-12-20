#pragma once

#include "D3D11Utils.h"
#include <directxtk/SimpleMath.h>

__declspec(align(256)) struct GlobalConstants
{
	DirectX::SimpleMath::Matrix	 view;
	DirectX::SimpleMath::Matrix	 proj;
	DirectX::SimpleMath::Vector3 eyeWorld;

	float globalTime = 0.0f;
};

template <typename T_CONSTS>
class ConstantBuffer
{
public:
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device)
	{
		D3D11Utils::CreateConstBuffer(device, mCpu, mGpu);
	}

	void Upload(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		D3D11Utils::UpdateBuffer(context, mCpu, mGpu);
	}

	T_CONSTS& GetCpu()
	{
		return mCpu;
	}

	const auto Get()
	{
		return mGpu.Get();
	}

	const auto GetAddressOf()
	{
		return mGpu.GetAddressOf();
	}

private:
	T_CONSTS							 mCpu;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mGpu;
};
