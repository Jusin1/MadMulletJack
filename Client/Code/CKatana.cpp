#include "pch.h"
#include "CKatana.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CImageUI.h"
#include "CMapFactory.h"
#include "CPlayer_HandR.h"
CKatana::CKatana(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMainWeapon(pGraphicDev)
{

}

CKatana::CKatana(const CKatana& rhs)
	: CMainWeapon(rhs)
{
}

CKatana::~CKatana()
{
}

HRESULT CKatana::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKatana::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    m_bActive = false;
    m_bRenderOn = false;

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    

    m_pKnifeHandleUI = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
    if (m_pKnifeHandleUI)
    {
        float handleX = WINCX * 0.5f - 50.f;
        float handleTargetY = WINCY * 0.5f + 60.f;
        float handleStartY = handleTargetY + 200.f;

        m_pKnifeHandleUI->Set_UISizeAndPos(400.f, 175.f, handleX, handleStartY);
        m_pKnifeHandleUI->RegisterTexture(L"Com_Texture_Handle",
            L"Prototype_Component_Texture_KatanaHandle", 0, 1, 0.f, true);
        m_pKnifeHandleUI->ChangeTexture(L"Com_Texture_Handle");
        m_pKnifeHandleUI->Set_New_TransInfo(600.f, 0.f);

        UIMoveInfo moveInfo;
        moveInfo.eUIMove = MV_UP;
        moveInfo.bStop = true;
        moveInfo.fRange = 200.f;  // 손 range와 동일
        moveInfo.fSumRange = 0.f;
        moveInfo.bRenderStop = false;
        m_pKnifeHandleUI->Set_UIMoveInfo(moveInfo);

        Add_Child(m_pKnifeHandleUI);
    }


    m_pKnifeUI = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
    if (m_pKnifeUI)
    {
        float knifeX = 195.f;
        float knifeTargetY = 455.f;
        float knifeStartY = knifeTargetY + 200.f;

        m_pKnifeUI->Set_UISizeAndPos(780.f, 100.f, knifeX, knifeStartY);

        m_pKnifeUI->RegisterTexture(L"Com_Texture_Katana",
            L"Prototype_Component_Texture_Knife", 0, 1, 0.f, true);
        m_pKnifeUI->ChangeTexture(L"Com_Texture_Katana");

        m_pKnifeUI->Set_New_TransInfo(600.f, 0.f);

        UIMoveInfo moveInfo;
        moveInfo.eUIMove = MV_UP;
        moveInfo.bStop = true;
        moveInfo.fRange = 200.f;  
        moveInfo.fSumRange = 0.f;
        moveInfo.bRenderStop = false;
        m_pKnifeUI->Set_UIMoveInfo(moveInfo);
        Add_Child(m_pKnifeUI);
    }

    m_pShineKnife = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
    if (m_pShineKnife)
    {
        float sheathX = WINCX * 0.5f - 175.f;
        float sheathTargetY = WINCY * 0.5f - 150.f;
        float sheathStartY = sheathTargetY + 220.f;

        m_pShineKnife->Set_UISizeAndPos(350.f, 110.f, sheathX, sheathStartY);
        m_pShineKnife->RegisterTexture(L"Com_Texture_Sheath",
            L"Prototype_Component_Texture_ShineKnife", 0, 1, 0.f, true);
        m_pShineKnife->ChangeTexture(L"Com_Texture_Sheath");
        m_pShineKnife->Set_New_TransInfo(600.f, 0.f);
        m_pShineKnife->Set_Active(false);
        Add_Child(m_pShineKnife);
    }


    m_pSheathUI = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
    if (m_pSheathUI)
    {
        float sheathX = WINCX * 0.5f - 450.f;
        float sheathTargetY = WINCY * 0.5f + 50.f;
        float sheathStartY = sheathTargetY + 210.f;

        m_pSheathUI->Set_UISizeAndPos(500.f, 160.f, sheathX, sheathStartY);
        m_pSheathUI->RegisterTexture(L"Com_Texture_Sheath",
            L"Prototype_Component_Texture_KatanaSheath", 0, 1, 0.f, true);
        m_pSheathUI->ChangeTexture(L"Com_Texture_Sheath");
        m_pSheathUI->Set_New_TransInfo(600.f, 0.f);

        UIMoveInfo moveInfo;
        moveInfo.eUIMove = MV_UP;
        moveInfo.bStop = true;
        moveInfo.fRange = 220.f; 
        moveInfo.fSumRange = 0.f;
        moveInfo.bRenderStop = false;
        m_pSheathUI->Set_UIMoveInfo(moveInfo);
       
        Add_Child(m_pSheathUI);
    }

    Set_Texture();
    return S_OK;
}

