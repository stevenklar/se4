#include "pch.h"
#include "Icetrix/Entrypoint.h"
#include "Icetrix/Layer/PanicKey.h"

void Icetrix::CreateApplication()
{
	// Icetrix layer
	new Icetrix::Hook::DxgiSwapChainPresent(0x4670);
	LAYER(Icetrix::Layer::PanicKey);
	LAYER(Icetrix::Layer::Watermark);
	LAYER(Icetrix::Layer::SimpleMenu);
	LAYER(Icetrix::Layer::Crosshair);

	// Game layer
	LAYER(AntiDebug);
	LAYER(LocalPlayer);
	LAYER(ESP);
	LAYER(Radar);
	LAYER(Godmode);
	LAYER(Patches);
	LAYER(NoClip);
}
