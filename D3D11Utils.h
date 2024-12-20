#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <string>
#include <d3dcompiler.h>
#include <vector>

class D3D11Utils
{
public:
	static bool CheckCreateShaderResult(HRESULT hr, ID3DBlob* errorBlob);

	static bool CheckResult(HRESULT hr);

	static void CreateVertexShaderAndInputLayout(
		Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
		std::wstring								 filename,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		Microsoft::WRL::ComPtr<ID3D11VertexShader>&	 m_vertexShader,
		Microsoft::WRL::ComPtr<ID3D11InputLayout>&	 m_inputLayout,
		const std::vector<D3D_SHADER_MACRO>			 shaderMacros = {});
	// ShaderMacros 사용할 때 예시
	// {D3D_SHADER_MACRO("SKINNED", "1"), D3D_SHADER_MACRO(NULL, NULL)};
	// 맨 뒤에 NULL, NULL 필수

	static void CreateHullShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::wstring&											   filename,
		Microsoft::WRL::ComPtr<ID3D11HullShader>&					   m_hullShader);

	static void CreateDomainShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::wstring&												 filename,
		Microsoft::WRL::ComPtr<ID3D11DomainShader>&						 m_domainShader);

	static void
	CreateGeometryShader(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& filename,
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>& m_geometryShader);

	static void
	CreateComputeShader(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& filename,
		Microsoft::WRL::ComPtr<ID3D11ComputeShader>& m_computeShader);

	static void CreatePixelShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::wstring&												filename,
		Microsoft::WRL::ComPtr<ID3D11PixelShader>&						m_pixelShader);

	static void CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::vector<uint32_t>&									indices,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&							indexBuffer);

	template <typename T_VERTEX>
	static void CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::vector<T_VERTEX>&									 vertices,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&							 vertexBuffer)
	{

		// D3D11_USAGE enumeration (d3d11.h)
		// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
		bufferDesc.StructureByteStride = sizeof(T_VERTEX);

		D3D11_SUBRESOURCE_DATA vertexBufferData = {
			0
		}; // MS 예제에서 초기화하는 방식
		vertexBufferData.pSysMem = vertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		if (CheckResult(device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf())))
			; // return false;
	}

	template <typename T_INSTANCE>
	static void CreateInstanceBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::vector<T_INSTANCE>&									   instances,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&							   instanceBuffer)
	{

		// 기본적으로 VertexBuffer와 비슷합니다.

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // <- 애니메이션에 사용
		bufferDesc.ByteWidth = UINT(sizeof(T_INSTANCE) * instances.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // <- CPU에서 복사
		bufferDesc.StructureByteStride = sizeof(T_INSTANCE);

		D3D11_SUBRESOURCE_DATA vertexBufferData = {
			0
		}; // MS 예제에서 초기화하는 방식
		vertexBufferData.pSysMem = instances.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		if (CheckResult(device->CreateBuffer(&bufferDesc, &vertexBufferData, instanceBuffer.GetAddressOf())))
			; // return false;
	}

	template <typename T_CONSTANT>
	static void CreateConstBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const T_CONSTANT&												constantBufferData,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&							constantBuffer)
	{

		static_assert((sizeof(T_CONSTANT) % 16) == 0,
			"Constant Buffer size must be 16-byte aligned");

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(constantBufferData);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = &constantBufferData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		if (CheckResult(device->CreateBuffer(&desc, &initData, constantBuffer.GetAddressOf())))
			; // return false;
	}

	template <typename T_DATA>
	static void UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
		const std::vector<T_DATA>&										  bufferData,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&							  buffer)
	{

		if (!buffer)
		{
			OutputDebugString(L"UpdateBuffer() buffer was not initialized.\n");
		}

		D3D11_MAPPED_SUBRESOURCE ms;
		context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, bufferData.data(), sizeof(T_DATA) * bufferData.size());
		context->Unmap(buffer.Get(), NULL);
	}

	template <typename T_DATA>
	static void UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
		const T_DATA&													  bufferData,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&							  buffer)
	{

		if (!buffer)
		{
			OutputDebugString(L"UpdateBuffer() buffer was not initialized.\n");
		}

		D3D11_MAPPED_SUBRESOURCE ms;
		context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, &bufferData, sizeof(bufferData));
		context->Unmap(buffer.Get(), NULL);
	}

	static void
	CreateTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>&	context,
		const std::string filename, const bool usSRGB,
		Microsoft::WRL::ComPtr<ID3D11Texture2D>&		  texture,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResourceView);

	static void CreateTexture(
		Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
		const std::string albedoFilename, const std::string opacityFilename,
		const bool usSRGB, Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResourceView);

	static void CreateUATexture(Microsoft::WRL::ComPtr<ID3D11Device>& device, const int width,
		const int height, const DXGI_FORMAT pixelFormat,
		Microsoft::WRL::ComPtr<ID3D11Texture2D>&		   texture,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>&	   rtv,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&  srv,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav);

	static void CreateTexture3D(Microsoft::WRL::ComPtr<ID3D11Device>& device, const int width,
		const int height, const int depth,
		const DXGI_FORMAT								   pixelFormat,
		const std::vector<float>&						   initData,
		Microsoft::WRL::ComPtr<ID3D11Texture3D>&		   texture,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>&	   rtv,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&  srv,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav);

	static void CreateStagingBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const UINT														  numElements,
		const UINT														  sizeElement,
		const void*														  initData,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&							  buffer);

	static void CopyFromStagingBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer, UINT size,
		void* dest);

	static void CopyToStagingBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer, UINT size,
		void* src)
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
		memcpy(ms.pData, src, size);
		context->Unmap(buffer.Get(), NULL);
	}

	static void CreateStructuredBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const UINT															 numElements,
		const UINT															 sizeElement,
		const void*															 initData,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&								 buffer,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&					 srv,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>&					 uav);

	static void CreateIndirectArgsBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const UINT															   numElements,
		const UINT															   sizeElement,
		const void*															   initData,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&								   buffer);

	static void CreateAppendBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const UINT														 numElements,
		const UINT sizeElement, const void* initData,
		Microsoft::WRL::ComPtr<ID3D11Buffer>&			   buffer,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&  srv,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav);

	static void CreateMetallicRoughnessTexture(
		Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
		const std::string metallicFiilename,
		const std::string roughnessFilename, Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv);

	static void
	CreateTextureArray(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>&		 context,
		const std::vector<std::string>						 filenames,
		Microsoft::WRL::ComPtr<ID3D11Texture2D>&			 texture,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&	 textureResourceView);

	// static void CreateDDSTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	//	const wchar_t* filename, const bool isCubeMap,
	//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texResView);

	static Microsoft::WRL::ComPtr<ID3D11Texture2D>
	CreateStagingTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const int width,
		const int height, const std::vector<uint8_t>& image,
		const DXGI_FORMAT pixelFormat, const int mipLevels,
		const int arraySize);

	static Microsoft::WRL::ComPtr<ID3D11Texture3D>
	CreateStagingTexture3D(Microsoft::WRL::ComPtr<ID3D11Device>& device, const int width,
		const int height, const int depth,
		const DXGI_FORMAT pixelFormat);

	static void WriteToPngFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>&				 context,
		Microsoft::WRL::ComPtr<ID3D11Texture2D>&					 textureToWrite,
		const std::string											 filename);

	static size_t GetPixelSize(DXGI_FORMAT pixelFormat);
};