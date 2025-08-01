#pragma once
#include "CBase.h"
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();

public:
	CComponent* Get_Component(COMPONENTID eID, const _tchar* pComponentTag);

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) { return S_OK; }
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject() {}

public:
	void Update_Position(_vec3 position) { m_vPosition = position; } // 위치 설정
	_vec3 Get_Position() { return m_vPosition; }
	_float Get_Radius() { return m_fRadius; } 
	_bool Get_Dead() { return m_bDead; }
	void Set_Dead(_bool bDead) { m_bDead = bDead; }
	void Set_Radius(_float fRadius) { m_fRadius = fRadius; }

protected:
	map<const _tchar*, CComponent*>			m_mapComponent[ID_END];
	_vec3									m_vPosition;
	_float									m_fRadius = 0.5f; // 충돌 반지름(원충돌)
	_bool									m_bDead = false;
	_matrix									m_CollisionMatrix; // 충돌 시 사용할 월드 행렬
	LPDIRECT3DDEVICE9						m_pGraphicDev;

private:
	CComponent* Find_Component(COMPONENTID eID, const _tchar* pComponentTag);
protected:
	virtual		void		Free();
};

END