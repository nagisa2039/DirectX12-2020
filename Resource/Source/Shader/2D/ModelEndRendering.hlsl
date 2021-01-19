#include "2DStanderd.hlsli"

float3 GetBulr(const Texture2D tex, const float dx, const float dy, const float2 uv)
{
	float3 ret = float3(0.0f, 0.0f, 0.0f);
	const uint lapCnt = 1;
	const uint cnt = 1 + 2 * lapCnt;
	const uint totalCnt = cnt * cnt;
	for (uint i = 0; i < totalCnt; ++i)
	{
		ret += tex.Sample(smp, 
			uv + float2( dx * fmod(i, cnt), dy * int(i / cnt) ) -float2(dx, dy) * int(cnt / 2) ).rgb;
	}
	ret /= totalCnt;
	return ret;
}

float3 GetShrinkColor(const Texture2D shrinkTex, const float2 uv)
{
	uint2 defaultSize;
	shrinkTex.GetDimensions(defaultSize.x, defaultSize.y);
	
	const int shrinkCnt = 4;
	float3 ret = float3(0.0f,0.0f,0.0f);
	
    float2 uvSize = float2(1.0f, 0.5f);
    float2 uvOffset = float2(0.0f, 0.0f);
    for (int j = 0; j < shrinkCnt; j++)
    {
		float2 dxdy = 1.0f / float2(defaultSize) * uvSize;
		ret += GetBulr(shrinkTex, dxdy.x, dxdy.y, uv * uvSize + uvOffset);
        uvOffset.y += uvSize.y;
        uvSize *= 0.5f;
    }
    ret /= shrinkCnt;
	
	return ret;
}

float GetLaplacianDepth(const Texture2D<float> depthTex, const float2 uv)
{
	float ret = 0.0f;
	
	uint w, h;
	depthTex.GetDimensions(w, h);
	float dx = 1.0f / w;
	float dy = 1.0f / h;
	
	ret += depthTex.Sample(smp, uv + float2(0.0f, -dy));
	ret += depthTex.Sample(smp, uv + float2(dx * -1.0f, 0.0f));
	ret += depthTex.Sample(smp, uv) * -4.0f;
	ret += depthTex.Sample(smp, uv + float2(dx * 1.0f, 0.0f));
	ret += depthTex.Sample(smp, uv + float2(0.0f, dy));
	
	return ret;
}

float GetLaplacianColor(const Texture2D<float4> normalTex, const float2 uv)
{
	float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	uint w, h;
	normalTex.GetDimensions(w, h);
	float dx = 1.0f / w;
	float dy = 1.0f / h;
	
	ret += normalTex.Sample(smp, uv + float2(0.0f, -dy));
	ret += normalTex.Sample(smp, uv + float2(dx * -1.0f, 0.0f));
	ret += normalTex.Sample(smp, uv) * -4.0f;
	ret += normalTex.Sample(smp, uv + float2(dx * 1.0f, 0.0f));
	ret += normalTex.Sample(smp, uv + float2(0.0f, dy));
	
	return dot(ret.rgb, float3(0.33f, 0.34f, 0.33f));
}

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
	Texture2D colorTex	= tex[addTexIndex[0]];
	Texture2D normalTex = tex[addTexIndex[1]];
	Texture2D brightTex = tex[addTexIndex[2]];
	Texture2D shrinkTex = tex[addTexIndex[3]];
	
	Texture2D baseTex = tex[pixcelInf[input.instanceID].texIndex];
	
	float4 baseColor = colorTex.Sample(smp, input.uv);
	float3 normal = (normalTex.Sample(smp, input.uv).rgb) * 2.0f - 1.0f;
	
	float3 lightDirNormal = normalize( /*light_dir*/float3(1.0f, -1.0f, 1.0f));
	float3 rLight = reflect(lightDirNormal, normal.rgb);
	
	float bright = saturate(dot(-lightDirNormal, normal.rgb));
	float3 shrinkColor = GetShrinkColor(shrinkTex, input.uv);
	float3 color = saturate(baseColor.rgb + shrinkColor * settingData.emmisionRate);
	
	float4 ret = float4(color * pixcelInf[input.instanceID].bright, baseColor.a * pixcelInf[input.instanceID].alpha);
	
    Texture2D<float> depthTex = depthTexVec[0];
	
    float outline = step(0.0001f, GetLaplacianDepth(depthTex, input.uv));
    outline += step(0.2f, GetLaplacianColor(normalTex, input.uv));
    saturate(outline);
    ret.rgb = lerp(ret.rgb, settingData.outlineColor, outline * settingData.outline);
	
	return ret;
}