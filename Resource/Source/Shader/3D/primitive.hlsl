struct Out
{
	float4 svpos : SV_POSITION;	// Pipelineに投げるためにはSV_POSITIONが必要	カメラからの座標
	float4 pos : POSITION;	// ワールド座標
	float4 tpos : POSITION1;
	float4 normal:NORMAL;	// 法線情報
	float2 uv : TEXCOORD;	// UV情報
};
SamplerState smp : register(s0);

cbuffer transBuffer : register(b0)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye;		// 視点
};

cbuffer worldBuffer : register(b1)
{
	matrix world;
};

struct PixelOutPut
{
	float4 col:SV_TARGET0;//カラー値を出力
	//float4 normal:SV_TARGET1;//法線を出力
	//float4 bright:SV_TARGET2;	// 輝度出力
};

// シャドウマップ用	デプス
Texture2D<float> lightDepthTex : register(t0);
Texture2D<float4> tmpTex : register(t1);

Out VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	Out o;
	pos = mul(world, pos);
	matrix camera = mul(proj, view);
	o.svpos = mul(camera, pos);
	o.tpos = mul(lightCamera, pos);
	o.pos = pos;

	normal.w = 0;
	o.normal = mul(world, normal);

	o.uv = uv;

	return o;
}

PixelOutPut PS(Out input) :SV_Target
{
	PixelOutPut po;

	po.col = float4(1,1,1,1);
	return po;
	
	
	//float4 texColor = tmpTex.Sample(smp, input.uv);
	//return float4(1,1,1,1);
	//return float4(input.normal.xy,1,1);
	//float3 light = normalize(float3(1, -1, 1));
	//float3 rLight = reflect(light, input.normal.xyz);
	//float3 eyeRay = normalize(input.pos.xyz - eye);
	//float specB = saturate(dot(rLight, -eyeRay));
	//if (specB)
	//{
	//	specB = pow(specB,20);
	//}

	//float shadowWeight = 1.0f;
	//float3 posFromLight = input.tpos.xyz / input.tpos.w;
	//float2 shadowUV = (posFromLight.xy + float2(1, -1)) * float2(0.5f, -0.5f);
	//float shadowZ = lightDepthTex.Sample(smp, shadowUV);
	//if (posFromLight.z > shadowZ + 0.0005f)
	//{
	//	shadowWeight = 0.7f;
	//}

	//float4 diffuse = float4(1, 1, 1, 1);
	//float bright = saturate(dot(-light, input.normal.rgb));
	//float4 difColor = saturate( float4(diffuse.rbg * bright,1));
	//float4 specColor = saturate(float4(specB, specB, specB, 0));
	//float3 ambient = float3(0.2,0.2,0.2);

	//float4 ret = float4(saturate((difColor.rgb * texColor.rgb) + specColor.rgb + ambient) * shadowWeight, diffuse.a);
	
	//po.col = ret;
	//po.col.a = 1;

	//po.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
	//po.normal.a = input.normal.a = 1;

	//po.bright = float4(0, 0, 0, 1);
	//float b = dot(po.col.rgb, float3(0.3f, 0.6, 0.1f)) > 0.9 ? 1.0f : 0.0f;
	//{
	//	po.bright = float4(b, b, b, 1);
	//}

	//return po;
}

// 影用座標変換
float4 ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD) : SV_POSITION
{
	pos = mul(world, pos);
	return mul(lightCamera, pos);
}
//ピクセルシェーダ
float4 ShadowPS(float4 pos : SV_POSITION) :SV_TARGET
{
	return float4(1,1,1,1);
}