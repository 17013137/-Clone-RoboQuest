#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "Client_Object.h"

CLoader::CLoader(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: m_pDevice(pDeviceOut), m_pDeviceContext(pDeviceContextOut)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

unsigned int APIENTRY ThreadEntryFunc(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_LevelIndex())
	{
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_DUNGEON:
		pLoader->Loading_ForDungeon();
		break;
	case LEVEL_BOSSTOWER:
		pLoader->Loading_ForBossTower();
		break;
	case LEVEL_BOSSDISCO:
		pLoader->Loading_ForBossDisco();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::NativeConstruct(LEVEL eLevel)
{
	m_eLevel = eLevel;
	
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntryFunc, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);


	lstrcpy(m_szLoading, TEXT("텍스쳐를 로드중입니다. "));

	/* For. Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Number"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Number/Number(%d).dds"), 10))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BulletBackground"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/BulletBackground/BulletBackground.dds"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Terrain/T(%d).dds"), 7))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_TerrainMask */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TerrainMask"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Brush */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/Burger%d.dds"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CrossHair"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/CrossHair/CrossHair(%d).dds"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerIcon"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/PlayerIcon/PlayerIcon.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerBackGround"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/PlayerBackGround/PlayerBackGround.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HealthBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/HealthBar/HealthBar(%d).dds"), 6))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Intro"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/BossIntro/Intro(%d).dds"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MuzzleEffect"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Effect/Muzzle/Muzzle(%d).dds"), 10))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FadeInOut"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/FadeInOut/Black.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ExpBall"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/ExpBall/ExpBall(%d).dds"), 15))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hit"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Hit/HitMaker(%d).dds"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Trail/Trail(%d).dds"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Bullet/Bullet(%d).dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Spark"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Trail/Spark(%d).dds"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Smoke"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Effect/Sprite/Smoke/Smoke(%d).dds"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SubSmoke"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Effect/Sprite/SubSmoke/SubSmoke(%d).dds"), 36))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Effect/Sprite/Fire/Fire(%d).dds"), 32))))
		return E_FAIL; 
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FirePattern"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/FirePattern/Fire_Pattern.dds"), 1))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sprint"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Status/Sprint/SprintLayer.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Shield"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Status/Shield/Shield.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Recovering"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Status/Recovering/Recovering.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponSound"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/WeaponSound/WeaponSound(%d).dds"), 12))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RifleIcon"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/WeaponIcon/Rifle/RifleIcon(%d).dds"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShotgunIcon"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/WeaponIcon/Shotgun/ShotgunIcon(%d).dds"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BowIcon"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/WeaponIcon/Bow/BowIcon(%d).dds"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UpgradeBall"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/UpgradeBall/Upgrade_Ball.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FlameThrower"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Effect/FlameThrower/FlameThrower.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Mask/Mask(%d).dds"), 10))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Laser"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Laser/Laser(%d).dds"), 4))))
		return E_FAIL;
	lstrcpy(m_szLoading, TEXT("모델을 로드중입니다. "));

	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;
	_matrix			PivotMatrix;


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossShield"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/BossShield/", "BossShield.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_OverHit"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/OverHit/", "OverHit.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_OverHitLaser"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/OverHitLaser/", "OverHitLaser.fbx", PivotMatrix))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Player/", "Player.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BaseCamp"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Map/", "BaseCamp.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Basic/Sphere/", "BlackSphere.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RocketBullet"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Weapon/RocketLauncher/", "RocketBullet.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Joy"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Joy/", "Joy.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HealBot"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/HealBot/", "HealBot.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SmallBot"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Enemy/SmallBot/", "SmallBot.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RocketPawn"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Enemy/RocketPawn/", "RocketPawn.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MortarPod"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Enemy/MortarPod/", "MortarPod.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MecanoBot"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Enemy/MecanoBot/", "MecanoBot.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MinePod"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Enemy/MinePod/", "MinePod.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JetFly"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Enemy/JetFly/", "JetFly.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlameCone"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/FlameCone/", "FlameCone.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlameTriangle"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/FlameTriangle/", "FlameTriangle.fbx", PivotMatrix))))
		return E_FAIL;


	// 무기 로드
	PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(0.01f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AssultM4"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Weapon/AssultM4/", "AssultM4.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rifle"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Weapon/Rifle/", "Rifle.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LMG"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Weapon/LMG/", "LMG.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HuntingShotgun"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Weapon/HuntingShotgun/", "HuntingShotgun.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TacticalShotgun"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Weapon/TacticalShotgun/", "TacticalShotgun.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TripleShotgun"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Weapon/TripleShotgun/", "TripleShotgun.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LongBow"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Weapon/LongBow/", "LongBow.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HeavyCrossBow"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Weapon/HeavyCrossBow/", "HeavyCrossBow.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RocketLauncher"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Weapon/RocketLauncher/", "RocketLauncher.fbx", PivotMatrix))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("게임오브젝트를 로드중입니다. "));

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Default */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_First"),
		CCamera_First::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Default"),
		CCamera_Default::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sword"),
		CSword::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NaviBox"),
		CNaviBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	
	/*   무기    */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AssultM4"),
		CBundleRifle::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HuntingShotgun"),
		CBundleShotgun::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LongBow"),
		CBundleBow::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/*   총알    */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RifleBullet"),
		CRifleBullet::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShotgunBullet"),
		CShotgunBullet::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BowBullet"),
		CBowBullet::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RocketBullet"),
		CRocketBullet::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Punch"),
		CPunch::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RocketExplosion"),
		CRocketExplosion::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/*	Enemy	*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SmallBot"),
		CSmallBot::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RocketPawn"),
		CRocketPawn::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MortarPod"),
		CMortarPod::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MecanoBot"),
		CMecanoBot::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MinePod"),
		CMinePod::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyBullet"),
		CEnemyBullet::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MortarHitbox"),
		CMortarHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MineHitBox"),
		CMineHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_JetFly"),
		CJetFly::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossShield"),
		CBossShield::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	/*   엔피씨    */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Joy"),
		CJoy::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HealBot"),
		CHealBot::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BaseCamp"),
		CBaseCamp::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	

	/*				UI			*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrossHairRect"),
		CCrossHairRect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerIcon"),
		CPlayerIcon::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerBackGround"),
		CPlayerBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HealthBar"),
		CHealthBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HealthBarBackGround"),
		CHealthBarBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExpBall"),
		CExpBall::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_HealthBar"),
		CHealthBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_HealthBarBackGround"),
		CHealthBarBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_ExpBar"),
		CExpBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_ExpBarBackGround"),
		CExpBarBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Hit"),
		CHit::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FadeOut"),
		CFadeOut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Spirnt"),
		CSprint::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Recovering"),
		CRecovering::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_Shield"),
		CShield::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_RifleIcon"),
		CRifleIcon::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_ShotgunIcon"),
		CShotgunIcon::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_BowIcon"),
		CBowIcon::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_BulletCount"),
		CBulletCount::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_BulletTenCount"),
		CBulletTenCount::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_BulletBackground"),
		CBulletBackground::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/*				Effect			*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_Muzzle"),
		CMuzzleEffect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_BulletTrail"),
		CExpTrail::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_Trail_Monster"),
		CTrail_Monster::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_Trail_Purple"),
		CTrail_Purple::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_Trail_Bow"),
		CTrail_Bow::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_SmokeSprite"),
		CSmokeSprite::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_SubSmoke"),
		CSubSmoke::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_Explosion"),
		CExplosion::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_WeaponSound"),
		CWeaponSound::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_CombineEffect"),
		CCombine_Effect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlameCone"),
		CFlameCone::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlameTriangle"),
		CFlameTriangle::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Effect_FireEffect"),
		CFire_Effect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_OverHit"),
		COverHit::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_OverHitLaser"),
		COverHitLaser::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다.. "));
	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForDungeon()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix			PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Stage1"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Map/", "Stage1.fbx", PivotMatrix))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Stage2"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Map/", "Stage2.fbx", PivotMatrix))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Stage3"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Map/", "Stage3.fbx", PivotMatrix))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Stage4"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Map/", "Stage4.fbx", PivotMatrix))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Stage5"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Map/", "Stage5.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_MagmaTile"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/MagmaTile/", "MagmaTile.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_SpringBoard"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Map/", "SpringBoard.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DoorStage"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Door/", "StageDoor.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DoorBoss"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Door/", "BossDoor.fbx", PivotMatrix))))
		return E_FAIL;




	lstrcpy(m_szLoading, TEXT("게임오브젝트를 로드중입니다. "));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage1"),
		CStage1::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage2"),
		CStage2::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage3"),
		CStage3::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage4"),
		CStage4::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage5"),
		CStage5::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorStage"),
		CDoorStage::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorBoss"),
		CDoorBoss::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MagmaTile"),
		CMagmaTile::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpringBoard"),
		CSpringBoard::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FadeOut"),
		CFadeOut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;



	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForBossTower()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoading, TEXT("모델을 로드중입니다. "));

	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSSTOWER, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로드중입니다. "));

	_matrix			PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSSTOWER, TEXT("Prototype_Component_Model_BossStage2"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Map/", "BossStage2.fbx", PivotMatrix))))
		return E_FAIL;

	/* 몬스터 로드 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSSTOWER, TEXT("Prototype_Component_Model_BossTower"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Enemy/BossTower/", "BossTower.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSSTOWER, TEXT("Prototype_Component_Model_TowerSocle"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Enemy/BossTower/", "TowerSocle.fbx", PivotMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("게임오브젝트를 로드중입니다. "));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CanonHitBox"),
		CCanonHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlameHitBox"),
		CFlameHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MortarHitBox"),
		CMortarHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HandBeam"),
		CHandLaser::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/*   Enemy    */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossTower"),
		CBossTower::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TowerSocle"),
		CTowerSocle::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/*    UI    */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IntroBackGround"),
		CIntroBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IntroTag"),
		CIntroTag::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FadeInOut"),
		CFadeInOut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FadeOut"),
		CFadeOut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_BossHpBar"),
		CBossHpBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_BossHpBarBackground"),
		CBossHpBarBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;




	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다.. "));
	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForBossDisco()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoading, TEXT("모델을 로드중입니다. "));

	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSSDISCO, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix			PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixTranslation(0.f, -4.5f, 0.f);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSSDISCO, TEXT("Prototype_Component_Model_BossStage1"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Map/", "BossStage1.fbx", PivotMatrix))))
		return E_FAIL;

	/* 몬스터 로드 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSSDISCO, TEXT("Prototype_Component_Model_BossDisco"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Resources/Model/Enemy/BossDisco/", "DiscoBall.fbx", PivotMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("게임오브젝트를 로드중입니다. "));
	/* For.Prototype_GameObject_ForkLift */
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Default */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_First"),
		CCamera_First::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NormalHitBox"),
		CNormalHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StickHitBox"),
		CStickHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaveHitBox"),
		CWaveHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LaserHitBox"),
		CLaserHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_JumpingHitBox"),
		CJumpingHitBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/*   Enemy    */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossDisco"),
		CBossDisco::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/*    UI    */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IntroBackGround"),
		CIntroBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IntroTag"),
		CIntroTag::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FadeInOut"),
		CFadeInOut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FadeOut"),
		CFadeOut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_BossHpBar"),
		CBossHpBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_UI_BossHpBarBackground"),
		CBossHpBarBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다.. "));
	m_isFinished = true;

	Safe_Release(pGameInstance);


	return S_OK;
}


CLoader * CLoader::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut, LEVEL eLevel)
{
	CLoader*	pInstance = new CLoader(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct(eLevel)))
	{
		MSG_BOX(TEXT("Failed to Created CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	
	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
