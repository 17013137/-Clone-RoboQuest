#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;

END

BEGIN(Client)

class COverHit final : public CGameObject
{
private:
	explicit COverHit(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit COverHit(const COverHit& rhs);
	virtual ~COverHit() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Search_Player();
	void Create_Laser();

private:	
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;	
	CModel*				m_pModelCom = nullptr;

private:
	_float m_Ambient = 0.f;
	_bool m_Damaged = false;
	_float m_AccTime = 0.f;
	_float m_Speed = 0.2f;
	_float m_ScailSize = 6.f;

	_float m_LaserTime = 3.f;
	_float m_LaserAccTime = 0.f;
	_bool m_isCreate = false;

	_float m_Duration = 6.f;
	_float m_AccDuration = 0.f;

	_float m_ActiveTime = 0.f;
	_float m_AccActiveTime = 0.f;
	_bool m_isActive = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	
public:
	static COverHit* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END