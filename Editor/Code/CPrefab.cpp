#include "Engine_Define.h"
#include "CTransform.h"
#include "CGuiManager.h"
#include "CObjectManager.h"
#include "CMapFactory.h"
#include "CPrefab.h"

CPrefab::CPrefab(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_bClone(false), m_eCategory(ObjectCategory::PREFAB)
{
	m_pChildrens.reserve(20);
}

CPrefab::CPrefab(CPrefab &rhs)
	: CGameObject(rhs), m_bClone(true), m_eCategory(ObjectCategory::PREFAB)
{
	m_pChildrens.reserve(20);
}

CPrefab::~CPrefab()
{
}

void CPrefab::Free()
{
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
	// 복사본 생성시 WorldMatrix 세팅
	if (m_bClone)
	{
		// 데이터가 있다면 ?
		if (PREFABDATA *pData = reinterpret_cast<PREFABDATA *>(pArg))
		{
			// 부모 월드
			m_pTransformCom->Set_Info(INFO::INFO_RIGHT, pData->ParentTransform.Right);
			m_pTransformCom->Set_Info(INFO::INFO_UP, pData->ParentTransform.Up);
			m_pTransformCom->Set_Info(INFO::INFO_LOOK, pData->ParentTransform.Look);
			m_pTransformCom->Set_Info(INFO::INFO_POS, pData->ParentTransform.Pos);

			// 데이터에 저장된 TransformData는 Local 정보
			for (int i = 0; i < pData->vecChildrensData.size(); ++i)
			{
				if (CGameObject *pGo = CMapFactory::GetInstance()->Create(pData->vecChildrensData[i].eCategory, pData->vecChildrensData[i].iType, &pData->vecChildrensData[i]))
				{
					if (pGo)
					{
						m_pChildrens.push_back(pGo);
					}
					else
					{
						MSG_BOX("CPrefab::Initialize, Children Create Failed");
						return E_FAIL;
					}
				}
			}

			Set_ChildrensMatrix();
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
	if (!PickingPoint || IsEmpty())
		return FALSE;

	for (int i = 0; i < m_pChildrens.size(); ++i)
	{
		if (m_pChildrens[i])
		{
			if (m_pChildrens[i]->Picking(PickingPoint))
				return TRUE;
		}
	}

	return FALSE;
}

void CPrefab::PickingTrue()
{
	CGuiManager::GetInstance()->SetTarget(this);
}

void CPrefab::ExportData(void *pData)
{
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
		Safe_Release(*itr);
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
		Safe_Release(m_pChildrens[_i]);
		m_pChildrens.erase(m_pChildrens.begin() + _i);
	}
	else
	{
		MSG_BOX("CPrefab::Remove_Children, m_pChildren[_i] was invalid");
		return;
	}
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
