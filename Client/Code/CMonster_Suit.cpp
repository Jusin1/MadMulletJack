#include "pch.h"
#include "CMonster_Suit.h"
#include "CColiderManager.h"
#include "CComponentMgr.h"

CMonster_Suit::CMonster_Suit(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CMonster_Suit::CMonster_Suit(const CMonster_Suit& rhs)
	: CMonster(rhs)
{
}

CMonster_Suit::~CMonster_Suit()
{
}

HRESULT CMonster_Suit::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Suit::Initialize(void* pArg)
{
  	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 위치 정보 세팅
	CTransform::TRANSFORMINFO TransformInfo;
	ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));
	TransformInfo.fSpeed = 5.f;
	TransformInfo.fRotationSpeed = D3DXToRadian(90.f);
	TransformInfo.vStartPos = _vec3(4.f, 0.f, 0.f);

	m_pTransformCom->SetTransformInfo(TransformInfo);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(4.f, 1.f, 0.f));
	m_pTransformCom->Set_Scale(0.5f, 1.f, 1.f);

	Change_Texture(TEXT("Com_Texture_Idle"));

	return S_OK;
}

_int CMonster_Suit::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		return DEAD;
	__super::Update_GameObject(fTimeDelta);
	return NO_EVENT;
}

void CMonster_Suit::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Set_Collider();
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CMonster_Suit::Render_GameObject()
{
	__super::Render_GameObject();

#ifdef _DEBUG
	if (g_ColiderRender && m_pColiderCom)
		m_pColiderCom->Render_ColliderSphere();
#endif
}

void CMonster_Suit::Set_Collider()
{
	m_pColiderCom->Update_ColliderSphere();

	if (CColiderManager::GetInstance()->CollisionGroup(
		CColiderManager::COLLISION_PLAYER,
		this,
		CColiderManager::COLLISION_SPHERE,
		nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
	}
}

_bool CMonster_Suit::Picking(_vec3* PickingPoint)
{
	if (true == m_pBufferCom->Picking(m_pTransformCom, PickingPoint))
	{
		Change_Texture(TEXT("Com_Texture_AIM"));
		return true;
	}
	else
		return false;
	return true;
}

HRESULT CMonster_Suit::Texture_Clone()
{
	CTexture::TEXINFO TextureInfo;
	ZeroMemory(&TextureInfo, sizeof(CTexture::TEXINFO));

	// IDLE
	TextureInfo.m_iStart = 0;
	TextureInfo.m_iEndTex = 12;
	TextureInfo.m_fSpeed = 6.f;
	TextureInfo.m_bLoop = true;

	if (FAILED(Add_Components(L"Com_Texture_Idle", SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_Idle",
		(CComponent**)&m_pTextureCom, &TextureInfo)))
		return E_FAIL;
	m_mapTexture.insert({ TEXT("Com_Texture_Idle"), m_pTextureCom });

	// AIM
	//TextureInfo.m_iStart = 0;
	//TextureInfo.m_iEndTex = 9;
	//TextureInfo.m_fSpeed = 6.f;
	//TextureInfo.m_bLoop = true;

	//if (FAILED(Add_Components(L"Com_Texture_AIM", SCENE_STAGE, L"Prototype_Component_Texture_MonsterAim",
	//	(CComponent**)&m_pTextureCom, &TextureInfo)))
	//	return E_FAIL;
	//m_mapTexture.insert({ TEXT("Com_Texture_AIM"), m_pTextureCom });

	return S_OK;
}

CMonster_Suit* CMonster_Suit::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonster_Suit* pInstance = new CMonster_Suit(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CMonster_Suit Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Suit::Clone(void* pArg)
{
	CMonster_Suit* pInstance = new CMonster_Suit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMonster_Suit Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Suit::Free()
{
	__super::Free();
}