#include "stdafx.h"
#include "..\Public\Punch.h"
#include "GameInstance.h"
#include "Player_Manager.h"

CPunch::CPunch(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CPunch::CPunch(const CPunch & rhs)
	: CBullet(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CPunch::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPunch::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameInstance);
	CTransform* PlayerTrans = (CTransform*)pGameInstance->Get_Component(m_iLevelIndex, L"Layer_Player", m_pTransformTag);
	Safe_Release(pGameInstance);

	_vector vLook = XMVector3Normalize(PlayerTrans->Get_State(CTransform::STATE_LOOK));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, PlayerTrans->Get_State(CTransform::STATE_POSITION) + vLook*1.5f);
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

_int CPunch::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_Dead == true)
		return 1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameInstance);
	CTransform* PlayerTrans = (CTransform*)pGameInstance->Get_Component(m_iLevelIndex, L"Layer_Player", m_pTransformTag);
	Safe_Release(pGameInstance);
	
	_vector vLook = XMVector3Normalize(PlayerTrans->Get_State(CTransform::STATE_LOOK));

	if (PlayerTrans != nullptr) {
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, PlayerTrans->Get_State(CTransform::STATE_POSITION) + vLook*1.5f);
	}
	else
		return 1;

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	Damage_Object();
	
	m_Dead = true;

	
	m_TimeTotal += TimeDelta;
	return 0;
}

void CPunch::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CPunch::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;

	
	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4	ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4	ProjMatrixTP;
	XMStoreFloat4x4(&ProjMatrixTP, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrixTP, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
#ifdef _DEBUG
//	m_pSphereCom->Render();
#endif //_DEBUG

	return S_OK;
}

void CPunch::Damage_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* EnemyList = pGameInstance->Get_GameObjectList(m_iLevelIndex, L"Layer_Enemy");

	if (EnemyList != nullptr) {
		for (auto& iter : *EnemyList) {
			CCollider* EnemyCollider = (CCollider*)iter->Get_Component(L"Com_Sphere");
			if (EnemyCollider == nullptr)
				continue;

			if (m_pSphereCom->InterSectSphere(*EnemyCollider->Get_SphereWorld()) == true) { //Com_Status
				if (m_isCollisonlist.size() > 0) {
					int flag = 0;

					for (auto& Dest : m_isCollisonlist) {
						if (iter = Dest) {
							flag = 1;
							break;
						}
					}

					if (flag == 1)
						continue;
				}

				CStatus* EnenmyStatus = (CStatus*)iter->Get_Component(L"Com_Status");
				EnenmyStatus->Damaged(m_Damage, CStatus::HIT_STATE_KNOCKBACK, 6.f);
			}
		}
	}

	Safe_Release(pGameInstance);
}

HRESULT CPunch::SetUp_Components()
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
	ColliderDesc.fRadius = 1.0f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CPunch * CPunch::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CPunch*	pInstance = new CPunch(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CPunch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPunch::Clone(void * pArg)
{
	CPunch*	pInstance = new CPunch(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CPunch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPunch::Free()
{
	__super::Free();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
