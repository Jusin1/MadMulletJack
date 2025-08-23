#include "Engine_Define.h"
#include "CTransform.h"
#include "CGuiManager.h"
#include "CObjectManager.h"
#include "CMapFactory.h"
#include "CPrefab.h"

CPrefab::CPrefab(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_bClone(false), m_eCategory(ObjectCategory::PREFAB), m_eType(PrefabType::NONE)
{
	m_pChildrens.reserve(20);
}

CPrefab::CPrefab(CPrefab &rhs)
	: CGameObject(rhs), m_bClone(true), m_eCategory(ObjectCategory::PREFAB), m_eType(PrefabType::NONE)
{
	m_pChildrens.reserve(20);
}

CPrefab::~CPrefab()
{
}

void CPrefab::Free()
{
	CGameObject::Free();
}

CPrefab *CPrefab::Create(LPDIRECT3DDEVICE9 pGraphicDev, void *pArg)
{
	CPrefab *pPrefab = new CPrefab(pGraphicDev);

	if (FAILED(pPrefab->Initialize(pArg)))
	{
		Safe_Release(pPrefab);
		MSG_BOX("CPrefab::Create, Failed");
		return nullptr;
	}

	return pPrefab;
}

CGameObject *CPrefab::Clone(void *pArg)
{
	CPrefab *pInstance = new CPrefab(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CPrefab::Clone, Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

HRESULT CPrefab::Ready_GameObject()
{
	return __super::Ready_GameObject();
}

HRESULT CPrefab::Initialize(void *pArg)
{
	if (FAILED(Set_Component()))
		return E_FAIL;

	// 복사본 생성시 WorldMatrix 세팅
	if (m_bClone)
	{
		// 데이터가 있다면 ?
		if (PREFABDATA *pData = reinterpret_cast<PREFABDATA *>(pArg))
		{
			if (FAILED(Set_Data(pData)))
				return E_FAIL;
		}
		// 복사 했는데 데이터가 없다? Editor 니까 그냥 생성 ( 빈껍데기 Prefab 초기 상태 )
		else
		{

		}
	}
	// 최초 생성시 Children Local만 세팅
	else
	{
		// 데이터가 있다면 ?
		if (PREFABDATA *pData = reinterpret_cast<PREFABDATA *>(pArg))
		{
			if (FAILED(Set_Data(pData)))
				return E_FAIL;
		}
		// 빈껍데기 Prefab Prototype
		else
		{

		}
	}

	return S_OK;
}

_int CPrefab::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead) return DEAD;

	return NO_EVENT;
}

void CPrefab::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead) return;
}

void CPrefab::Render_GameObject()
{
	if (m_bDead) return;
}

_bool CPrefab::Picking(_vec3 *PickingPoint)
{
	return TRUE;
}

void CPrefab::PickingTrue()
{
}

void CPrefab::ExportData(void *pData)
{
	if (PREFABDATA *p = reinterpret_cast<PREFABDATA *>(pData))
	{
		if (m_eType < PrefabType::SIGN_PILLAR || m_eType >= PrefabType::NONE)
		{
			MSG_BOX("CPrefab::ExportData, Prefab type was invalid");
			return;
		}
		
		// type
		p->eType = m_eType;

		// transform
		::memcpy(&p->ParentTransform.Right, &((*m_pTransformCom->Get_World()).m[0][0]), sizeof(_vec3));
		::memcpy(&p->ParentTransform.Up, &((*m_pTransformCom->Get_World()).m[1][0]), sizeof(_vec3));
		::memcpy(&p->ParentTransform.Look, &((*m_pTransformCom->Get_World()).m[2][0]), sizeof(_vec3));
		::memcpy(&p->ParentTransform.Pos, &((*m_pTransformCom->Get_World()).m[3][0]), sizeof(_vec3));

		// children
		size_t iSize = m_pChildrens.size();
		p->vecChildrensData = vector<MAPOBJECTDATA>{ iSize };
		for (size_t i = 0; i< iSize; ++i)
		{
			m_pChildrens[i]->ExportData(&p->vecChildrensData[i]);
		}
	}
	else
	{
		MSG_BOX("CPrefab::ExportData, pData is 맛감");
	}
}

