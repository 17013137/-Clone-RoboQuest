#include "stdafx.h"
#include "..\Public\CanonHitBox.h"
#include "GameInstance.h"
#include "Boss_Manager.h"

CCanonHitBox::CCanonHitBox(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CCanonHitBox::CCanonHitBox(const CCanonHitBox & rhs)
	: CBullet(rhs)
{
}

HRESULT CCanonHitBox::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanonHitBox::NativeConstruct(void * pArg)
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
	
	m_Owner = (CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Enemy", m_pTransformTag, 0);

	if (m_Owner == nullptr)
		return E_FAIL;
	m_pTransformCom->MyLookSet(m_vDir);

	m_pTransformCom->Scaled(_float3(5.f, 5.f, 45.f));

	Safe_Release(pGameinstance);
	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_Damage = 10.f;
	return S_OK;

}

_int CCanonHitBox::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_AccTime += TimeDelta;

	m_pTransformCom->MyLookSet(m_Owner->Get_State(CTransform::STATE_LOOK));
	m_pTransformCom->Scaled(_float3(5.f, 5.f, 45.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_Owner->Get_State(CTransform::STATE_POSITION) - m_Owner->Get_State(CTransform::STATE_RIGHT)*3.f + m_Owner->Get_State(CTransform::STATE_LOOK) * 25.f + m_Owner->Get_State(CTransform::STATE_UP) * 3.f);

	if (CBoss_Manager::Get_Instance()->Get_CanonFinish() == true) {
		CBoss_Manager::Get_Instance()->Set_CanonFinish(false);
		return 1;
	}

	m_pOBBCom->Update(m_pTransformCom->Get_PureWorldMatrix());
	
	Collide_Target(L"Layer_Player");

	return 0;
}

void CCanonHitBox::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	ZBillBoard();
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
}

HRESULT CCanonHitBox::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;
	
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(18)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
//	m_pOBBCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CCanonHitBox::Collide_Target(const _tchar * LayerTag)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	CCollider* PlayerCollide =  (CCollider*)pGameinstance->Get_Component(m_iLevelIndex, LayerTag, L"Com_AABB");
	if (PlayerCollide == nullptr)
		return;
	CStatus* PlayerStatus = (CStatus*)pGameinstance->Get_Component(m_iLevelIndex, LayerTag, L"Com_Status");

	if (m_pOBBCom->InterSectAABB(*PlayerCollide->Get_AABBWorld()) == true) {
		_vector Dir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		PlayerStatus->Damaged(m_Damage, CStatus::HIT_STATE_KNOCKBACK, 5.f, &Dir);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CCanonHitBox::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Laser"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(4.f, 4.f, 45.f);
	if (FAILED(__super::SetUp_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanonHitBox::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("AccTime", &m_AccTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 2)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CCanonHitBox * CCanonHitBox::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CCanonHitBox*	pInstance = new CCanonHitBox(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CCanonHitBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCanonHitBox::Clone(void * pArg)
{
	CCanonHitBox*	pInstance = new CCanonHitBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CCanonHitBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCanonHitBox::Free()
{
	__super::Free();
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
