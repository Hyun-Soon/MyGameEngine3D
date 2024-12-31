#include "Common.hlsli"

Texture2D albedoTex : register(t0);
SamplerState g_sampler : register(s0);

//float4 main(PixelShaderInput input) : SV_Target
//{
//    return g_texture0.Sample(g_sampler, input.texcoord);
//}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.pixelColor = albedoTex.Sample(g_sampler, input.texcoord);
    
    return output;
}
