#include "pch.h"
#include "CStage_Snipe.h"
#include "CBackGround.h"
#include "CDataManager.h"
#include "CFileManager.h"
#include "CMapFactory.h"
#include "CObjectManager.h"
#include "CPlayer.h"
#include "CMonster_Suit.h"
#include "CGameDataManager.h"
#include "CDynamicCamera.h"
#include "CSkyBox.h"
#include "CPickingManager.h"
#include "CUIManager.h"
#include "CGlobal_Info.h"
#include "CLoading_Scene.h"
#include "CImageUI.h"
#include "Sound_Manager.h"

static void attachAndSlide(CImageUI* ui, float x, float y, float w, float h)
{
    ui->Set_UISizeAndPos(w, h, x, y);  
    ui->Set_RenderOn(true);
}



CStage_Snipe::CStage_Snipe(LPDIRECT3DDEVICE9 pGraphiCStage_Snipe)
    : Engine::CScene(pGraphiCStage_Snipe)
    , m_iKillCount(0)
    , m_bSpawned(false)
    , m_vMonsters()
    , m_vDeathMarked()
    , m_vSavedPos()
    , m_iNextActivate(0)
    , m_iInitialActivate(1)
    , m_iTargetKills(10)
    , m_vKillIcons()
    , m_iKillMax(10)
    , m_killUIStartX(-350.f)
    , m_killUIStartY(150.f)
    , m_killUISpacing(30.f)
    , m_killUISize(80.f)
{

}

CStage_Snipe::~CStage_Snipe()
{
}

