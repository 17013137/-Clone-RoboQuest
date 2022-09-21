#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Status.h"
#include "Boss_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CGoliathBig final : public CGameObject
{
public:
	typedef struct SKILLDESC {
		_double TotalTime = 0.0;
		_double Duration = 5.0;
	}SKILLDESC;

private:
	enum STATE { STATE_IDLE, STATE_MOVE, STATE_ATTACK, STATE_REROAD, STATE_DAMAGED, STATE_CC, STATE_DIE, STATE_END };
	
	enum SKILL_TYPE { SKILL_BASH, SKILL_CHARGE, SKILL_HOOK, SKILL_MORTAR, SKILL_SHOCKWAVE, SKILL_END };

	enum ANIM_STATE { };

	enum 
private:
	typedef struct MOVINGDESC {
		_bool isStart = false;
		_double TotalTime = 0.0;
		_double Duration = 2.0;
	}MOVINGDESC;

	typedef struct ATTACKESC {
		_bool isAttack = false;
		_bool isCoolTime = false;
		_int Progress = 0;
		_double CoolTime = 5.0;
		_double TotalTime = 0.0;

		void Initialize() {
			Progress = 0;
			TotalTime = 0.0;
		}
		void Reset_CoolTime() {
			isCoolTime = true;
			TotalTime = 0.0;
		}
	}ATTACKESC;

private:
	explicit CGoliathBig(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CGoliathBig(const CGoliathBig& rhs);
	virtual ~CGoliathBig() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStatus*			m_pStatus = nullptr;
	CCollider*			m_pSphereCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	void Animation_Tick(_double TimeDelta);
	void State_Tick(_double TimeDelta);
	_int Status_Tick(_double TimeDelta);
	void Set_Bullet();

private:
	CTransform* m_Player = nullptr;
	ANIM_STATE m_iAnimIndex = ANIM_IDLE;
	MOVINGDESC m_MovingDesc;
	ATTACKESC m_AttackDesc;

	_bool m_LockOn = false;
	_int m_randomMove = 0;
	_int m_Stagger = 0;
	_int m_prevStagger = 0;
	_bool m_Damaged = false;

public:
	static CGoliathBig* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END