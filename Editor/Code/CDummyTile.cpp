#include "CVIBuffer_Rect.h"
#include "CRenderer.h"
#include "CEditorPickingManager.h"
#include "Editor_Define.h"
#include "CGridPanel.h"
#include "CPicking.h"
#include "CVIBuffer_GridPanel.h"
#include "CObjectManager.h"
#include "CTexture.h"
#include "CDummyTile.h"

CDummyTile::CDummyTile(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CGameObject(pGraphicDevice)
{
}

CDummyTile::CDummyTile(const CDummyTile &rhs)
	: CGameObject(rhs)
{
}

CDummyTile::~CDummyTile()
{
}

void CDummyTile::Free()
{
	CGameObject::Free();
	CEditorPickingManager::GetInstance()->Remove_PickingGroup(this);
}

CDummyTile *CDummyTile::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDummyTile *pNew = new CDummyTile(pGraphicDev);

	if (FAILED(pNew->Ready_GameObject()))
	{
		Safe_Release(pNew);
		MSG_BOX("DummyTile Create Failed");
		return nullptr;
	}

	return pNew;
}

CGameObject *CDummyTile::Clone(void *pArg)
{
	CDummyTile *pInstance = new CDummyTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("DummyTile Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CDummyTile::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyTile::Initialize(void *pArg)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CDummyTile::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return DEAD;

	CEditorPickingManager::GetInstance()->Remove_PickingGroup(this);
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	PosUpdate();
	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

	return NO_EVENT;
}

void CDummyTile::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return;

	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	CEditorPickingManager::GetInstance()->Add_PickingGroup(this);
	Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDummyTile::Render_GameObject()
{
	if (m_bDead)
		return;

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	m_pTexture->Set_Texture();
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDummyTile::PosUpdate()
{
	if (CGameObject *pGo = CEditorPickingManager::GetInstance()->GetPickedObject_ForDummy())
	{
		if (CGridPanel *pGridPanel = dynamic_cast<CGridPanel *>(pGo))
		{
			if (CVIBuffer_GridPanel *pBuffer = static_cast<CVIBuffer_GridPanel *>(pGridPanel->GetBuffer()))
			{
				CTransform *pTransform = GetTransform();
				_vec3 pickPos = CEditorPickingManager::GetInstance()->Get_DummyPickingPos();
				pTransform->Set_Info(INFO::INFO_POS, pickPos);

				switch (pBuffer->Get_Data()->eType)
				{
					case PanelType::WALL_HOR:
					{
						
					} break;
					case PanelType::WALL_VER:
					{

					} break;
					case PanelType::INCLINE:
					{

					} break;
					case PanelType::FLOOR:
					{

					} break;
					case PanelType::CEILING:
					{

					} break;
				}

				if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000))
				{
					MAPOBJECTDATA tTestData;
					::memcpy(&tTestData.transform.Pos, &pickPos, sizeof(_vec3));
					tTestData.texture.OriginComponentName = L"Proto_GridTrigger";
					if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultTile", SCENE_EDITOR, L"Tile Layer", &tTestData)))
					{
						MSG_BOX("NOOOOOOOOOOOOOOOOOOOOOO");
					}
				}					
			}
		}
	}
}

_bool CDummyTile::Picking(_vec3 *PickingPoint)
{
	return FALSE;
}

void CDummyTile::PickingTrue()
{
	
}

HRESULT CDummyTile::Set_Component(void *pArg)
{
	// VIBuffer Default
	if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Component_Buffer_TileDefault", (CComponent **)&m_pBuffer)))
		return E_FAIL;

	// Texture Default
	if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_GridCollider", (CComponent **)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}
