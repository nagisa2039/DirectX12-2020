#include "2DStanderd.hlsli"

float3 Trans(float3 p)
{
	return p;
	
	float range = 10.0f;
	return fmod(p, range * 2.0f) - range;
}

float SphreDistance(float3 p, float3 c, float r)
{
	return length(Trans(p) - c) - r;
}

float PlaneDistance(float3 p, float3 c)
{
	return abs(p.y - c.y);
}

//[RootSignature(RS)]
//float4 PS(Output input) : SV_TARGET
//{
//	PixcelInf pinf = pixcelInf[input.instanceID];
//	float scale = 0.2f;
//	float2 uv = input.uv * 1.0f / scale;
	
//	if (input.uv.x < scale && input.uv.y < scale)
//	{
//		float d = depthTex.Sample(smp, uv);
//		return float4(d, d, d, 1.0f);
//	}
//	if (input.uv.x < scale && input.uv.y < scale * 2.0f)
//	{
//		float d = lightDepthTex.Sample(smp, uv);
//		return float4(d, d, d, 1.0f);
//	}
	
//	float4 texColor = tex[pinf.texIndex].Sample(smp, input.uv);
//	return float4(texColor.rgb * pixcelInf[input.instanceID].bright, texColor.a * pixcelInf[input.instanceID].alpha);
//}

[RootSignature(RS)]
float4 PS(Output input):SV_TARGET
{
	float w, h, level;
	tex[pixcelInf[input.instanceID].texIndex].GetDimensions(0, w, h, level);
	
	float divide = 1.0f;
	float2 uv = fmod(input.uv, 1.0f / divide) * divide;
	float4 texColor = tex[pixcelInf[input.instanceID].texIndex].Sample(smp, uv);
	//return float4(texColor.rgb * pixcelInf[input.instanceID].bright, texColor.a * pixcelInf[input.instanceID].alpha);
	
	float2 uvpos = uv * float2(2.0f, -2.0f) - float2(1.0f, -1.0f);
	
	//return float4(uvpos * float2(0.5f, -0.5f) + float2(0.5f, 0.5f), 0, 1);
	
	float3 light = normalize(float3(1.0f, -1.0f, 1.0f));
	
	float2 aspect = float2(w / h, 1);
	float3 eye = float3(0, 0, -1);
	float3 tpos = float3(uvpos * aspect * h, 0);
	
	float3 sphCenter = float3(0, 1, 0);
	float rsph = 0.5f;
	
	float3 planeCenter = float3(0,0,0);
	
	float3 ray = normalize(tpos - eye);
	int tryCnt = 64;
	for (int j = 0; j < tryCnt; j++)
	{
		float len = 100000.0f;
		len = min(SphreDistance(eye, sphCenter, rsph), len);
		//len = min(PlaneDistance(eye, planeCenter), len);
		
		eye += ray * len;
		if (len < 0.001f)
		{
			return float4(1, 1, 1, 1);
			float lightBright = dot(normalize(eye - sphCenter), -light);
			float3 color = float3(uv, 1);
			float bright = (float(tryCnt - j) / float(tryCnt)) * lightBright;
			color = color * bright;
			return float4(color, 1);
		}
	}
	return float4(0, 0, 0, 1);
}