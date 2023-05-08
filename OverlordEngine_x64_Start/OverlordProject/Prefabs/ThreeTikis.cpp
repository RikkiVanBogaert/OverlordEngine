#include "stdafx.h"
#include "ThreeTikis.h"
#include "Tiki.h"

ThreeTikis::ThreeTikis()
{
	auto pTiki = new Tiki();
	AddChild(pTiki);

	/*auto pTiki = new Tiki();
	pTiki->GetTransform()->Translate(0, 0, 12);
	AddChild(pTiki);

	auto pTiki = new Tiki();
	pTiki->GetTransform()->Translate(0, 0, 12);
	AddChild(pTiki);*/
}

void ThreeTikis::Update(const SceneContext& sceneContext)
{
	for (auto t : m_pTikis)
	{
		t->Update(sceneContext);
	}
}

