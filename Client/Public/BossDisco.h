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

class CBossDisco final : public CGameObject
{
public:
	typedef struct SKILLDESC {
		_double TotalTime = 0.0;
		_double Duration = 10.0;
	}SKILLDESC;

	typedef struct CHARGEDESC {
		_double TotalTime = 0.0;
		_double ChargeSpeed = 8.0;
		_double ChargeDuration = 2.0;
		_double RecoveryDuration = 5.0;
		_float TotalRot = 360.f;
		_float NowRot = 0;
		_vector vDir;
	}CHARGEDESC;

	typedef struct ARTILLERY_HITBOXDESC {
		_double NormalRegenTime = 0.3;
		_double NormalTime = 0.3;
		_uint NormalTimeStartPointing = 0;
		_double StickRegenTime = 1.0;
		_double StickTime = 1.0;
		_uint StickTimeStartPointing = 0;
		_double WaveRegenTime = 3.0;
		_double WaveTime = 2.0;
		_uint WaveTimeStartPointing = 0;
	}ARTILLERY_HITBOXDESC;

	typedef struct BOUNCINGDESC {
		_double AccTime = 0.0;
		_double Duration = 0.25;
	}BOUNCINGDESC;

	typedef struct MANNERDESC {
		_bool isMannerTime = false;
		_double TimeAcc = 0.0;
	}MANNERDESC;

public:
	enum STATE { STATE_IDLE, STATE_MOVE, STATE_ATTACK, STATE_REROAD, STATE_DAMAGED, STATE_CC, STATE_DIE, STATE_END };

	enum DEFAULT_STATE{ DEFAULT_INTRO, DEFAULT_IDLE, DEFAULT_END};

	enum ARTILLERY_STATE{ ARTILLERY_OUT, ARTILLERY_PRESHOOT, ARTILLERY_SHOOT	, ARTILLERY_END	};

	enum BOUNCINGBOLT_STATE{ BOUNCINGBOLT_OUT, BOUNCINGBOLT_PRESHOOT, BOUNCINGBOLT_SHOOT, BOUNCINGBOLT_END};

	enum CHARGE_STATE{ CHARGE_FOCUS, CHARGE_LOOP, CHARGE_OUT, CHARGE_PRESHOOT, CHARGE_END};

	enum LASER_STATE { LASER_LOOP, LASER_OUT, LASER_PRESHOOT, LASER_END};

	enum SHIELD_STATE { SHIELD_ON, SHIELD_END };

	enum SKILL_TYPE{ SKILL_ARTILLERY, SKILL_BOUNCINGBOLT, SKILL_CHARGE, SKILL_LASER, SKILL_SHIELD, SKILL_END};

	enum BONE_TYPE { BONE_FRMT, BONE_FRMD, BONE_BRMT, BONE_BRMD, BONE_FLMT,  BONE_FLMD,  BONE_BLMT,  BONE_BLMD, BONE_END};

	const char* Bone_Str[BONE_END] = { "Plate_F_R_MT", "Plate_F_L_MT","Plate_F_R_MD", "Plate_F_L_MD", "TinyGun_R_R", "TinyGun_L_R","Plate_B_R_MD", "Plate_B_L_MD" };
private:
	explicit CBossDisco(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBossDisco(const CBossDisco& rhs);
	virtual ~CBossDisco() = default;

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
	void Set_Bone();
	void KeyInput();

private:
	CTransform* m_Player = nullptr;
	DEFAULT_STATE m_State = DEFAULT_INTRO;
	_uint m_iAnimIndex = 0;
	_bool m_isIntro = true;
	_bool m_isSkill[SKILL_END] = { false };
	_uint m_Progression[SKILL_END] = { 0 };
	_float	m_DefaultY = 20.f;
	_bool m_CreateIntro = false;
	_bool m_EndIntro = false;
	_uint m_PrevPattern = 0;


	CBoss_Manager* m_Manager = CBoss_Manager::Get_Instance();
	SKILLDESC m_SkillDesc[SKILL_END];
	CHARGEDESC m_ChargeDesc;
	ARTILLERY_HITBOXDESC m_Artilley_HitBoxDesc;
	BOUNCINGDESC m_BouncingDesc;
	MANNERDESC m_Manner;
	_vector m_PrevPos = _vector();



	_float4 m_BonePart[BONE_END] = { _float4(0.f, 0.f, 0.f, 0.f) };
public:
	static CBossDisco* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END