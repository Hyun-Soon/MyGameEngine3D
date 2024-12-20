#pragma once

#include <d3d11.h>
#include <wrl/client.h>

struct Mesh
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

	UINT vertexCount;
	UINT indexCount;
	UINT stride;
};
