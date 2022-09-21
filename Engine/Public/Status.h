#pragma once

#include "Component.h"
#include "Weapon.h"
/* 객체의 월드상태를 표현하는 월드행렬을 가지고 있는다. */
/* 객체에게 필요한 월드 변환의 기능을 가진다. (직진, 후진, 회전 ) */

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CStatus final : public CComponent
{
public:
	enum STATUS_INFO {INFO_HP, INFO_SHILED, INFO_DAMAGE, INFO_SPEED, INFO_SKILLCOOLTIME, INFO_STATE, INFO_END};
	enum STATUS_STATE { STATUS_STATE_DEFAULT, STATUS_STATE_DEAD ,STATUS_STATE_DAMAGED, STATUS_STATE_HIT, STATUS_STATE_END };
	enum HIT_STATE { HIT_STATE_DEFAULT, HIT_STATE_PUSH, HIT_STATE_KNOCKBACK, HIT_STATE_FIRE, HIT_STATE_TYPEWEAPON_END };

public:
	typedef struct tagStatusDesc //초기 세팅용
	{
		_float			fMaxHp = 0.f;
		_float			fMaxShield = 0.f;
		_float			fSpeed = 0.f;

	}STATUSDESC;
	
	typedef struct NuckBackTime {
		_bool	isStart = false;
		_double Duration = 0.2;
		_double AccTime = 0.0;
	}NUCKBACKDESC;

private:
	explicit CStatus(ID3D11Device* ppDeviceOut, ID3D11DeviceContext* ppDeviceContextOut);
	virtual ~CStatus() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

public:
	void Set_State(STATUS_STATE _State) { m_State = _State; }
	const STATUS_STATE& Get_State() { return m_State; }
	const _float Get_StatusInfo(STATUS_INFO iIndex);
	void Set_Status(STATUS_INFO iIndex, _float _value);
	_int Status_Tick(_double TimeDelta);

	HIT_STATE Get_HitState() { return m_HitState; }
	_float Get_DamagedPower() { return m_DamagedPower; }


public:
	void Set_Damage(_float Damage) { m_Damage = Damage; }
	_float Get_Damage() { return m_Damage; }
	_int Damaged(_float Damage, HIT_STATE HitState, _float Power, _fvector* Dir = nullptr);
	
	void Heal(_float Heal);


	void Set_Exp(_float flag) { m_EXP += flag; }
	_float Get_Exp() { return m_EXP; }

	_float Get_MaxHP() { return m_StatusDesc.fMaxHp; }
	_float Get_HP() { return m_Hp; }

	void Set_Super(_bool flag) { m_Super = flag; }
	_bool Get_Super() { return m_Super; }

	void Set_KnockbackDir(_fvector flag) { m_KnockbackDir = flag; }
	_vector Get_KnockbackDir() { return m_KnockbackDir; }

public:
	static CStatus* Create(ID3D11Device* ppDeviceOut, ID3D11DeviceContext* ppDeviceContextOut);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

private:
	STATUSDESC		m_StatusDesc;
	_float			m_Hp = 0.f;
	_float			m_EXP = 0.f;
	_float			m_Shield = 0.f;
	_float			m_SkillCooltime = 0.f;
	_float			m_Damage = 0.f;
	_float			m_DamagedPower = 0.f;
	_float			m_Power = 0.f;
	_float			m_Speed = 0.f;
	
	_bool			m_Super = false;
	_bool			m_Dead = false;

	_vector			m_KnockbackDir = _vector();
	STATUS_STATE	m_State = STATUS_STATE_DEFAULT;
	HIT_STATE		m_HitState = HIT_STATE_DEFAULT;
	NUCKBACKDESC    m_NuckBackDesc;
};

END