#pragma once
#include <vector>

class Mesh;

/// <summary>
/// 3D�`��N���X�̃C���^�[�t�F�[�X
/// </summary>
class Renderer
{
public:
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawShadow()=0;

	/// <summary>
	/// �`�悷��Mesh��ǉ�
	/// </summary>
	/// <param name="mesh">Mesh</param>
	void Add(std::shared_ptr<Mesh> mesh);

private:
	std::vector<std::shared_ptr<Mesh>> drawMeshs_;
};

