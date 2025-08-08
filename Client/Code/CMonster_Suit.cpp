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
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Suit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;

	//Test
	Change_Texture(TEXT("Com_Texture_Idle"));

	m_pTransformCom->Set_Info(INFO_POS, _vec3(4.f, 1.f, 0.f));
	m_pTransformCom->Set_Scale(0.5f, 1.f, 1.f);
	return S_OK;
}

_int CMonster_Suit::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		return DEAD;
	return NO_EVENT;
}

void CMonster_Suit::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Set_Collider();
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMonster_Suit::Render_GameObject()
{
	__super::Render_GameObject();
#ifdef _DEBUG
	if (g_ColiderRender && m_pColiderHead != nullptr)
		m_pColiderHead->Render_ColliderSphere();
#endif
}


HRESULT CMonster_Suit::Set_Component(void* pArg)
{
	CColider_Sphere::COLLINFO CollSphereInfo;
	ZeroMemory(&CollSphereInfo, sizeof(CColider_Sphere::COLLINFO));
	CollSphereInfo.fRadius = 1.f;                    // 반지름 1
	CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);    // 중심 오프셋 없음
	// Colider_Sphere
	if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere", (CComponent**)&m_pColiderHead, &CollSphereInfo)))
		return E_FAIL;
	m_pColiderHead->Set_Transform(m_pTransformCom); // set tranform을 해줘야함 <- 그 전에 transform 생성 해줘야 하고
}

void CMonster_Suit::Set_Collider(void)
{
	m_pColiderHead->Update_ColliderSphere();
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_PLAYER, this, CColiderManager::COLLISION_SPHERE, nullptr))
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
	CTexture::TEXINFO		TextureInfo;
	ZeroMemory(&TextureInfo, sizeof(CTexture::TEXINFO));

	// IDLE
	TextureInfo.m_iStart = 0;
	TextureInfo.m_iEndTex = 12;
	TextureInfo.m_fSpeed = 6;
	TextureInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Idle", SCENE_STAGE, L"Prototype_Component_Texture_MonsterIdle", (CComponent**)&m_pTextureCom, &TextureInfo)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Idle"), m_pTextureCom));

	// AIM
	TextureInfo.m_iStart = 0;
	TextureInfo.m_iEndTex = 9;
	TextureInfo.m_fSpeed = 6;
	TextureInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_AIM", SCENE_STAGE, L"Prototype_Component_Texture_MonsterAim", (CComponent**)&m_pTextureCom, &TextureInfo)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_AIM"), m_pTextureCom));

	return S_OK;
}


CMonster_Suit* CMonster_Suit::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonster_Suit* pInstance = new CMonster_Suit(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("pMonster Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Suit::Clone(void* pArg)
{
	CMonster_Suit* pInstance = new CMonster_Suit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("pMonster Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Suit::Free()
{
	Engine::CGameObject::Free();
}