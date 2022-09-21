#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Texture.h"
#include "Shader.h"
#include "Channel.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_MeshContainers(rhs.m_MeshContainers)
	, m_iNumMeshContainers(rhs.m_iNumMeshContainers)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Animations(rhs.m_Animations)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iPrevAnimIndex(rhs.m_iPrevAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_eType(rhs.m_eType)
	, m_pDatFilePath(rhs.m_pDatFilePath)
	, m_ClonedHierarchyNodes(rhs.m_HierarchyNodes)
{
	for (auto& pMeshContainer : m_MeshContainers)
		Safe_AddRef(pMeshContainer);

	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_AddRef(pMaterial.pMaterials[i]);
		}
	}

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);
}

_float4x4 * CModel::Get_CombinedTransformationMatrix(const char * pNodeName)
{
	CHierarchyNode*		pNode = nullptr;

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		if (!strcmp(pNodeName, pHierarchyNode->Get_Name()))
		{
			pNode = pHierarchyNode;
			break;
		}
	}

	if (nullptr == pNode)
		return nullptr;

	return pNode->Get_CombinedFloat4x4Ptr();
}

HRESULT CModel::NativeConstruct_Prototype(TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	char			szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	// 만약 같은 경로의 같은 이름의 dat파일을 열었을 경우
	string str{ szFullPath };
	_uint lastDotPos = _uint(str.find_last_of("."));
	string datFilePath = str.substr(0, lastDotPos) + string{ ".dat" };

	ifstream In{ datFilePath, ios::in | ios::binary };

	m_eType = eType;

	// 파일이 존재한다면
	if (In) {
		// Dat 파일로 모델을 연다.

		// 모델의 타입을 가져온다.
		_uint iType = 0;
		In >> iType;

		// 타입을 저장한다.
		m_eType = (TYPE)iType;

		// 피봇 행렬을 저장한다.
		XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

		// 타입에 따른 FBX 파일을 로드한다.
		if (FAILED(Load_FBX(In)))
			return E_FAIL;

		// 필요없지만 혹시 몰라 놔둔다.
		m_eType = (TYPE)iType;

		return S_OK;
	}
	// 파일이 존재하지 않는다면
	else {
		_uint		iFlag;

		if (TYPE_NONANIM == m_eType)
			iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_Triangulate;
		else
			iFlag = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_Triangulate;

		m_pScene = m_Importer.ReadFile(szFullPath, iFlag);
		if (nullptr == m_pScene) {
			const char* pStr = m_Importer.GetErrorString();
			return E_FAIL;
		}

		XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

		if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode)))
			return E_FAIL;

		sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)->_bool
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});

		if (FAILED(Ready_MeshContainers()))
			return E_FAIL;

		if (FAILED(Ready_Materials(pModelFilePath)))
			return E_FAIL;

		if (FAILED(Ready_Animations()))
			return E_FAIL;

		// 읽어온 fbx 파일에 대한 정보를 같은 경로의 dat파일에 저장한다.
		ofstream Out{ datFilePath, ios::out | ios::binary };

		if (!Out) {
			MSG_BOX(TEXT("Failed to open FBX at .dat File"));
			return E_FAIL;
		}

		if (FAILED(Save_FBX(Out))) {
			MSG_BOX(TEXT("Failed to Save FBX at .dat File"));
			return E_FAIL;
		}

		const char* szDatFilePath = datFilePath.c_str();
		Out.write(szDatFilePath, strlen(szDatFilePath) + 1);
	}

	return S_OK;
}

HRESULT CModel::NativeConstruct_Prototype(const char * pModelDatFilePath, _fmatrix PivotMatrix)
{
	ifstream In{ pModelDatFilePath, ios::in | ios::binary };

	if (!In)
		return E_FAIL;

	Load_FBX(In);

	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	m_isCloned = true;

	if (nullptr == m_pScene) {
		if (FAILED(Clone_Dat_HierarchyNodes()))
			return E_FAIL;

		if (FAILED(Clone_Dat_MeshContainers()))
			return E_FAIL;

		if (FAILED(Clone_Dat_Animations()))
			return E_FAIL;
	}
	else {
		if (FAILED(Ready_HierarchyNodes(m_pScene->mRootNode)))
			return E_FAIL;

		sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)->_bool
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});

		if (FAILED(Clone_MeshContainers()))
			return E_FAIL;

		if (FAILED(Clone_Animations()))
			return E_FAIL;

		return S_OK;
	}

	return S_OK;
}

