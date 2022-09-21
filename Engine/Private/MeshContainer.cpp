#include "..\Public\MeshContainer.h"
#include "HierarchyNode.h"

CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)
	, m_eType(rhs.m_eType)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_pAIMesh(rhs.m_pAIMesh)
	, m_vHierarchyIndexes(rhs.m_vHierarchyIndexes)
	, m_MeshContainerNodes(rhs.m_MeshContainerNodes)
{

}

HRESULT CMeshContainer::NativeConstruct_Prototype(CModel::TYPE eType, aiMesh * pAIMesh, _float4x4 PivotMatrix, vector<CHierarchyNode*>	HierarchyNodes)
{
	m_eType = eType;
	m_PivotMatrix = PivotMatrix;
	m_pAIMesh = pAIMesh;

#pragma region VERTEX_BUFFER
	HRESULT			hr = 0;

	if (CModel::TYPE_NONANIM == eType)
		hr = Create_VertexBuffer_NonAnim(pAIMesh);

	else
		hr = Create_VertexBuffer_Anim(pAIMesh, HierarchyNodes);
#pragma endregion


#pragma region INDEX_BUFFER
	m_iPrimitiveIndicesSize = sizeof(FACELISTINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.StructureByteStride = 0;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;

	FACELISTINDICES32*		pIndices = new FACELISTINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACELISTINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct_Prototype(CModel::TYPE eType, ifstream & In, _float4x4 PivotMatrix, vector<CHierarchyNode*> HierarchyNodes)
{
	if (!In)
		return E_FAIL;

	m_eType = eType;
	m_PivotMatrix = PivotMatrix;

	if (CModel::TYPE_NONANIM == m_eType) {
		Load_VertexBuffer_NonAnim(In);
	}
	else if (CModel::TYPE_ANIM == m_eType) {
		Load_VertexBuffer_Anim(In, HierarchyNodes);
	}

	Load_IndexBuffer(In);

	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct(void * pArg)
{
	vector<CHierarchyNode*>		HierarchyNodes = *(vector<CHierarchyNode*>*)pArg;

	if (0 == m_iNumBones)
	{
		if (1 == HierarchyNodes.size())
			return S_OK;

		auto	iter = find_if(HierarchyNodes.begin(), HierarchyNodes.end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(m_pAIMesh->mName.data, pNode->Get_Name());
		});

		if (iter == HierarchyNodes.end())
			return E_FAIL;

		m_pHierarchyNode = *iter;

		m_vHierarchyIndexes.push_back(_uint(distance(HierarchyNodes.begin(), iter)));

		Safe_AddRef(*iter);

		return S_OK;
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pAIBone = m_pAIMesh->mBones[i];

		auto	iter = find_if(HierarchyNodes.begin(), HierarchyNodes.end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(pAIBone->mName.data, pNode->Get_Name());
		});

		if (iter == HierarchyNodes.end())
			return E_FAIL;

		_float4x4	OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		(*iter)->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_vHierarchyIndexes.push_back(_uint(distance(HierarchyNodes.begin(), iter)));

		m_MeshContainerNodes.push_back((*iter));

		Safe_AddRef(*iter);
	}
	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct_Dat(void * pArg)
{
	vector<CHierarchyNode*>		HierarchyNodes = *(vector<CHierarchyNode*>*)pArg;

	if (CModel::TYPE_NONANIM == m_eType)
		return S_OK;

	if (0 == m_iNumBones)
	{
		if (1 == HierarchyNodes.size())
			return S_OK;

		m_pHierarchyNode = HierarchyNodes[m_vHierarchyIndexes[0]];

		return S_OK;
	}

	vector<CHierarchyNode*> vHierarchyNodes;
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		CHierarchyNode* pHierarchyNode = HierarchyNodes[m_vHierarchyIndexes[i]];

		pHierarchyNode->Set_OffsetMatrix(m_MeshContainerNodes[i]->Get_OffsetMatrix());
		vHierarchyNodes.push_back(pHierarchyNode);
	}

	m_MeshContainerNodes.clear();
	m_MeshContainerNodes = vHierarchyNodes;

	return S_OK;
}

HRESULT CMeshContainer::Create_VertexBuffer_NonAnim(aiMesh* pAIMesh)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumPrimitive = pAIMesh->mNumFaces;
	m_iStride = sizeof(VTXNORTEX);
	m_iNumVBuffers = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), XMLoadFloat4x4(&m_PivotMatrix)));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), XMLoadFloat4x4(&m_PivotMatrix))));

		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Create_VertexBuffer_Anim(aiMesh* pAIMesh, vector<CHierarchyNode*>	HierarchyNodes)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumPrimitive = pAIMesh->mNumFaces;
	m_iStride = sizeof(VTXANIMMODEL);
	m_iNumVBuffers = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	if (FAILED(Create_SkinnedInfo(pAIMesh, pVertices, HierarchyNodes)))
		return E_FAIL;

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Create_SkinnedInfo(aiMesh* pAIMesh, VTXANIMMODEL* pVertices, vector<CHierarchyNode*>	HierarchyNodes)
{
	m_iNumBones = pAIMesh->mNumBones;

	if (0 == m_iNumBones)
	{
		auto	iter = find_if(HierarchyNodes.begin(), HierarchyNodes.end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(pAIMesh->mName.data, pNode->Get_Name());
		});

		if (iter == HierarchyNodes.end())
			return E_FAIL;

		m_vHierarchyIndexes.push_back(_uint(distance(HierarchyNodes.begin(), iter)));

		m_pHierarchyNode = *iter;

		Safe_AddRef(*iter);

		return S_OK;
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];

		auto	iter = find_if(HierarchyNodes.begin(), HierarchyNodes.end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(pAIBone->mName.data, pNode->Get_Name());
		});

		if (iter == HierarchyNodes.end())
			return E_FAIL;

		_float4x4	OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		(*iter)->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_vHierarchyIndexes.push_back(_uint(distance(HierarchyNodes.begin(), iter)));

		m_MeshContainerNodes.push_back((*iter));

		Safe_AddRef(*iter);

		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}

			else
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndex.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}
	return S_OK;
}

