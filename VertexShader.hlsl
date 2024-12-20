#include "Common.hlsli"

cbuffer VertexConstantBuffer : register(b0)
{
    matrix world;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    
    float scale = 0.01;
    float4 pos = float4(input.pos * scale, 1.0f);
    
    pos = mul(pos, world);
    pos = mul(pos, view);
    pos = mul(pos, projection);
   
    output.posProj = pos;
    output.texcoord = input.texcoord;
    
    return output;
}