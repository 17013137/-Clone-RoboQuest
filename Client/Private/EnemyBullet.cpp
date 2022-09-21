#include "stdafx.h"
#include "..\Public\EnemyBullet.h"
#include "GameInstance.h"

CEnemyBullet::CEnemyBullet(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{
}

CEnemyBullet::CEnemyBullet(const CEnemyBullet & rhs)
	: CBullet(rhs)
{
}

HRESULT CEnemyBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemyBullet::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Scaled(_float3(0.4f, 0.4f, 0.4f));

	m_Speed = 3.f;

	Create_Trail();

	return S_OK;
}

_int CEnemyBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	if (m_Dead == true)
		return 1;

	Collide_Enemy(TimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_vDir * 0.3f);
	vPos = XMVectorSetW(vPos, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (m_TimeTotal >= 10.0) {
		m_Dead = true;
	}

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_TimeTotal += TimeDelta;
	return 0;
}

void CEnemyBullet::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	LookAtCameara();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONLIGHT, this);
}

HRESULT CEnemyBullet::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return S_OK;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
#ifdef _DEBUG
//	m_pSphereCom->Render();
#endif //_DEBUG

	return S_OK;
}

_int CEnemyBullet::Collide_Enemy(_double Timedelta)
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	CGameObject* CollideEnemy = pGameinstance->Collison_AABBLayer(m_iLevelIndex, this, L"Layer_Player");
	if (CollideEnemy != nullptr) {
		CStatus* EnemyStatus= (CStatus*)CollideEnemy->Get_Component(L"Com_Status");
		EnemyStatus->Damaged(10.f, CStatus::HIT_STATE_DEFAULT, 0.f);
		m_Dead = true;
		return 1;
	}

	Safe_Release(pGameinstance);

	return 0;
}

void CEnemyBullet::Create_Trail()
{
	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	WRAP temp;
	temp.pTransform = m_pTransformCom;
	temp.pTarget = this;
	if (FAILED(pGameinstance->Add_GameObjectToLayer(m_iLevelIndex, L"Trail_Monster", L"Prototype_Effect_Trail_Monster", &temp)))
		int a = 10;

	RELEASE_INSTANCE(CGameInstance);
}


void CEnemyBullet::Bound_Bullet()
{

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);

	_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_PrevPos;
	_float3 NaviNormal = _float3();

	if (true == pGameinstance->CollideWithNavi(m_pSphereCom, vDir, &NaviNormal)) {
		if (m_BoundCnt > 0) {
			m_pTransformCom->MyLookSet(XMVector3Normalize(XMLoadFloat3(&NaviNormal)));
		}
		else
			m_Dead = true;
	}

	Safe_Release(pGameinstance);

}

HRESULT CEnemyBullet::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 0.5f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemyBullet::SetUp_ConstantTable()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("MyColor", &m_MyColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CEnemyBullet * CEnemyBullet::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CEnemyBullet*	pInstance = new CEnemyBullet(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CEnemyBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEnemyBullet::Clone(void * pArg)
{
	CEnemyBullet*	pInstance = new CEnemyBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CEnemyBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemyBullet::Free()
{
	__super::Free();
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