HRESULT CMeshContainer::Get_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	if (0 == m_iNumBones)
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixTranspose(m_pHierarchyNode->Get_CombinedMatrix() * PivotMatrix));
	}

	_uint		iIndex = 0;
	for (auto& pHierarchyNode : m_MeshContainerNodes)
	{
		XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(pHierarchyNode->Get_OffsetMatrix() * pHierarchyNode->Get_CombinedMatrix() * PivotMatrix));
	}

	return S_OK;
}

HRESULT CMeshContainer::Load_VertexBuffer_NonAnim(ifstream & In)
{
	if (!In)
		return E_FAIL;

	In >> m_iMaterialIndex;
	In >> m_iNumVertices;
	In >> m_iNumPrimitive;
	In >> m_iStride;
	In >> m_iNumVBuffers;

	char szName[MAX_PATH];
	In.getline(szName, MAX_PATH);

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	In.read((char*)pVertices, sizeof(VTXNORTEX)*m_iNumVertices);

	In.getline(szName, MAX_PATH);

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Load_VertexBuffer_Anim(ifstream & In, vector<CHierarchyNode*> HierarchyNodes)
{
	if (!In)
		return E_FAIL;

	In >> m_iMaterialIndex;
	In >> m_iNumVertices;
	In >> m_iNumPrimitive;
	In >> m_iStride;
	In >> m_iNumVBuffers;

	char szName[MAX_PATH];
	In.getline(szName, MAX_PATH);
	//In.getline(szName, MAX_PATH);

	int iIndexSize = 0;
	In >> iIndexSize;
	for (int i = 0; i < iIndexSize; ++i) {
		int iIndex = 0;
		In >> iIndex;
		m_vHierarchyIndexes.push_back(iIndex);
	}

	In.getline(szName, MAX_PATH);

	//for (auto Index : m_vHierarchyIndexes)
	//	Out << Index << " ";
	//Out << endl;

	//In >> m_vHierarchyIndexes.size() << " ";
	//for (auto Index : m_vHierarchyIndexes)
	//	Out << Index << " ";
	//Out << endl;

	//// 버텍스 버퍼 저장
	//Out.write((char*)pVertices, sizeof(VTXANIMMODEL)*m_iNumVertices);
	//Out << endl;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	// 버티시즈 정보 저장 이후 
	In.read((char*)pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);
	In.getline(szName, MAX_PATH);

	In >> m_iNumBones;

	// 뼈의 갯수가 0개라면
	if (0 == m_iNumBones)
	{
		// 메쉬 루트 노드 인덱스 정보 읽어오기
		int iIndex = 0;
		In >> iIndex;

		m_pHierarchyNode = HierarchyNodes[iIndex];

		In.getline(szName, MAX_PATH);
	}
	// 뼈의 갯수가 0개가 아니라면
	else {
		// 메쉬를 이루는 노드들 정보 읽어오기
		int iHierarchyNodeNum = 0;
		In >> iHierarchyNodeNum;

		for (int i = 0; i < iHierarchyNodeNum; ++i) {
			int iNumber = 0;
			In >> iNumber;
			m_MeshContainerNodes.push_back(HierarchyNodes[iNumber]);
		}

		In.getline(szName, MAX_PATH);

		for_each(m_MeshContainerNodes.begin(), m_MeshContainerNodes.end(), [&](CHierarchyNode* pNode) {
			_float4x4 OffsetMatrix;
			In.read((char*)&OffsetMatrix.m, sizeof(_float4x4));

			pNode->Set_OffsetMatrix(XMLoadFloat4x4(&OffsetMatrix));
			In.getline(szName, MAX_PATH);
		});
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Load_IndexBuffer(ifstream & In)
{
	m_iPrimitiveIndicesSize = sizeof(FACELISTINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.StructureByteStride = 0;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;

	FACELISTINDICES32*		pIndices = new FACELISTINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACELISTINDICES32) * m_iNumPrimitive);

	In.read((char*)pIndices, sizeof(FACELISTINDICES32) * m_iNumPrimitive);

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMeshContainer::Save_VertexIndexInfo(ostream & Out, aiMesh * pAIMesh)
{
	if (!Out)
		return E_FAIL;


	return S_OK;
}

HRESULT CMeshContainer::Save_VertexBuffer_NoAnim(ostream& Out, aiMesh * pAIMesh)
{
	if (!Out)
		return E_FAIL;

	Out << m_iMaterialIndex << " " << m_iNumVertices << " " << m_iNumPrimitive << " " << m_iStride << " " << m_iNumVBuffers << " " << endl;

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), XMLoadFloat4x4(&m_PivotMatrix)));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), XMLoadFloat4x4(&m_PivotMatrix))));

		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	Out.write((char*)pVertices, sizeof(VTXNORTEX)*m_iNumVertices);
	Out << endl;


	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Save_VertexBuffer_Anim(ostream& Out, aiMesh * pAIMesh, vector<CHierarchyNode*>	HierarchyNodes)
{
	if (!Out)
		return E_FAIL;

	Out << m_iMaterialIndex << " " << m_iNumVertices << " " << m_iNumPrimitive << " " << m_iStride << " " << m_iNumVBuffers << " " << endl;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	if (FAILED(Create_SkinnedInfo(pAIMesh, pVertices, HierarchyNodes)))
		return E_FAIL;

	// Create_SkinnedInfo함수를 1번 더 부르면서 사이즈의 값이 2배가 되어 버렸다. 그래서 반으로 만들어준다.
	_uint iSize = _uint (m_vHierarchyIndexes.size() * 0.5);

	Out << iSize << " ";

	for (_uint i = 0; i < iSize; ++i)
		Out << m_vHierarchyIndexes[i] << " ";

	Out << endl;

	// 버텍스 버퍼 저장
	Out.write((char*)pVertices, sizeof(VTXANIMMODEL)*m_iNumVertices);
	Out << endl;


	// m_MeshContainerNodes들에 있는 노드들 순서대로 정보 넣어주기

	// 뼈 정보 저장
	Out << m_iNumBones << " ";

	// 뼈의 갯수가 0개라면
	if (0 == m_iNumBones)
	{
		// m_pHierarchyNode의 노드 번호의 인덱스 저장
		auto& iter = find_if(HierarchyNodes.begin(), HierarchyNodes.end(), [&](CHierarchyNode* pNode) {
			return pNode->Get_HierarchyNumber() == m_pHierarchyNode->Get_HierarchyNumber();
		});

		size_t iIndex = distance(HierarchyNodes.begin(), iter);
		Out << iIndex << endl;
	}
	// 뼈의 갯수가 0개가 아니라면
	else {
		// m_MeshContainerNodes의 노드 번호들의 인덱스를 저장

		// 이번에 2번째로 Create_SkinnedInfo함수가 불려졌기 때문에 값이 2번 들어가있다. 그래서 1번으로 줄어주기 위해 반으로 줄인다.
		_uint iSize = _uint(m_MeshContainerNodes.size() * 0.5);
		Out << iSize << " ";

		for (_uint  i = 0; i < iSize; ++i) {
			Out << m_MeshContainerNodes[i]->Get_HierarchyNumber() << " ";
		}

		Out << endl;

		for (_uint i = 0; i < iSize; ++i) {
			_float4x4 OffsetMatrix;
			XMStoreFloat4x4(&OffsetMatrix, m_MeshContainerNodes[i]->Get_OffsetMatrix());
			Out.write((char*)&OffsetMatrix.m, sizeof(_float4x4));
			Out << endl;
		}
	}

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Save_IndexBuffer(ostream & Out, aiMesh * pAIMesh)
{
	FACELISTINDICES32*		pIndices = new FACELISTINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACELISTINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
	}

	Out.write((char*)pIndices, sizeof(FACELISTINDICES32) * m_iNumPrimitive);
	Out << endl;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CModel::TYPE eType, aiMesh * pAIMesh, _float4x4 PivotMatrix, vector<class CHierarchyNode*> HierarchyNodes)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType, pAIMesh, PivotMatrix, HierarchyNodes)))
	{
		MSG_BOX(TEXT("Failed to Created CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CModel::TYPE eType, ifstream & In, _float4x4 PivotMatrix, vector<class CHierarchyNode*> HierarchyNodes)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType, In, PivotMatrix, HierarchyNodes)))
	{
		MSG_BOX(TEXT("Failed to Created CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*	pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMeshContainer::Clone_Dat(void * pArg)
{
	CMeshContainer*	pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->NativeConstruct_Dat(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	Safe_Release(m_pHierarchyNode);

	for (auto& pHierarchyNode : m_MeshContainerNodes)
		Safe_Release(pHierarchyNode);

	m_MeshContainerNodes.clear();
}