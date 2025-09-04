#include "pch.h"
#include "CMini_Gun.h"
#include "CPlayer.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CMapFactory.h"
#include "CImageUI.h"
#include "Engine_Function.h"
#include "CDInputMgr.h"
#include "CObjectPoolManager.h"

CMini_Gun::CMini_Gun(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGun(pGraphicDev), m_fScale(0.f), m_fEffCoolTime(0.f), m_fEffRenderTime(0.f)
{
}

CMini_Gun::CMini_Gun(const CMini_Gun& rhs)
	:CGun(rhs), m_fScale(rhs.m_fScale), m_fEffCoolTime(rhs.m_fEffCoolTime), m_fEffRenderTime(rhs.m_fEffRenderTime)
{
}

CMini_Gun::~CMini_Gun()
{
}

HRESULT CMini_Gun::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMini_Gun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	m_bActive = false;
	m_bRenderOn = false;
	m_bIsInfinite = true;

	// 파워 / 정확도 / attack cool time
	m_iPower = 20;
	m_iPrecision = 5;
	m_fCoolTime = 0.f;

	// 최대 불렛
	//m_iMaxBullet = 6; //origin : 6 / test 3
	//m_iBullet = m_iMaxBullet;

	Set_Texture();

	m_fScale = 1.f;

	return S_OK;
}

_int CMini_Gun::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CMini_Gun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	if ((m_tInfo.ePlayerState == ATTACK ||
		m_tInfo.ePlayerState == ATTACK_ZOOM )&&
		IS_LBUTTON_UP)
	{
		CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
	}

	// attack일때
// effect 회전
	CUI* pEff = nullptr;

	switch (m_tInfo.ePlayerState)
	{

	case ATTACK:
	case ATTACK_ZOOM:
		pEff = dynamic_cast<CUI*>(Find_Child_ByTag(TEXT("Eff")));

		if (pEff)
		{
			// render time이 0이 아니라면 보여줘라
			if (m_fEffRenderTime != 0)
			{
				pEff->Set_RenderOn(true);
				m_fEffRenderTime -= fTimeDelta;

				if (m_fEffRenderTime <= 0)
				{
					m_fEffRenderTime = 0.f;
					m_fEffCoolTime = 0.05f;


					// bullet 발사
					BulletData tData;
					Engine::CTransform* pPlayerTransformCom = nullptr; _vec3 vPlayerLook; _vec3 vPlayerPos;
					// player의 look,pos  벡터 가져옴
					CPlayer* pPlayer = static_cast<CPlayer *>(CObjectManager::GetInstance()->Get_ObjectList(CManagement::GetInstance()->Get_CurrentSceneIdx(), L"Player_Layer")->front());
					if (!pPlayer)
						return NO_EVENT;
					pPlayerTransformCom = pPlayer->GetTransform();
					if (pPlayerTransformCom == nullptr)
						return;

					vPlayerLook = pPlayerTransformCom->Get_Info(INFO_LOOK);
					D3DXVec3Normalize(&vPlayerLook, &vPlayerLook); // 정규화

					vPlayerPos = pPlayerTransformCom->Get_Info(INFO_POS);
					tData.fSpeed = pPlayer->Get_NormalSpeed() * 2.f;
					tData.vMuzzlePosition = vPlayerPos + vPlayerLook * 1.2f;

					tData.vLookDir = vPlayerLook;
					tData.eOwner = BulletData::OWNER::PLAYER;

					//tData.vLookDir = { 0.f,0.f,1.f }; // test : z 방향

					CObjectPoolManager::GetInstance()->Spawn(PoolType::BULLET, &tData);
				}

			}
			// render time이 0이 되면 cooltime 이 0이 될때까지 render 를 멈춰라
			else if (m_fEffCoolTime != 0)
			{
				pEff->Set_RenderOn(false);
				m_fEffCoolTime -= fTimeDelta;

				if (m_fEffCoolTime <= 0)
				{
					m_fEffCoolTime = 0.f;
					m_fEffRenderTime = 0.05f;
				}
			}
		}
		break;

	case ZOOMING:
		// size를 점점 키우기
		m_fScale += fTimeDelta * 0.5;

		if (m_fScale >= 1.1f)
		{
			CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
			GetTransform()->Set_Scale(1.1f, 1.1f, 0.f);
		}
		else
			//GetTransform()->Set_Scale( m_fScale,m_fScale,0.f );

			break;

	case ZOOM:
		// 키운 사이즈 유지
		//GetTransform()->Set_Scale(1.5f, 1.5f, 1.f);
		break;

	case OPENING:
		Move_UI(fTimeDelta); // 위로 움직이기 위해
		break;

	case ZOOMOUT:
		// size 줄이기 -> 실패
		m_fScale -= fTimeDelta * 0.5;

		if (m_fScale <= 1.f)
		{
			CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
			GetTransform()->Set_Scale(1.f, 1.f, 0.f);
		}
		else
			//GetTransform()->Set_Scale(m_fScale, m_fScale, 0.f);

			break;
	}

	return;
}

void CMini_Gun::Render_GameObject()
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

