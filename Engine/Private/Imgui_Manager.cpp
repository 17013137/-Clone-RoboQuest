
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "..\Public\Imgui_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Navigation.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{

}

HRESULT CImgui_Manager::InitImGui(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	ImGui_ImplDX11_Init(*ppDeviceOut, *ppDeviceContextOut);

	m_pDevice = *ppDeviceOut;
	m_DeviceContext = *ppDeviceContextOut;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_DeviceContext);

	return S_OK;
}

HRESULT CImgui_Manager::StartFrame(void)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	return S_OK;
}

HRESULT CImgui_Manager::Set_Contents(void)
{
	static bool show_demo_window = true;
	static bool show_another_window = false;
	static bool Show_test = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("NaviMesh Plz!!!!!!!!!");                          // Create a window called "Hello, world!" and append into it.
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		//ImGui::Checkbox("Another Window", &show_another_window);

		//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgetgs return true when edited/activated)
		//	counter++;
		//ImGui::SameLine();
		ImGui::Checkbox("NaviMode", &m_isNaviMode);

		ImGui::SameLine();
		Save_Text();

		ImGui::SameLine();
		if (ImGui::Button("Load File"))
			Load_NaviFile();

		ImGui::SameLine();
		if (ImGui::Button("Save"))
			Save_NaviFile();


		ImGui::Text("Cell : %d", m_Cell.size());
		
		ImGui::SameLine();
		if (ImGui::Button("Undo Cell"))
			Undo_NaviBox();

		ImGui::Combo("CellIndex", &m_CellIndex, CellIndex, IM_ARRAYSIZE(CellIndex));

		if (ImGui::SliderFloat("Set Y", &f, 0.f, 30.f)) {
			m_SetY = f;
		}

		if (ImGui::Button("Push Cell"))
			Push_Cell();

		Move_NaviBox();

		ImGui::End();

		
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	return S_OK;
}

HRESULT CImgui_Manager::Render()
{
	// Imgui render옵션이 켜져 있다면
	if (m_bImguiEnable) {
		StartFrame();
		Set_Contents();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	return S_OK;
}

void CImgui_Manager::InitWin32(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
}

LRESULT CImgui_Manager::WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

void CImgui_Manager::Shutdown(void)
{
}

void CImgui_Manager::Get_NaviPoint(CGameObject* Obj)
{
	//CTransform* Trans = Obj->Get_Transform();
	//_float3 vPos;
	//XMStoreFloat3(&vPos, Trans->Get_State(CTransform::STATE_POSITION));
	//
	//switch (m_PointCnt)
	//{
	//case 0:
	//	m_Point.PointA = vPos;
	//	break;
	//case 1:
	//	m_Point.PointB = vPos;
	//	break;
	//case 2:
	//	m_Point.PointC = vPos;
	//	break;
	//default:
	//	break;
	//}
	//
	m_NaviObj[m_PointCnt] = Obj;
	m_PointCnt++;
	m_vNaviBox.push_back(Obj);
	m_Current_Transform = Obj->Get_Transform();
	//


	//if (m_PointCnt >= 3) {
	//	m_Cell.push_back(m_Point);
	//	m_PointCnt = 0;
	//}
}

_bool CImgui_Manager::Save_NaviFile()
{
	_ulong		dwByte = 0;

	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile) {
		return false;
		m_isSuccess = 0;
	}
	for (auto iter : m_Cell) {
		WriteFile(hFile, &iter, sizeof(CELLDESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameInstance);

	CTransform* TerrainTrans = pGameInstance->Get_Transform(pGameInstance->Get_LevelIndex(), L"Layer_BackGround");

	if (FAILED(pGameInstance->Initialize_Navigation(m_pDevice, m_DeviceContext, TEXT("../Bin/Data/Navigation.dat"), TerrainTrans))) {
		return false;
	}

	Safe_Release(pGameInstance);

	m_isSuccess = 1;
	return true;
}

HRESULT CImgui_Manager::Load_NaviFile()
{
	_ulong			dwByte = 0;

	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Navigation.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		CELLDESC vPoint;

		ReadFile(hFile, &vPoint, sizeof(CELLDESC), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		

		_float3 fPos[3];
		memcpy(fPos, &vPoint, sizeof(_float3) * 3);
		
		CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
		Safe_AddRef(pGameinstance);

		m_Cell.push_back(vPoint);
		m_CellIndex++;
		for (int i = 0; i < 3; i++) 
			m_vNaviBox.push_back(pGameinstance->Add_GameObjToLayer(pGameinstance->Get_LevelIndex(), L"NaviBox", L"Prototype_GameObject_NaviBox", &XMLoadFloat3(&fPos[i])));


		Safe_Release(pGameinstance);


	}

	CloseHandle(hFile);

	return S_OK;
}

void CImgui_Manager::Save_Text()
{
	switch (m_isSuccess)
	{
	case 0:
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Failed Save File!!");
		break;
	case 1:
		ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Succes Save File!!");
		break;
	default:
		ImGui::Text("Default...");
		break;
	}
}

void CImgui_Manager::Free()
{


	Safe_Release(m_pDevice);
	Safe_Release(m_DeviceContext);


	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CImgui_Manager::Undo_NaviBox()
{
	CNavigation::GetInstance()->Remove_LastCell();
	m_Cell.pop_back();
}

void CImgui_Manager::Move_NaviBox()
{
	if (m_Current_Transform == nullptr)
		return;

	if (GetKeyState(VK_NUMPAD6) & 0x8000)
	{
		m_Current_Transform->Go_X(-0.005f);
	}

	if (GetKeyState(VK_NUMPAD4) & 0x8000)
	{
		m_Current_Transform->Go_X(0.005f);
	}

	if (GetKeyState(VK_NUMPAD8) & 0x8000)
	{
		m_Current_Transform->Go_Z(0.005f);
	}

	if (GetKeyState(VK_NUMPAD2) & 0x8000)
	{
		m_Current_Transform->Go_Z(-0.005f);
	}

	if (GetKeyState(VK_NUMPAD1) & 0x8000)
	{
		m_Current_Transform->Go_Y(-0.005f);
	}

	if (GetKeyState(VK_NUMPAD3) & 0x8000)
	{
		m_Current_Transform->Go_Y(0.005f);
	}

	if (GetAsyncKeyState(VK_NUMPAD5) & 0x0001)
	{
		Push_Cell();
	}

	if (GetAsyncKeyState(VK_NUMPAD7) & 0x0001)
	{
		Undo_NaviBox();
	}

}



void CImgui_Manager::Push_Cell()
{
	CTransform* CPos;
	_float3 fPos;
	if (m_PointCnt == 3) {
		CPos = m_NaviObj[0]->Get_Transform();
		XMStoreFloat3(&fPos, CPos->Get_State(CTransform::STATE_POSITION));
		m_Point.PointA = fPos;

		CPos = m_NaviObj[1]->Get_Transform();
		XMStoreFloat3(&fPos, CPos->Get_State(CTransform::STATE_POSITION));
		m_Point.PointB = fPos;

		CPos = m_NaviObj[2]->Get_Transform();
		XMStoreFloat3(&fPos, CPos->Get_State(CTransform::STATE_POSITION));
		m_Point.PointC = fPos;
		m_Point.Index = m_CellIndex;

		m_Cell.push_back(m_Point);
		CNavigation::GetInstance()->Push_Cell(m_pDevice, m_DeviceContext, &m_Point);

		m_PointCnt = 0;
		//Save_NaviFile();
	}

}
