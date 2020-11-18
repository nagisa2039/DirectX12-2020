#include "Primitive.hlsli"

[RootSignature(RS)]
PixelOut PS(VertexOut input)
{
	PixelOut po;
	
	Texture2D<float> lightDepthTex = depthTex[1];

	//float4 texColor = tex[].Sample(smp, input.uv);
	float3 light = normalize(float3(1, -1, 1));
	float3 rLight = reflect(light, input.normal.xyz);
	float3 eyeRay = normalize(input.pos.xyz - eye);
	float specB = saturate(dot(rLight, -eyeRay));
	if (specB)
	{
		specB = pow(specB, 20);
	}

	float shadowWeight = 1.0f;
	float3 posFromLight = input.tpos.xyz / input.tpos.w;
	float2 shadowUV = (posFromLight.xy + float2(1, -1)) * float2(0.5f, -0.5f);
	float shadowZ = lightDepthTex.Sample(smp, shadowUV);
	if (posFromLight.z > shadowZ + 0.0005f)
	{
		shadowWeight = 0.5f;
	}

	float4 diffuse = float4(1, 1, 1, 1);
	float bright = saturate(dot(-light, input.normal.rgb));
	float4 difColor = saturate(float4(diffuse.rbg * bright, 1));
	float4 specColor = saturate(float4(specB, specB, specB, 0));
	float3 ambient = float3(0.0, 0.0, 0.0);

	float4 ret = float4(saturate(max((difColor.rgb /** texColor.rgb*/) + specColor.rgb, ambient)) * shadowWeight, diffuse.
	a);
	
	po.color = ret;
	po.color.a = 1;

	po.color = ret;

	po.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
	po.normal.a = input.normal.a = 1;

	po.bright = float4(0, 0, 0, 1);
	float b = step(dot(po.color.rgb, float3(0.3f, 0.4f, 0.3f)), 0.9f);
	{
		po.bright = float4(b, b, b, 1);
	}

	return po;
}

float4 ShadowPS(float4 pos : SV_POSITION) : SV_TARGET
{
	return float4(1, 0, 1, 1);
}