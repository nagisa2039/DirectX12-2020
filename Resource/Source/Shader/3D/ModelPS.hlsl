#include "Model.hlsli"

SamplerState smp : register(s0);
SamplerState toomSmp : register(s1);
SamplerComparisonState shadowSmp : register(s2);

Texture2D<float4> tex[512] : register(t0, space0);

struct materialInf
{
	float4 diffuse;
	float4 specular;
	float4 ambient;
	float power;
	
// テクスチャ
	uint texIdx;
// スフィアマップ	乗算
	uint sphIdx;
// スフィアマップ	加算
	uint spaIdx;
// 追加テクスチャ
	uint addtexIdx;
// toonテクスチャ
	uint toonIdx;
};

StructuredBuffer<materialInf> matInfs : register(t0, space1);

struct MaterialIndex
{
	min16uint1 index;
};
StructuredBuffer<MaterialIndex> materialIndexs : register(t0, space3);

// マテリアル用スロット

// 座標変換用スロット
cbuffer transBuffer : register(b1)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye; // 視点
};

// 設定
cbuffer Setting : register(b3)
{
	uint directional_light;
	float3 light_dir;

	float4 limColor;

	float edgeWidth;
	float edgePower;
	uint antialiasing;
	uint insNum;

	uint dof;
	uint ao;
	float aoRadius;
	float gomi;

	float emissive;
	float3 emissiveColor;

	float time;
	float divider;
	uint debug;
};

struct PixelOutPut
{
	float4 col : SV_TARGET; //カラー値を出力
	//float4 normal : SV_TARGET1; //法線を出力
	//float4 bright : SV_TARGET2; // 輝度出力
};
// シャドウマップ用	デプス
Texture2D<float> lightDepthTex : register(t0);

//ピクセルシェーダ
[RootSignature(RS)]
PixelOutPut PS(Out input)
{
	PixelOutPut po;
	po.col = float4(1.0f, 0.0f, 0.0f, 1.0f);
	return po;
	
	materialInf matInf = matInfs[input.vetexID];
	
	
	//return float4(input.normal.xyz,1);
	//// 光源ベクトルの反射ベクトル
	float3 lightDirNormal = normalize( /*light_dir*/float3(1.0f, -1.0f, 1.0f));
	float3 rLight = reflect(lightDirNormal, input.normal.rgb);

	//// 視線ベクトル
	float3 eyeRay = normalize(input.pos.rgb - eye);
	//// 光源ベクトルの反射ベクトル
	float3 rEye = reflect(eyeRay, input.normal.xyz);

	float2 sphUV = (input.normal.xy * float2(1.0f, -1.0f) + float2(1.0f, 1.0f)) / 2.0f;
	float2 normalUV = (input.normal.xy + float2(1, -1)) * float2(0.5, -0.5);

	//// スペキュラ
	float specB = saturate(dot(rLight, -eyeRay));
	if (specB > 0 && matInf.power > 0)
	{
		specB = pow(specB, matInf.power);
	}


	float bright = saturate(dot(input.normal.xyz, -lightDirNormal));
	float4 toonColor = tex[matInf.toonIdx].Sample(toomSmp, float2(0, 1.0 - bright));

	float4 texColor = tex[matInf.texIdx].Sample(smp, input.uv);
	
	po.col = float4(texColor.rgb, 1.0f);
	return po;
	
	float4 sphColor = tex[matInf.sphIdx].Sample(smp, sphUV);
	float4 spaColor = tex[matInf.spaIdx].Sample(smp, sphUV);

	float4 diffuseColor = matInf.diffuse * float4(sphColor.rgb, 1) + float4(spaColor.rgb, 0);
	float4 specColor = float4((matInf.specular * specB).rgb, 0);
	float4 ambientColor = float4((matInf.ambient * 0.005f).rgb, 0);
	float4 ret = diffuseColor * texColor * toonColor + specColor + ambientColor;
	
	po.col = ret;
	return po;


	//float shadowWeight = 1.0f;
	//float3 posFromLight = input.tpos.xyz / input.tpos.w;
	//float2 shadowUV = (posFromLight.xy + float2(1, -1)) * float2(0.5f, -0.5f);
	//float shadowZ = lightDepthTex.SampleCmpLevelZero(shadowSmp, shadowUV, posFromLight.z - 0.005f);
	//if (posFromLight.z > shadowZ + 0.0005f)
	//{
	//	shadowWeight = 0.7f;
	//}

	//float edge = abs(dot(eyeRay, input.normal.xyz)) < edgeWidth ? 1 - edgePower : 1;
	//float lim = saturate(1 - dot(-eyeRay, input.normal.xyz));
	//lim = pow(lim, limColor.a);

	//ret = float4(saturate(ret.rgb * edge + lim * limColor.rgb) * shadowWeight, ret.a);
	
	//po.col = ret;

	//po.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
	//po.normal.a = input.normal.a = 1;

	//po.bright = float4(0, 0, 0, 1);
	//float b = dot(po.col.rgb, float3(0.3f, 0.6, 0.1f)) > 0.9f ? 1.0f : 0.0f;
	//{
	//	po.bright = float4(b, b, b, 1);
	//}

	//return po;
	}