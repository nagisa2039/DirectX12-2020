#include "VMDMotion.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include "Utility/Tool.h"

using namespace std;
using namespace DirectX;

bool VMDMotion::LoadVMDMotion(std::string filePath)
{
	FILE* fp = nullptr;
	fopen_s(&fp, filePath.c_str(), "rb");

	assert(fp != nullptr);
	if (fp == nullptr)return false;

	fseek(fp, 50, SEEK_CUR);

	// ���[�V�������̓ǂݍ���
	keyFrameNum_ = 0;
	fread(&keyFrameNum_, sizeof(keyFrameNum_), 1, fp);


#pragma pack(1)
	struct t_keyframe
	{
		char boneName[15];	// �{�[����
		uint32_t frameNo;	// �t���[���ԍ�
		DirectX::XMFLOAT3 location;	// ���W
		DirectX::XMFLOAT4 quaternion;	// ��](�N�H�[�^�j�I��)
		uint8_t interpolation[64];	// �⊮
	};
#pragma pack()

	// �L�[���̓ǂݍ���
	vector<t_keyframe> keyFrames(keyFrameNum_);
	fread(keyFrames.data(), sizeof(keyFrames[0]) * keyFrames.size(), 1, fp);

	fclose(fp);

	// �{�[�������ƂɃ}�b�v�ɒǉ�
	for (auto& key : keyFrames)
	{
		animation_[key.boneName].emplace_back(
			KeyFrame(
				key.frameNo, 
				key.location,
				key.quaternion,
				XMFLOAT2((float)key.interpolation[3+15]/127.0f, (float)key.interpolation[7+15]/127.0f),
				XMFLOAT2((float)key.interpolation[11+15]/127.0f, (float)key.interpolation[15+15]/127.0f)));
	}

	// �L�[�t���[���̔ԍ����ɏ����ŕ��ёւ�
	// ��A�ŏI�t���[���̎擾
	lastFrame_ = 0;
	for (auto& anim : animation_)
	{
		auto& animVec = anim.second;
		std::sort(animVec.begin(), animVec.end(), [](const KeyFrame& key1, const KeyFrame& key2)
		{
			return key1.frameNo < key2.frameNo;
		});

		int vecSize = static_cast<int>(animVec.size());
		if (vecSize > 0)
		{
			lastFrame_ = max(lastFrame_, animVec[vecSize - 1].frameNo);
		}
	}

	return true;
}

VMDMotion::VMDMotion(std::wstring filePath)
{
	LoadVMDMotion(StringFromWString(filePath));
}


VMDMotion::~VMDMotion()
{
}

unsigned int VMDMotion::GetKeyFrameNum()
{
	return keyFrameNum_;
}

unsigned int VMDMotion::GetLastFrame()
{
	return lastFrame_;
}

std::map<std::string, std::vector<VMDMotion::KeyFrame>> VMDMotion::GetAnimation()
{
	return animation_;
}
