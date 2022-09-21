#include "stdafx.h"
#include "..\Public\BossShield.h"
#include "GameInstance.h"
#include "KeyMgr.h"

CBossShield::CBossShield(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CBossShield::CBossShield(const CBossShield & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossShield::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossShield::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	
	
	Set_Position();
	m_pTransformCom->Scaled(_float3(7.f, 7.f, 7.f));
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	CSound_Manager::GetInstance()->SoundPlay(L"ShieldLoop.ogg", 5, 1.f);
	return S_OK;
}

_int CBossShield::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_Dead == true) {
		m_DisolveTime -= TimeDelta * 0.3f;
		if (m_DisolveTime < 0.f) {
			return 1;
		}
		return 0;
	}
	
	Status_Tick(TimeDelta);

	if (m_isDisovle == false) {
		if (m_DisolveTime > 1.f) {
			m_DisolveTime = 1.f;
			m_isDisovle = true;
			CSound_Manager::GetInstance()->StopSound(5);
		}
		m_DisolveTime += TimeDelta * 0.3f;
	}

	if (m_Duration < m_AccDuration)
		m_Dead = true;

	m_AccDuration += TimeDelta;

	return 0;
}

void CBossShield::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CBossShield::Render()
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
		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 5)))
			return E_FAIL;
	}	

#ifdef _DEBUG
//	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

_int CBossShield::Status_Tick(_double TimeDelta)
{
	int flag = m_pStatus->Status_Tick(TimeDelta);
	if (flag == 1)
		m_Dead = true;


	if (flag == 2) {
		m_Damaged = true;
		m_Ambient = 1.f;
	}

	m_Ambient -= TimeDelta * 4.f;

	if (m_Ambient < 0.f)
		m_Ambient = 0.f;

	return 0;
}

void CBossShield::Set_Position()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* BossTrans = (CTransform*)pGameInstance->Get_Transform(m_iLevelIndex, L"Layer_Enemy", 0);
	
	_vector vPos = BossTrans->Get_State(CTransform::STATE_POSITION) + (BossTrans->Get_State(CTransform::STATE_LOOK)*10.f);
	
	vPos = XMVectorSetY(vPos, 8.0f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->MyLookSet(BossTrans->Get_State(CTransform::STATE_LOOK));

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBossShield::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossShield"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CStatus::STATUSDESC temp;
	temp.fMaxHp = 500.f;
	temp.fMaxShield = 0.f;
	temp.fSpeed = 2.f;
	m_Speed = temp.fSpeed;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Status"), LEVEL_STATIC, TEXT("Prototype_Component_Status"), (CComponent**)&m_pStatus, &temp)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.0f, 0.5f);
	ColliderDesc.fRadius = 1.2f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_DisolveTexture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"), (CComponent**)&m_DisolveTexCom)))
		return E_FAIL;


	

	return S_OK;
}

HRESULT CBossShield::SetUp_ConstantTable()
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
	if (FAILED(m_pShaderCom->Set_RawValue("Ambient", &m_Ambient, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("Disolve", &m_DisolveTime, sizeof(_float))))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_PatternTex", 7)))
		return E_FAIL;
	if (FAILED(m_DisolveTexCom->SetUp_ShaderResourceView(m_pShaderCom, "g_DisolveTex", 9)))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBossShield * CBossShield::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CBossShield*	pInstance = new CBossShield(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CBossShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossShield::Clone(void * pArg)
{
	CBossShield*	pInstance = new CBossShield(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CBossShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossShield::Free()
{
	__super::Free();	
	Safe_Release(m_DisolveTexCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pStatus);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
