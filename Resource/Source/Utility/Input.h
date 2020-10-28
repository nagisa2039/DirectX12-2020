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
	/// ���t���[���X�V���s��
	/// </summary>
	void Update(void);

	/// <summary>
	/// �R�}���h�ݒ�
	/// </summary>
	/// <param name="cmd">�R�}���h������</param>
	/// <param name="perType">���͔ԍ�</param>
	/// <param name="code">Dxlib���̓R�[�h</param>
	void AddCommand(const std::string& cmd, const PeripheralType perType, const int code);

	/// <summary>
	/// ���A������Ă��܂�
	/// </summary>
	/// <param name="cmd">�R�}���h������</param>
	bool GetButton(const std::string& cmd) const;

	/// <summary>
	/// ���܂��ɉ�����܂���
	/// </summary>
	/// <param name="cmd">�R�}���h������</param>
	bool GetButtonDown(const std::string& cmd) const;

	/// <summary>
	/// �{�^���������̂���߂��u��
	/// </summary>
	/// <param name="cmd">�R�}���h������</param>
	bool GetButtonUp(const std::string& cmd) const;

	/// <summary>
	/// _inputTbl�Ɏw�肵���R�}���h�����݂��邩�m�F
	/// </summary>
	/// <param name="cmd">�R�}���h������</param>
	bool CheckCommand(const std::string& cmd) const;

	/*/// <summary>
	/// �}�E�X���W�̎擾
	/// </summary>
	const Vector2i& GetMousePos()const;*/

	/// <summary>
	/// �{�^���������Ă��邩���擾
	/// </summary>
	/// <param name="keycode">DxLib�̃L�[�R�[�h</param>
	bool GetButton(const BYTE keycode)const;

	/// <summary>
	/// �{�^�����������t���[�������擾
	/// </summary>
	/// <param name="keycode">DxLib�̃L�[�R�[�h</param>
	bool GetButtonDown(const BYTE keycode)const;

	/// <summary>
	/// �{�^���𗣂����t���[�����擾
	/// </summary>
	/// <param name="keycode">DxLib�̃L�[�R�[�h</param>
	bool GetButtonUp(const BYTE keycode)const;

private:
	// �v���C���[�ԍ��ƁA���͕����񂩂�
	// ���̓��͂��킩��B�܂�L�[�R���t�B�O�ꗗ
	// ��\������Ƃ��ɂ���
	mutable std::map<std::string, std::array<int, static_cast<size_t>(PeripheralType::max)>> inputTbl_;

	using inputFunc_t = std::function<void(const std::string&, const int)>;
	/// <summary>
	/// �@�킲�Ƃɓ��͏����X�V����֐��e�[�u��
	/// </summary>
	std::array<inputFunc_t, static_cast<size_t>(PeripheralType::max)> peripheralInfFuncs_;

	Vector2i mousePos_;		// �}�E�X�̍��W

	/// <summary>
	/// ���̓e�[�u�����擾����
	/// </summary>
	const std::map<std::string, std::array<int, static_cast<size_t>(PeripheralType::max)>>& GetInputTable()const;

	/// <summary>
	/// ���̓��̓e�[�u��������������
	/// </summary>
	/// <param name="rowInputTable">����������l</param>
	void SetRowInputTable(const std::map<std::string, std::array<int, static_cast<size_t>(PeripheralType::max)>>& rowInputTable)const;

	/// <summary>
	/// �o�^�ς݂̃R�}���h������������
	/// </summary>
	/// <param name="cmd">�R�}���h��</param>
	/// <param name="perType">�@���</param>
	/// <param name="code">���̓}�X�N��</param>
	/// <returns>��������������</returns>
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