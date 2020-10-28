#pragma once
#include <dinput.h>
#include <map>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <windows.h>
#include "Utility/ComPtr.h"
#include "Utility/Geometry.h"

class Input
{
public:
	Input(const HWND hwnd);
	~Input();

	enum class PeripheralType
	{
		keybord,
		//gamepad,
		//mouse,
		max
	};

	/// <summary>
	/// 毎フレーム更新を行う
	/// </summary>
	void Update(void);

	/// <summary>
	/// コマンド設定
	/// </summary>
	/// <param name="cmd">コマンド文字列</param>
	/// <param name="perType">入力番号</param>
	/// <param name="code">Dxlib入力コード</param>
	void AddCommand(const std::string& cmd, const PeripheralType perType, const int code);

	/// <summary>
	/// 今、押されています
	/// </summary>
	/// <param name="cmd">コマンド文字列</param>
	bool GetButton(const std::string& cmd) const;

	/// <summary>
	/// 今まさに押されました
	/// </summary>
	/// <param name="cmd">コマンド文字列</param>
	bool GetButtonDown(const std::string& cmd) const;

	/// <summary>
	/// ボタンを押すのをやめた瞬間
	/// </summary>
	/// <param name="cmd">コマンド文字列</param>
	bool GetButtonUp(const std::string& cmd) const;

	/// <summary>
	/// _inputTblに指定したコマンドが存在するか確認
	/// </summary>
	/// <param name="cmd">コマンド文字列</param>
	bool CheckCommand(const std::string& cmd) const;

	/*/// <summary>
	/// マウス座標の取得
	/// </summary>
	const Vector2i& GetMousePos()const;*/

	/// <summary>
	/// ボタンを押しているかを取得
	/// </summary>
	/// <param name="keycode">DxLibのキーコード</param>
	bool GetButton(const BYTE keycode)const;

	/// <summary>
	/// ボタンを押したフレームかを取得
	/// </summary>
	/// <param name="keycode">DxLibのキーコード</param>
	bool GetButtonDown(const BYTE keycode)const;

	/// <summary>
	/// ボタンを離したフレームを取得
	/// </summary>
	/// <param name="keycode">DxLibのキーコード</param>
	bool GetButtonUp(const BYTE keycode)const;

private:
	// プレイヤー番号と、入力文字列から
	// 元の入力がわかる。つまりキーコンフィグ一覧
	// を表示するときにつかう
	mutable std::map<std::string, std::array<int, static_cast<size_t>(PeripheralType::max)>> inputTbl_;

	using inputFunc_t = std::function<void(const std::string&, const int)>;
	/// <summary>
	/// 機器ごとに入力情報を更新する関数テーブル
	/// </summary>
	std::array<inputFunc_t, static_cast<size_t>(PeripheralType::max)> peripheralInfFuncs_;

	Vector2i mousePos_;		// マウスの座標

	/// <summary>
	/// 入力テーブルを取得する
	/// </summary>
	const std::map<std::string, std::array<int, static_cast<size_t>(PeripheralType::max)>>& GetInputTable()const;

	/// <summary>
	/// 生の入力テーブルを書き換える
	/// </summary>
	/// <param name="rowInputTable">書き換える値</param>
	void SetRowInputTable(const std::map<std::string, std::array<int, static_cast<size_t>(PeripheralType::max)>>& rowInputTable)const;

	/// <summary>
	/// 登録済みのコマンドを書き換える
	/// </summary>
	/// <param name="cmd">コマンド名</param>
	/// <param name="perType">機器種</param>
	/// <param name="code">入力マスク等</param>
	/// <returns>書き換え成功か</returns>
	bool ChangeCommand(const std::string& cmd, const PeripheralType perType, const int code)const;

	std::array<BYTE, 256> GetKeybordState()const;
	std::array<BYTE, 256> GetKeybordDownState()const;
	/*int GetPadState()const;
	int GetPadDownState()const;
	int GetMouseState()const;*/

	ComPtr<IDirectInput8A>		 directInput_;
	ComPtr<IDirectInputDevice8A> keybordDevice_;

	const HWND hwnd_;
};