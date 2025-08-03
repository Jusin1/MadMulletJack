#include "CRenderer.h"

CRenderer::CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{

}
CRenderer::~CRenderer()
{
	Free();
}

HRESULT CRenderer::Ready_Render()
{
	return S_OK;
}

HRESULT CRenderer::Initialize(void* pArg)
{
	return S_OK;
}

// 렌더링 그룹에 오브젝트 추가
HRESULT CRenderer::Add_RenderGroup(RENDERID eType, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderGroup[eType].push_back(pGameObject);

	pGameObject->Add_Ref();

	return S_OK;
}


// 모든 그룹 렌더링
HRESULT CRenderer::Render_GameObject()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonAlpha()))
		return E_FAIL;
	if (FAILED(Render_Alpha()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;
	return S_OK;

}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderGroup[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}

	m_RenderGroup[RENDER_NONALPHA].clear();
	return S_OK;
}

// 아직 추가안함(알파sorting해서 정렬예정)
HRESULT CRenderer::Render_Alpha()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}

	m_RenderGroup[RENDER_ALPHA].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}

	m_RenderGroup[RENDER_UI].clear();
	return S_OK;
}

CRenderer* CRenderer::Create(LPDIRECT3DDEVICE9 pGrahpicDev)
{
	CRenderer* pRender = new CRenderer(pGrahpicDev);

	if (FAILED(pRender->Ready_Render()))
	{
		MSG_BOX("Renderer Create Failed");
		Safe_Release(pRender);
	}

	return pRender;
}

// 공유용이므로 자기 자신 반환(참조만 증가시킴)
CComponent* CRenderer::Clone(void* pArg)
{
	this->Add_Ref();

	return this;
}

void CRenderer::Free()
{
	CComponent::Free();
}
