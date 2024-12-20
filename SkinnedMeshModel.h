#pragma once

#include "Model.h"
#include "StructuredBuffer.h"

class SkinnedMeshModel : public Model
{
public:
	SkinnedMeshModel(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>&	   context,
		const std::string&								   basePath,
		const std::string&								   filename,
		const std::vector<std::string>&					   animFilenames)
	{
		Initialize(device, context, basePath, filename, animFilenames);
	}

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>&	  context,
		const std::string&								  basePath,
		const std::string&								  filename,
		const std::vector<std::string>&					  animFilenames)
	{
		AssetLoader	  assetLoader;
		AnimationData aniData = assetLoader.LoadAnimations(basePath, animFilenames);

		/*GeometryGenerator::Normalize(Vector3(0.0f), 1.0f, modelLoader.m_meshes,
			modelLoader.m_aniData);*/

		InitAnimationData(device, aniData);

		Model::Initialize(device, context, basePath, filename);
	}

	virtual void InitMeshBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device, const MeshData& meshData,
		Mesh* newMesh) override
	{
		D3D11Utils::CreateVertexBuffer(device, meshData.skinnedVertices,
			newMesh->vertexBuffer);
		newMesh->indexCount = UINT(meshData.indices.size());
		newMesh->vertexCount = UINT(meshData.skinnedVertices.size());
		newMesh->stride = UINT(sizeof(SkinnedVertex));
		D3D11Utils::CreateIndexBuffer(device, meshData.indices,
			newMesh->indexBuffer);
	}

	void InitAnimationData(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const AnimationData&									 aniData)
	{
		if (!aniData.clips.empty())
		{
			mAniData = aniData;

			// ���⼭�� AnimationClip�� SkinnedMesh��� �����ϰڽ��ϴ�.
			// �Ϲ������δ� ��� Animation�� SkinnedMesh Animation�� �ƴմϴ�.
			mBoneTransforms.mCpu.resize(aniData.clips.front().keys.size());

			// ����: ��� keys() ������ �������� ���� ���� �ֽ��ϴ�.
			for (int i = 0; i < aniData.clips.front().keys.size(); i++)
				mBoneTransforms.mCpu[i] = DirectX::SimpleMath::Matrix();
			mBoneTransforms.Initialize(device);
		}
	}

	// void UpdateAnimation(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, int clipId,
	//	int frame) override
	//{

	//	mAniData.Update(clipId, frame);

	//	for (int i = 0; i < mBoneTransforms.m_cpu.size(); i++)
	//	{
	//		mBoneTransforms.m_cpu[i] =
	//			m_aniData.Get(clipId, i, frame).Transpose();
	//	}

	//	mBoneTransforms.Upload(context);
	//}

	// void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) override
	//{

	//	// ConstBuffer ��� StructuredBuffer ���
	//	// context->VSSetConstantBuffers(3, 1, m_skinnedConsts.GetAddressOf());

	//	context->VSSetShaderResources(
	//		9, 1, m_boneTransforms.GetAddressOfSRV()); // �׻� slot index ����

	//	// Skinned VS/PS�� GetPSO()�� ���ؼ� �����Ǳ� ������
	//	// Model::Render(.)�� ���� ��� ����

	//	Model::Render(context);
	//};

	// SkinnedMesh�� BoundingBox�� �׸� �� Root�� Transform�� �ݿ��ؾ� �մϴ�.
	// virtual void RenderWireBoundingBox(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
	// virtual void RenderWireBoundingSphere(Microsoft::WRL::ComPtr<ID3D11DeviceContext>
	// &context);

private:
	// ConstantBuffer<SkinnedConsts> m_skinnedConsts;
	StructuredBuffer<DirectX::SimpleMath::Matrix> mBoneTransforms;

	AnimationData mAniData;
};