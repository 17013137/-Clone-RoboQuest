#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CFlameCone final : public CGameObject
{
private:
	explicit CFlameCone(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CFlameCone(const CFlameCone& rhs);
	virtual ~CFlameCone() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Set_TowerBossBone();
	void Collide_Target(const _tchar* LayerTag);


private:	
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;	
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pOBBCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	
	_float				m_Damage = 5.f;
	_float				m_AccTime = 0.0f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();


public:
	static CFlameCone* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END