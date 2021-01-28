#pragma once
#include "3D/Component.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include "3D/Actor.h"
#include <functional>
#include <array>

/// <summary>
/// �J�����N���X
/// </summary>
class Camera
	: public Component
{
public:
	/// <param name="owner">���L��</param>
	Camera(std::weak_ptr<Actor>owner);
	~Camera();

	/// <summary>
	/// ������
	/// </summary>
	void Init()override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update()override;

	/// <summary>
	/// �^�[�Q�b�g���W�̎擾
	/// </summary>
	DirectX::XMFLOAT3 GetTargetPos()const;

	/// <summary>
	/// View�s��̎擾
	/// </summary>
	DirectX::XMMATRIX GetViewMatrix()const;

	/// <summary>
	/// Projection�s��̎擾
	/// </summary>
	DirectX::XMMATRIX GetProjMatrix()const;

	/// <summary>
	/// FOV�̎擾
	/// </summary>
	float GetFOV()const;

	/// <summary>
	/// ImGui�̕`��
	/// </summary>
	void DrawImGui()override;

	// Projection�s��̐ݒ�
	enum class ProjectionMode
	{
		// �������e
		Perspective,
		// �����e
		Orthographic,
		Max
	};

	/// <summary>
	/// ���e���[�h��ݒ�
	/// </summary>
	/// <param name="pm">���e���[�h</param>
	void SetProjectionMode(const ProjectionMode pm);

private:
	using ProjectionFunc = std::function<DirectX::XMMATRIX(float, float, float)>;
	std::array<ProjectionFunc, static_cast<size_t>(ProjectionMode::Max)> projectionFuncTable_;
	ProjectionMode projectionMode_;

	float fov_;
};

/// <summary>
/// �J�����I�u�W�F�N�g
/// </summary>
class CameraObject
	: public Actor
{
public:
	/// <param name="keybordMove">�L�[�{�[�h�ł̈ړ���L�������邩</param>
	CameraObject(const bool keybordMove);
	~CameraObject();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="pm">���e���[�h</param>
	void Init(const Camera::ProjectionMode pm);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update()override;

	/// <summary>
	/// �J�����̎擾
	/// </summary>
	std::shared_ptr<Camera>& GetCamera();

private:
	std::shared_ptr<Camera> camera_;
	bool keybordMove_;
};