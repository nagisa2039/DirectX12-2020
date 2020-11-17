#include "2DStanderd.hlsli"

float4 GetMosaicColor(Texture2D tex, float2 uv, float div, float2 aspect)
{
	float fmodNum = 1.0f / div;
	float2 mosaicUV = uv - fmod(uv, float2(fmodNum * aspect.y, fmodNum * aspect.x));
	return tex.Sample(smp, mosaicUV);
}

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
	PixcelInf pinf = pixcelInf[input.instanceID];
	float w, h, level;
	tex[pinf.texIndex].GetDimensions(0, w, h, level);
	float2 aspect = float2(w / h, 1.0f);
	
	float4 color = GetMosaicColor(tex[pinf.texIndex], input.uv, 100.0f, aspect);
	return float4(color.rgb * pinf.bright, color.a * pinf.alpha);
}