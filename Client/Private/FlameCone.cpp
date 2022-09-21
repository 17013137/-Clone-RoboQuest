#include "stdafx.h"
#include "..\Public\FlameCone.h"
#include "GameInstance.h"
#include "Boss_Manager.h"

CFlameCone::CFlameCone(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CFlameCone::CFlameCone(const CFlameCone & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFlameCone::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlameCone::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(175.f, 5.f, 180.f, 1.f));

	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	Set_TowerBossBone();
	return S_OK;
}

_int CFlameCone::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (CBoss_Manager::Get_Instance()->Get_isFlameShoot() == false)
		return 1;

	
	Collide_Target(L"Layer_Player");
	Set_TowerBossBone();
	m_AccTime -= (_float)TimeDelta;

	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	return 0;
}

void CFlameCone::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
}

HRESULT CFlameCone::Render()
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
		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 3)))
			return E_FAIL;
	}	

#ifdef _DEBUG
//	m_pOBBCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CFlameCone::Set_TowerBossBone()
{
	if (m_iLevelIndex == LEVEL_BOSSTOWER) {
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CTransform* BossTrans = (CTransform*)pGameInstance->Get_Transform(m_iLevelIndex, L"Layer_Enemy", 0);

		if (BossTrans == nullptr) {
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		_matrix WorldMtx = CBoss_Manager::Get_Instance()->Get_IK_Mtx();
		//		m_pTransformCom->MyLookSet(BossTrans->Get_State(CTransform::STATE_LOOK));

		m_pTransformCom->MyLookSet(WorldMtx.r[0]);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMtx.r[3]);

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CFlameCone::Collide_Target(const _tchar * LayerTag)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	CCollider* PlayerCollide = (CCollider*)pGameinstance->Get_Component(m_iLevelIndex, LayerTag, L"Com_AABB");
	if (PlayerCollide == nullptr)
		return;
	CStatus* PlayerStatus = (CStatus*)pGameinstance->Get_Component(m_iLevelIndex, LayerTag, L"Com_Status");

	if (m_pOBBCom->InterSectAABB(*PlayerCollide->Get_AABBWorld()) == true) {
		_vector Dir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		PlayerStatus->Damaged(m_Damage, CStatus::HIT_STATE_KNOCKBACK, 5.f, &Dir);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CFlameCone::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlameCone"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPosition = _float3(0.f, 0.f, 25.f);
	ColliderDesc.vSize = _float3(3.f, 3.f, 50.f);

	if (FAILED(__super::SetUp_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlameCone::SetUp_ConstantTable()
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

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_PatternTex", 0)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}



CFlameCone * CFlameCone::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CFlameCone*	pInstance = new CFlameCone(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CFlameCone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlameCone::Clone(void * pArg)
{
	CFlameCone*	pInstance = new CFlameCone(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CFlameCone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlameCone::Free()
{
	__super::Free();	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
