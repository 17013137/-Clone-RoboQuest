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

class CBossTower final : public CGameObject
{
public:
	enum STATE{STATE_IDLE, STATE_MOVE, STATE_ATTACK, STATE_REROAD, STATE_DAMAGED, STATE_CC, STATE_DIE, STATE_END};

	enum CANON_STATE { CANON_IDLE, CANON_IN, CANON_OUT, CANON_PRESHOOT, CANON_SHOOT, CANON_SHOOT_L, CANON_SHOOT_R, CANON_LOOP, CANON_END };
	
	enum FLAME_STATE { FLAME_IDLE, FLAME_IN, FLAME_OUT, FLAME_PRESHOOT, FLAME_SHOOT, FLAME_END};
	// IN -> PRESHOOT -> SHOOT -> LOOP -> OUT -> IDLE
	enum HANDBEAM_STATE { HANDBEAM_IN, HANDBEAM_OUT, HANDBEAM_SHOOT_CLOSE, HANDBEAM_SHOOT_OPEN, HANDBEAM_END};

	enum NORMAL_STATE { NORMAL_IDLE, NORMAL_INTRO, NORMAL_MORTAR_PRESHOOT, NORMAL_MORTAR_SHOOT, NORMAL_SAW_OFF, NORMAL_SAW_ON, NORMAL_DAMAGED_LOOP, NORMAL_END};

	// CANON_END + PLAME_END + HANDBEAM_END + NORMAL_INTRO

	enum SKILL_STATE { SKILL_CANON, SKILL_FLAME, SKILL_HANDBEAM, SKILL_MORTAR, SKILL_END	};

	enum BOSS_PATTERN { PATTERN_CANON_ROTATION_R, PATTERN_CANON_ROTATION_L, PATTERN_FLAME_SHOOT, PATTERN_HANDBEAM_SHOOT, PATTERN_MORTAR_SHOOT};

	enum BONE_TYPE { BONE_MORTAR_TOP, BONE_MORTAR_BODY, BONE_L_WRIST, BONE_R_CANON_IK ,BONE_END };

	const char* Bone_Str[BONE_END] = { "Mortar_Top", "Mortar_Body", "L_Wrist", "R_Canon_Tip_Scale" };

	typedef struct MORTARDESC {
		_uint MortarCnt = 0;
		_uint Cnt = 0;
	}MORTARDESC;
	
	typedef struct CANONROTATION {
		_bool isStart = false;
		_double TotalTime = 0.0;
	}CANONROTATION;
	
	typedef struct MANNERDESC {
		_bool isMannerTime = false;
		_double TimeAcc = 0.0;
	}MANNERDESC;

private:
	explicit CBossTower(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBossTower(const CBossTower& rhs);
	virtual ~CBossTower() = default;

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
	CTexture*			m_pTexture = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	void Animation_Tick(_double TimeDelta);
	_int Status_Tick(_double Timedelta);
	void KeyInput();
	void Set_Bone();

private:
	CTransform* m_Player = nullptr;
	NORMAL_STATE m_State = NORMAL_INTRO;
	_uint m_iAnimIndex = 0;
	_bool m_isIntro = true;
	_bool m_isSkill[SKILL_END] = { false };
	_uint m_Progression[SKILL_END] = { 0 };
	_uint m_FlameCnt = 0;
	_uint m_PrevPattern = 0;
	_float m_fAmbient = 0.f;

	MORTARDESC m_MortarDesc;
	CANONROTATION m_CanonState;
	CBoss_Manager* m_Manager = CBoss_Manager::Get_Instance();
	_bool m_CreateIntro = false;
	MANNERDESC m_Manner;

	_bool m_isDealTime = false;
	_double m_DealTime = 3.0;
	_double m_AccDealTime = 0.0;

	_float m_DisolveTime = 1.f;
	_bool m_isDisolve = false;

	_float4 m_BonePart[BONE_END] = { _float4(0.f, 0.f, 0.f, 0.f) };
public:
	static CBossTower* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END