#include "pch.h"
#include "CPistol_Gun.h"
#include "CTimerMgr.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CMapFactory.h"
#include "CImageUI.h"
#include "Engine_Function.h"
#include "Sound_Manager.h"

CPistol_Gun::CPistol_Gun(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGun(pGraphicDev)
{
}

CPistol_Gun::CPistol_Gun(const CPistol_Gun& rhs)
	:CGun(rhs)
{
}

CPistol_Gun::~CPistol_Gun()
{
}

HRESULT CPistol_Gun::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_WapPisotl"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPistol_Gun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	m_bActive = false;
	m_bRenderOn = false;
	m_bIsInfinite = false;

	// 파워 / 정확도 / attack cool time
	m_iPower = 3;
	m_iPrecision = 10;
	m_fCoolTime = 0.1f;
	// 최대 불렛
	m_iMaxBullet = 9; //origin : 9 / test 3
	m_iBullet = m_iMaxBullet;

	Set_Texture();

	return S_OK;
}

_int CPistol_Gun::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CPistol_Gun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	// 만약 지금 idle texture 라면
	if (m_CurrentAnimTag == TEXT("Com_Texture_Pistol_Idle"))
	{
		// scene을 받아옴
		_uint iCurScene = CMapFactory::GetInstance()->GetTargetSceneIndex();

		// handR을 받아옴
		CUIBase* pHandR = dynamic_cast<CUIBase*>(CObjectManager::GetInstance()
			->Find_Object(iCurScene, L"UI_Layer", 1))
			->Find_Child_ByTag(L"HandRUI");

		// handR 위치를 기준으로 pos 갱신 (offset 적용)
		if (pHandR)
		{
			Set_UIPos(pHandR->GetTransform()->Get_Info(INFO_POS), -120.f, 350.f);
		}
	}

	// 만약 지금 idle texture가 아니고 ani가 끝났다면
	else if (m_pTextureCom->Is_AnimFinished())
	{
		// state 끝났다고 알려줌
		CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
	}

	DeleteSmoke();
}

