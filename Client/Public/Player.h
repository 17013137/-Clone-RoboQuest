#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Status.h"
#include "Player_Manager.h"
#include "Event_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	typedef struct SITDESC {
		_float fTime = 0.f;
		_float fSize = 0.f;
		_float fPower = 0.f;
		_bool isSit = false;
	}SITDESC;

	typedef struct SUPERDESC {
		_bool Super = false;
		_double SuperTime = 2.0;
		_double SuperAccTime = 0.0;
	}SUPERDESC;

public:
	enum STATE{STATE_IDLE, STATE_MOVE, STATE_DASH, STATE_JUMP, STATE_SLIDE,  STATE_SIT, STATE_SITMOVE, STATE_ATTACK, STATE_REROAD, STATE_DAMAGED, STATE_CC, STATE_DIE, STATE_END};
	
	enum ANIMATION_STATE{ANIM_DEATH, ANIM_SECONDARYSHOOT, ANIM_STAB, ANIM_IDLE, ANIM_RUN_FALL, ANIM_RUN_LOOP, ANIM_SPRINT_FALL, ANIM_SPRINT_LOOP, ANIM_RUN, ANIM_SPRINT, ANIM_SWITCH_IN, ANIM_END};
	
	enum ANIMATION_SKILL{ANIM_SWAP, ANIM_PUCNH, ANIM_CUT1, ANIM_CUT2, ANIM_MAGIC_ATTACK , ANIM_SKILL_END}; //44

	enum ANIMATION_RIFLE{ RIF_REROAD, RIF_SHOOT, RIF_SHOOT2, RIF_END};

private:
	explicit CPlayer(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

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
	CCollider*			m_pAABBCom = nullptr;
	CCollider*			m_pSphereCom = nullptr;

private:
	void Mouse_Move(_double TimeDelta);
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	void Key_Input(double TimeDelta);

public:
	void State_Tick(_double TimeDelta);
	void Status_Tick(_double TimeDelta);
	void Animation_Tick(_double TimeDelta);
	void Update_WeaponState();
	void Setting_Bullet();
	void Shoot_Punch();
	void Collison_Object(_double Timedelta);
	void Sound_Update(_double Timedelta);

private:
	STATE	m_State = STATE_IDLE;
	POSE_TYPE m_Anim_Type = TYPE_0;
	ANIMATION_STATE m_Anim_State = ANIM_IDLE;
	ANIMATION_SKILL m_Anim_Skill = ANIM_SKILL_END;

	ANIMATION_RIFLE m_Rifle_State = RIF_END;

	// 해당 무기에 맞는 타입 변경 + 무기동작변경
	WEAPONS m_Weapon = RIFLE0;

	WEAPON_TYPE m_WeaponType = TYPE_RIFLE;

	CPlayer_Manager* m_Manager = nullptr;
	CEvent_Manager* m_EventMgr = CEvent_Manager::Get_Instance();

	_uint m_iAnimIndex = 0;
	_bool m_isJump = false;
	_bool m_isSit = false;
	_bool m_isAttack = false;
	_bool m_isSecondAttack = false;
	_bool m_isSkill = false;
	_bool m_isReroad = false;
	_bool m_isSwap = false;
	_bool m_isDie = false;
	_bool m_isDamaged = false;
	_bool m_SuperAmor = false;
	_bool m_isSprint = false;
	_bool m_isMove = false;

	_float NowSpeed = 2.0f;
	_float MaxSpeed = 3.3f;
	_float m_Sensitive = 0.1f;

	_vector m_OriginPos;
	_vector m_PrevPos;
	_float m_PrevSpeed;
	SUPERDESC m_SuperDesc;

	_float m_FootSound = 0.f;
	_float m_DefaultY = 2.3f;

	_int m_RifleBullet = 30;
	_int m_ShotgunBullet = 7;

public:
	static CPlayer* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END