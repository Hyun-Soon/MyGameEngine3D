#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

StructuredBuffer<float4x4> boneTransforms : register(t9);

// 공용 Constants
cbuffer GlobalConstants : register(b0)
{
    matrix view;
    matrix proj;
    float3 eyeWorld;
    float globalTime;
};

cbuffer MeshConstants : register(b1)
{
    matrix world; // Model(또는 Object) 좌표계 -> World로 변환
};

struct VertexShaderInput
{
    float3 posModel : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    
    float4 boneWeights0 : BLENDWEIGHT0;
    float4 boneWeights1 : BLENDWEIGHT1;
    uint4 boneIndices0 : BLENDINDICES0;
    uint4 boneIndices1 : BLENDINDICES1;
};


struct PixelShaderInput
{
    float4 posWorld : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target;
};


#endif