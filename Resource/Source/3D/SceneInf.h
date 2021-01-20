#pragma once
#include "Utility/ComPtr.h"
#include <memory>

class Command;
class ID3D12Device;
class ID3D12Resource;
class ID3D12DescriptorHeap;
class CameraObject;
struct SceneStruct;

/// <summary>
/// 3Dシーン情報を管理
/// </summary>
class SceneInf
{
public:
	/// <param name="cmd">コマンド管理クラス</param>
	/// <param name="dev">デバイス</param>
	SceneInf(Command& cmd, ID3D12Device& dev);
	~SceneInf();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラのヒープを設定
	/// </summary>
	/// <param name="rootParamIdx">rootSignatureインデックス</param>
	void SetCameraDescriptorHeap(const unsigned int rootParamIdx);

	/// <summary>
	/// ImGuiの描画
	/// </summary>
	void DrawImGui();

	/// <summary>
	/// カメラオブジェクトの取得
	/// </summary>
	std::shared_ptr<CameraObject>& GetCameraObject();

private:
	Command& cmd_;
	ID3D12Device& dev_;

	// カメラ行列用定数バッファ
	ComPtr<ID3D12Resource> cameraCB_ = nullptr;
	// transCBを入れるヒープ
	ComPtr<ID3D12DescriptorHeap> cameraHeap_ = nullptr;

	// 定数バッファのアドレスを格納	
	// _cameraCBの内容を変更したいときはこいつを通じて変更してね
	SceneStruct* mappedScene_;

	// カメラ
	std::shared_ptr<CameraObject> cameraObj_;
	// ライト
	std::shared_ptr<CameraObject> lightObj_;

	// カメラの作成
	bool CreateCameraConstantBufferAndView();
};