void CModel::Update(_double TimeDelta)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return;

	m_VecPrevChannel = m_Animations[m_iPrevAnimIndex]->Get_Channel();

	/* 현재 애님에서 사용되는 뼈들의 지역행렬(m_TransformationMatrix)을 갱신했다. */
	m_Animations[m_iCurrentAnimIndex]->Update(TimeDelta, m_Speed, m_LinearSpeed, m_isRepeat, m_VecPrevChannel, &m_isLinear);

	/* 노드들을 순회하면서(부모에서부터 자식으로) 갱신된 m_TransformationMatrix랑 부모행렬을 곱하여 저장한다. */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}


HRESULT CModel::Render(CShader* pShader, const char* pBoneMatricesName, _uint iMeshContainerIndex, _uint iPassIndex)
{
	if (iMeshContainerIndex >= m_iNumMeshContainers)
		return E_FAIL;

	if (TYPE_ANIM == m_eType)
	{
		_float4x4		BoneMatrices[128];

		ZeroMemory(BoneMatrices, sizeof(_float4x4) * 128);

		m_MeshContainers[iMeshContainerIndex]->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

		pShader->Set_RawValue(pBoneMatricesName, BoneMatrices, sizeof(_float4x4) * 128);
	}

	pShader->Begin(iPassIndex);

	if (nullptr != m_MeshContainers[iMeshContainerIndex])
		m_MeshContainers[iMeshContainerIndex]->Render();

	return S_OK;
}

HRESULT CModel::Bind_Material_OnShader(CShader * pShader, aiTextureType eType, const char * pConstantName, _uint iMeshContainerIndex)
{
	_uint		iMaterialIndex = m_MeshContainers[iMeshContainerIndex]->Get_NumMaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pMaterials[eType]->SetUp_ShaderResourceView(pShader, pConstantName, 0);
}

const _bool & CModel::Get_isAnimEnd(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex]->Get_isFinished();
}

_int CModel::Get_CurrentIndex(_uint iAnimIndex)
{
	int temp = m_Animations[iAnimIndex]->Get_CurrentKeyFrame();
	return temp;
}

void CModel::Set_FinishFalse(_uint iAnimIndex, _bool FrameZero)
{
	m_Animations[iAnimIndex]->Set_FinishFalse(FrameZero);
}

