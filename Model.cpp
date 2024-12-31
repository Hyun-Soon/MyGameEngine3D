#include "Model.h"

Model::Model()
{
}

// FBX without animation
Model::Model(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>&   context,
	const std::string&							   basePath,
	const std::string&							   filename)
{
	Initialize(device, context, basePath, filename);
}

Model::~Model()
{
	for (size_t i = 0; i < mMeshes.size(); i++)
		delete mMeshes[i];
}

void Model::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>&			 context,
	const std::string&										 basePath,
	const std::string&										 filename)
{
	AssetLoader assetLoader;

	std::vector<MeshData> meshDatas = assetLoader.LoadModelWithoutAnimation(basePath, filename);

	mModelConsts.Initialize(device);

	for (const auto& meshData : meshDatas)
	{
		Mesh* newMesh = new Mesh;

		InitMeshBuffers(device, meshData, newMesh);

		if (!meshData.albedoTextureFilename.empty())
		{
			if (std::filesystem::exists(meshData.albedoTextureFilename))
			{
				D3D11Utils::CreateTexture(device, context, meshData.albedoTextureFilename, true, newMesh->texture, newMesh->srv);
			}
			else
			{
				std::cout << "meshData.albedoTextureFilename "
						  << meshData.albedoTextureFilename
						  << " does not exists. Skip texture reading." << std::endl;
			}
		}

		this->mMeshes.push_back(newMesh);
	}
}

void Model::UpdateWorldRow(const DirectX::SimpleMath::Matrix& worldRow)
{
	this->mWorldRow = worldRow;

	mModelConsts.GetCpu().world = worldRow.Transpose();
}

void Model::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	mModelConsts.Upload(context);
	context->VSSetConstantBuffers(1, 1, mModelConsts.GetAddressOf());
	for (const Mesh* mesh : mMeshes)
	{
		// 물체 렌더링할 때 여러가지 텍스춰 사용 (register t0 부터시작)
		context->PSSetShaderResources(0, 1, mesh->srv.GetAddressOf());

		context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
		context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(mesh->indexCount, 0, 0);

		// Release resources
		// context->PSSetShaderResources(0, 1, nullptr);
	}
}

void Model::InitMeshBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const MeshData&												  meshData,
	Mesh*														  newMesh)
{

	D3D11Utils::CreateVertexBuffer(device, meshData.vertices, newMesh->vertexBuffer);
	D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->indexBuffer);
	newMesh->vertexCount = UINT(meshData.vertices.size());
	newMesh->indexCount = UINT(meshData.indices.size());
	newMesh->stride = UINT(sizeof(SKINNED_VERTEX_TYPE));
	newMesh->offset = 0;
}

// FBX with animation

// Model::Model(const std::string path, const std::string filename)
//{
//	AssetLoader assetLoader;
//	tie(this->mMeshes, this->mAniData) = assetLoader.Load(path, filename);
//
//	// temp
//	mVertexConstantData.model = DirectX::SimpleMath::Matrix::CreateTranslation(mPosWorld); // DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(120.0f)) *
//	mVertexConstantData.view = DirectX::XMMatrixLookToLH(
//		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
//		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f),
//		DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
//	mVertexConstantData.projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.0f), float(1280) / 960, 1.0f, 10000.0f); // temp farZ
//
//	mVertexConstantData.model = mVertexConstantData.model.Transpose();
//	mVertexConstantData.view = mVertexConstantData.view.Transpose();
//	mVertexConstantData.projection = mVertexConstantData.projection.Transpose();
// }
//
// const size_t Model::GetVSize() const
//{
//	// return this->mMeshes.size();
//	// return this->mVertices.size();
//	return this->mMeshes[2].skinnedVertices.size();
// }
//
// const size_t Model::GetISize() const
//{
//	// return this->mIndices.size();
//	return this->mMeshes[2].indices.size();
// }
//
// const VERTEX_TYPE const* Model::GetVData() const
//{
//	// return this->mVertices.data();
//	return this->mMeshes[2].skinnedVertices.data();
// }
//
// const uint32_t const* Model::GetIData() const
//{
//	// return this->mIndices.data();
//	return this->mMeshes[2].indices.data();
// }
//
// const char* Model::GetCstyleTextureFilename() const
//{
//	std::cout << "textureName : " << mMeshes[2].albedoTextureFilename << std::endl;
//	return this->mMeshes[2].albedoTextureFilename.c_str();
// }
//
// VertexConstantData* Model::GetPVertexConstantData()
//{
//	return &(this->mVertexConstantData);
// }
//
// Microsoft::WRL::ComPtr<ID3D11Buffer>& Model::GetVertexBuffer()
//{
//	return this->mVertexBuffer;
// }
//
// Microsoft::WRL::ComPtr<ID3D11Buffer>& Model::GetIndexBuffer()
//{
//	return this->mIndexBuffer;
// }
//
// Microsoft::WRL::ComPtr<ID3D11Texture2D>& Model::GetTexture()
//{
//	return this->mTexture;
// }
//
// Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Model::GetTextureResourceView()
//{
//	return this->mTextureResourceView;
// }
//
// void Model::Transform(DirectX::SimpleMath::Vector3 vec)
//{
//	mPosWorld += vec;
//	this->mVertexConstantData.model = DirectX::SimpleMath::Matrix::CreateTranslation(mPosWorld);
//	this->mVertexConstantData.model = this->mVertexConstantData.model.Transpose();
// }
// void Model::RotateX(float rad)
//{
//	this->mVertexConstantData.model = DirectX::SimpleMath::Matrix::CreateRotationX(rad) * DirectX::SimpleMath::Matrix::CreateTranslation(mPosWorld);
//	this->mVertexConstantData.model = this->mVertexConstantData.model.Transpose();
// }
// void Model::RotateY(float rad)
//{
//	this->mVertexConstantData.model = DirectX::SimpleMath::Matrix::CreateRotationY(rad) * DirectX::SimpleMath::Matrix::CreateTranslation(mPosWorld);
//	this->mVertexConstantData.model = this->mVertexConstantData.model.Transpose();
// }
//// device, context -> must handled by directXManager
