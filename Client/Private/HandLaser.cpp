#include "stdafx.h"
#include "..\Public\HandLaser.h"
#include "GameInstance.h"
#include "Boss_Manager.h"

CHandLaser::CHandLaser(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CHandLaser::CHandLaser(const CHandLaser & rhs)
	: CBullet(rhs)
{
}

HRESULT CHandLaser::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHandLaser::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	m_pTransformCom->MyLookSet(m_vDir);
	m_pTransformCom->Go_Straight(5.f);
	Safe_Release(pGameinstance);
	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_Damage = 10.f;
	return S_OK;

}

_int CHandLaser::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	

	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());

	Collide_Target(L"Layer_Player");

	if (m_TotalTime >= 1.5f)
		return 1;

	m_TotalTime += TimeDelta;
	return 0;
}

void CHandLaser::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
}

HRESULT CHandLaser::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;
	
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	//m_pOBBCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CHandLaser::Collide_Target(const _tchar * LayerTag)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	CCollider* PlayerCollide =  (CCollider*)pGameinstance->Get_Component(m_iLevelIndex, LayerTag, L"Com_AABB");
	if (PlayerCollide == nullptr)
		return;
	CStatus* PlayerStatus = (CStatus*)pGameinstance->Get_Component(m_iLevelIndex, LayerTag, L"Com_Status");

	if (m_pOBBCom->InterSectAABB(*PlayerCollide->Get_AABBWorld()) == true) {
		_vector Dir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		PlayerStatus->Damaged(m_Damage, CStatus::HIT_STATE_KNOCKBACK, 1.f, &Dir);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CHandLaser::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainMenu"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(5.f, 5.f, 50.f);
	if (FAILED(__super::SetUp_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHandLaser::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CHandLaser * CHandLaser::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CHandLaser*	pInstance = new CHandLaser(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CHandLaser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHandLaser::Clone(void * pArg)
{
	CHandLaser*	pInstance = new CHandLaser(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CHandLaser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHandLaser::Free()
{
	__super::Free();
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
