#include "Model.hlsli"
uint GetMaterialIndex(const uint primitiveID, const uint materialNum)
{
	float end = 0.0f;
	float indicesNum = (float) primitiveID * 3.0f;
	for (uint i = 0; i < materialNum; ++i)
	{
		end += constandFloat[i];
		if (end > indicesNum)
		{
			return i;
		}
	}
	return 0;
}

//ピクセルシェーダ
[RootSignature(RS)]
PixelOut PS(VertexOut input, uint primitiveID : SV_PrimitiveID)
{
	PixelOut po;
	uint matNum, stride;
	materialBase.GetDimensions(matNum, stride);
	uint matIdx = GetMaterialIndex(primitiveID, matNum);
	MaterialBase mat = materialBase[matIdx];
	
	Texture2D<float4> sphTex  = tex[addTexIndex[matIdx * 4 + 0]];
	Texture2D<float4> spaTex  = tex[addTexIndex[matIdx * 4 + 1]];
	Texture2D<float4> addTex  = tex[addTexIndex[matIdx * 4 + 2]];
	Texture2D<float4> toonTex = tex[addTexIndex[matIdx * 4 + 3]];
	
	Texture2D<float4> noiseTex = tex[addTexIndex[matNum * 4]];
	float noiseThreshold = constandFloat[matNum];
	
	Texture2D<float> lightDepthTex = depthTex[1];
	
	// 光源ベクトルの反射ベクトル
	float3 lightDirNormal = normalize(scene.lightVec);
	float3 rLight = reflect(lightDirNormal, input.normal.rgb);

	float bright = saturate(dot(input.normal.xyz, -lightDirNormal));
	float4 toonColor = float4(toonTex.Sample(toomSmp, float2(0, 1.0 - bright)).rgb, 1.0f);

	float4 texColor = tex[mat.textureIndex].Sample(smp, input.uv);
	
	float2 sphUV = (input.normal.xy * float2(1.0f, -1.0f) + float2(1.0f, 1.0f)) / 2.0f;
	float4 sphColor = float4(sphTex.Sample(smp, sphUV).rgb, 1.0f);
	float4 spaColor = spaTex.Sample(smp, sphUV);

	// スペキュラ
	// 視線ベクトル
	float3 eyeRay = normalize(input.pos.rgb - scene.eye);
	float specB = saturate(dot(rLight, -eyeRay));
	specB = pow(max(specB, 0), max(mat.power, 0));
	float4 specColor = float4(saturate(mat.specular * specB).rgb, 0);
	float4 ambientColor = float4((mat.ambient * 0.005f).rgb, 0);
	
	float4 baseColor = saturate(mat.diffuse * texColor);
	float4 ret = float4(saturate(float3(baseColor.rgb * toonColor.rgb * sphColor.rgb + specColor.rgb + spaColor.rgb + ambientColor.rgb)), baseColor.a);
	
	float3 posFromLight = input.posFromLight.xyz / input.posFromLight.w;
	float2 shadowUV = (posFromLight.xy + float2(1, -1)) * float2(0.5f, -0.5f);
	float shadowZ = lightDepthTex.SampleCmpLevelZero(shadowSmp, shadowUV, posFromLight.z);
	float shadowWeight = 1.0f - 0.3f * step(shadowZ + 0.0005f, posFromLight.z);

	float4 noiseColor = noiseTex.Sample(smp, input.uv);
	float noiseValue = dot(noiseColor.rgb, float3(0.33f, 0.34f, 0.33));
	float alpha = ret.a * step(noiseThreshold, noiseValue);
	
	
	ret = float4(ret.rgb * shadowWeight, alpha);
	
	po.color = ret;

	po.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
	po.normal.a = input.normal.a = 1;

	float b = step(0.9f, dot(po.color.rgb, float3(0.3f, 0.4f, 0.3f)));
	float mask = step(noiseValue, noiseThreshold) * step(noiseThreshold, noiseValue + 0.05f);
	po.bright = saturate(float4((float3(1.0f, 0.1f, 0.1f)) * mask, 1.0f));
	
	if (alpha + mask <= 0.0f)
	{
		discard;
	}
	
	return po;
}

//ピクセルシェーダ
float4 ShadowPS(ShadowVertexOut input, uint primitiveID : SV_PrimitiveID) : SV_TARGET
{
	uint matNum, stride;
	materialBase.GetDimensions(matNum, stride);
	uint matIdx = GetMaterialIndex(primitiveID, matNum);
	MaterialBase mat = materialBase[matIdx];
	Texture2D<float4> noiseTex = tex[addTexIndex[matNum * 4]];
	float4 noiseColor = noiseTex.Sample(smp, input.uv);
	float noiseThreshold = constandFloat[matNum];
	float noiseValue = dot(noiseColor.rgb, float3(0.33f, 0.34f, 0.33));
	
	if (step(noiseThreshold, noiseValue) == 0.0f)
	{
		discard;
	}
	
	return float4(1, 1, 1, 1);
}