HRESULT CModel::Ready_MeshContainers()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumMeshContainers = m_pScene->mNumMeshes;

	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		/* 정점정보를 읽어서 각각의 메시컨테이너 객체가 해당 정볼르 가지게끔 만드단ㄷ. */
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_eType, m_pScene->mMeshes[i], m_PivotMatrix, m_HierarchyNodes);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_MeshContainers.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Clone_MeshContainers()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumMeshContainers = m_pScene->mNumMeshes;

	vector<CMeshContainer*>		MeshContainer;

	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = (CMeshContainer*)m_MeshContainers[i]->Clone(&m_HierarchyNodes);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		Safe_Release(m_MeshContainers[i]);

		MeshContainer.push_back(pMeshContainer);
	}
	m_MeshContainers.clear();

	m_MeshContainers = MeshContainer;

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumMaterials = m_pScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial*			pAIMaterial = m_pScene->mMaterials[i];
		if (nullptr == pAIMaterial)
			return E_FAIL;

		MODELMATERIAL		Materials;
		ZeroMemory(&Materials, sizeof(MODELMATERIAL));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			char		szTextureFilePath[MAX_PATH] = "";

			aiString	strPath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			char		szFileName[MAX_PATH] = "";
			char		szEXT[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			strcpy_s(szTextureFilePath, pModelFilePath);
			strcat_s(szTextureFilePath, szFileName);
			strcat_s(szTextureFilePath, szEXT);

			_tchar		szFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, (int)strlen(szTextureFilePath), szFullPath, MAX_PATH);

			Materials.pMaterials[j] = CTexture::Create(m_pDevice, m_pDeviceContext, szFullPath);
			if (nullptr == Materials.pMaterials[j])
				return S_OK;
		}
		m_Materials.push_back(Materials);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumAnimations = m_pScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*	pAIAnimation = m_pScene->mAnimations[i];

		CAnimation*		pAnimation = CAnimation::Create(pAIAnimation, m_HierarchyNodes);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Clone_Animations()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	vector<CAnimation*>		Animations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*	pAIAnimation = m_pScene->mAnimations[i];

		CAnimation*		pAnimation = m_Animations[i]->Clone(pAIAnimation, m_HierarchyNodes);
		if (nullptr == pAnimation)
			return E_FAIL;

		Safe_Release(m_Animations[i]);

		Animations.push_back(pAnimation);
	}

	m_Animations = Animations;

	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	static _uint iIdNum = 0;

	if (nullptr == m_pScene)
		return E_FAIL;

	_uint	iNumChildren = pNode->mNumChildren;

	CHierarchyNode*			pHierarchyNode = CHierarchyNode::Create(pNode, pParent, iDepth);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	for (_uint i = 0; i < iNumChildren; ++i)
	{
		if (FAILED(Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1)))
			return E_FAIL;
	}

	pHierarchyNode->Set_HierarchyNumber(++iIdNum);
	m_HierarchyNodes.push_back(pHierarchyNode);

	return S_OK;
}

HRESULT CModel::Clone_Dat_HierarchyNodes()
{
	int* iIndex = new int[m_ClonedHierarchyNodes.size()];

	// 루트 노드를 제외하기에 1부터 시작
	iIndex[0] = 0;

	for (int i = 1; i < m_ClonedHierarchyNodes.size(); ++i) {
		iIndex[i] = m_ClonedHierarchyNodes[i]->Get_ParentIndex();
	}

	for (int i = 0; i < m_ClonedHierarchyNodes.size(); ++i) {
		m_HierarchyNodes.push_back(m_ClonedHierarchyNodes[i]->Clone());
	}

	// 루트 노드를 제외하기에 1부터 시작
	for (int i = 1; i < m_ClonedHierarchyNodes.size(); ++i) {
		m_HierarchyNodes[i]->Set_ParentIndex(iIndex[i]);
		m_HierarchyNodes[i]->SetParent(m_HierarchyNodes[iIndex[i]]);
	}

	Safe_Delete_Array(iIndex);

	return S_OK;
}

HRESULT CModel::Clone_Dat_MeshContainers()
{
	vector<CMeshContainer*>		MeshContainer;

	for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	{
		CMeshContainer*		pMeshContainer = (CMeshContainer*)m_MeshContainers[i]->Clone_Dat(&m_HierarchyNodes);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		//Safe_Release(m_MeshContainers[i]);

		MeshContainer.push_back(pMeshContainer);
	}
	m_MeshContainers.clear();

	m_MeshContainers = MeshContainer;

	return S_OK;
}

HRESULT CModel::Clone_Dat_Animations()
{
	vector<CAnimation*>		Animations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation*		pAnimation = m_Animations[i]->Clone(m_HierarchyNodes);
		if (nullptr == pAnimation)
			return E_FAIL;

		Safe_Release(m_Animations[i]);

		Animations.push_back(pAnimation);
	}

	m_Animations = Animations;

	return S_OK;
}

