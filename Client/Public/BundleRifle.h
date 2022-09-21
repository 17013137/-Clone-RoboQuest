#pragma once

#include "Client_Defines.h"
#include "WeaponObj.h"
#include "Weapon.h"
#include "Player_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CBundleRifle final : public CWeaponObj
{
public:

private:
	explicit CBundleRifle(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBundleRifle(const CBundleRifle& rhs);
	virtual ~CBundleRifle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Anim_Tick(_double TimeDelta);
	HRESULT Update_WeaponState();
	void Update_Growing();
	void Sound_Update();
private:
	CCollider*			m_pAABBCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;	
	CModel*				m_pModelCom = nullptr;

	CModel*				m_pM4Com = nullptr;
	CModel*				m_pRifleCom = nullptr;
	CModel*				m_pLMGCom = nullptr;

	CWeapons*			m_pWeaponCom = nullptr;

	WEAPONDESC m_WeaponDesc;
	WEAPONS m_Weapons = RIFLE0;
	_vector m_MuzzlePos[3] = { XMVectorSet(0.f, 0.18f, -1.35f, 1.f),  XMVectorSet(0.f, 0.18f, -1.35f, 1.f) , XMVectorSet(0.f, 0.124f, -1.6f, 1.f) };

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	CPlayer_Manager::PLAYER_STATE m_PlayerState = CPlayer_Manager::PLAYER_IDLE;

public:
	static CBundleRifle* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END