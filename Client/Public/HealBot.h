#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
class CModel;
END

BEGIN(Client)

class CHealBot final : public CGameObject
{
public:
	enum NORMAL_STATE{STATE_IDLE, STATE_EVENT};

private:
	explicit CHealBot(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CHealBot(const CHealBot& rhs);
	virtual ~CHealBot() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Anim_Tick(_double TImeDelta);
	void Update_Alpha(_double Timedelta);


private:	
	CCollider*			m_pSphereCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;	
	CModel*				m_pModelCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();	
	_bool Heal_Player();

private:
	NORMAL_STATE m_State = STATE_IDLE;
	_bool	m_Trigger = false;
	_int	m_BlackWhite = 0;
	_double m_TotalTime = 0.0;

	_float	m_fAlpha = 0.f;
	_bool	m_flag = false;
public:
	static CHealBot* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END