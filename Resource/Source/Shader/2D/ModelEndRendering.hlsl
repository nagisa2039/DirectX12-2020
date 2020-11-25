#include "2DStanderd.hlsli"

float3 GetBulr(const Texture2D tex, const float dx, const float dy, const float2 uv)
{
	float3 ret = float3(0.0f, 0.0f, 0.0f);
	const int lapCnt = 0;
	const int cnt = 1 + 2 * lapCnt;
	const int totalCnt = cnt * cnt;
	for (int i = 0; i < totalCnt; ++i)
	{
		ret += tex.Sample(smp, uv + float2( dx * fmod(i, cnt), dy * int(i / cnt) ) -float2(dx, dy) * int(cnt / 2) );
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
	float3 color = saturate(baseColor.rgb/* + shrinkColor*/) /** bright*/;
	
	return float4(color * pixcelInf[input.instanceID].bright, baseColor.a * pixcelInf[input.instanceID].alpha);
}