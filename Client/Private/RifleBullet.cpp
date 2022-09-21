#include "stdafx.h"
#include "..\Public\RifleBullet.h"
#include "GameInstance.h"

CRifleBullet::CRifleBullet(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CRifleBullet::CRifleBullet(const CRifleBullet & rhs)
	: CBullet(rhs)
{
}

HRESULT CRifleBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRifleBullet::NativeConstruct(void * pArg)
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
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vDir;

	m_BoundCnt = 1;
	m_pTransformCom->MyLookSet(m_vDir);
	m_pTransformCom->Scaled(_float3(1.f, 0.4f, 20.f));
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	
	return S_OK;
}

_int CRifleBullet::Tick(_double TimeDelta)
{
	if (m_Dead == true)
		return 1;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vDir*0.05f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	_float Speed = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_PrevPos));

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_Alpha <= 0.f) {
		m_Dead = true;
	}

	m_Alpha -= (_float)TimeDelta;
	return 0;
}

void CRifleBullet::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	m_PrevPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	ZBillBoard();

	Set_CameraDistance();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_ALPHABLEND, this);
}

HRESULT CRifleBullet::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
#ifdef _DEBUG
	//m_pSphereCom->Render();
#endif //_DEBUG

	return S_OK;
}

void CRifleBullet::Bound_Bullet()
{

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);

	_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_PrevPos;
	_float3 NaviNormal = _float3();

	if (true == pGameinstance->CollideWithNavi(m_pSphereCom, vDir, &NaviNormal)) {
		if (m_BoundCnt > 0) {
			m_pTransformCom->MyLookSet(XMVector3Normalize(XMLoadFloat3(&NaviNormal)));
		}
		else
			m_Dead = true;
	}

	RELEASE_INSTANCE(CGameInstance);
}


void CRifleBullet::Collison_Bullet(const _tchar* LayerTag)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* EnemyList = pGameInstance->Get_GameObjectList(m_iLevelIndex, LayerTag);

	if (EnemyList != nullptr) {
		for (auto& iter : *EnemyList) {
			CCollider* EnemyCollider = (CCollider*)iter->Get_Component(L"Com_Sphere");
			if (EnemyCollider == nullptr)
				continue;

			if (m_pSphereCom->InterSectSphere(*EnemyCollider->Get_SphereWorld()) == true) { //Com_Status
				CStatus* EnenmyStatus = (CStatus*)iter->Get_Component(L"Com_Status");
				EnenmyStatus->Damaged(m_Damage, CStatus::HIT_STATE_DEFAULT, 0.1f);
				m_Dead = true;
			}
		}
	}

	Safe_Release(pGameInstance);

}

HRESULT CRifleBullet::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Spark"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 0.1f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRifleBullet::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("MyColor", &m_Color, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("fAlpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CRifleBullet * CRifleBullet::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CRifleBullet*	pInstance = new CRifleBullet(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CRifleBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRifleBullet::Clone(void * pArg)
{
	CRifleBullet*	pInstance = new CRifleBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CRifleBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRifleBullet::Free()
{
	__super::Free();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
