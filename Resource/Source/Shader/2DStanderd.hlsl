struct Out 
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);

cbuffer indexBuffer : register(b0)
{
	matrix posTrans;
	matrix uvTrans;
}

//頂点シェーダ
Out VS(float4 pos : POSITION, float4 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
	Out o;

	o.svpos = mul(posTrans, pos);
	//o.svpos = pos;

	/*float2 wsize = float2(1280.0f, 720.0f);
	o.svpos.x = o.svpos.x / (wsize.x / 2.0f) - 1.0f;
	o.svpos.y = (o.svpos.y / (wsize.y / 2.0f) - 1.0f) * -1.0f;*/

	float4 calUV = mul(uvTrans, uv);
	o.uv = float2(calUV.x, calUV.y);

	return o;
}

//ピクセルシェーダ
float4 PS(Out input) :SV_Target
{
	//return float4(1,0,0,1);
	//return float4(input.uv,1,1);
	return tex.Sample(smp, input.uv);
}