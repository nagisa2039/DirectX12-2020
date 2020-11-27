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
		unsigned int frameNo;			// キーフレーム番号
		DirectX::XMFLOAT3 location;		// 移動
		DirectX::XMFLOAT4 quaternion;	// 回転
		DirectX::XMFLOAT2 point1, point2;		// ベジェ曲線用ポイント
		KeyFrame() : frameNo(0), location(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)), 
			quaternion(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)), 
			point1(DirectX::XMFLOAT2(0.0f, 0.0f)), point2(DirectX::XMFLOAT2(0.0f, 0.0f)) {};
		KeyFrame(unsigned int fno, DirectX::XMFLOAT3 l, DirectX::XMFLOAT4& q, DirectX::XMFLOAT2 p1, DirectX::XMFLOAT2 p2)
			: frameNo(fno), location(l), quaternion(q), point1(p1), point2(p2){};
	};

	// (モーションファイルパス)
	VMDMotion(std::wstring filePath);
	~VMDMotion();

	// キーフレーム数の取得
	unsigned int GetKeyFrameNum();
	// 最後のキーのフレーム数取得
	unsigned int GetLastFrame();
	// アニメーション情報の取得
	std::map<std::string, std::vector<KeyFrame>> GetAnimation();

private:
	unsigned int keyFrameNum_;
	unsigned int lastFrame_;	// モーションの最終フレーム数
	std::map<std::string, std::vector<KeyFrame>> animation_;

	bool LoadVMDMotion(std::string filePath);

};

