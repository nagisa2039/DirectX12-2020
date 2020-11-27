#pragma once
#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <DirectXMath.h>

class VMDMotion
{
public:
	struct KeyFrame
	{
		unsigned int frameNo;			// �L�[�t���[���ԍ�
		DirectX::XMFLOAT3 location;		// �ړ�
		DirectX::XMFLOAT4 quaternion;	// ��]
		DirectX::XMFLOAT2 point1, point2;		// �x�W�F�Ȑ��p�|�C���g
		KeyFrame() : frameNo(0), location(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)), 
			quaternion(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)), 
			point1(DirectX::XMFLOAT2(0.0f, 0.0f)), point2(DirectX::XMFLOAT2(0.0f, 0.0f)) {};
		KeyFrame(unsigned int fno, DirectX::XMFLOAT3 l, DirectX::XMFLOAT4& q, DirectX::XMFLOAT2 p1, DirectX::XMFLOAT2 p2)
			: frameNo(fno), location(l), quaternion(q), point1(p1), point2(p2){};
	};

	// (���[�V�����t�@�C���p�X)
	VMDMotion(std::wstring filePath);
	~VMDMotion();

	// �L�[�t���[�����̎擾
	unsigned int GetKeyFrameNum();
	// �Ō�̃L�[�̃t���[�����擾
	unsigned int GetLastFrame();
	// �A�j���[�V�������̎擾
	std::map<std::string, std::vector<KeyFrame>> GetAnimation();

private:
	unsigned int keyFrameNum_;
	unsigned int lastFrame_;	// ���[�V�����̍ŏI�t���[����
	std::map<std::string, std::vector<KeyFrame>> animation_;

	bool LoadVMDMotion(std::string filePath);

};

