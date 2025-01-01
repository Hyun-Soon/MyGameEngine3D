#include "Common.hlsli"

Texture2D albedoTex : register(t0);
SamplerState g_sampler : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.pixelColor = albedoTex.Sample(g_sampler, input.texcoord);
    
    
    return output;
}
