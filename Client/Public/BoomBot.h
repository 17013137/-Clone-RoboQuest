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

class CBoomBot final : public CGameObject
{
public:
	enum ANIM_STATE{ ANIM_IDLE, ANIM_PRESHOOT, ANIM_RUN_FRONT = 3, ANIM_SHOOT = 8, ANIM_STUN_LOOP, ANIM_STUN_START, ANIM_END
	};

private:
	typedef struct ANIMDESC{
		_bool isStart = false;
		_double TotalTime = 0.0;
	}ANIMDESC;

private:
	explicit CBoomBot(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBoomBot(const CBoomBot& rhs);
	virtual ~CBoomBot() = default;

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
	ANIMDESC m_AnimDesc;
	_bool m_LockOn = false;
	_int m_randomMove = 0;
	_bool m_Damaged = false;

public:
	static CBoomBot* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END