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

class CBossBeetle final : public CGameObject
{
public:
	enum STATE { STATE_IDLE, STATE_MOVE, STATE_ATTACK, STATE_REROAD, STATE_DAMAGED, STATE_CC, STATE_DIE, STATE_END };

	enum DEFAULT_STATE{ DEFAULT_INTRO, DEFAULT_IDLE, DEFAULT_END};

	enum DESTRUCTPLATE_STATE{ DESTRUCTPLATE_PRESHOOT, DESTRUCTPLATE_SHOOT, DESTRUCTPLATE_END};

	enum MINE_STATE{ MINE_LOOP, MINE_LOOP_LEFT, MINE_LOOP_RIGHT, MINE_PRESHOOT, MINE_END};

	enum ATTACK_STATE{ ATTACK_PRESHOOT, ATTACK_SHOOT, ATTACK_END };

	enum SHOTGUN_STATE { SHOTGUN_PRESHOOT, SHOTGUN_SHOOT, SHOTGUN_END};

	enum TROMBE_STATE {TROMBE_LOOP, TROMBE_PRESHOOT, TROMBE_END};

	enum SKILL_TYPE{ SKILL_DESTRUCTPLATE, SKILL_MINE, SKILL_NORMA, SKILL_SHOTGUN, SKILL_TROMBLE, SKILL_END};

private:
	explicit CBossBeetle(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBossBeetle(const CBossBeetle& rhs);
	virtual ~CBossBeetle() = default;

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
	void KeyInput();

private:
	CTransform* m_Player = nullptr;
	DEFAULT_STATE m_State = DEFAULT_INTRO;
	_uint m_iAnimIndex = 0;
	_bool m_isIntro = true;
	_bool m_isSkill[SKILL_END] = { false };
	_uint m_Progression[SKILL_END] = { 0 };

	_uint m_Destructplate_rand = 0;
	_uint m_Destructplate_cnt = 0;

	_double m_DestructTime = 0.0;
public:
	static CBossBeetle* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END