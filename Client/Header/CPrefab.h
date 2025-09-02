#pragma once
#include "CGameObject.h"

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

	HRESULT Set_Component();
public:
	void Add_Children(CGameObject *_p) { if (!_p) return; _p->SetParent(this); m_pChildrens.push_back(_p); Set_ChildrensMatrix(); }
	void Remove_Children(CGameObject *_p);
	void Remove_Children(_uint _i);
	CGameObject *Get_Children(_uint _i) { if (IsEmpty()) return nullptr; return m_pChildrens[_i]; }
	CGameObject *Get_HeadChildren() { if (IsEmpty()) return nullptr; return m_pChildrens[0]; }
	CGameObject *Get_TailChildren() { if (IsEmpty()) return nullptr; return m_pChildrens.back(); }
	const vector<CGameObject *> &Get_ChildrenList() const { if (IsEmpty()) return {}; return m_pChildrens; }
	_bool IsEmpty() const { return m_pChildrens.size() <= 0; }

	PrefabType GetType() const { return m_eType; }
	void SetPrefabType(PrefabType _e) { m_eType = _e; }
	
	void Set_Dead_All();

	ObjectCategory GetCategory() const { return m_eCategory; }
	bool IsClone() const { return m_bClone; }
	void Set_ChildrensMatrix();
private:
	HRESULT Set_Data(PREFABDATA *_pData);
private:
	bool m_bClone;
	ObjectCategory m_eCategory;
	PrefabType m_eType;
	vector<CGameObject *> m_pChildrens;
};