HRESULT CPrefab::Set_Component()
{
	if (FAILED(Add_Components(L"Com_Transform", SCENE_STATIC, L"Proto_Transform", (CComponent **)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

void CPrefab::Remove_Children(CGameObject *_p)
{
	if (IsEmpty() || !_p)
	{
		MSG_BOX("CPrefab::Remove_Children, Failed");
		return;
	}
	vector<CGameObject *>::iterator itr = m_pChildrens.begin();
	for (; itr != m_pChildrens.end(); ++itr)
	{
		if ((*itr) == _p)
			break;
	}

	if (itr != m_pChildrens.end())
	{
		(*itr)->Set_Dead(TRUE);
		m_pChildrens.erase(itr);
	}
	else
	{
		MSG_BOX("CPrefab::Remove_Children, CGameObject * was invalid");
		return;
	}
}

void CPrefab::Remove_Children(_uint _i)
{
	if (IsEmpty())
	{
		MSG_BOX("CPrefab::Remove_Children, Failed");
		return;
	}

	if (m_pChildrens[_i])
	{
		m_pChildrens[_i]->Set_Dead(TRUE);
		m_pChildrens.erase(m_pChildrens.begin() + _i);
	}
	else
	{
		MSG_BOX("CPrefab::Remove_Children, m_pChildren[_i] was invalid");
		return;
	}
}

HRESULT CPrefab::Set_Data(PREFABDATA *_pData)
{
	if (_pData->eType < PrefabType::SIGN_PILLAR || _pData->eType >= PrefabType::NONE)
	{
		MSG_BOX("CPrefab::Set_Data, type was invalid in Loaded data");
		return E_FAIL;
	}

	SetPrefabType(_pData->eType);

	// 부모 월드
	m_pTransformCom->Set_Info(INFO::INFO_RIGHT, _pData->ParentTransform.Right);
	m_pTransformCom->Set_Info(INFO::INFO_UP, _pData->ParentTransform.Up);
	m_pTransformCom->Set_Info(INFO::INFO_LOOK, _pData->ParentTransform.Look);
	m_pTransformCom->Set_Info(INFO::INFO_POS, _pData->ParentTransform.Pos);

	// 데이터에 저장된 TransformData는 Local 정보
	for (int i = 0; i < _pData->vecChildrensData.size(); ++i)
	{
		_pData->vecChildrensData[i].bChild = true;
		if (CGameObject *pGo = CMapFactory::GetInstance()->Create(
			_pData->vecChildrensData[i].eCategory,
			_pData->vecChildrensData[i].iType,
			&_pData->vecChildrensData[i]))
		{
			if (pGo)
			{
				m_pChildrens.push_back(pGo);
				pGo->SetParent(this);
			}
			else
			{
				MSG_BOX("CPrefab::Set_Data, Children Create Failed");
				return E_FAIL;
			}
		}
	}

	Set_ChildrensMatrix();

	return S_OK;
}

void CPrefab::Set_ChildrensMatrix()
{
	if (IsEmpty())
		return;

	_matrix matResult;
	CTransform *pTransform;
	for (int i = 0; i < m_pChildrens.size(); ++i)
	{
		if (m_pChildrens[i])
		{
			if (pTransform = m_pChildrens[i]->GetTransform())
			{
				matResult = (*pTransform->Get_Local()) * (*GetTransform()->Get_World());
				pTransform->Set_Info(INFO::INFO_RIGHT, (*(_vec3*)&matResult[0]));
				pTransform->Set_Info(INFO::INFO_UP, (*(_vec3 *)&matResult[1]));
				pTransform->Set_Info(INFO::INFO_LOOK, (*(_vec3 *)&matResult[2]));
				pTransform->Set_Info(INFO::INFO_POS, (*(_vec3 *)&matResult[3]));
			}
		}
	}
}
