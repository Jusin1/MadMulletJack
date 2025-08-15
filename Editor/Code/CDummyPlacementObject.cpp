#include "CVIBuffer_Cube_Color.h"
#include "Engine_Define.h"
#include "CDInputMgr.h"
#include "CGuiManager.h"
#include "CRenderer.h"
#include "CGuiManager.h"
#include "CGridPanel.h"
#include "CEditorPickingManager.h"
#include "CVIBuffer_GridPanel.h"
#include "Editor_Define.h"
#include "CObjectManager.h"
#include "CDummyPlacementObject.h"

CDummyPlacementObject::CDummyPlacementObject(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CGameObject(pGraphicDevice), m_pBuffer(nullptr)
{
}

CDummyPlacementObject::CDummyPlacementObject(const CDummyPlacementObject &rhs)
	: CGameObject(rhs)
{
}

CDummyPlacementObject::~CDummyPlacementObject()
{
}

void CDummyPlacementObject::Free()
{
	CGameObject::Free();
}

CDummyPlacementObject *CDummyPlacementObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDummyPlacementObject *pNew = new CDummyPlacementObject(pGraphicDev);

	if (FAILED(pNew->Ready_GameObject()))
	{
		Safe_Release(pNew);
		MSG_BOX("DummyPlacementObject Create Failed");
		return nullptr;
	}

	return pNew;
}

