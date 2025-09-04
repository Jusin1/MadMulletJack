#pragma once
#include "CBase.h"
#include "CGameObject.h"

BEGIN(Engine)

// GameObject 관리
class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer();

public:
	// 인덱스 기반으로 Get
	class CComponent* Get_Component(const _tchar* pComponentTag, _uint iIndex = 0);
	class CGameObject* Get_Object(_uint iIndex = 0);
	list<CGameObject*>* Get_ObjectList() { return &m_objList; };

public:
	HRESULT			Ready_Layer();
	HRESULT			Add_GameObject(CGameObject* pGameObject);
	void			Update_Layer(const _float& fTimeDelta);
	void			LateUpdate_Layer(const _float& fTimeDelta);

	void			Sort_By_Z();
private:
	list<class CGameObject*> m_objList;

public:
	static CLayer* Create();
	virtual void	Free();
};

END