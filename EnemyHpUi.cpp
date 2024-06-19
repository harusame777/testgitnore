#include "stdafx.h"
#include "EnemyHpUi.h"
#include "Game.h"
#include "Enemy.h"
#include "Player.h"

namespace {
	/// <summary>
	/// delaytimer�̏����ݒ莞��
	/// </summary>
	float m_delaytime = 0.7f;
	/// <summary>
	/// B��C�̊�_
	/// </summary>
	const Vector2 n_pivot_BC{ 0,0.5 };
	/// <summary>
	/// �\�����E����
	/// </summary>
	const float LimitedRange = 600.0f;
}

bool EnemyHpUi::Start()
{
	//�摜�ݒ�
	m_hpUI_A.Init("Assets/modelData/ui_hp/Enemy_UI_A.DDS", 100.0f, 15.0f);
	m_hpUI_B.Init("Assets/modelData/ui_hp/Enemy_UI_B.DDS", 100.0f, 10.0f);
	m_hpUI_C.Init("Assets/modelData/ui_hp/Enemy_UI_C.DDS", 100.0f, 10.0f);
	//��_�ݒ�
	m_hpUI_B.SetPivot(n_pivot_BC);
	m_hpUI_C.SetPivot(n_pivot_BC);

	//�Q�[���̃C���X�^���X�������Ă���
	m_game = FindGO<Game>("game");
	//�v���C���[�̃C���X�^���X�������Ă���
	m_player = FindGO<Player>("player");

	//HP��������
	SetEnemyHp();

	return true;
}

void EnemyHpUi::Update()
{
	//�\�������v�Z����
	DisplayDis();
	//�ʒu�X�V����
	PositionUpdate();
	//���Z�v�Z����
	Adjustment();
	//���ߌ��Z�v�Z����
	AdjustmentTransparent();
	//�`�揈��
	m_hpUI_A.Update();
	m_hpUI_B.Update();
	m_hpUI_C.Update();
}

void EnemyHpUi::DeleteUi()
{
	DeleteGO(this);
}

void EnemyHpUi::DisplayDis()
{
	//�v���C���[�̈���擾
	Vector3 DisplayCenterPos = m_player->GetPosition();
	//���g�̔z��ԍ������v����G�l�~�[�̈ʒu���擾
	Vector3 DisplayTargetPos = m_game->GetEnemyListPos(m_Vectornum);

	//�擾�����v���C���[�̈ʒu����擾�����G�l�~�[�̈ʒu�܂ŐL�т�x�N�g�����v�Z
	Vector3 diff = DisplayTargetPos - DisplayCenterPos;

	if (diff.LengthSq() >= LimitedRange * LimitedRange)
	{
		//�\�����Ȃ��悤�ɂ���B
		m_isImage = false;
	}
	else
	{
		//�\������
		m_isImage = true;
	}
}

void EnemyHpUi::PositionUpdate()
{
	Vector3 position = m_game->GetEnemyListPos(m_Vectornum);
	//�I�u�W�F�N�g�̏�̕��ɉ摜��\���������̂ŁB
	//y���W�������傫������B
	position.y += 80.0f;
	//���[���h���W����X�N���[�����W���v�Z�B
	//�v�Z���ʂ�m_positionA�ɑ�������B
	g_camera3D->CalcScreenPositionFromWorldPosition(m_positionA, position);
	m_hpUI_A.SetPosition(Vector3(m_positionA.x, m_positionA.y, 0.0f));
	//B��C�ɂ��ʒu�ݒ�
	m_hpUI_B.SetPosition(Vector3(m_positionA.x - 50.0f, m_positionA.y, 0.0f));
	m_hpUI_C.SetPosition(Vector3(m_positionA.x - 50.0f, m_positionA.y, 0.0f));
}

