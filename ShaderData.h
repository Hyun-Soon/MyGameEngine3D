#pragma once

#include <map>
#include <directxtk/SimpleMath.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// debug
#include <iostream>

struct Vertex
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector2 texcoord;
};

struct SkinnedVertex
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 normalModel;
	DirectX::SimpleMath::Vector2 texcoord;

	float	blendWeights[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }; // BLENDWEIGHT0 and 1
	uint8_t boneIndices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };						  // BLENDINDICES0 and 1

	SkinnedVertex()
		: position(0.0f, 0.0f, 0.0f), normalModel(0.0f, 0.0f, 0.0f), texcoord(0.0f, 0.0f) {};
};

struct VertexConstantData
{
	DirectX::SimpleMath::Matrix model;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
	DirectX::SimpleMath::Matrix invTranspose;
};
static_assert((sizeof(VertexConstantData) % 16) == 0, "Vertex Constant Buffer size must be 16-byte aligned.");

struct AnimationClip
{

	struct Key
	{
		DirectX::SimpleMath::Vector3	pos = DirectX::SimpleMath::Vector3(0.0f);
		DirectX::SimpleMath::Vector3	scale = DirectX::SimpleMath::Vector3(1.0f);
		DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion();

		DirectX::SimpleMath::Matrix GetTransform()
		{
			return DirectX::SimpleMath::Matrix::CreateScale(scale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(rot)
				* DirectX::SimpleMath::Matrix::CreateTranslation(pos);
		}
	};

	std::string					  name;		   // Name of this animation clip
	std::vector<std::vector<Key>> keys;		   // m_key[boneIdx][frameIdx]
	int							  numChannels; // Number of bones
	int							  numKeys;	   // Number of frames of this animation clip
	double						  duration;	   // Duration of animation in ticks
	double						  ticksPerSec; // Frames per second
};

struct AnimationData
{
	std::map<std::string, int32_t>			 boneNameToId; // 뼈 이름과 인덱스 정수
	std::vector<std::string>				 boneIdToName; // boneNameToId의 Id 순서대로 뼈 이름 저장
	std::vector<int32_t>					 boneParents;  // 부모 뼈의 인덱스
	std::vector<DirectX::SimpleMath::Matrix> offsetMatrices;
	std::vector<DirectX::SimpleMath::Matrix> boneTransforms;
	std::vector<AnimationClip>				 clips;
	DirectX::SimpleMath::Matrix				 defaultTransform;
	DirectX::SimpleMath::Matrix				 rootTransform = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Matrix				 accumulatedRootTransform = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Vector3			 prevPos = DirectX::SimpleMath::Vector3(0.0f);
};

using VERTEX_TYPE = Vertex;
using SKINNED_VERTEX_TYPE = SkinnedVertex;
using INDEX_TYPE = uint32_t;
using VERTEX_CONSTANT_DATA_TYPE = VertexConstantData;

struct MeshData
{
	std::vector<VERTEX_TYPE>		 vertices;
	std::vector<SKINNED_VERTEX_TYPE> skinnedVertices;
	std::vector<INDEX_TYPE>			 indices;
	std::string						 albedoTextureFilename;
};