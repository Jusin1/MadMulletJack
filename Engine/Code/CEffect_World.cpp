#include "Engine_Define.h"
#include "CRenderer.h"
#include "CVIBuffer_Rect.h"
#include "CTexture.h"
#include "CCullingManager.h"
#include "CEffect_World.h"

CEffect_World::CEffect_World(LPDIRECT3DDEVICE9 pGraphivDevice)
	: CGameObject(pGraphivDevice)
{
}

CEffect_World::CEffect_World(const CEffect_World &rhs)
	: CGameObject(rhs)
{
}

CEffect_World::~CEffect_World()
{
}

void CEffect_World::Free()
{
	CGameObject::Free();
}

CEffect_World *CEffect_World::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_World *pInstance = new CEffect_World(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CEffect_World Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject *CEffect_World::Clone(void *pArg)
{
	CEffect_World *pInstance = new CEffect_World(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CEffect_World Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CEffect_World::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_World::Initialize(void *pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;

	return S_OK;
}

_int CEffect_World::Update_GameObject(const _float &fTimeDelta)
{
	if (m_pTextureCom->Is_AnimFinished())
		return DEAD;

	CGameObject::Update_GameObject(fTimeDelta);
	
	m_pTextureCom->MoveFrame();
	
	return NO_EVENT;
}

void CEffect_World::LateUpdate_GameObject(const _float &fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	Update_Position(m_pTransformCom->Get_Info(INFO_POS));
	Compute_CamDistance(Get_Position());

	SetUp_BillBoard();

	if (CCullingManager::GetInstance()->Is_In_Frustum(Get_Position(), m_fRadius))
		if (m_pRendererCom) m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);
}

void CEffect_World::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	Update_Animation();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	if(m_bTintFlag)
		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_TintColor);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEffect_World::Spawn_Pooling(void *pArg)
{
	if(FAILED(CGameObject::Spawn_Pooling(pArg)))
		return E_FAIL;

	if (EFFECTINFO *pData = reinterpret_cast<EFFECTINFO *>(pArg))
	{
		// Texture
		if (FAILED(Set_TextureInit(pData->eType)))
		{
			MSG_BOX("CEffect_World::Spawn_Pooling, Type is invalid");
			return E_FAIL;
		}
		_uint iWidth;
		_uint iHeight;
		m_pTextureCom->GetFrameSize(0, iWidth, iHeight);
		_float iWidth_Ratio = (_float)iWidth / 512;
		_float iHeight_Ratio = (_float)iHeight / 512;

		// Transform
		m_pTransformCom->Set_Scale(iWidth_Ratio * pData->fSize, iHeight_Ratio * pData->fSize, 1.f);
		m_pTransformCom->RotationDegree(m_pTransformCom->Get_Info(INFO::INFO_LOOK), pData->fAngle);
	}
	else
	{
		MSG_BOX("CEffect_World::Spawn_Pooling, Data is invalid");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_World::Despawn_Pooling()
{
	if (FAILED(CGameObject::Despawn_Pooling()))
		return E_FAIL;

	m_pTransformCom->ClearRotation();
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_bTintFlag = false;

	if (m_pTextureCom)
		m_pTextureCom->Set_Zero_Frame();

	return S_OK;
}

HRESULT CEffect_World::Texture_Clone()
{
	CTexture::TEXINFO info{};
	struct AnimationDeffinition
	{
		const _tchar *tag;
		const _tchar *proto;
		int start;
		int end;
		float speed;
		bool loop;
	};
	AnimationDeffinition anims[] =
	{
		{ L"Electric",	L"Proto_Effect_Electric",			0,	17,	15.f,	false },
		{ L"Explosion",	L"Proto_Effect_Explosion",			0,	16,	10.f,	false },
		{ L"Splatter",	L"Proto_Effect_Splatter",			0,  9,	5.f,	false },
		{ L"Hitted",	L"Proto_Effect_Hitted",				0,	6,	5.f,	false },
		{ L"Spread",    L"Proto_Effect_Spread",				0,  12, 10.f,   false },
		{ L"Smoke",		L"Proto_Effect_Smoke",				0,	22,	15.f,	false },
		{ L"BE",		L"Proto_Effect_BloodExplosion",		0,	22,	15.f,	false },
		{ L"BE2",		L"Proto_Effect_BloodExplosion2",	0,	16,	10.f,	false },
		{ L"BE3",		L"Proto_Effect_BloodExplosion3",	0,	64,	30.f,	false },
		{ L"BE4",		L"Proto_Effect_BloodExplosion4",	0,	16,	10.f,	false },
		{ L"BigExplosion", L"Proto_Effect_BigExplosion",	0,	26, 13.f,	false},
	};

	for (AnimationDeffinition &Element : anims)
	{
		::ZeroMemory(&info, sizeof(info));
		info.m_iStart = Element.start;
		info.m_iEndTex = Element.end;
		info.m_fSpeed = Element.speed;
		info.m_bLoop = Element.loop;

		if (FAILED(Add_Components(
			Element.tag,
			SCENE_STATIC,
			Element.proto,
			(CComponent **)&m_pTextureCom, &info)))
			return E_FAIL;

		m_mapTexture.insert({ Element.tag, m_pTextureCom });
	}


	return S_OK;
}

HRESULT CEffect_World::Set_Component()
{
	if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent **)&m_pBufferCom)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	return S_OK;
}

void CEffect_World::Update_Animation()
{
	if (m_pTextureCom)
	{
		m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
		m_pTextureCom->MoveFrame();
	}
}

void CEffect_World::SetUp_BillBoard()
{
	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3 vRight = *(_vec3 *)&matView.m[0][0];
	_vec3 vUp = *(_vec3 *)&matView.m[1][0];
	_vec3 vLook = *(_vec3 *)&matView.m[2][0];

	m_pTransformCom->Set_Info(INFO_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_Info(INFO_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_Info(INFO_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * m_pTransformCom->Get_Scale().z);
}

HRESULT CEffect_World::Set_TextureInit(WorldEffectType _e)
{
	switch (_e)
	{
	case Engine::WorldEffectType::ELCETRIC:
		return Change_Texture(L"Electric");
	case Engine::WorldEffectType::EXPLOSION:
		return Change_Texture(L"Explosion");
	case Engine::WorldEffectType::BLOOD_SPREAD:
		return Change_Texture(L"Splatter");
	case Engine::WorldEffectType::HITTED:
		return Change_Texture(L"Hitted");
	case Engine::WorldEffectType::FAN_SPREAD:
		return Change_Texture(L"Spread");
	case Engine::WorldEffectType::SMOKE:
		return Change_Texture(L"Smoke");
	case Engine::WorldEffectType::BLOOD_EXPLOSION:
		return Change_Texture(L"BE");
	case Engine::WorldEffectType::BLOOD_EXPLOSION2:
		return Change_Texture(L"BE2");
	case Engine::WorldEffectType::BLOOD_EXPLOSION3:
		return Change_Texture(L"BE3");
	case Engine::WorldEffectType::BLOOD_EXPLOSION4:
		return Change_Texture(L"BE4");
	case Engine::WorldEffectType::BIG_EXPLOSION:
		return Change_Texture(L"BigExplosion");
	}

	return E_FAIL; 
}

HRESULT CEffect_World::Change_Texture(const _tchar *LayerTag)
{
	if (FAILED(CGameObject::Change_Component(LayerTag, (CComponent **)&m_pTextureCom)))
		return E_FAIL;

	if (m_pTextureCom)
		m_pTextureCom->Set_Zero_Frame();

	return S_OK;
}
