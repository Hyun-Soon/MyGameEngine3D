#pragma once

#include <assert.h>
#include <iostream>
#include <vector>

#include "D3D11Utils.h"

template <typename T_ELEMENT>
class StructuredBuffer
{
public:
	virtual void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const UINT												  numElements)
	{
		mCpu.resize(numElements);
		Initialize(device);
	}

	virtual void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device)
	{
		D3D11Utils::CreateStructuredBuffer(device, UINT(mCpu.size()),
			sizeof(T_ELEMENT), mCpu.data(),
			mGpu, mSrv, mUav);
	}

	void Upload(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		Upload(context, mCpu);
	}

	void Download(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		Download(context, mCpu);
	}

	const auto GetBuffer() { return mGpu.Get(); }
	const auto GetSRV() { return mSrv.Get(); }
	const auto GetUAV() { return mUav.Get(); }
	const auto GetAddressOfSRV() { return mSrv.GetAddressOf(); }
	const auto GetAddressOfUAV() { return mUav.GetAddressOf(); }

	std::vector<T_ELEMENT>				 mCpu;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mGpu;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  mSrv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mUav;
};
