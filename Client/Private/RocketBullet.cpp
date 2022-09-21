#include "stdafx.h"
#include "..\Public\RocketBullet.h"
#include "GameInstance.h"

CRocketBullet::CRocketBullet(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CRocketBullet::CRocketBullet(const CRocketBullet & rhs)
	: CBullet(rhs)
{
}

HRESULT CRocketBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRocketBullet::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->MyLookSet(m_vDir);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (FAILED(Search_MyNaviIndex())) {
		return E_FAIL;
		int a = 10;
	}

	return S_OK;
}

_int CRocketBullet::Tick(_double TimeDelta)
{

	if (__super::Tick(TimeDelta) == 1) {
		//Create_Explosion();
		return 1;
	}

	m_pTransformCom->Go_Straight(TimeDelta * 10.f);

	if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) > -0.8f) {
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_Gravitation);
		m_Gravitation += TimeDelta * 0.01f;
	}

	if (m_TotalTime >= 10.f)
		m_Dead = true;


	//	Bound_Bullet();
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_TotalTime += TimeDelta;

	return 0;
}

void CRocketBullet::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CRocketBullet::Render()
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
		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 2)))
			return E_FAIL;
	}

#ifdef _DEBUG
//	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CRocketBullet::Create_Trail()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	WRAP temp;
	temp.pTransform = m_pTransformCom;
	temp.pTarget = this;
	temp.fColor = _float3(0.8f, 0.8f, 0.8f);
	if (FAILED(pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"BulletTrail", L"Prototype_Effect_Trail_Bow", &temp)))
		int a = 10;

	RELEASE_INSTANCE(CGameInstance);
}

void CRocketBullet::Create_Explosion()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);

	BULLETDESC Data;
	m_Damage = 0.1f;
	m_CriticalPer = 1.f;
	m_Speed = 1.f;
	m_Size = 1.f;

	pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Explsoion", L"Prototype_GameObject_RocketExplosion", &Data);

	RELEASE_INSTANCE(CGameInstance);

}

HRESULT CRocketBullet::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RocketBullet"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.fRadius = 0.3f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRocketBullet::SetUp_ConstantTable()
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

	/* For.LightDesc */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightPos", &pLightDesc->vPosition, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRange", &pLightDesc->fRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CRocketBullet * CRocketBullet::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CRocketBullet*	pInstance = new CRocketBullet(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CRocketBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRocketBullet::Clone(void * pArg)
{
	CRocketBullet*	pInstance = new CRocketBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CRocketBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRocketBullet::Free()
{
	__super::Free();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
