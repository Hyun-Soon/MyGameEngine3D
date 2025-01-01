#include "Common.hlsli" // ���̴������� include ��� ����

PixelShaderInput main(VertexShaderInput input)
{
    // �� ��ǥ��� NDC�̱� ������ ���� ��ǥ�� �̿��ؼ� ���� ���
    
    PixelShaderInput output;
    
    //// ���� �ڷ�: Luna DX 12 ����
    
    //float weights[8];
    //weights[0] = input.boneWeights0.x;
    //weights[1] = input.boneWeights0.y;
    //weights[2] = input.boneWeights0.z;
    //weights[3] = input.boneWeights0.w;
    //weights[4] = input.boneWeights1.x;
    //weights[5] = input.boneWeights1.y;
    //weights[6] = input.boneWeights1.z;
    //weights[7] = input.boneWeights1.w;
    
    //uint indices[8]; // ��Ʈ: �� ���!
    //indices[0] = input.boneIndices0.x;
    //indices[1] = input.boneIndices0.y;
    //indices[2] = input.boneIndices0.z;
    //indices[3] = input.boneIndices0.w;
    //indices[4] = input.boneIndices1.x;
    //indices[5] = input.boneIndices1.y;
    //indices[6] = input.boneIndices1.z;
    //indices[7] = input.boneIndices1.w;

    //float3 posModel = float3(0.0f, 0.0f, 0.0f);
    //float3 tangentModel = float3(0.0f, 0.0f, 0.0f);
    
    //// Uniform Scaling ����
    //// (float3x3)boneTransforms ĳ�������� Translation ����
    //for (int i = 0; i < 8; ++i)
    //{
    //    posModel += weights[i] * mul(float4(input.posModel, 1.0f), boneTransforms[indices[i]]).xyz;
    //}

    //input.posModel = posModel;

    output.posWorld = mul(float4(input.posModel, 1.0f), world);


    output.posWorld = mul(output.posWorld, proj);
    output.texcoord = input.texcoord;

    return output;
}