CGameObject *CDummyPlacementObject::Clone(void *pArg)
{
	CDummyPlacementObject *pInstance = new CDummyPlacementObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("DummyPlacementObject Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CDummyPlacementObject::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyPlacementObject::Initialize(void *pArg)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CDummyPlacementObject::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return DEAD;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	PosUpdate();
	
	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

	return NO_EVENT;
}

void CDummyPlacementObject::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return;

	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDummyPlacementObject::Render_GameObject()
{
	if (m_bDead)
		return;

	m_pGraphicDev->SetTexture(0, nullptr);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

_bool CDummyPlacementObject::Picking(_vec3 *PickingPoint)
{
	if (!PickingPoint || !GetBuffer())
		return FALSE;

	if (m_pBuffer->Picking(m_pTransformCom, PickingPoint))
	{
		return TRUE;
	}

	return FALSE;
}

void CDummyPlacementObject::PickingTrue()
{
}

void CDummyPlacementObject::ExportData(void *pData)
{
}

void CDummyPlacementObject::MakeObject(PlacementObjectData *pData)
{
	switch (pData->eCategory)
	{
		case MapEditorObjectCategory::WALL:
		{
			MSG_BOX("CDummyPlacementObject::MakeColor, wrong type");
		} return;
		case MapEditorObjectCategory::TILE:
		{
			MSG_BOX("CDummyPlacementObject::MakeColor, wrong type");
		} return;
		case MapEditorObjectCategory::ENV_OBJ:
		{
			MakeEnvObject(static_cast<MapEditorEnvObjectType>(pData->iType), pData);
		} break;
		case MapEditorObjectCategory::MONSTER:
		{
			MakeMonsterObject(static_cast<MapEditorMonsterType>(pData->iType), pData);
		} break;
		case MapEditorObjectCategory::LIGHT:
		{
			MSG_BOX("CDummyPlacementObject::MakeColor, todo");
		}
		break;
	}
}

void CDummyPlacementObject::MakeMonsterObject(MapEditorMonsterType _e, PlacementObjectData *pData)
{
	switch (_e)
	{
	case MapEditorMonsterType::SUIT:
	{
		pData->dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	} break;
	default:
		MSG_BOX("CDummyPlacementObject::MonsterColor, wrong type");
		break;
	}

	if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPlacementObject", SCENE_EDITOR, L"Monster_Layer", pData)))
	{
		MSG_BOX("NOOOOOOOOOOOOOOOOOOOOOO");
	}
}

void CDummyPlacementObject::MakeEnvObject(MapEditorEnvObjectType _e, PlacementObjectData *pData)
{
	switch (_e)
	{
	case MapEditorEnvObjectType::BOTTLE:
	{
		pData->dwColor = D3DXCOLOR(0.65f, 0.33f, 0.0f, 1.0f);
	} break;
	case MapEditorEnvObjectType::VENDINGMACHINE:
	{
		pData->dwColor = D3DXCOLOR(0.0f, 1.f, 0.0f, 1.0f);
	} break;
	default:
		MSG_BOX("CDummyPlacementObject::EnvColor, wrong type");
		break;
	}

	if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPlacementObject", SCENE_EDITOR, L"Env_Layer", pData)))
	{
		MSG_BOX("NOOOOOOOOOOOOOOOOOOOOOO");
	}
}

HRESULT CDummyPlacementObject::Set_Component(void *pArg)
{
	// VIBuffer Default
	D3DXCOLOR srcColor = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.7f);
	if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Component_Buffer_CubeColor", (CComponent **)&m_pBuffer, &srcColor)))
		return E_FAIL;

	GetTransform()->Set_Scale(0.2f, 0.2f, 0.2f);

	return S_OK;
}

void CDummyPlacementObject::PosUpdate()
{
	if (CGameObject *pGo = CEditorPickingManager::GetInstance()->GetPickedObject_ForDummy())
	{
		if (CGridPanel *pGridPanel = dynamic_cast<CGridPanel *>(pGo))
		{
			if (CVIBuffer_GridPanel *pBuffer = static_cast<CVIBuffer_GridPanel *>(pGridPanel->GetBuffer()))
			{
				CTransform *pTransform = GetTransform();
				_vec3 pickPos = CEditorPickingManager::GetInstance()->Get_DummyPickingPos();
				
				if (CGuiManager::GetInstance()->IsSnap())
				{
					switch (pBuffer->Get_Data()->eType)
					{
					case PanelType::WALL_HOR:
					{
						pickPos.x = (int)pickPos.x + 0.5f;
						pickPos.y = (int)pickPos.y + 0.5f;
						pickPos.z -= (pTransform->Get_Scale().z + 0.001f);
					} break;
					case PanelType::WALL_VER:
					{
						pTransform->SetDegreeForEditor(_vec3{ 0.f,1.f,0.f }, 90.f);
						pickPos.x += (pTransform->Get_Scale().x + 0.001f);
						pickPos.y = (int)pickPos.y + 0.5f;
						pickPos.z = (int)pickPos.z + 0.5f;
					} break;
					case PanelType::INCLINE:
					{

					} break;
					case PanelType::FLOOR:
					{
						pTransform->SetDegreeForEditor(_vec3{ 1.f,0.f,0.f }, 90.f);
						pickPos.x = (int)pickPos.x + 0.5f;
						pickPos.y += (pTransform->Get_Scale().y + 0.001f);
						pickPos.z = (int)pickPos.z + 0.5f;
					} break;
					case PanelType::CEILING:
					{
						pTransform->SetDegreeForEditor(_vec3{ 1.f,0.f,0.f }, -90.f);
						pickPos.x = (int)pickPos.x + 0.5f;
						pickPos.y -= (pTransform->Get_Scale().y + 0.001f);
						pickPos.z = (int)pickPos.z + 0.5f;
					} break;
					}
				}
				else
				{
					switch (pBuffer->Get_Data()->eType)
					{
					case PanelType::WALL_HOR:
					{
						pickPos.z -= (pTransform->Get_Scale().z + 0.001f);
					} break;
					case PanelType::WALL_VER:
					{
						pTransform->SetDegreeForEditor(_vec3{ 0.f,1.f,0.f }, 90.f);
						pickPos.x += (pTransform->Get_Scale().x + 0.001f);
					} break;
					case PanelType::INCLINE:
					{

					} break;
					case PanelType::FLOOR:
					{
						pTransform->SetDegreeForEditor(_vec3{ 1.f,0.f,0.f }, 90.f);
						pickPos.y += (pTransform->Get_Scale().y + 0.001f);
					} break;
					case PanelType::CEILING:
					{
						pTransform->SetDegreeForEditor(_vec3{ 1.f,0.f,0.f }, -90.f);
						pickPos.y -= (pTransform->Get_Scale().y + 0.001f);
					} break;
					}
				}
				

				pTransform->Set_Info(INFO::INFO_POS, pickPos);

				if (IS_LBUTTON_DOWN)
				{
					PlacementObjectData tTestData;
					_vec3 right = pTransform->Get_Info(INFO::INFO_RIGHT);
					_vec3 up = pTransform->Get_Info(INFO::INFO_UP);
					_vec3 look = pTransform->Get_Info(INFO::INFO_LOOK);
					::memcpy(&tTestData.transform.Right, &right, sizeof(_vec3));
					::memcpy(&tTestData.transform.Up, &up, sizeof(_vec3));
					::memcpy(&tTestData.transform.Look, &look, sizeof(_vec3));
					::memcpy(&tTestData.transform.Pos, &pickPos, sizeof(_vec3));
					tTestData.eCategory = CGuiManager::GetInstance()->GetCategory();
					tTestData.iType = CGuiManager::GetInstance()->GetObjectType();
					MakeObject(&tTestData);
				}
			}
		}
	}
}
