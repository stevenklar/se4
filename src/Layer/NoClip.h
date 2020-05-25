#pragma once

#include "Icetrix.h"
#include "Game.h"

class NoClip
{
private:
	Icetrix::Memory::PatternPatch* patch = PP{ "No Fall", { 0x89, 0x83, 0x88, 0x00, 0x00, 0x00, 0x8B, 0x47 }, { 0x89, 0x83, 0x88, 0x00, 0x00, 0x00 }, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 } };

	float relx = 0.0f, rely = 0.0f, relz = 0.0f;
	float newx, newy, newz;

public:
	NoClip()
	{
		g_sApp.dispatcher.sink<Icetrix::LayerEvent::Attach>().connect<&NoClip::Attach>(*this);
		g_sApp.dispatcher.sink<Icetrix::LayerEvent::Update>().connect<&NoClip::Update>(*this);
		g_sApp.dispatcher.sink<Icetrix::LayerEvent::Detach>().connect<&NoClip::Detach>(*this);
	}

	void Attach() { g_sFeatures->Push(new Icetrix::Feature{ "NoClip", false }); }
	void Detach() { Icetrix::Memory::BytePatch::Unpatch(g_sMainModule, patch); }

	void Update()
	{
		if (!g_sFeatures->Get("NoClip")->enabled)
			return Icetrix::Memory::BytePatch::Unpatch(g_sMainModule, patch);

		Icetrix::Memory::BytePatch::Patch(g_sMainModule, patch);
		float* mvp = (float*)(g_dMainModuleBase + 0xC1E480);
		WorldPosition* positionPtrChain = *(WorldPosition**)(g_dMainModuleBase + 0xC15C68);

		if (!mvp)
			return;

		if (!positionPtrChain && !positionPtrChain->p1 && !positionPtrChain->p1->p2 && !positionPtrChain->p1->p2->p3)
			return;

		Vector3& coords = positionPtrChain->p1->p2->p3->m_pos;

		float multiplier = 0.5f;

		if (GetAsyncKeyState(VK_SPACE) != 0)
			coords.y += multiplier;

		if (GetAsyncKeyState(VK_LCONTROL) != 0)
			coords.y -= multiplier;

		relx = 0.0f;
		rely = 0.0f;
		relz = 0.0f;

		//FLY FORWARD/BACKWARD
		if (GetAsyncKeyState(VK_UP))
			relz += multiplier;
		if (GetAsyncKeyState(VK_DOWN))
			relz -= multiplier;

		// FLY LEFT/RIGHT
		if (GetAsyncKeyState(VK_LEFT))
			relx -= multiplier;
		if (GetAsyncKeyState(VK_RIGHT))
			relx += multiplier;

		newx = (*mvp) * relx + (*(mvp + 0x10)) * rely + (*(mvp + 0x20)) * relz;
		newy = (*(mvp + 0x4)) * relx + (*(mvp + 0x14)) * rely + (*(mvp + 0x24)) * relz;
		newz = (*(mvp + 0x8)) * relx + (*(mvp + 0x18)) * rely + (*(mvp + 0x28)) * relz;

		coords.x += newx;
		coords.z += newy;
		coords.y += newz;
	}
};