void CPistol_Gun::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
	m_pTextureCom->MoveFrame();
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x01);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	CUI::Render_GameObject();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CPistol_Gun::Texture_Clone()
{
	CTexture::TEXINFO texInfo = {};

	// IDLE
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 3;
	texInfo.m_fSpeed = 1.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_Idle", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Idle", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_Idle"), m_pTextureCom });

	// Opening
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 9;
	texInfo.m_fSpeed = 25.f;
	texInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_Op", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Op", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_Op"), m_pTextureCom });

	// Attack
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 6;
	texInfo.m_fSpeed = 20.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_Att", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Attack", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_Att"), m_pTextureCom });

	// Reload
	texInfo.m_iStart = 0;
	texInfo.m_iEndTex = 13;
	texInfo.m_fSpeed = 20.f;
	texInfo.m_bLoop = false;
	if (FAILED(Add_Components(L"Com_Texture_Pistol_Re", SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Re", (CComponent**)&m_pTextureCom, &texInfo)))
		return E_FAIL;
	m_mapTextures.insert({ TEXT("Com_Texture_Pistol_Re"), m_pTextureCom });

	return S_OK;
}

HRESULT CPistol_Gun::Set_Texture() {
	m_bRenderOn = true;

	switch (m_tInfo.ePlayerState)
	{
	case OPENING:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Pistol_Op"))))
			return E_FAIL;
		Set_UISizeAndPos(201.f, 457.f, WINCX * 0.5f + 350.f, WINCY * 0.5f - 50.f );
		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\weapon\pistol\sfx_wp_pistol_intro.wav"
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/weapon/pistol/sfx_wp_pistol_intro.wav", SOUND_WEAPON, 0.5f,false);
		
		break;

	case ATTACK:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Pistol_Att"))))
			return E_FAIL;
		Set_UISizeAndPos(360.f, 720.f, WINCX * 0.5f + 460.f, WINCY * 0.5f + 200.f);

		m_iBullet--;
		
		SpawnEff({ 300.f, 300.f,-300.f, 160.f });
		// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\weapon\pistol\sfx_wp_pistol_shot_04.wav"
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/weapon/pistol/sfx_wp_pistol_shot_04.wav", SOUND_WEAPON, 0.3f, false);
		break;

	case ATTACK_INSTANT:
		m_bRenderOn = false;
		break;

	case RELOAD:
		if (FAILED(Change_Texture(TEXT("Com_Texture_Pistol_Re"))))
			return E_FAIL;
		Set_UISizeAndPos(360.f, 660.f, WINCX * 0.5f + 450.f, WINCY * 0.5f + 150.f);

		Reload_Bullet();
		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\weapon\pistol\sfx_wp_pistol_reload_b_03.wav"
		CSound_Manager::GetInstance()->PlaySoundPitch(L"../Bin/Resource/Sounds/eunbi/weapon/pistol/Gun_Reload_5_4.wav", SOUND_WEAPON, 0.5f,0.8f,false);
		break;

	case PLAYERDEAD:
		m_bActive = false;
		DeleteEff();
		DeleteSmoke();
		break;

	case CLEAR:
		DeleteEff();
		DeleteSmoke();
		break;

	case ATTEND:
		CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
		DeleteEff();
		SpwanSmoke({ 400.f, 400.f, -50.f, 20.f });
		break;

	default:
		DeleteEff();
		if (FAILED(Change_Texture(TEXT("Com_Texture_Pistol_Idle"))))
			return E_FAIL;
		Set_UISizeAndPos(165.f, 500.f, WINCX * 0.5f + 300.f, WINCY * 0.5f + 200.f); // pos를 정하고

		break;
	}

	return S_OK;
}

HRESULT CPistol_Gun::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

void CPistol_Gun::SpawnEff(_vec4 _vSizeOffset)
{
	auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

	CImageUI* pFx = dynamic_cast<CImageUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
	if (!pFx) return;

	const _vec3 base = m_pTransformCom->Get_Info(INFO_POS);

	pFx->Set_UISizeAndPos(_vSizeOffset.x, _vSizeOffset.y, base.x + _vSizeOffset.z, base.y + _vSizeOffset.w);
	pFx->RegisterTexture(L"Com_Texture_PistolEff", L"Prototype_Component_Texture_WapPistol_Eff", 0, 10, 50.f, false);
	pFx->ChangeTexture(L"Com_Texture_PistolEff");

	pFx->Set_ObjTag(L"Eff");

	Add_Child(pFx);
}

void CPistol_Gun::DeleteEff()
{
	CImageUI* pEff = dynamic_cast<CImageUI*> (Find_Child_ByTag(TEXT("Eff")));
	if (!pEff)
		return;

	pEff->Set_Dead(true);
	Remove_Child(pEff);
}

void CPistol_Gun::SpwanSmoke(_vec4 _vSizeOffset)
{
	auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

	CImageUI* pFx = dynamic_cast<CImageUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
	if (!pFx) return;

	const _vec3 base = m_pTransformCom->Get_Info(INFO_POS);

	pFx->Set_UISizeAndPos(_vSizeOffset.x, _vSizeOffset.y, base.x + _vSizeOffset.z, base.y + _vSizeOffset.w);
	pFx->RegisterTexture(L"Com_Texture_PistolSmoke", L"Prototype_Component_Texture_WapPistol_EffSmoke", 0, 15, 30.f, false);
	pFx->ChangeTexture(L"Com_Texture_PistolSmoke");

	pFx->Set_ObjTag(L"Eff_Smoke");

	Add_Child(pFx);
}

void CPistol_Gun::DeleteSmoke()
{
	CImageUI* pEff = dynamic_cast<CImageUI*> (Find_Child_ByTag(TEXT("Eff_Smoke")));

	if (pEff && pEff->GetTextureCom()->Is_AnimFinished())
	{
		pEff->Set_Dead(true);
		Remove_Child(pEff);
	}
}

CPistol_Gun* CPistol_Gun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPistol_Gun* pInstance = new CPistol_Gun(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CPistol_Gun Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPistol_Gun::Clone(void* pArg)
{
	CPistol_Gun* pInstance = new CPistol_Gun(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CPistol_Gun Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPistol_Gun::Free()
{
	__super::Free();
}