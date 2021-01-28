#pragma once
#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <DirectXMath.h>

/// <summary>
/// VMDモーションデータ
/// </summary>
class VMDMotion
{
public:
	/// <summary>
	/// キーフレーム
	/// </summary>
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

	/// <param name="filePath">モーションファイルパス</param>
	VMDMotion(std::wstring filePath);
	~VMDMotion();

	/// <summary>
	/// キーフレーム数の取得
	/// </summary>
	unsigned int GetKeyFrameNum();

	/// <summary>
	/// 最後のキーのフレーム数取得
	/// </summary>
	unsigned int GetLastFrame();

	/// <summary>
	/// アニメーション情報の取得
	/// </summary>
	std::map<std::string, std::vector<KeyFrame>> GetAnimation();

private:
	unsigned int keyFrameNum_;
	unsigned int lastFrame_;	// モーションの最終フレーム数
	std::map<std::string, std::vector<KeyFrame>> animation_;

	bool LoadVMDMotion(std::string filePath);

};