HRESULT CMini_Gun::Texture_Clone()
{
	CTexture::TEXINFO info{};
	struct AnimDef { const wchar_t* tag; const wchar_t* proto; int start; int end; float speed; bool loop; };
	AnimDef anims[] = {
		// normal
		{ L"Com_Texture_MiniG_Idle",			L"Prototype_Component_Texture_WapMiniG_Idle",		0, 2, 10.f,  true },
		{ L"Com_Texture_MiniG_Att",				L"Prototype_Component_Texture_WapMiniG_Att",			0, 7,30.f,  true },
		{ L"Com_Texture_MiniG_Zooming",				L"Prototype_Component_Texture_WapMiniG_Zooming",			0, 4,10.f,  true }
	};

	for (auto& a : anims)
	{
		ZeroMemory(&info, sizeof(info));
		info.m_iStart = a.start;
		info.m_iEndTex = a.end;
		info.m_fSpeed = a.speed;
		info.m_bLoop = a.loop;

		if (FAILED(Add_Components(a.tag, SCENE_STATIC, a.proto, (CComponent**)&m_pTextureCom, &info)))
			return E_FAIL;

		m_mapTextures.insert({ a.tag, m_pTextureCom });
	}

	return S_OK;
}

HRESULT CMini_Gun::Set_Texture() {

	m_bRenderOn = true;

	DeleteEff();
	m_tMoveInfo.eUIMove = MV_NON; // 기본으로 안 움직이게

	Set_UISizeAndPos(1093.f, 614.f, WINCX * 0.5f, WINCY * 0.5f + 200.f); //1366 768

	switch (m_tInfo.ePlayerState)
	{
	case IDLE:
		if (FAILED(Change_Texture(TEXT("Com_Texture_MiniG_Idle"))))
			return E_FAIL;
		break;

	case ATTACK:
		if (FAILED(Change_Texture(TEXT("Com_Texture_MiniG_Att"))))
			return E_FAIL;
		m_fEffRenderTime = 0.05f;
		m_fEffCoolTime = 0.f;
		SpawnEff({ 400.f,150.f,0.f,300.f });

		break;

	case ZOOMING:
		if (FAILED(Change_Texture(TEXT("Com_Texture_MiniG_Zooming"))))
			return E_FAIL;
		break;
	case ZOOM:
		if (FAILED(Change_Texture(TEXT("Com_Texture_MiniG_Idle"))))
			return E_FAIL;
		Set_UISizeAndPos(1093.f * 1.1f, 614.f * 1.1f, WINCX * 0.5f, WINCY * 0.5f + 200.f);
		// 유지
		break;

	case OPENING:
		if (FAILED(Change_Texture(TEXT("Com_Texture_MiniG_Idle"))))
			return E_FAIL;
		Set_UISizeAndPos(1093.f, 614.f, WINCX * 0.5f, WINCY * 0.5f + 300.f); //1366 768
		
		Set_New_TransInfo(100.f,0.f);
		m_tMoveInfo = { MV_UP, false, 0.f,0.f };
		break;

	case PLAYERDEAD:
		m_bRenderOn = false;
		break;

	case ATTEND:
		CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
		break;

	case ATTACK_ZOOM:
		if (FAILED(Change_Texture(TEXT("Com_Texture_MiniG_Att"))))
			return E_FAIL;
		m_fEffRenderTime = 0.05f;
		m_fEffCoolTime = 0.f;
		Set_UISizeAndPos(1093.f * 1.1f, 614.f * 1.1f, WINCX * 0.5f, WINCY * 0.5f + 200.f);

		SpawnEff({ 440.f,165.f,0.f,320.f });
		break;

	case ZOOMOUT:
		if (FAILED(Change_Texture(TEXT("Com_Texture_MiniG_Zooming"))))
			return E_FAIL;
		break;
	}
	

	return S_OK;
}

HRESULT CMini_Gun::Change_Texture(const _tchar* pTextureTag)
{
	if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	//m_pTextureCom->Set_Zero_Frame();
	m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
	return S_OK;
}

void CMini_Gun::SpawnEff(_vec4 _vSizeOffset)
{
	auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

	CImageUI* pFx = dynamic_cast<CImageUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
	if (!pFx) return;

	const _vec3 base = m_pTransformCom->Get_Info(INFO_POS);

	pFx->Set_UISizeAndPos(_vSizeOffset.x, _vSizeOffset.y, base.x + _vSizeOffset.z, base.y + _vSizeOffset.w);
	pFx->RegisterTexture(L"Com_Texture_MiniGEff", L"Prototype_Component_Texture_WapMiniG_Eff", 0, 0.f, 0.f, false);
	pFx->ChangeTexture(L"Com_Texture_MiniGEff");

	pFx->Set_New_TransInfo(10.f, 89.f); // 회전하기 위해 속도 부여
	pFx->Set_UIMoveInfo({MV_ROTATIONZ, false, 0.f,0.f}); // move info 설정

	pFx->Set_ObjTag(L"Eff");

	Add_Child(pFx);
}

void CMini_Gun::DeleteEff()
{
	CImageUI* pEff = dynamic_cast<CImageUI*> (Find_Child_ByTag(TEXT("Eff")));
	if (!pEff)
		return;

	pEff->Set_Dead(true);
	Remove_Child(pEff);
}

CMini_Gun* CMini_Gun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMini_Gun* pInstance = new CMini_Gun(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CMini_Gun Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMini_Gun::Clone(void* pArg)
{
	CMini_Gun* pInstance = new CMini_Gun(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMini_Gun Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMini_Gun::Free()
{
	__super::Free();
}