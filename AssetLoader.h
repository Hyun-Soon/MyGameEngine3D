#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

#include "ShaderData.h"

class AssetLoader
{
public:
	std::vector<MeshData> LoadModelWithoutAnimation(std::string basePath, std::string filename)
	{
		mBasePath = basePath;
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(
			basePath + filename,
			aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		// ReadFile()에서 경우에 따라서 여러가지 옵션들 설정 가능
		// aiProcess_JoinIdenticalVertices | aiProcess_PopulateArmatureData |
		// aiProcess_SplitByBoneCount |
		// aiProcess_Debone); // aiProcess_LimitBoneWeights

		if (pScene)
		{

			// 1. 모든 메쉬에 대해서 버텍스에 영향을 주는 뼈들의 목록을 만든다.
			FindDeformingBones(pScene);

			// 2. 트리 구조를 따라 업데이트 순서대로 뼈들의 인덱스를 결정한다.
			int counter = 0;
			UpdateBoneIDs(pScene->mRootNode, &counter);

			// 3. 업데이트 순서대로 뼈 이름 저장 (boneIdToName)
			mAniData.boneIdToName.resize(mAniData.boneNameToId.size());
			for (auto& i : mAniData.boneNameToId)
				mAniData.boneIdToName[i.second] = i.first;

			mAniData.boneParents.resize(mAniData.boneNameToId.size(), -1);

			DirectX::SimpleMath::Matrix tr; // Initial transformation
			ProcessNode(pScene->mRootNode, pScene, tr);
		}
		else
		{
			std::cout << "Failed to read file: " << basePath + filename
					  << std::endl;
			auto errorDescription = importer.GetErrorString();
			std::cout << "Assimp error: " << errorDescription << std::endl;
		}

		return mMeshDatas;
	}

	std::tuple<std::vector<MeshData>, AnimationData> LoadModelWithAnimation(std::string basePath, std::string filename)
	{
		mBasePath = basePath;
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(
			basePath + filename,
			aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		// ReadFile()에서 경우에 따라서 여러가지 옵션들 설정 가능
		// aiProcess_JoinIdenticalVertices | aiProcess_PopulateArmatureData |
		// aiProcess_SplitByBoneCount |
		// aiProcess_Debone); // aiProcess_LimitBoneWeights

		if (pScene)
		{

			// 1. 모든 메쉬에 대해서 버텍스에 영향을 주는 뼈들의 목록을 만든다.
			FindDeformingBones(pScene);

			// 2. 트리 구조를 따라 업데이트 순서대로 뼈들의 인덱스를 결정한다.
			int counter = 0;
			UpdateBoneIDs(pScene->mRootNode, &counter);

			// 3. 업데이트 순서대로 뼈 이름 저장 (boneIdToName)
			mAniData.boneIdToName.resize(mAniData.boneNameToId.size());
			for (auto& i : mAniData.boneNameToId)
				mAniData.boneIdToName[i.second] = i.first;

			// 디버깅용
			// cout << "Num boneNameToId : " << mAniData.boneNameToId.size() <<
			// endl; for (auto &i : mAniData.boneNameToId) {
			//    cout << "NameId pair : " << i.first << " " << i.second << endl;
			//}
			// cout << "Num boneIdToName : " << mAniData.boneIdToName.size() <<
			// endl; for (size_t i = 0; i < mAniData.boneIdToName.size(); i++) {
			//    cout << "BoneId: " << i << " " << mAniData.boneIdToName[i] <<
			//    endl;
			//}
			// exit(-1);

			// 각 뼈의 부모 인덱스를 저장할 준비
			mAniData.boneParents.resize(mAniData.boneNameToId.size(), -1);

			DirectX::SimpleMath::Matrix tr; // Initial transformation
			ProcessNode(pScene->mRootNode, pScene, tr);

			// 디버깅용
			// cout << "Num boneIdToName : " << mAniData.boneIdToName.size() <<
			// endl; for (size_t i = 0; i < mAniData.boneIdToName.size(); i++) {
			//    cout << "BoneId: " << i << " " << mAniData.boneIdToName[i]
			//         << " , Parent: "
			//         << (mAniData.boneParents[i] == -1
			//                 ? "NONE"
			//                 : mAniData.boneIdToName[mAniData.boneParents[i]])
			//         << endl;
			//}

			// 애니메이션 정보 읽기
			if (pScene->HasAnimations())
				ReadAnimation(pScene);
		}
		else
		{
			std::cout << "Failed to read file: " << basePath + filename
					  << std::endl;
			auto errorDescription = importer.GetErrorString();
			std::cout << "Assimp error: " << errorDescription << std::endl;
		}

		return { mMeshDatas, mAniData };
	}

	AnimationData LoadAnimation(const std::string& basePath, const std::string& animFilename)
	{
		mBasePath = basePath;
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(
			basePath + animFilename,
			aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

		if (pScene)
		{

			// 1. 모든 메쉬에 대해서 버텍스에 영향을 주는 뼈들의 목록을 만든다.
			FindDeformingBones(pScene);

			// 2. 트리 구조를 따라 업데이트 순서대로 뼈들의 인덱스를 결정한다.
			int counter = 0;
			UpdateBoneIDs(pScene->mRootNode, &counter);

			// 3. 업데이트 순서대로 뼈 이름 저장 (boneIdToName)
			mAniData.boneIdToName.resize(mAniData.boneNameToId.size());
			for (auto& i : mAniData.boneNameToId)
				mAniData.boneIdToName[i.second] = i.first;

			// 디버깅용
			// cout << "Num boneNameToId : " << mAniData.boneNameToId.size() <<
			// endl; for (auto &i : mAniData.boneNameToId) {
			//    cout << "NameId pair : " << i.first << " " << i.second << endl;
			//}
			// cout << "Num boneIdToName : " << mAniData.boneIdToName.size() <<
			// endl; for (size_t i = 0; i < mAniData.boneIdToName.size(); i++) {
			//    cout << "BoneId: " << i << " " << mAniData.boneIdToName[i] <<
			//    endl;
			//}
			// exit(-1);

			// 각 뼈의 부모 인덱스를 저장할 준비
			mAniData.boneParents.resize(mAniData.boneNameToId.size(), -1);

			DirectX::SimpleMath::Matrix tr; // Initial transformation
			ProcessNode(pScene->mRootNode, pScene, tr);

			// 디버깅용
			// cout << "Num boneIdToName : " << mAniData.boneIdToName.size() <<
			// endl; for (size_t i = 0; i < mAniData.boneIdToName.size(); i++) {
			//    cout << "BoneId: " << i << " " << mAniData.boneIdToName[i]
			//         << " , Parent: "
			//         << (mAniData.boneParents[i] == -1
			//                 ? "NONE"
			//                 : mAniData.boneIdToName[mAniData.boneParents[i]])
			//         << endl;
			//}

			// 애니메이션 정보 읽기
			if (pScene->HasAnimations())
				ReadAnimation(pScene);
		}
		else
		{
			std::cout << "Failed to read file: " << basePath + animFilename << std::endl;
			auto errorDescription = importer.GetErrorString();
			std::cout << "Assimp error: " << errorDescription << std::endl;
		}

		return mAniData;
	}

	AnimationData LoadAnimations(const std::string& basePath, const std::vector<std::string>& animFilenames)
	{
		AnimationData animData;

		for (const std::string& clipname : animFilenames)
		{
			AssetLoader assetLoader;

			AnimationData temp = assetLoader.LoadAnimation(basePath, clipname);

			if (animData.clips.empty())
			{
				animData = temp;
			}
			else
			{
				animData.clips.push_back(temp.clips.front());
			}
		}

		return animData;
	}

	void FindDeformingBones(const aiScene* scene)
	{
		for (uint32_t i = 0; i < scene->mNumMeshes; i++)
		{
			const auto* mesh = scene->mMeshes[i];
			if (mesh->HasBones())
			{
				for (uint32_t i = 0; i < mesh->mNumBones; i++)
				{
					const aiBone* bone = mesh->mBones[i];

					// bone과 대응되는 node의 이름은 동일
					// 뒤에서 node 이름으로 부모를 찾을 수 있음
					mAniData.boneNameToId[bone->mName.C_Str()] = -1;

					// debug
					// cout << "boneName : " << bone->mName.C_Str() << '\n';

					// 주의: 뼈의 순서가 업데이트 순서는 아님

					// 기타: bone->mWeights == 0일 경우에도 포함시켰음
					// 기타: bone->mNode = 0이라서 사용 불가
				}
			}
		}
	}

	void UpdateBoneIDs(aiNode* node, int* counter)
	{
		static int id = 0; // fbx 여러개 읽을 때, 0부터 시작하게 초기화 해줘야 할듯
		if (node)
		{
			if (mAniData.boneNameToId.count(node->mName.C_Str()))
			{
				mAniData.boneNameToId[node->mName.C_Str()] = *counter;
				*counter += 1;
			}
			for (UINT i = 0; i < node->mNumChildren; i++)
			{
				UpdateBoneIDs(node->mChildren[i], counter);
			}
		}
	}

	void ProcessNode(aiNode* node, const aiScene* scene, DirectX::SimpleMath::Matrix tr)
	{
		if (node->mParent && mAniData.boneNameToId.count(node->mName.C_Str()) && FindParent(node->mParent))
		{
			const int32_t boneId = mAniData.boneNameToId[node->mName.C_Str()];
			mAniData.boneParents[boneId] = mAniData.boneNameToId[FindParent(node->mParent)->mName.C_Str()];
		}

		DirectX::SimpleMath::Matrix m(&node->mTransformation.a1);
		m = m.Transpose() * tr;

		for (UINT i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh*	 mesh = scene->mMeshes[node->mMeshes[i]];
			MeshData newMesh = this->ProcessMesh(mesh, scene);
			for (VERTEX_TYPE& v : newMesh.vertices)
			{
				v.position = DirectX::SimpleMath::Vector3::Transform(v.position, m);
			}
			mMeshDatas.push_back(newMesh);
		}

		for (UINT i = 0; i < node->mNumChildren; i++)
		{
			this->ProcessNode(node->mChildren[i], scene, m);
		}
	}

	// 디자인을 위한 노드들을 건너뛰고 부모 노드 찾기
	const aiNode* FindParent(const aiNode* node)
	{
		if (!node)
			return nullptr;
		if (mAniData.boneNameToId.count(node->mName.C_Str()) > 0)
			return node;
		return FindParent(node->mParent);
	}

	void ReadAnimation(const aiScene* pScene)
	{
		mAniData.clips.resize(pScene->mNumAnimations);

		for (uint32_t i = 0; i < pScene->mNumAnimations; i++)
		{
			auto& clip = mAniData.clips[i];

			const aiAnimation* ani = pScene->mAnimations[i];

			clip.duration = ani->mDuration;
			clip.ticksPerSec = ani->mTicksPerSecond;
			clip.keys.resize(mAniData.boneNameToId.size());
			clip.numChannels = ani->mNumChannels;

			for (uint32_t c = 0; c < ani->mNumChannels; c++)
			{
				const aiNodeAnim* nodeAnim = ani->mChannels[c];
				const int		  boneId = mAniData.boneNameToId[nodeAnim->mNodeName.C_Str()];
				clip.keys[boneId].resize(nodeAnim->mNumPositionKeys);
				for (uint32_t k = 0; k < nodeAnim->mNumPositionKeys; k++)
				{
					const auto pos = nodeAnim->mPositionKeys[k].mValue;
					const auto rot = nodeAnim->mRotationKeys[k].mValue;
					const auto scale = nodeAnim->mScalingKeys[k].mValue;
					auto&	   key = clip.keys[boneId][k];
					key.pos = { pos.x, pos.y, pos.z };
					key.rot = DirectX::SimpleMath::Quaternion(rot.x, rot.y, rot.z, rot.w);
					key.scale = { scale.x, scale.y, scale.z };
				}
			}
		}
	}

	MeshData ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{

		MeshData						  newMesh;
		std::vector<VERTEX_TYPE>&		  vertices = newMesh.vertices;
		std::vector<INDEX_TYPE>&		  indices = newMesh.indices;
		std::vector<SKINNED_VERTEX_TYPE>& skinnedVertices = newMesh.skinnedVertices;

		// Walk through each of the mesh's vertices
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			VERTEX_TYPE vertex;

			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;

			if (mesh->mTextureCoords[0])
			{
				vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
				vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
			}

			vertices.emplace_back(vertex);
		}

		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; j++)
				indices.emplace_back(face.mIndices[j]);
		}

		if (mesh->HasBones())
		{
			std::vector<std::vector<float>>	  boneWeights(vertices.size());
			std::vector<std::vector<uint8_t>> boneIndices(vertices.size());

			mAniData.offsetMatrices.resize(mAniData.boneNameToId.size());
			mAniData.boneTransforms.resize(mAniData.boneNameToId.size());

			int count = 0;
			for (uint32_t i = 0; i < mesh->mNumBones; i++)
			{
				const aiBone*  bone = mesh->mBones[i];
				const uint32_t boneId = mAniData.boneNameToId[bone->mName.C_Str()];

				mAniData.offsetMatrices[boneId] =
					DirectX::SimpleMath::Matrix((float*)&bone->mOffsetMatrix).Transpose();

				// 이 뼈가 영향을 주는 Vertex의 개수
				for (uint32_t j = 0; j < bone->mNumWeights; j++)
				{
					aiVertexWeight weight = bone->mWeights[j];
					assert(weight.mVertexId < boneIndices.size());
					boneIndices[weight.mVertexId].push_back(boneId);
					boneWeights[weight.mVertexId].push_back(weight.mWeight);
				}
			}

			int maxBones = 0;
			for (int i = 0; i < boneWeights.size(); i++)
			{
				maxBones = max(maxBones, int(boneWeights[i].size()));
			}

			skinnedVertices.resize(vertices.size());
			for (int i = 0; i < vertices.size(); i++)
			{
				skinnedVertices[i].position = vertices[i].position;
				skinnedVertices[i].texcoord = vertices[i].texcoord;

				for (int j = 0; j < boneWeights[i].size(); j++)
				{
					skinnedVertices[i].blendWeights[j] = boneWeights[i][j];
					skinnedVertices[i].boneIndices[j] = boneIndices[i][j];
				}
			}
		}

		if (mesh->mMaterialIndex >= 0)
		{

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			newMesh.albedoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_BASE_COLOR);
			if (newMesh.albedoTextureFilename.empty())
			{
				newMesh.albedoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_DIFFUSE);
			}
		}

		return newMesh;
	}

	std::string ReadTextureFilename(const aiScene* scene, aiMaterial* material, aiTextureType type)
	{

		if (material->GetTextureCount(type) > 0)
		{
			aiString filepath;
			material->GetTexture(type, 0, &filepath);

			std::string fullPath = mBasePath + std::string(std::filesystem::path(filepath.C_Str()).filename().string());

			// 1. 실제로 파일이 존재하는지 확인
			if (!std::filesystem::exists(fullPath))
			{
				// 2. 파일이 없을 경우 혹시 fbx 자체에 Embedded인지 확인
				const aiTexture* texture =
					scene->GetEmbeddedTexture(filepath.C_Str());
				if (texture)
				{
					// 3. Embedded texture가 존재하고 png일 경우 저장
					if (std::string(texture->achFormatHint).find("png") != std::string::npos)
					{
						std::ofstream fs(fullPath.c_str(), std::ios::binary | std::ios::out);
						fs.write((char*)texture->pcData, texture->mWidth);
						fs.close();
						// 참고: compressed format일 경우 texture->mHeight가 0
					}
				}
				else
				{
					std::cout << fullPath << " doesn't exists. Return empty filename." << std::endl;
				}
			}
			else
			{
				return fullPath;
			}

			return fullPath;
		}
		else
		{
			return "";
		}
	}

private:
	std::string			  mBasePath;
	AnimationData		  mAniData;
	std::vector<MeshData> mMeshDatas;
};