_int CKatana::Update_GameObject(const _float& fTimeDelta)
{
    __super::Update_GameObject(fTimeDelta);
    Move_UI(fTimeDelta);
    if (m_pKnifeHandleUI) m_pKnifeHandleUI->Move_UI(fTimeDelta);
    if (m_pSheathUI)      m_pSheathUI->Move_UI(fTimeDelta);
    if (m_pKnifeUI)       m_pKnifeUI->Move_UI(fTimeDelta);
    if (m_pShineKnife)    m_pShineKnife->Move_UI(fTimeDelta);

    if (m_CurrentAnimTag == TEXT("Com_Texture_Katana_Idle"))
    {
        _uint iCurScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
        _uint iPlayerUI_Idx = 0;
        switch (iCurScene)
        {
        case SCENE_DEV:
            iPlayerUI_Idx = 1;
            break;

        case SCENE_TUTORIAL:
            iPlayerUI_Idx = 1;
            break;
        }
        CUIBase* pHandR = dynamic_cast<CUIBase*>(CObjectManager::GetInstance()
            ->Find_Object(iCurScene, L"UI_Layer", iPlayerUI_Idx))
            ->Find_Child_ByTag(L"HandRUI");
        if (pHandR)
        {
            Set_UIPos(pHandR->GetTransform()->Get_Info(INFO_POS), 50.f, 750.f);
        }
    }
    else if (m_pTextureCom->Is_AnimFinished())
    {
        CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
    }
    

    auto setMove = [](CImageUI* pUI, UIMOVE dir, float range, float speed)
        {
            if (!pUI) return;
            UIMoveInfo moveInfo = pUI->Get_UIMoveInfo();
            moveInfo.eUIMove = dir;
            moveInfo.bStop = true;
            moveInfo.fRange = range;
            moveInfo.fSumRange = 0.f;
            moveInfo.bRenderStop = false;
            pUI->Set_UIMoveInfo(moveInfo);
            pUI->Set_New_TransInfo(speed, 0.f);
        };

    if (m_tInfo.ePlayerState == OPENING && m_tInfo.eWeapon == WP_KATANA)
    {
        if (m_pKnifeHandleUI &&
            m_pKnifeHandleUI->Get_UIMoveInfo().eUIMove == MV_UP &&
            m_pKnifeHandleUI->Get_UIMoveInfo().IsRangeEnd())
        {
            m_pKnifeUI->Set_Active(false);
            m_pShineKnife->Set_Active(true);

            setMove(m_pKnifeHandleUI, MV_RIGHT, 80.f, 2000.f);
            setMove(m_pKnifeUI, MV_RIGHT, 80.f, 2000.f);

            setMove(m_pSheathUI, MV_LEFT, 100.f, 2000.f);
        }

        if (m_pKnifeHandleUI->Get_UIMoveInfo().eUIMove == MV_RIGHT && m_pKnifeHandleUI->Get_UIMoveInfo().IsRangeEnd())
        {
            m_fWaitTimer += fTimeDelta;
            if (m_fWaitTimer >= 0.7f)
            {
                m_pShineKnife->Set_Active(false);
                m_pKnifeUI->Set_Active(true);

                setMove(m_pKnifeHandleUI, MV_RIGHT, 1000.f, 2000.f);
                setMove(m_pKnifeUI, MV_RIGHT, 1000.f, 2000.f);

                setMove(m_pSheathUI, MV_LEFT, 400.f, 2000.f);
            }
        }
    }

    if (m_bInCombo) {
        m_fComboTimer += fTimeDelta;

        if (m_pTextureCom && m_pTextureCom->Is_AnimFinished()) {
            if (m_iComboStep >= 3) {
                m_bInCombo = false;
                m_iComboStep = 0;
                CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
            }
            else {
                if (m_fComboTimer > m_fComboWindow) {
                    m_bInCombo = false;
                    m_iComboStep = 0;
                    CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
                }
            }
        }
    }
    CleanupFinishedFx();
    return NO_EVENT;
}

