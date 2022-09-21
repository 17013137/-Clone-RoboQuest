#include "stdafx.h"
#include "..\Public\OverHitLaser.h"
#include "GameInstance.h"
#include "KeyMgr.h"

COverHitLaser::COverHitLaser(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

COverHitLaser::COverHitLaser(const COverHitLaser & rhs)
	: CGameObject(rhs)
{
}

HRESULT COverHitLaser::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT COverHitLaser::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	
	m_pTransformCom->Scaled(_float3(m_ScailSize, 20.f, m_ScailSize));

	if (pArg != nullptr) {
		_vector vPos = _vector();
		memcpy(&vPos, pArg, sizeof(_vector));
		vPos = XMVectorSetY(vPos, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}


	m_pAABB->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

_int COverHitLaser::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	if (m_Dead == true){
		return 1;
	}
	Collider_Enemy();


	m_AccTime += TimeDelta*2.f;
	if (m_ScailSize < 0.f)
		m_Dead = true;
	
	if (m_AccDuration > m_Duration) {
		return 1;
	}

	m_pAABB->Update(m_pTransformCom->Get_WorldMatrix());
	m_AccDuration += TimeDelta;
	return 0;
}

void COverHitLaser::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Set_CameraDistance();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
}

HRESULT COverHitLaser::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;


	_uint		iNumMeshContainers = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 7)))
			return E_FAIL;
	}	

#ifdef _DEBUG
//	m_pAABB->Render();
#endif // _DEBUG

	return S_OK;
}

void COverHitLaser::Collider_Enemy()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider* Collide = (CCollider*)pGameInstance->Get_Component(m_iLevelIndex, L"Layer_Player", L"Com_AABB");
	if (Collide == nullptr)
		return;

	if (Collide->Collsion_AABB(m_pAABB) == true) {
		CStatus* PlayerStatus = (CStatus*)pGameInstance->Get_Component(m_iLevelIndex, L"Layer_Player", L"Com_Status");
		PlayerStatus->Damaged(m_Damage, CStatus::HIT_STATE_DEFAULT, 1.f);
	}

	RELEASE_INSTANCE(CGameInstance);
}


HRESULT COverHitLaser::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_OverHitLaser"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 1.f, 0.5f);

	if (FAILED(__super::SetUp_Components(TEXT("Com_AABB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABB, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT COverHitLaser::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionFloat4(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("AccTime", &m_AccTime, sizeof(_float))))
		return E_FAIL;

	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

COverHitLaser * COverHitLaser::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	COverHitLaser*	pInstance = new COverHitLaser(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created COverHitLaser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * COverHitLaser::Clone(void * pArg)
{
	COverHitLaser*	pInstance = new COverHitLaser(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created COverHitLaser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COverHitLaser::Free()
{
	__super::Free();
	Safe_Release(m_pAABB);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
