struct Out 
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);

cbuffer trans : register(b0)
{
	matrix posTrans;
	matrix uvTrans;
}

//頂点シェーダ
Out VS(float4 pos : POSITION, float4 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
	Out o;

	o.svpos = mul(posTrans, pos);

	float4 calUV = mul(uvTrans, uv);
	o.uv = float2(calUV.x, calUV.y);

	return o;
}

//ピクセルシェーダ
float4 PS(Out input) :SV_Target
{
	return tex.Sample(smp, input.uv);
}