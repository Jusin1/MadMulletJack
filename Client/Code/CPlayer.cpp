#include "pch.h"
#include "CPlayer.h"
#include "CRenderer.h"
#include "CColiderManager.h"
#include "CTimerMgr.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CPlayer::CPlayer(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;

	Set_CollisionMatrix();       

	m_vPosition = { 2.f, 1.f, 1.f };
	m_pTransformCom->Set_Info(INFO_POS, m_vPosition); 
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_PLAYER, this);
	m_pRenderCom->Add_RenderGroup(RENDER_ALPHA, this);
	return S_OK;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{

	Key_Input(fTimeDelta);
	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	Set_Collider();
	
	if (nullptr != m_pRenderCom)
		m_pRenderCom->Add_RenderGroup(RENDER_NONALPHA, this);


	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();

	m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
	m_pTextureCom->MoveFrame(m_TimerTag);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

#ifdef _DEBUG
	if (g_ColiderRender && m_pColliderCom != nullptr)
	{
		m_pColliderCom->Render_ColliderBox(); // 충돌체 디버그 렌더
	}
	//if (g_ColiderRender && m_pColiderSphere != nullptr)
	//{
	//	m_pColiderSphere->Render_ColliderSphere(); // 충돌체 디버그 렌더
	//}
#endif
}

HRESULT CPlayer::Set_Component()
{
	if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_Player"))))
		return E_FAIL;

	m_TimerTag = TEXT("Timer_Player");

	// Render
	if(FAILED(Add_Components(L"Com_Renderer", SCENE_STATIC, L"Proto_Renderer", (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	CColider_Cube::COLLRECTDESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CColider_Cube::COLLRECTDESC));
	CollCubeDesc.fRadiusY = 1.f;
	CollCubeDesc.fRadiusX = 1.f;
	CollCubeDesc.fRadiusZ = 1.f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	// Colider
	if (FAILED(Add_Components(L"Com_Collider_Cube", SCENE_STATIC, L"Proto_Colider_Cube", (CComponent**)&m_pColliderCom, &CollCubeDesc)))
		return E_FAIL;


	//CColider_Sphere::COLLINFO CollSphereInfo;
	//ZeroMemory(&CollSphereInfo, sizeof(CColider_Sphere::COLLINFO));
	//CollSphereInfo.fRadius = 1.f;                    // 반지름 1
	//CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);    // 중심 오프셋 없음
	//// Colider_Sphere
	//if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere", (CComponent**)&m_pColliderCom, &CollSphereInfo)))
	//	return E_FAIL;

	// VIBuffer
	if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// Texture
	if (Texture_Clone())
		return E_FAIL;

	CTransform::TRANSFORMINFO TransformInfo;
	ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));

	TransformInfo.fSpeed = 5.f;
	TransformInfo.fRotationSpeed = D3DXToRadian(90.f);
	TransformInfo.vStartPos = _vec3(0.f, 0.f, 0.f);

	if (FAILED(Add_Components(L"Com_Transform", SCENE_STATIC, L"Proto_Transform", (CComponent**)&m_pTransformCom, &TransformInfo)))
		return E_FAIL;
	
	return S_OK;
}

void CPlayer::Set_Collider(void)
{
	Set_CollisionMatrix();
   
	_vec3 test =m_pTransformCom->Get_Info(INFO_POS);
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_MONSTER, this, CColiderManager::COLLISION_CUBE, nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
	}
	//if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_MONSTER, this, CColiderManager::COLLISION_SPHERE, nullptr))
	//{
	//	_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
	//}

}



void CPlayer::Key_Input(const _float& fTimeDelta)
{
	if (GetAsyncKeyState(VK_UP))
	{
		m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		m_pTransformCom->Move_Backward(fTimeDelta, m_vPosition.y);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		m_pTransformCom->Move_Left(fTimeDelta, m_vPosition.y);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		m_pTransformCom->Move_Right(fTimeDelta, m_vPosition.y);
	}
	//if (GetAsyncKeyState('Q'))
	//{
	//	m_pTransformCom->Rotation(_vec3(0.f, 1.f, 0.f), fTimeDelta);
	//}
	//if (GetAsyncKeyState('E'))
	//{
	//	m_pTransformCom->Rotation(_vec3(0.f, -1.f, 0.f), fTimeDelta);
	//}
}

HRESULT CPlayer::Texture_Clone()
{
	CTexture::TEXINFO		TextureInfo;
	ZeroMemory(&TextureInfo, sizeof(CTexture::TEXINFO));

	TextureInfo.m_iStart = 0;
	TextureInfo.m_iEndTex = 5;
	TextureInfo.m_fSpeed = 2;

	if (FAILED(Add_Components(L"Com_Texture_Test", SCENE_STAGE, L"Prototype_Component_Texture_PlayerTest", (CComponent**)&m_pTextureCom, &TextureInfo)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

HRESULT CPlayer::Change_Texture(const _tchar* LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();

	return S_OK;
}

void CPlayer:: Set_CollisionMatrix()
{
	D3DXMatrixIdentity(&m_CollisionMatrix);
	memcpy(*(_vec3*)&m_CollisionMatrix.m[3][0], m_pTransformCom->Get_Info(INFO_POS), sizeof(_vec3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	//m_pColiderSphere->Update_ColliderSphere(m_CollisionMatrix);
}

_vec3 CPlayer::Get_Pos()
{
	return (m_pTransformCom->Get_Info(INFO_POS));
}

_vec3 CPlayer::Get_Look()
{
	return (m_pTransformCom->Get_Info(INFO_LOOK));
}

_vec3 CPlayer::Get_Right()
{
	return (m_pTransformCom->Get_Info(INFO_RIGHT));
}


CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("pPlayer Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	Engine::CGameObject::Free();
}
