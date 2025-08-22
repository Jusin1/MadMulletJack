#pragma once
#include "CGameObject.h"

//prefab Scene에서 저장하면
//Prefab 오브젝트 리스트 추가하고 여기서 list 가 추가되는 쪽으로 설계

// 도로 만들때 아이디어
// 커스텀 LinkedList를 통해서 연결 연결 연결 하면 되지않을까 굳이 껐다 키지 않아도?
// 두번째 index의 리스트에 접근했을때 그냥 다음 list

// Renderer 가 필요없음
class CPrefab : public CGameObject
{
private:
	explicit CPrefab(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPrefab(CPrefab &rhs);
	virtual ~CPrefab();

	virtual void Free();
public:
	static CPrefab *Create(LPDIRECT3DDEVICE9 pGraphicDev, void *pArg = nullptr);
	virtual CGameObject *Clone(void *pArg = nullptr) override;

	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	virtual _bool Picking(_vec3 *PickingPoint) override;
	virtual void PickingTrue() override;
	virtual void ExportData(void *pData) override;
public:
	void Add_Children(CGameObject *_p) { m_pChildrens.push_back(_p); }
	void Remove_Children(CGameObject *_p);
	void Remove_Children(_uint _i);
	CGameObject *Get_Children(_uint _i) { if (IsEmpty()) return nullptr; return m_pChildrens[_i]; }
	CGameObject *Get_HeadChildren() { if (IsEmpty()) return nullptr; return m_pChildrens[0]; }
	CGameObject *Get_TailChildren() { if (IsEmpty()) return nullptr; return m_pChildrens.back(); }
	const vector<CGameObject *> &Get_ChildrenList() const
	{ if (m_pChildrens.size() <= 0) return {}; return m_pChildrens; }
	_bool IsEmpty() const { return m_pChildrens.size() <= 0; }

	ObjectCategory GetCategory() const { return m_eCategory; }
private:
	void Set_ChildrensMatrix();
private:
	ObjectCategory m_eCategory;
	bool m_bClone;
	vector<CGameObject*> m_pChildrens;
};

