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

class CSmallBot final : public CGameObject
{
public:
	enum ANIM_STATE{ ANIM_IDLE, ANIM_RUN_BACK, ANIM_RUN_FRONT, ANIM_RUN_LEFT, ANIM_RUN_RIGHT,
		ANIM_STAGGER_BACK = 7, ANIM_STAGGER_FRONT, ANIM_STAGGER_LEFT, ANIM_STAGGER_RIGHT, ANIM_STURN_LOOP, ANIM_STURN_START
	};
	//R_Canon_Scale01
private:
	typedef struct ANIMDESC{
		_bool isStart = false;
		_double TotalTime = 0.0;
	}ANIMDESC;

private:
	explicit CSmallBot(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CSmallBot(const CSmallBot& rhs);
	virtual ~CSmallBot() = default;

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
	void Create_ExpBall();

private:
	CTransform* m_Player = nullptr;
	ANIM_STATE m_iAnimIndex = ANIM_IDLE;
	ANIMDESC m_AnimDesc;
	_bool m_LockOn = false;
	_int m_randomMove = 0;
	_int m_Stagger = 0;
	_int m_prevStagger = 0;
	_bool m_Damaged = false;

public:
	static CSmallBot* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END