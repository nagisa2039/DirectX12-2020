#include "2DStanderd.hlsli"

float3 Trans(float3 p)
{
	float range = 4.0f;
	return (fmod(abs(p), range) - range / 2.0f) * sign(p);
}

float SphreDistance(float3 p)
{
	float r = 0.5f;
	float3 transPos = Trans(p);
	return length(transPos ) - r;
}

float BoxDistnace(float3 p)
{
	float size = 0.7f;
	return length(max(abs(Trans(p)) - float3(size, size, size), 0.0f)) - 0.1f;
}

float GetDistnace(float3 p)
{
	float sphereD = SphreDistance(p);
	float boxD = BoxDistnace(p);
	return boxD;
	//return max(sphereD, boxD);
}

float3 GetNormal(float3 p)
{
	float d = 0.0001;
	return normalize(float3(
    GetDistnace(p + float3(d, 0.0f, 0.0f)) - GetDistnace(p + float3(-d, 0.0f, 0.0f)),
	GetDistnace(p + float3(0.0f, d, 0.0f)) - GetDistnace(p + float3(0.0f, -d, 0.0f)),
	GetDistnace(p + float3(0.0f, 0.0f, d)) - GetDistnace(p + float3(0.0f, 0.0f, -d))));
}

float PlaneDistance(float3 p, float3 c)
{
	return abs(p.y - c.y);
}

[RootSignature(RS)]
float4 PS(Output input):SV_TARGET
{
		PixcelInf pinf = pixcelInf[input.instanceID];
		float w, h, level;
		tex[pinf.texIndex].GetDimensions(0, w, h, level);
	
		float divide = 1.0f;
		float2 uv = fmod(input.uv, 1.0f / divide) * divide;
		float4 texColor = tex[pinf.texIndex].Sample(smp, uv);
    
		float scale = 0.2f;
		float2 debugTexUV = input.uv * (1.0f / scale);
		if (input.uv.x < scale && input.uv.y < scale)
		{
			float d = 1.0f - depthTex[0].Sample(smp, debugTexUV);
			return float4(d, d, d, 1.0f);
		}
		if (input.uv.x < scale && input.uv.y < scale * 2.0f)
		{
			float d = 1.0f - depthTex[1].Sample(smp, debugTexUV);
			return float4(d, d, d, 1.0f);
		}
    
		if (texColor.a <= 0.0)
		{
			float2 uvpos = uv * float2(2.0f, -2.0f) - float2(1.0f, -1.0f);
	
			float3 light = normalize(float3(1.0f, -1.0f, 1.0f));
	
			float2 aspect = float2(w / h, 1.0f);
			float3 eye = float3(0, 0, -1);
			float3 tpos = float3(uvpos * aspect, 0);
	
			float3 sphCenter = float3(0, 0, 0);
			float rsph = 0.5f;
	
			float3 planeCenter = float3(0, 0, 0);
	
			float3 pos = eye;
			float3 ray = normalize(tpos - eye);
			int tryCnt = 128;
			for (int j = 0; j < tryCnt; j++)
			{
				float len = GetDistnace(pos);
				 //len = min(PlaneDistance(eye, planeCenter), len);
		
				pos += ray * len;
				if (len < 0.001f)
				{
					float3 normal = GetNormal(pos);
					float lightBright = dot(normal, -light);
					float lim = 1.0f - dot(-ray, normal);
					float3 color = float3(1, 1, 1);
					float fog = j / float(tryCnt);
                
					float3 refLight = reflect(light, normal);
					float spec = dot(refLight, -ray);
                
				color = saturate(color * lightBright * fog + lim + pow(spec, 5.0f));
					return float4(color, 1);
				}
			}
        
			float b = saturate(1.0f - saturate(length(uvpos) * 2.0f));
			return float4(b, b, b, 1);
		}
	
		return float4(texColor.rgb * pinf.bright, texColor.a * pinf.alpha);
	}