HRESULT CModel::Save_FBX(ofstream & Out)
{
	if (!Out)
		return E_FAIL;

	// 메쉬의 타입 저장
	Out << m_eType << endl;

	// HierarchyNodes 정보 저장 - 갯수, 내부내용
	Out << m_HierarchyNodes.size() << endl;

	for_each(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&Out](CHierarchyNode* pNode) {
		// 자신의 정보를 저장한다.
		pNode->SaveDatInfo(Out);
	});

	// m_HierarchyNodes의 갯수만큼 반복한다.
	for_each(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode) {

		// 부모가 없다면 -1을
		if (nullptr == pNode->GetParent())
			Out << -1 << " ";
		// 부모가 없는 경우가 아니라면 반드시 HierarchyNumber는 존재한다.
		else {
			// 부모의 HierarchyNumber를 가져온다.
			int iParentNum = pNode->GetParent()->Get_HierarchyNumber();

			// 부모의 HierarchyNumber를 가진 인덱스를 찾아 해당 인덱스 번호를 저장한다.
			auto& iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&iParentNum](CHierarchyNode* pNode) {
				return pNode->Get_HierarchyNumber() == iParentNum;
			});

			// 거리를 구해 넣는다. 이것이 인덱스 번호이다. 
			_uint iIndex = _uint(distance(m_HierarchyNodes.begin(), iter));
			pNode->Set_ParentIndex(iIndex);
			Out << iIndex << " ";
		}
	});

	// HierarchyNodes에 대한 정보 저장이 끝났으니, 메쉬 컨테이너로 가서 메쉬 컨테이너의 갯수를 저장한다.
	Out << m_MeshContainers.size() << " ";

	// 메쉬 컨테이너의 갯수만큼 for문을 돌며 모델의 타입에 따라 다른 값을 저장한다.
	// 논 애니 : 초기 필요한 정보 + 인디시즈 + 버티시즈
	// 애니 : 초기 필요한 정보 + 인디시즈 + 버티시즈 + 애니메이션의 채널정보(키프레임)
	for (int i = 0; i < m_MeshContainers.size(); ++i) {
		// 타입이 논 애니일 경우
		if (TYPE_NONANIM == m_eType) {
			// 버티시즈 및 인디시즈 저장
			m_MeshContainers[i]->Save_VertexBuffer_NoAnim(Out, m_pScene->mMeshes[i]);
		}
		// 타입이 애니일 경우
		else if (TYPE_ANIM == m_eType) {
			// 버티시즈 및 인디시즈 저장
			m_MeshContainers[i]->Save_VertexBuffer_Anim(Out, m_pScene->mMeshes[i], m_HierarchyNodes);
		}

		m_MeshContainers[i]->Save_IndexBuffer(Out, m_pScene->mMeshes[i]);
	}
	// 마테리얼의 총갯수 저장
	Out << m_Materials.size() << endl;

	vector<MATERIALTEXTURE> vMTInfo;

	// 텍스쳐와 관련된 정보를 저장
	for (int i = 0; i < m_Materials.size(); ++i) {
		for (int j = 0; j < AI_TEXTURE_TYPE_MAX; ++j) {
			if (nullptr == m_Materials[i].pMaterials[j])
				continue;

			_tchar strUnicode[MAX_PATH] = TEXT("");

			char strMultibyte[MAX_PATH];
			ZeroMemory(strMultibyte, MAX_PATH);

			wcscpy_s(strUnicode, MAX_PATH, m_Materials[i].pMaterials[j]->Get_TextureFilePath());

			int len = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, strMultibyte, len, NULL, NULL);

			MATERIALTEXTURE MT;
			MT.iMaterialIndex = i;
			MT.iAI_TEXTURE_TYPE = j;
			strcpy_s(MT.strFilePath, strMultibyte);
			vMTInfo.push_back(MT);
		}
	}

	Out << vMTInfo.size() << endl;

	Out.write((char*)vMTInfo.data(), sizeof(MATERIALTEXTURE) * vMTInfo.size());

	Out << endl;
	// 전체 애니메이션의 갯수를 저장한다.
	Out << m_Animations.size() << endl;

	for (auto& Animation : m_Animations) {
		Animation->Save_AnimationInfo(Out);
	}

	return S_OK;
}