void CKatana::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CKatana::Render_GameObject()
{
    if (m_bSelfActive && m_bRenderOn)
    {
        m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        if (m_pTextureCom)
        {
            m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
            m_pTextureCom->MoveFrame();
        }

        m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x01);
        m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

        CUI::Render_GameObject();
    }

    for (auto& pChild : m_vecChildren)
    {
        if (pChild && pChild->Is_Active() && pChild->Get_RenderOn())
            pChild->Render_GameObject();
    }

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CKatana::Set_Texture()
{
    m_bSelfActive = false;
    m_bRenderOn = true;
	m_tMoveInfo.eUIMove = MV_NON;

	switch (m_tInfo.ePlayerState)
	{
    case IDLE:
    case JUMP:
        m_bSelfActive = true;
        if (FAILED(Change_Texture(TEXT("Com_Texture_Katana_Idle"))))
            return E_FAIL;
        Set_UISizeAndPos(1700.f, 1900.f, WINCX * 0.5f +  400.f, WINCY * 0.5f);
        Set_New_TransInfo(5.f, 0.f);
        m_tMoveInfo = { MV_UpDown , false, 5.f,0.f };
		break;
    case ATTACK:
        m_bSelfActive = true;
        if (FAILED(Change_Texture(TEXT("Com_Texture_Katana_Attack"))))
            return E_FAIL;
        Set_UISizeAndPos(1600.f, 200.f, WINCX * 0.5f - 650.f, WINCY * 0.5f + 70.f);
        Set_New_TransInfo(3500.f, 0.f);
        m_tMoveInfo = { MV_RIGHT , false, 1000.f,0.f };
        OnAttackInput();
        break;
	}

	return S_OK;
}

HRESULT CKatana::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// IDLE
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 1;
	texInfo.m_fSpeed = 1.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Katana_Idle", SCENE_STATIC, L"Prototype_Component_Texture_Katana_IDLE", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Katana_Idle"), m_pTextureCom });

    // Attack
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_Katana_Attack", SCENE_STATIC, L"Prototype_Component_Texture_KatanaAttack1", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Katana_Attack"), m_pTextureCom });

    // Attack2
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_Katana_Attack2", SCENE_STATIC, L"Prototype_Component_Texture_KatanaAttack2", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Katana_Attack2"), m_pTextureCom });

    // Attack3
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_Katana_Attack3", SCENE_STATIC, L"Prototype_Component_Texture_KatanaAttack3", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Katana_Attack3"), m_pTextureCom });
	return S_OK;


}

HRESULT CKatana::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

void CKatana::OnAttackInput()
{
    // 콤보 중이 아니면 1스텝 시작
    if (!m_bInCombo) {
        m_bInCombo = true;
        m_iComboStep = 1;
        StartComboStep(1);
        return;
    }

    // 콤보 중이면 -> 다음 스텝
    if (m_fComboTimer <= m_fComboWindow && m_iComboStep < 3) {
        m_iComboStep++;
        StartComboStep(m_iComboStep);
    }
}

