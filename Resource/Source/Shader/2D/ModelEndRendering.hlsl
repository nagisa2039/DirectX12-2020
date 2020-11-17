#include "2DStanderd.hlsli"

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
	Texture2D colorTex	= tex[addTexIndex[0]];
	Texture2D normalTex = tex[addTexIndex[1]];
	Texture2D brightTex = tex[addTexIndex[2]];
	
	Texture2D baseTex = tex[pixcelInf[input.instanceID].texIndex];
	
	float4 baseColor = colorTex.Sample(smp, input.uv);
	float3 normal = (normalTex.Sample(smp, input.uv).rgb) * 2.0f - 1.0f;
	
	float3 lightDirNormal = normalize( /*light_dir*/float3(1.0f, -1.0f, 1.0f));
	float3 rLight = reflect(lightDirNormal, normal.rgb);
	
	float bright = saturate(dot(-lightDirNormal, normal.rgb));
	float3 color = baseColor.rgb * bright;
	
	return float4(color * pixcelInf[input.instanceID].bright, baseColor.a * pixcelInf[input.instanceID].alpha);
}