HRESULT CModel::Load_FBX(ifstream & In)
{
	if (!In)
		return E_FAIL;

	// 전체 HierarchyNode의 갯수를 가져온다.
	_uint iHierarchyNodesCnt = 0;
	In >> iHierarchyNodesCnt;

	char szName[MAX_PATH];	// 이름
	char temp[MAX_PATH];	// 줄 건너뛸 때 사용하는 값
	_uint iDepth = 0;
	_uint iParentNum = 0;

	_float4x4 TransformationMatrixTP;

	// 가져온 노드의 갯수만큼 반복하며 노드를 생성한다.
	for (_uint i = 0; i < iHierarchyNodesCnt; ++i) {
		// 이름 정보
		// 깊이, 부모 번호의 정보를 읽는다.
		In.getline(szName, MAX_PATH);
		In.getline(szName, MAX_PATH);
		In >> iDepth >> iParentNum;
		In.getline(temp, MAX_PATH);

		In.read((char*)TransformationMatrixTP.m, sizeof(_float4x4));

		m_HierarchyNodes.push_back(CHierarchyNode::Create(szName, XMLoadFloat4x4(&TransformationMatrixTP), iDepth));
	}

	for (_uint i = 0; i < iHierarchyNodesCnt; ++i) {
		int iIndex = 0;
		In >> iIndex;
		if (-1 != iIndex) {
			m_HierarchyNodes[i]->SetParent(m_HierarchyNodes[iIndex]);
			m_HierarchyNodes[i]->Set_ParentIndex(iIndex);
		}
	}

	// 메쉬 컨테이너 정보 로드
	In >> m_iNumMeshContainers;

	for (_uint i = 0; i < m_iNumMeshContainers; ++i) {
		Load_VertexBuffer(In);
	}

	// 마테리얼 정보 로드
	In >> m_iNumMaterials;
	In.getline(szName, MAX_PATH);

	// 모델이 사용하는 마테리얼의 개수
	int iMTInfoSize = 0;
	In >> iMTInfoSize;
	In.getline(szName, MAX_PATH);


	vector<MATERIALTEXTURE> vMTInfo;
	MATERIALTEXTURE MT;

	for (int i = 0; i < iMTInfoSize; ++i) {
		In.read((char*)&MT, sizeof(MATERIALTEXTURE));
		vMTInfo.push_back(MT);
	}

	for (_uint i = 0; i < m_iNumMaterials; ++i) {
		MODELMATERIAL* MM = new MODELMATERIAL;
		ZeroMemory(MM, sizeof(MODELMATERIAL));
		m_Materials.push_back(*MM);
	}

	for (auto& MT : vMTInfo) {
		_tchar		szFullPath[MAX_PATH] = TEXT("");
		Mul
		m_Materials[MT.iMaterialIndex].pMaterials[MT.iAI_TEXTURE_TYPE] = CTexture::Create(m_pDevice, m_pDeviceContext, szFullPath);
	}

	// 애니메이션 정보 로드
	int iAnimationCnt = 0;
	In >> iAnimationCnt;
	In.getline(szName, MAX_PATH);

	m_iNumAnimations = iAnimationCnt;

	for (int i = 0; i < iAnimationCnt; ++i) {
		Load_Animation(In);
	}

	In.getline(szName, MAX_PATH);

	if (nullptr == m_pDatFilePath) {
		m_pDatFilePath = new char[strlen(szName) + 1];
		strcpy_s(m_pDatFilePath, strlen(szName) + 1, szName);
	}

	return S_OK;
}

HRESULT CModel::Load_VertexBuffer(ifstream & In)
{
	CMeshContainer* pContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_eType, In, m_PivotMatrix, m_HierarchyNodes);

	if (nullptr == pContainer)
		return E_FAIL;

	m_MeshContainers.push_back(pContainer);

	return S_OK;
}

HRESULT CModel::Load_Animation(ifstream & In)
{
	CAnimation* pAnimation = CAnimation::Create(In, m_HierarchyNodes);

	if (nullptr == pAnimation)
		return E_FAIL;

	m_Animations.push_back(pAnimation);

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType, pModelFilePath, pModelFileName, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pMeshContainer : m_MeshContainers)
		Safe_Release(pMeshContainer);

	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();



	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_Release(pMaterial.pMaterials[i]);
		}
	}
	m_Materials.clear();

	m_Importer.FreeScene();
}
