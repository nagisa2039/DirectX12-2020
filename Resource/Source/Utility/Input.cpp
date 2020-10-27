#include <algorithm>
#include <cassert>
#include "Input.h"
#include "Utility/Cast.h"
#include "Utility/Constant.h"
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace std;

namespace
{
	constexpr unsigned int INPUT_RECORD_SIZE = 2;///<summary>入力バッファ格納数</summary>
	constexpr unsigned int INPUT_KEY_SIZE = 256;

	using InputState = std::map<std::string, bool>;
	std::array<InputState, INPUT_RECORD_SIZE> InputState_;
	size_t currentInputStateIdx_;

	std::array<std::array<BYTE, INPUT_KEY_SIZE>, INPUT_RECORD_SIZE> keystate_;		//キーボードの状況

	//std::array<int, INPUT_RECORD_SIZE> padState_;				// 現在のパッドの状況
	//std::array<int, INPUT_RECORD_SIZE> mouseState_;					// 現在のマウス入力情報

	/// <summary>
	/// 次の入力バッファインデックスを返す
	/// </summary>
	/// <returns> 次の入力バッファインデックス </returns>
	size_t GetNextInputBufferIndex()
	{
		return (currentInputStateIdx_ + 1) % INPUT_RECORD_SIZE;
	};

	// 1ﾌﾚｰﾑ前の入力バッファインデックスを返す
	size_t GetLastInputBufferIndex()
	{
		return (currentInputStateIdx_ - 1 + INPUT_RECORD_SIZE) % INPUT_RECORD_SIZE;
	};

	const bool& CurrentInput(const std::string& cmd)
	{
		return InputState_[currentInputStateIdx_][cmd];
	}

	const bool& LastInput(const std::string& cmd)
	{
		return InputState_[GetLastInputBufferIndex()][cmd];
	}
}

Input::Input(const HWND hwnd):hwnd_(hwnd)
{
	// IDirectInput8の作成
	auto ret = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)directInput_.ReleaseAndGetAddressOf(), NULL);
	H_ASSERT(ret);

	ret = directInput_->CreateDevice(GUID_SysKeyboard, keybordDevice_.ReleaseAndGetAddressOf(), NULL);
	H_ASSERT(ret);

	ret = keybordDevice_->SetDataFormat(&c_dfDIKeyboard);
	H_ASSERT(ret);

	ret = keybordDevice_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	H_ASSERT(ret);

	currentInputStateIdx_ = 0;

	InputState_ = {};
	keystate_ = {};

	//mouseState_ = {};
	//padState_ = {};

	//// ゲームパッド
	//peripheralInfFuncs_[static_cast<size_t>(PeripheralType::gamepad)] = [](const string& cmd, const int code)
	//{
	//	InputState_[currentInputStateIdx_][cmd]|= static_cast<bool>(code & padState_[currentInputStateIdx_]);
	//};

	// キーボード
	peripheralInfFuncs_[static_cast<size_t>(PeripheralType::keybord)] = [](const string& cmd, const int code)
	{
		InputState_[currentInputStateIdx_][cmd] |= static_cast<bool>(keystate_[currentInputStateIdx_][code] & 0x80);
	};

	//// マウス
	//peripheralInfFuncs_[static_cast<size_t>(PeripheralType::mouse)] = [](const string& cmd, const int code)
	//{
	//	InputState_[currentInputStateIdx_][cmd] |= static_cast<bool>(code & mouseState_[currentInputStateIdx_]);
	//};
}

Input::~Input()
{
}

void Input::Update(void)
{
	currentInputStateIdx_ = GetNextInputBufferIndex();

	// キーボードの入力状態更新
	std::fill(keystate_[currentInputStateIdx_].begin(), keystate_[currentInputStateIdx_].end(), 0);
	
	auto keybordStateUpdate = [&keybordDev = keybordDevice_, &keystate = keystate_]()
	{
		return keybordDev->GetDeviceState(
			sizeof(keystate[currentInputStateIdx_][0]) * INPUT_KEY_SIZE,
			keystate[currentInputStateIdx_].data());
	};

	auto ret = keybordStateUpdate();
	if (FAILED(ret))
	{
		keybordDevice_->Acquire();
		keybordStateUpdate();
	}

	//// マウスの入力情報更新
	//mouseState_[currentInputStateIdx_] = GetMouseInput();
	//GetMousePoint(&mousePos_.x, &mousePos_.y);

	//// パッド入力
	//padState_[currentInputStateIdx_] = GetJoypadInputState(DX_INPUT_PAD1);

	// 状態のリセット
	for (auto& state : InputState_[currentInputStateIdx_])
	{
		state.second = false;
	}

	// pair = <コマンド文字列, PeripheralInfo(code = 入力コード, periNo = 入力機器番号)>
	for (auto pair : inputTbl_)
	{
		for (int i = 0; i < pair.second.size();++i)
		{
			peripheralInfFuncs_[i](pair.first, Int32(pair.second[i]));
		}
	}
}

