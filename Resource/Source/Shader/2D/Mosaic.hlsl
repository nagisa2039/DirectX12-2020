#include "2DStanderd.hlsli"

float4 GetMosaicColor(Texture2D tex, float2 uv, float div, float2 aspect)
{
    float2 space = float2(aspect.y, aspect.x) / div;
    float2 fraction = fmod(uv, space) + space / 2.0f;
    float2 mosaicUV = uv - fraction;
	
    float toCenterLen
	= length(float2(0.5f, 0.5f) - fmod(uv * div, float2(1.0f, 1.0f)) * aspect);

    float mask = step(toCenterLen, 0.5f);
	return saturate(tex.Sample(smp, mosaicUV) * mask * 1.5f);
}

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
	PixcelInf pinf = pixcelInf[input.instanceID];
	float w, h, level;
	tex[pinf.texIndex].GetDimensions(0, w, h, level);
	float2 aspect = float2(w / h, 1.0f);
	
	float4 color = GetMosaicColor(tex[pinf.texIndex], input.uv, 200.0f, aspect);
	return float4(color.rgb * pinf.bright, color.a * pinf.alpha);
}