void CKatana::StartComboStep(int step)
{
    m_fComboTimer = 0.f;
    m_bSelfActive = true;
    switch (step)
    {
    case 1:
        Change_Texture(TEXT("Com_Texture_Katana_Attack"));
        Set_UISizeAndPos(1600.f, 200.f, WINCX * 0.5f - 650.f, WINCY * 0.5f + 70.f);
        Set_New_TransInfo(3500.f, 0.f);
        m_tMoveInfo = { MV_RIGHT, false, 1000.f, 0.f };
        SpawnSlashFx(1);
        break;
    case 2:
        Change_Texture(TEXT("Com_Texture_Katana_Attack2"));
        Set_UISizeAndPos(3000.f, 2300.f, WINCX * 0.5f - 600.f, WINCY * 0.5f - 620.f);
        Set_New_TransInfo(7000.f, 0.f);
        m_tMoveInfo = { MV_RDOWN, false, 1000.f, 0.f };
        SpawnSlashFx(2);
        break;
    case 3:
        Change_Texture(TEXT("Com_Texture_Katana_Attack3"));
        Set_UISizeAndPos(3000.f, 2300.f, WINCX * 0.5f + 820.f, WINCY * 0.5f - 670.f);
        Set_New_TransInfo(7000.f, 0.f);
        m_tMoveInfo = { MV_LDOWN, false, 1000.f, 0.f };
        SpawnSlashFx(3);
        break;
    }
    _uint iCurScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
    _uint iPlayerUI_Idx = 1; 
    CUIBase* pHandRBase = nullptr;
    if (auto pRoot = CObjectManager::GetInstance()->Find_Object(iCurScene, L"UI_Layer", iPlayerUI_Idx))
        pHandRBase = dynamic_cast<CUIBase*>(pRoot)->Find_Child_ByTag(L"HandRUI");

    if (auto pHandR = dynamic_cast<CPlayer_HandR*>(pHandRBase))
        pHandR->SetKatanaComboStep(step);
}

void CKatana::SpawnSlashFx(int step)
{
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

    CImageUI* pFx = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
    if (!pFx) return;

    const _vec3 base = m_pTransformCom->Get_Info(INFO_POS);

    float fxW = 900.f, fxH = 220.f, offX = 250.f, offY = -30.f;
    const wchar_t* proto = L"";

    switch (step) {
    case 1:
        proto = L"Prototype_Component_Texture_KatanaAttackEffect1";
        fxW = 1700.f; fxH = 400.f; offX = 240.f; offY = 140.f;
        break;
    case 2:
        proto = L"Prototype_Component_Texture_KatanaAttackEffect2";
        fxW = 1700.f; fxH = 700.f; offX = 550.f; offY = -550.f;
        break;
    case 3:
        proto = L"Prototype_Component_Texture_KatanaAttackEffect3";
        fxW = 1400.f; fxH = 700.f; offX = -650.f; offY = -550.f;
        break;
    }

    pFx->Set_UISizeAndPos(fxW, fxH, base.x + offX, base.y + offY);

    pFx->RegisterTexture(L"Com_Texture_Slash",
        proto,
        0, 5, 40.f, false);
    pFx->ChangeTexture(L"Com_Texture_Slash");
    UIMoveInfo mv{};
    mv.eUIMove = MV_NON;
    mv.bStop = true;
    mv.fRange = 0.f;
    mv.fSumRange = 0.f;
    mv.bRenderStop = false;
    pFx->Set_UIMoveInfo(mv);

    Add_Child(pFx);
    m_vSlashFx.push_back(pFx);
}

void CKatana::CleanupFinishedFx()
{
    for (auto it = m_vSlashFx.begin(); it != m_vSlashFx.end(); )
    {
        CImageUI* pFx = *it;
        if (!pFx) { it = m_vSlashFx.erase(it); continue; }

        if (pFx->GetTextureCom()->Is_AnimFinished()) {
            pFx->Set_Active(false);
            pFx->Set_RenderOn(false);
            Safe_Release(pFx);
            it = m_vSlashFx.erase(it);
        }
        else {
            ++it;
        }
    }
}

CKatana* CKatana::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKatana* pInstance = new CKatana(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CSniper_Gun Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CKatana::Clone(void* pArg)
{
	CKatana* pInstance = new CKatana(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSniper_Gun Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CKatana::Free()
{
	__super::Free();
}