HRESULT CStage_Snipe::Ready_Scene()
{
    if (FAILED(CScene::Ready_Scene()))
        return E_FAIL;

    SetData(SCENE_SNIPE);

    if (FAILED(Ready_Wall_Layer(L"Wall_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Tile_Layer(L"Tile_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Floor_Layer(L"Floor_Layer")))
        return E_FAIL;

    if (FAILED(Ready_SlideWall_Layer(L"SlideWall_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Ceiling_Layer(L"Ceiling_Layer")))
        return E_FAIL;

    /*if (FAILED(Ready_Camera_Layer(L"Camera_Layer")))
        return E_FAIL;*/

    if (FAILED(Ready_EnvObj_Layer(L"Env_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Player_Layer(L"Player_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Camera_Layer(L"Camera_Layer")))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Prefab_Layer(L"Prefab_Layer")))
        return E_FAIL;

    // GameDataManager에 바닥을 z기준 정렬
    CGameDataManager::GetInstance()->Bind_FloorList(CObjectManager::GetInstance()->Get_ObjectList(SCENE_SNIPE, L"Floor_Layer"));

    if (FAILED(Ready_Monster_Layer(L"Monster_Layer")))
        return E_FAIL;
    CSound_Manager::GetInstance()->PlayBGM(L"../Bin//Resource/Sounds/mx_stage_boss_sniper.wav", 0.6f, true);
    CPickingManager::GetInstance()->Ready_Picking();

    return S_OK;
}

_int CStage_Snipe::Update_Scene(const _float &fTimeDelta)
{
    const _int iExit = Engine::CScene::Update_Scene(fTimeDelta);

    static _bool bPrevF1 = false;
    if (GetAsyncKeyState(VK_F1) & 0x8000) { if (!bPrevF1) { g_ColiderRender = !g_ColiderRender; bPrevF1 = true; } }
    else { bPrevF1 = false; }

    TickDeathsAndProgress();

    CPickingManager::GetInstance()->Picking();
    CUIManager::GetInstance()->Update(fTimeDelta);
    return iExit;
}

void CStage_Snipe::LateUpdate_Scene(const _float &fTimeDelta)
{
    Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CStage_Snipe::Render_Scene()
{
    // 디버깅용 코드
}

HRESULT CStage_Snipe::Ready_SlideWall_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Snipe::Ready_Floor_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Snipe::Ready_Wall_Layer(const _tchar *pLayerTag)
{
    InstancingObjects(L"Wall_Layer");
    return S_OK;
}

HRESULT CStage_Snipe::Ready_Ceiling_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Snipe::Ready_Tile_Layer(const _tchar *pLayerTag)
{
    InstancingObjects(L"Tile_Layer");
    return S_OK;
}

HRESULT CStage_Snipe::Ready_Prefab_Layer(const _tchar *pLayerTag)
{
    InstancingPrefabs();
    return S_OK;
}

HRESULT CStage_Snipe::Ready_EnvObj_Layer(const _tchar *pLayerTag)
{
    InstancingObjects(L"Env_Layer");
    return S_OK;
}

HRESULT CStage_Snipe::Ready_Camera_Layer(const _tchar *pLayerTag)
{
    // Camera
    CDynamicCamera::CAMINFO				CamInfo;
    ZeroMemory(&CamInfo, sizeof(CDynamicCamera::CAMINFO));

    CamInfo.vEye = _vec3(0.f, 2.f, -5.f);
    CamInfo.vAt = _vec3(0.f, 0.f, 0.f);

    CamInfo.fFov = D3DXToRadian(60.0f);
    CamInfo.fAspect = (_float)WINCX / WINCY;
    CamInfo.fNear = 0.1f;
    CamInfo.fFar = 1000.f;

    CamInfo.TransformInfo.fSpeed = 10.f;
    CamInfo.TransformInfo.fRotationSpeed = D3DXToRadian(90.0f);

    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_Camera_FPS", SCENE_SNIPE, pLayerTag, &CamInfo)))
        return E_FAIL;


    return S_OK;
}

HRESULT CStage_Snipe::Ready_Player_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Snipe::Ready_Monster_Layer(const _tchar *pLayerTag)
{
    // 툴 배치 몬스터 생성
    InstancingObjects(L"Monster_Layer");

    // 몬스터 전부 비활성화
    SetMonsterActive();

    // 1마리 활성화
    ActivateNext(m_iInitialActivate);

    return S_OK;
}

HRESULT CStage_Snipe::Ready_GameLogic_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Snipe::Ready_UI_Layer(const _tchar *pLayerTag)
{
    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_UIRoot", SCENE_SNIPE, pLayerTag)))
        return E_FAIL;

    return S_OK;
}



void CStage_Snipe::SetData(_uint _iSceneIndex)
{
    CDataManager::GetInstance()->Clear();
    CGameDataManager::GetInstance()->AllClear();
    CPickingManager::GetInstance()->Clear_Picking();

    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Wall_Layer");
    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Tile_Layer");
    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Env_Layer");
    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Monster_Layer");
    for (int i = 0; i < g_PrefabTypeCount; ++i)
    {
        CFileManager::GetInstance()->LoadPrefabDataFile(static_cast<PrefabType>(i));
    }
    CFileManager::GetInstance()->LoadInstancedPrefabDataFile(_iSceneIndex);

    CMapFactory::GetInstance()->SetTargetSceneIndex(_iSceneIndex);
}

void CStage_Snipe::InstancingObjects(const wstring &_Layer)
{
    if (vector<MAPOBJECTDATA> *pVecData = CDataManager::GetInstance()->FindData(_Layer))
    {
        for (MAPOBJECTDATA &element : *pVecData)
        {
            CMapFactory::GetInstance()->Create(element.eCategory, element.iType, &element);
        }
    }
}

void CStage_Snipe::InstancingPrefabs()
{
    if (vector<PREFABDATA> *pVecData = CDataManager::GetInstance()->GetInstancedPrefabDataList())
    {
        for (PREFABDATA &element : *pVecData)
        {
            CMapFactory::GetInstance()->Create(ObjectCategory::PREFAB, static_cast<_uint>(element.eType), &element);
        }
    }
}

void CStage_Snipe::SetMonsterActive()
{
    m_vMonsters.clear();
    m_vSavedPos.clear();

    auto* pList = CObjectManager::GetInstance()->Get_ObjectList(SCENE_SNIPE, L"Monster_Layer");
    if (!pList) return;

    for (CGameObject* obj : *pList)
    {
        if (!obj) continue;
        if (auto* mon = dynamic_cast<CMonster_Suit*>(obj))
        {
            _vec3 saved = mon->GetTransform()->Get_Info(INFO_POS);
            m_vSavedPos.push_back(saved);

            mon->Set_Active(false);
            mon->Set_RenderOn(false);
            _vec3 hide = saved; hide.y -= 10000.f;
            mon->GetTransform()->Set_Info(INFO_POS, hide);
            CPickingManager::GetInstance()->Remove_PickingGroup(mon);

            m_vMonsters.push_back(mon);
        }
    }

    m_vDeathMarked.assign(m_vMonsters.size(), false);
    m_iNextActivate = 0;

    if (m_iTargetKills > (int)m_vMonsters.size())
        m_iTargetKills = (int)m_vMonsters.size();
}

void CStage_Snipe::ActivateNext(int n)
{
    const int N = (int)m_vMonsters.size();
    for (int c = 0; c < n && m_iNextActivate < N; ++c, ++m_iNextActivate)
    {
        auto* mon = m_vMonsters[m_iNextActivate];
        if (!mon) continue;

        _vec3 p = m_vSavedPos[m_iNextActivate];
        p.y += 0.6f;
        p.z -= 1.2f;    
        mon->GetTransform()->Set_Info(INFO_POS, p);

        mon->Set_Active(true);
        mon->Set_RenderOn(true);
        mon->ForceSniperMode();   
    }
}

void CStage_Snipe::TickDeathsAndProgress()
{
    for (int i = 0; i < (int)m_vMonsters.size(); ++i)
    {
        auto* mon = m_vMonsters[i];
        if (!mon || m_vDeathMarked[i]) continue;

        if (mon->Get_Dead())
        {
            m_vDeathMarked[i] = true;
            ++m_iKillCount;

            SpawnKillIconAtIndex(m_iKillCount - 1);
            ActivateNext(1); // 한마리 활성화
        }
    }

    if (m_iKillCount >= m_iTargetKills)
    {
        // 다음 씬 이동
        // 여기에 영상 재생 시킬거임
        if (FAILED(CManagement::GetInstance()->Open_Scene(
            SCENE_LOADING, CLoading_Scene::Create(m_pGraphicDev, SCENE_BOSS))))
        {
            return;
        }
    }
}

CStage_Snipe *CStage_Snipe::Create(LPDIRECT3DDEVICE9 pGraphiCStage_Snipe)
{
    CStage_Snipe *pStage = new CStage_Snipe(pGraphiCStage_Snipe);

    if (FAILED(pStage->Ready_Scene()))
    {
        MSG_BOX("pStage Create Failed");
        Safe_Release(pStage);
        return nullptr;
    }

    return pStage;
}

void CStage_Snipe::SpawnKillIconAtIndex(int idx)
{
    if ((int)m_vKillIcons.size() >= m_iKillMax) {
        if (!m_vKillIcons.empty()) {
            CImageUI* tail = m_vKillIcons.back();
            if (tail) tail->Set_Active(false); 
            m_vKillIcons.pop_back();
        }
    }

    const _uint sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (auto* icon = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer")))
    {
        icon->RegisterTexture(L"Com_Texture_SniperIcon",
            L"Prototype_Component_Texture_SniperMosnterIcon",
            0, 0, 0.f, false);
        icon->ChangeTexture(L"Com_Texture_SniperIcon");
        icon->SetColorMode(CImageUI::ColorMode::TextureOnly);
        icon->SetAdditive(false);
        icon->SetTintRGBA(255, 255, 255, 255);

        m_vKillIcons.insert(m_vKillIcons.begin(), icon);

        SetLayoutIcon();
    }
}

void CStage_Snipe::ClearKillIcons()
{
    m_vKillIcons.clear();
}

void CStage_Snipe::SetLayoutIcon()
{
    const int   N = (int)m_vKillIcons.size();     
    const float step = (m_killUISize + m_killUISpacing);
    const float baseX = m_killUIStartX;               
    const float y = m_killUIStartY;

    for (int i = 0; i < N; ++i)
    {
        CImageUI* icon = m_vKillIcons[i];
        if (!icon) continue;
        const float x = baseX + i * step;

        attachAndSlide(icon, x, y, m_killUISize, m_killUISize);
    }
}

void CStage_Snipe::Free()
{
    Engine::CScene::Free();

    CGlobal_Info::Destroy_Instance();
}
