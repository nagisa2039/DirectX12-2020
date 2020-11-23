#include "Primitive.hlsli"

[RootSignature(RS)]
PixelOut PS(VertexOut input)
{
	PixelOut po;
	
	Texture2D<float> lightDepthTex = depthTex[1];
	
	MaterialBase mat = materialBase[0];
	float4 texColor = tex[mat.textureIndex].Sample(smp, input.uv);
	float3 light = normalize(float3(1, -1, 1));
	float3 rLight = reflect(light, input.normal.xyz);
	float3 eyeRay = normalize(input.pos.xyz - eye);
	
	float specB = saturate(dot(rLight, -eyeRay));
	specB = pow(max(specB, 0), 20);

	float3 posFromLight = input.tpos.xyz / input.tpos.w;
	float2 shadowUV = (posFromLight.xy + float2(1, -1)) * float2(0.5f, -0.5f);
	float shadowZ = lightDepthTex.Sample(smp, shadowUV);
	float shadowWeight = 1.0f - 0.3f * step(shadowZ + 0.0005f, posFromLight.z);

	float4 diffuse = mat.diffuse;
	float bright = saturate(dot(-light, input.normal.rgb));
	float3 specColor = saturate(saturate(mat.specular) * specB);
	float3 ambient = mat.ambient;

	float4 baseColor = saturate(diffuse * texColor);
	float4 ret = float4(
		saturate((baseColor.rgb + specColor) * bright * shadowWeight + ambient),
		baseColor.a);
	
	po.color = ret;

	po.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
	po.normal.a = input.normal.a = 1;
	
    float b = step(0.95f, dot(po.color.rgb * bright, float3(0.3f, 0.4f, 0.3f)));
	po.bright = float4(b, b, b, 1);

	return po;
}

float4 ShadowPS(float4 pos : SV_POSITION) : SV_TARGET
{
	return float4(1, 0, 1, 1);
}