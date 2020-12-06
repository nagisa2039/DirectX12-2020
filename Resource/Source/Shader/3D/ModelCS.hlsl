// ���[�g�V�O�l�`���̐錾
#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
                    "DescriptorTable(UAV(u0, numDescriptors = 1, space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                    "DescriptorTable(SRV(t0, numDescriptors = unbounded, space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                    "DescriptorTable(CBV(b0, numDescriptors = 1, space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                    "DescriptorTable(CBV(b1, numDescriptors = 1, space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                    "StaticSampler(s0)"\

struct Vertex
{
    float4 pos;
    float4 normal;
    float2 uv;
    int4 boneIdx;
    float4 weight;
};

// �������ݐ�
RWStructuredBuffer<Vertex> vertexUAV : register(u0);//0

// ���̒��_�f�[�^
StructuredBuffer<Vertex> baseVertex : register(t0);//1

// �{�[���s��
cbuffer bones : register(b0)//2
{
    matrix boneMats[512];
};

// ���W�s��p�X���b�g
cbuffer worldBuffer : register(b1)//3
{
    matrix world;
};

matrix GetTransform(const int4 boneno, const float4 weight)
{
    matrix mixMat =
		  boneMats[boneno.x] * weight.x
		+ boneMats[boneno.y] * weight.y
		+ boneMats[boneno.z] * weight.z
		+ boneMats[boneno.w] * weight.w;
	
    return mixMat = mul(world, mixMat);
}

[RootSignature(RS)]
[numthreads(1, 1, 1)]
void CS(uint3 groupID : SV_GroupID)
{
    uint index = groupID.x;
    Vertex baseVert = baseVertex[index];
    
    matrix trans = GetTransform(baseVert.boneIdx, baseVert.weight);
    baseVert.pos = mul(trans, baseVert.pos);
    baseVert.normal = mul(trans, baseVert.normal);
    vertexUAV[index] = baseVert;
    vertexUAV[index].uv = float2(1.0f, 0.5f);
}