struct Out
{
	float4 svpos : SV_POSITION;	// Pipelineに投げるためにはSV_POSITIONが必要	カメラからの座標
	float4 pos : POSITION;	// ワールド座標
	float4 tpos : POSITION1;	// 頂点変換後の座標
	float4 normal:NORMAL;	// 法線情報
	float2 uv : TEXCOORD;	// UV情報
	uint instanceID : SV_InstanceID;
};

SamplerState smp : register(s0);
SamplerState toomSmp : register(s1);
SamplerComparisonState shadowSmp : register(s2);

// マテリアル用スロット
cbuffer materialBuffer : register(b0)
{
	float4 diffuse;
	float4 specular;
	float4 ambient;
	float power;
};

// 座標変換用スロット
cbuffer transBuffer : register(b1)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye;		// 視点
};

// 座標行列用スロット
cbuffer worldBuffer : register(b2)
{
	matrix world;
};

// ボーン行列
cbuffer bones : register(b3)
{
	matrix boneMats[512];
};

cbuffer Setting : register(b4)
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
	float4 col:SV_TARGET0;//カラー値を出力
	float4 normal:SV_TARGET1;//法線を出力
	float4 bright:SV_TARGET2;	// 輝度出力
};

// テクスチャ
Texture2D<float4> tex : register(t0);
// スフィアマップ	乗算
Texture2D<float4> sph : register(t1);
// スフィアマップ	加算
Texture2D<float4> spa : register(t2);
// 追加テクスチャ
Texture2D<float4> addtex : register(t3);
// toonテクスチャ
Texture2D<float4> toon : register(t4);
// シャドウマップ用	デプス
Texture2D<float> lightDepthTex : register(t5);

//頂点シェーダ
Out VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, 
	int4 boneno  : BONENO, float4 weight : WEIGHT, uint instanceID : SV_InstanceID)
{
	Out o;

	matrix mixMat = boneMats[boneno.x] * weight.x
		+ boneMats[boneno.y] * weight.y
		+ boneMats[boneno.z] * weight.z
		+ boneMats[boneno.w] * weight.w;

	mixMat = mul(world, mixMat);

	pos = mul(mixMat, pos);
	pos.z += 5 * instanceID;

	matrix camera = mul(proj, view);
	o.svpos = mul(camera, pos);

	o.pos = pos;

	o.tpos = mul(lightCamera, pos);

	normal.w = 0;
	o.normal = mul(mixMat, normal);

	o.uv = uv;

	o.instanceID = instanceID;

	return o;
}

//ピクセルシェーダ
PixelOutPut PS(Out input) : SV_TARGET
{
	PixelOutPut po;

	//return float4(input.normal.xyz,1);
	//// 光源ベクトルの反射ベクトル
	float3 lightDirNormal = normalize(light_dir);
	float3 rLight = reflect(lightDirNormal, input.normal.rgb);

	//// 視線ベクトル
	float3 eyeRay = normalize(input.pos.rgb - eye);
	//// 光源ベクトルの反射ベクトル
	float3 rEye = reflect(eyeRay, input.normal.xyz);

	float2 sphUV = (input.normal.xy*float2(1.0f, -1.0f) + float2(1.0f, 1.0f)) / 2.0f;
	float2 normalUV = (input.normal.xy + float2(1, -1))*float2(0.5, -0.5);

	//// スペキュラ
	float specB = saturate(dot(rLight, -eyeRay));
	if (specB > 0 && power > 0)
	{
		specB = pow(specB, power);
	}


	float bright = saturate(dot(input.normal.xyz, -lightDirNormal));
	float4 toonColor = toon.Sample(toomSmp, float2(0, 1.0 - bright));

	float4 texColor = tex.Sample(smp, input.uv);
	float4 sphColor = sph.Sample(smp, sphUV);
	float4 spaColor = spa.Sample(smp, sphUV);

	float4 diffuseColor = diffuse * float4(sphColor.rgb, 1) + float4(spaColor.rgb,0);
	float4 specColor = float4((specular * specB).rgb, 0);
	float4 ambientColor = float4((ambient * 0.005f).rgb, 0);
	float4 ret = diffuseColor * texColor * toonColor + specColor + ambientColor;
	//return float4(ret);


	float shadowWeight = 1.0f;
	float3 posFromLight = input.tpos.xyz / input.tpos.w;
	float2 shadowUV = (posFromLight.xy + float2(1, -1)) * float2(0.5f, -0.5f);
	float shadowZ = lightDepthTex.SampleCmpLevelZero(shadowSmp, shadowUV, posFromLight.z - 0.005f);
	if (posFromLight.z > shadowZ + 0.0005f)
	{
		shadowWeight = 0.7f;
	}

	float edge = abs(dot(eyeRay, input.normal.xyz)) < edgeWidth ? 1 - edgePower : 1;
	float lim = saturate(1 - dot(-eyeRay, input.normal.xyz));
	lim = pow(lim, limColor.a);

	ret = float4(saturate(ret.rgb * edge + lim * limColor.rgb) * shadowWeight, ret.a);
	
	po.col = ret;

	po.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
	po.normal.a = input.normal.a = 1;

	po.bright = float4(0, 0, 0, 1);
	float b = dot(po.col.rgb, float3(0.3f, 0.6, 0.1f)) > 0.9f ? 1.0f: 0.0f;
	{
		po.bright = float4(b,b,b, 1);
	}

	return po;
}

// 影用座標変換
float4 ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD,
	int4 boneno : BONENO, float4 weight : WEIGHT) : SV_POSITION
{
	matrix mixMat = boneMats[boneno.x] * weight.x
		+ boneMats[boneno.y] * weight.y
		+ boneMats[boneno.z] * weight.z
		+ boneMats[boneno.w] * weight.w;

	pos = mul(world, mul(mixMat, pos));

	return mul(lightCamera, pos);
}
//ピクセルシェーダ
float4 ShadowPS(float4 pos : SV_POSITION) :SV_TARGET
{
	return float4(1,1,1,1);
}