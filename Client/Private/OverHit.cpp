#include "stdafx.h"
#include "..\Public\OverHit.h"
#include "GameInstance.h"
#include "KeyMgr.h"

COverHit::COverHit(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

COverHit::COverHit(const COverHit & rhs)
	: CGameObject(rhs)
{
}

HRESULT COverHit::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT COverHit::NativeConstruct(void * pArg)
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

	Search_Player();

	m_AccTime = (_float)(rand() % 30) / 10.f;

	return S_OK;
}

_int COverHit::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	if (m_AccTime < m_AccActiveTime) {
		m_isActive = true;
	}
	else {
		m_AccActiveTime += TimeDelta;
		return 0;
	}

	if (m_Duration < m_AccDuration)
		return 1;
	
	if (m_LaserAccTime > m_LaserTime && m_isCreate == false) {
		Create_Laser();
		m_isCreate = true;
		m_Speed = 1.f;
	}

	m_AccDuration += TimeDelta;
	m_LaserAccTime += TimeDelta;
	m_AccTime += TimeDelta*m_Speed;
	return 0;
}

void COverHit::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Set_CameraDistance();

	if (nullptr != m_pRendererCom && m_isActive == true)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
}

HRESULT COverHit::Render()
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
		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 6)))
			return E_FAIL;
	}	


	return S_OK;
}

void COverHit::Search_Player()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);

	CTransform* Player = (CTransform*)pGameinstance->Get_Component(m_iLevelIndex, L"Layer_Player", m_pTransformTag);
	_vector TargetPos = Player->Get_State(CTransform::STATE_POSITION);
	_vector RandPos = XMVectorSet((_float)(rand() % 101 - 50) / 10.f, 0.f, (_float)(rand() % 101 - 50) / 10.f, 0.f) + TargetPos;

	RandPos = XMVectorSetY(RandPos, 0.f);
	//pGameinstance->Get_GameObjectList(m_iLevelIndex, L"MortarHitBox");

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,  + RandPos);

	RELEASE_INSTANCE(CGameInstance);
}

void COverHit::Create_Laser()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	pGameInstance->Add_GameObjectToLayer(m_iLevelIndex, L"Effect", L"Prototype_GameObject_OverHitLaser", &vPos);

	RELEASE_INSTANCE(CGameInstance);
}


HRESULT COverHit::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_OverHit"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT COverHit::SetUp_ConstantTable()
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

COverHit * COverHit::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	COverHit*	pInstance = new COverHit(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created COverHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * COverHit::Clone(void * pArg)
{
	COverHit*	pInstance = new COverHit(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created COverHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COverHit::Free()
{
	__super::Free();	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
