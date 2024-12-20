#include "Common.hlsli"

//cbuffer PixelConstantBuffer
//{
    
//};

Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PixelShaderInput input) : SV_Target
{
    return g_texture0.Sample(g_sampler, input.texcoord);
}