void Input::AddCommand(const std::string& cmd, const Input::PeripheralType perType, const int code)
{
	if (inputTbl_.find(cmd) == inputTbl_.end())
	{
		inputTbl_[cmd] = {0};
	}
	inputTbl_[cmd][static_cast<size_t>(perType)] = code;

	// 押下情報を初期化
	for (auto& inputState : InputState_)
	{
		inputState[cmd] = false;
	}
}

bool Input::GetButton(const std::string& cmd) const
{
	// 範囲外制御
	if (!CheckCommand(cmd))
	{
		return false;
	}

	return CurrentInput(cmd);
}

bool Input::GetButtonDown(const std::string& cmd) const
{
	// 範囲外制御
	if (!CheckCommand(cmd))
	{
		return false;
	}

	return CurrentInput(cmd) && !LastInput(cmd);
}

bool Input::GetButtonUp(const std::string& cmd) const
{
	// 範囲外制御
	if (!CheckCommand(cmd))
	{
		return false;
	}

	return !CurrentInput(cmd) && LastInput(cmd);
}

bool Input::CheckCommand(const std::string& cmd) const
{
	size_t lastIdx = GetLastInputBufferIndex();

	return (InputState_[currentInputStateIdx_].find(cmd)	!= InputState_[currentInputStateIdx_].end()
		 && InputState_[lastIdx].find(cmd)					!= InputState_[lastIdx].end());
}

//const Vector2i& Input::GetMousePos()const
//{
//	return mousePos_;
//}

bool Input::GetButton(const char keycode)const
{
	return keystate_[currentInputStateIdx_][keycode];
}

bool Input::GetButtonDown(const char keycode)const
{
	size_t lastIdx = (currentInputStateIdx_ - 1 + INPUT_RECORD_SIZE) % INPUT_RECORD_SIZE;
	return keystate_[currentInputStateIdx_][keycode] && !keystate_[lastIdx][keycode];
}

bool Input::GetButtonUp(const char keycode)const
{
	size_t lastIdx = (currentInputStateIdx_ - 1 + INPUT_RECORD_SIZE) % INPUT_RECORD_SIZE;
	return !keystate_[currentInputStateIdx_][keycode] && keystate_[lastIdx][keycode];
}

const std::map<std::string, std::array<int, static_cast<size_t>(Input::PeripheralType::max)>>& Input::GetInputTable() const
{
	return inputTbl_;
}

void Input::SetRowInputTable(const std::map<std::string, std::array<int, static_cast<size_t>(PeripheralType::max)>>& rowInputTable) const
{
	inputTbl_ = rowInputTable;
}

bool Input::ChangeCommand(const std::string& cmd, const PeripheralType perType, const int code)const
{
	if (inputTbl_.find(cmd) == inputTbl_.end())
	{
		return false;
	}

	inputTbl_[cmd][static_cast<size_t>(perType)] = code;
	return true;
}

std::array<BYTE, 256> Input::GetKeybordState() const
{
	return keystate_[currentInputStateIdx_];
}

std::array<BYTE, 256> Input::GetKeybordDownState() const
{
	auto downState = std::array<BYTE, INPUT_KEY_SIZE>();
	auto lastIdx = GetLastInputBufferIndex();
	for (int i = 0; i < INPUT_KEY_SIZE; ++i)
	{
		bool current = static_cast<bool>(keystate_[currentInputStateIdx_][i]);
		bool last = static_cast<bool>(keystate_[lastIdx][i]);
		downState[i] = static_cast<char>(current && !last);
	}
	return downState;
}

//int Input::GetPadState() const
//{
//	return padState_[currentInputStateIdx_];
//}
//
//int Input::GetPadDownState() const
//{
//	int downState = 0;
//	auto lastIdx = GetLastInputBufferIndex();
//	for (int i = 0; i < sizeof(downState) * 8; ++i)
//	{
//		auto bit = 1 << i;
//		bool current = static_cast<bool>(padState_[currentInputStateIdx_] & bit);
//		bool last = static_cast<bool>(padState_[lastIdx] & bit);
//		if (current && !last)
//		{
//			downState |= bit;
//		}
//	}
//	return downState;
//}
//
//int Input::GetMouseState() const
//{
//	return mouseState_[currentInputStateIdx_];
//}