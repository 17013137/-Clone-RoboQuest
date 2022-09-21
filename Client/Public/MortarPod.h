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

class CMortarPod final : public CGameObject
{
public:
	enum ANIM_STATE{ ANIM_IDLE, ANIM_PRESHOOT, ANIM_SHOOT, ANIM_STAGGER, ANIM_STUN_LOOP, ANIM_STUN_START, ANIM_END
	};
	//R_Canon_Scale01
private:
	typedef struct ATTACKESC {
		_bool isAttack = false;
		_bool isCoolTime = false;
		_int Progress = 0;
		_double CoolTime = 7.0;
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
	explicit CMortarPod(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CMortarPod(const CMortarPod& rhs);
	virtual ~CMortarPod() = default;

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
	ATTACKESC m_AttackDesc;
	_bool m_LockOn = false;
	_bool m_Damaged = false;

public:
	static CMortarPod* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END