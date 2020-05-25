#pragma once

#include "Icetrix.h"
#include "Game.h"

class LocalPlayer : public Icetrix::Layer::SimpleHook
{
public:
	LocalPlayer() : Icetrix::Layer::SimpleHook{}
	{
		g_sApp.dispatcher.sink<Icetrix::Layer::MenuDefaultWindowEvent>().connect<&LocalPlayer::Menu>(*this);
	}

	void Menu()
	{
		Entity* localPlayer = g_mono("player");
		char entityAddress[16];
		sprintf(entityAddress, "%I64x", (DWORD64)localPlayer);

		ImGui::Text("%s: %s", "Local Player", entityAddress);
	}

protected:
	virtual Icetrix::Memory::PatternPatch* GetPatch() override { return patch; }
	virtual Icetrix::Layer::Detour* GetHookFunc() override { return Hook; }

	Icetrix::Memory::PatternPatch* patch = PP{ "LocalPlayer", { 0x48, 0x8B, 0x83, 0xF0, 0xFE, 0xFF, 0xFF, 0x48, 0x8D, 0x8B, 0xF0, 0xFE, 0xFF, 0xFF }, { 0x48, 0x8B, 0x83, 0xF0, 0xFE, 0xFF, 0xFF, 0x48, 0x8D, 0x8B, 0xF0, 0xFE, 0xFF, 0xFF }, { } };

	static void Hook(blackbone::IAsmHelper& a)
	{
		Icetrix::Memory::Asm::PushRegisters(a);
		a.GenCall((size_t)&Collect, { a->zbx }, blackbone::eCalligConvention::cc_cdecl);
		Icetrix::Memory::Asm::PopRegisters(a);

		a->mov(a->zax, asmjit::host::ptr(a->zbx, -0x110));
		a->mov(a->zcx, asmjit::host::ptr(a->zbx, -0x110));
	}

	static void Collect(DWORD64 base)
	{
		Entity* localPlayer = (Entity*)(base - 0x110);

		if (!localPlayer)
			return;

		if (*(DWORD64*)localPlayer != 0x140A4CED0) // is entity
			return;

		g_mono("player") = localPlayer;
	}
};

