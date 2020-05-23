#pragma once

#include <Icetrix.h>
#include <Icetrix\Memory\Asm.h>

class Godmode : public Icetrix::Layer::SimpleHook
{
protected:
	virtual Icetrix::Memory::PatternPatch* GetPatch() override { return patch; }
	virtual Icetrix::Layer::Detour* GetHookFunc() override { return Hook; }

	Icetrix::Memory::PatternPatch* patch = PP{ "Godmode", { 0xF3, 0x0F, 0x5C, 0xCF, 0x0F, 0x2F, 0xCE, 0xF3, 0x0F, 0x11, 0x4B }, { 0xF3, 0x0F, 0x5C, 0xCF, 0x0F, 0x2F, 0xCE, 0xF3, 0x0F, 0x11, 0x4B, 0x50 }, { } };

	static void Hook(blackbone::IAsmHelper& a)
	{
		using namespace asmjit;
		using namespace Icetrix::Memory::Asm;

		asmjit::Label l_player = a->newLabel();

		a->cmp(x86::dword_ptr(a->zbx, 0x1C), 0x00);
		a->jz(l_player);
		a->subss(x86::xmm1, x86::xmm7);
		a->bind(l_player);
		a->comiss(x86::xmm1, x86::xmm6); 
		a->movss(x86::ptr(a->zbx, 50), x86::xmm1);
	}
};

