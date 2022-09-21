#include "..\Public\WeaponObj.h"
#include "GameInstance.h"

CWeaponObj::CWeaponObj(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
	
}

CWeaponObj::CWeaponObj(const CWeaponObj & rhs)
	: CGameObject(rhs.m_pDevice, rhs.m_pDeviceContext)
{
	
}

HRESULT CWeaponObj::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CWeaponObj::NativeConstruct(void * pArg, CTransform::TRANSFORMDESC * pTransformDesc)
{
	if (FAILED(__super::NativeConstruct(pArg, pTransformDesc)))

		return E_FAIL;

	return S_OK;
}

_int CWeaponObj::Tick(_double TimeDelta)
{
	Get_PlayerBone();

	if (m_GrowDesc.iLevel < 2) {
		if (m_GrowDesc.Exp >= 10.f) {
			m_GrowDesc.iLevel++;
			m_fAlpha = 1.f;
			m_GrowDesc.Exp = 0.f;
			Create_UpgradeEff();
		}
	}

	if (m_fAlpha <= 0.f) {
		m_fAlpha = 0.f;
	}
	else
		m_fAlpha -= (_float)TimeDelta*0.2f;

	return 0;
}

void CWeaponObj::LateTick(_double TimeDelta)
{

}

HRESULT CWeaponObj::Render()
{
	return S_OK;
}

void CWeaponObj::Update_WorldState()
{
	_matrix PlayerHand = XMMatrixMultiply(XMLoadFloat4x4(m_SocketDesc.pModelCom->Get_CombinedTransformationMatrix(m_SocketDesc.pBoneName)), XMLoadFloat4x4(&m_SocketDesc.pModelCom->Get_PivotMatrix()));

	PlayerHand = XMMatrixMultiply(PlayerHand, m_SocketDesc.pPlayerTransCom->Get_WorldMatrix());

	_float4x4 Result;

	XMStoreFloat4x4(&Result, PlayerHand);

	m_pTransformCom->Set_WorldMTX(Result);

	m_pTransformCom->Scaled(_float3(0.01f, 0.01f, 0.01f));
}

void CWeaponObj::Get_PlayerBone()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);

	_float4x4* Bone = m_SocketDesc.pModelCom->Get_CombinedTransformationMatrix("Weapon_Attachement");
	if (Bone == nullptr)
		return;

	RELEASE_INSTANCE(CGameInstance);

	_matrix BoneWorld = XMMatrixMultiply(XMLoadFloat4x4(Bone), XMLoadFloat4x4(&m_SocketDesc.pModelCom->Get_PivotMatrix()));
	BoneWorld = XMMatrixMultiply(BoneWorld, m_SocketDesc.pPlayerTransCom->Get_WorldMatrix());
	m_AttachmentMatrix = BoneWorld;
	
	m_AttachmentMatrix.r[0] = XMVector3Normalize(m_AttachmentMatrix.r[0]);
	m_AttachmentMatrix.r[1] = XMVector3Normalize(m_AttachmentMatrix.r[1]);
	m_AttachmentMatrix.r[2] = XMVector3Normalize(m_AttachmentMatrix.r[2]);


}

void CWeaponObj::Create_UpgradeEff()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);

	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Effect", TEXT("Prototype_Effect_CombineEffect"));

	RELEASE_INSTANCE(CGameInstance);
}

void CWeaponObj::Free()
{
	__super::Free();
}