void EnemyHpUi::Adjustment()
{
	//0�ȉ��ɂ��Ȃ�
	if (m_nowhp < 0) {
		m_nowhp = 0;
	}
	//����HP������������
	if (m_nowhp < m_oldhp)
	{
		float a = m_nowhp;
		float b = m_fullhp;
		//����HP�ƍő�HP�̊������v�Z
		float ratio = a / b;
		//���݂�m_hpUI_B��scale.x�Ɗ������v�Z
		Vector3 changeS_B = m_scale_B;
		changeS_B.x = m_scale_B.x * ratio;
		m_hpUI_B.SetScale(changeS_B);
		//�ߋ�HP�Ɍ��݂�HP��ۑ�
		m_oldhp = m_nowhp;
		//���ߒx���J�n
		m_decrease_TRAN = en_DelayTimeON_TRAN;
	}
}

void EnemyHpUi::AdjustmentTransparent()
{
	//�X�^���o�C��Ԃŏ��������s���Ȃ�
	if (m_decrease_TRAN == en_Standby_TRAN)
		return;

	//����UI�����v�Z���܂��͓��ߌ������ł����
	if (m_decrease_TRAN == en_TransparentDecreaseCalc_TRAN 
		|| m_decrease_TRAN == en_TransparentDecreaseON_TRAN)
	{
		if (m_decrease_TRAN == en_TransparentDecreaseCalc_TRAN)
		{
			float a = m_nowhp;
			float b = m_fullhp;
			//����HP�ƍő�HP�̊������v�Z
			float ratio = a / b;
			//���݂�m_hpUI_B��scale.x�Ɗ������v�Z
			m_changeS_C.x = m_scale_C.x * ratio;
			//���������Ɉڍs
			m_decrease_TRAN = en_TransparentDecreaseON_TRAN;
		}

		//���������A�J��Ԃ������Ōv�Z���������ƌ��݂�Scale.x�Ɠ��l�ɂȂ�܂Ō��Z
		if (m_decrease_TRAN == en_TransparentDecreaseON_TRAN)
		{
			//�߂��Ⴍ����ׂ������炷
			m_nowScale.x -= 0.009;
			//0�ȉ���������0�ɖ߂�
			if (m_nowScale.x < 0)
			{
				m_nowScale.x = 0;
			}
			//�傫����ύX����
			m_hpUI_C.SetScale(m_nowScale);

			//����scale���ύX��scale�ȉ��ɂȂ�����
			if (m_changeS_C.x > m_nowScale.x)
			{
				//�O�̂��ߏ�����
				m_changeS_C = m_scale_C;
				m_nowScale = m_hpUI_C.GetScale();
				//�X�^���o�C�Ɉڍs
				m_decrease_TRAN = en_Standby_TRAN;
				return;
			}
			//�ύX��ȏ�̏ꍇ
			else
				return;
		}
	}

	//�������牺�̏����͌����x���̃^�C�}�[����

	//delaytimer��������
	if (0.0 < m_delaytimer 
		&& m_decrease_TRAN == en_DelayTimeLoop_TRAN)
	{
		m_delaytimer -= g_gameTime->GetFrameDeltaTime();
	}
	//delaytimer��0�ɂȂ�����
	else if (m_delaytimer < 0.0 
		&& m_decrease_TRAN == en_DelayTimeLoop_TRAN)
	{
		//����UI���Z�����v�Z�����Ɉڍs
		m_decrease_TRAN = en_TransparentDecreaseCalc_TRAN;
		return;
	}

	//m_delayloop��true�̎��͂��������͏������Ȃ�
	if (m_decrease_TRAN == en_DelayTimeLoop_TRAN)
		return;

	//���ߌ����x���^�C�}�[������
	m_delaytimer = m_delaytime;
	//�x�������Ɉڍs
	m_decrease_TRAN = en_DelayTimeLoop_TRAN;
}

void EnemyHpUi::Render(RenderContext& rc)
{
	if (m_isImage)
	{
		//HPUI�̕`�揈������
		m_hpUI_A.Draw(rc);
		//�o�[�̕`�揈�������
		m_hpUI_B.Draw(rc);
		//���߃o�[�̕`�揈������Ԍ��
		m_hpUI_C.Draw(rc);
	}
}