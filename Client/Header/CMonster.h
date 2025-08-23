#pragma once

#include "CCharacter.h"
#include "CComponentMgr.h"
#include "Clinet_Define.h"
#include "Client_Global.h"

// 추가예정
// 1. 2개의 Door WorldPosition을 기준(z기준 min, max를 key값)으로 하는 unordered_multimap을 생성하고 해당 반경내에 있는 Monster들 캐싱
// + room내에 있는 PANELDATA(벽 들) 또한 캐싱
// 2. Player WorldPosition기준 캐싱된 unordered_map에 있는 Monster들만 반경 검사
// 3. Player와 Monster 사이에 벽이 있는지 없는지 검사 후 추적 State

class CMonster : public CCharacter
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev, MonsterType _iType);
	explicit CMonster(const CMonster& rhs, MonsterType _eType);
	virtual ~CMonster();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg);
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	ObjectCategory GetCategory() const { return m_eCategory; }
	MonsterType GetType() const { return m_eType; }
protected:
	HRESULT	Set_Component();
	

protected: 
	virtual _bool Picking(_vec3* PickingPoint) override;
	virtual void PickingTrue() override;

protected:
	void SetUp_BillBoard();
	void Set_Collider_With_Wall();

protected:
	virtual HRESULT Texture_Clone();
	virtual HRESULT Change_Texture(const _tchar* LayerTag);
	
protected:
	Engine::CColider_Sphere* m_pColiderCom;
	Engine::CTexture* m_pTextureCom; // 기본 텍스쳐
	map<const _tchar*, CTexture*> m_mapTexture;

public:
	ObjectCategory m_eCategory;
	MonsterType m_eType;
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
