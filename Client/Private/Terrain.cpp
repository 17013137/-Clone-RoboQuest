#include "stdafx.h"
#include "..\Public\Terrain.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "Collider.h"
#include "KeyMgr.h"
#include "NaviBox.h"

CTerrain::CTerrain(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	switch (m_iLevelIndex)
	{
	case LEVEL_GAMEPLAY:
		if (FAILED(pGameInstance->Initialize_Navigation(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/BaseCamp/Navigation.dat"), m_pTransformCom)))
			return E_FAIL;
		m_iImageIndex = 0;
		break;
	case LEVEL_DUNGEON:
		if (FAILED(pGameInstance->Initialize_Navigation(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/Dungeon/Navigation.dat"), m_pTransformCom)))
			return E_FAIL;
		m_iImageIndex = 0;
		break;

	case LEVEL_BOSSTOWER:
		if (FAILED(pGameInstance->Initialize_Navigation(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/BossTower/Navigation.dat"), m_pTransformCom)))
			return E_FAIL;
		m_iImageIndex = 5;
		break;
	case LEVEL_BOSSDISCO:
		if (FAILED(pGameInstance->Initialize_Navigation(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/BOssDisco/Navigation.dat"), m_pTransformCom)))
			return E_FAIL;
		m_iImageIndex = 6;
		break;
	default:
		break;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CTerrain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	_vector vPos = _vector();
	CGameObject* newnavi = nullptr;
	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameinstance);
	if (CImgui_Manager::GetInstance()->Get_isNaviMode() == true) {
		if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON)) {
			if (CImgui_Manager::GetInstance()->m_PointCnt < 3) {
				CGameObject* NaviObj = pGameinstance->Get_PickingObject(m_iLevelIndex, L"NaviBox");
				if (NaviObj != nullptr) {
					CTransform* NaviTrans = NaviObj->Get_Transform();
					vPos = NaviTrans->Get_State(CTransform::STATE_POSITION);
					newnavi = pGameinstance->Add_GameObjToLayer(m_iLevelIndex, L"NaviBox", L"Prototype_GameObject_NaviBox", &vPos);
					CImgui_Manager::GetInstance()->Get_NaviPoint(newnavi);
					
					CCollider* NaviColider = (CCollider*)newnavi->Get_Component(L"Com_AABB");
					NaviColider->Set_LineColor(XMLoadFloat4(&_float4(1.f, _float(rainbow / 10.f), 1.f, 1.f)));
					rainbow += 1;
					rainbow = rainbow % 10;
				}
				else {
					vPos = m_pAABBCom->InterSectsRay(pGameinstance->Get_WorldRayPos(), pGameinstance->Get_WorldRay());
					if (XMVectorGetX(XMVector3Length(vPos)) > 0.f) {
						vPos = XMVectorSetY(vPos, CImgui_Manager::GetInstance()->m_SetY);
						newnavi = pGameinstance->Add_GameObjToLayer(m_iLevelIndex, L"NaviBox", L"Prototype_GameObject_NaviBox", &vPos);
						CImgui_Manager::GetInstance()->Get_NaviPoint(newnavi);
					}
				}
			}
		}
	}

	Safe_Release(pGameinstance);
	return 0;
}

void CTerrain::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CTerrain::Render()
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
	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	////m_pAABBCom->Render();
	//pGameInstance->Render_Navigation();
	//RELEASE_INSTANCE(CGameInstance);
#endif // _DEBUG
	return S_OK;
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), m_iLevelIndex, TEXT("Prototype_Component_VIBuffer_Terrain"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Diffuse */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Diffuse"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Mask */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Mask"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TerrainMask"), (CComponent**)&m_pTextureCom[TYPE_MASK])))
		return E_FAIL;

	/* For.Com_Brush */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Brush"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"), (CComponent**)&m_pTextureCom[TYPE_BRUSH])))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_AABB */
	ColliderDesc.vPosition = _float3(500.f, 0.0f, 500.f);
	ColliderDesc.vSize = _float3(1000.0f, 0.01f, 1000.0f);

	if (FAILED(__super::SetUp_Components(TEXT("Com_AABB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;	

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->SetUp_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", m_iImageIndex)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CTerrain*	pInstance = new CTerrain(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();


	for (_uint i = 0; i < TYPEWEAPON_END; ++i)
		Safe_Release(m_pTextureCom[i]);	

	Safe_Release(m_pAABBCom);
	Safe_Release(m_pMaskSRV);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
