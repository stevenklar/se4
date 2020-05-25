#pragma once

#include <pch.h>
#include "Game.h"

static std::array<Entity*, 1024>(entities);

class ESP : public Icetrix::Layer::SimpleHook
{
public:
	ESP() : SimpleHook{}
	{
		g_sApp.dispatcher.sink<Icetrix::Layer::MenuDrawEvent>().connect<&ESP::Draw>(*this);
		g_sApp.dispatcher.sink<Icetrix::Layer::MenuDrawEvent>().connect<&ESP::Menu>(*this);
	}

private:
	Icetrix::Memory::PatternPatch* patch = PP{
		"ESP",
		{ 0xF3, 0x0F, 0x5C, 0x73, 0x44, 0xF3, 0x0F, 0x5C, 0x7B, 0x48, 0xF3, 0x44, 0x0F, 0x5C, 0x43, 0x4C, 0x48, 0x63, 0x48, 0x08 },
		{ 0xF3, 0x0F, 0x5C, 0x73, 0x44, 0xF3, 0x0F, 0x5C, 0x7B, 0x48 },
		{ }
	};

	float position_modifier_x = 0.f;
	float position_modifier_y = 0.f;
	float position_modifier_z = 0.f;

	bool headEsp = true;
	bool nameEsp = false;
	bool distanceEsp = false;
	bool healthEsp = false;

	Vector3 NDC = *(Vector3*)(g_dMainModuleBase + 0xEA4700);
	float* mvp = (float*)(g_dMainModuleBase + 0xC1E480);
	int width = *(int*)(g_dMainModuleBase + 0xC0EE0C);
	int height = *(int*)(g_dMainModuleBase + 0xC0EE10);
	Vector2 window = { static_cast<float>(width), static_cast<float>(height) };
	float modifier = *(float*)(g_dMainModuleBase + 0xA943E0);
	float boundary = *(float*)(g_dMainModuleBase + 0xA94550);
	LocalPlayer1* localPlayerPtr = (LocalPlayer1*)(g_dMainModuleBase + 0xD4B0D0);

public:
	void Menu(const Icetrix::Layer::MenuDrawEvent& menu)
	{
		if (!g_sFeatures->Get(patch->name)->enabled)
		{
			entities.fill(nullptr);
			return;
		}

		if (!menu.show)
			return;

		ImGui::Begin("ESP Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Checkbox("Name", &nameEsp);
		ImGui::Checkbox("Head", &headEsp);
		ImGui::Checkbox("Health", &healthEsp);
		ImGui::Checkbox("Distance", &distanceEsp);

#ifdef _DEBUG
		ImGui::SliderFloat("Modifier X", &position_modifier_x, -100, +100);
		ImGui::SliderFloat("Modifier Y", &position_modifier_y, -100, +100);
		ImGui::SliderFloat("Modifier Z", &position_modifier_z, -100, +100);

		if (ImGui::ListBoxHeader("Entities", (int)entities.size()))
		{
			Entity* player = g_mono("player");
			for (int i = 0; i < entities.size(); i++)
			{
				auto entity = entities[i];

				if (!entity)
					continue;

				char* playerMarker = "";
				if (player && player == entity)
					playerMarker = "*";

				char entityAddress[16];
				sprintf(entityAddress, "[%d%s] %I64x", i, playerMarker, (DWORD64)entity);
				if (ImGui::Selectable(entityAddress, false))
					Icetrix::Platform::Windows::CopyToClipboard(entityAddress);
			}
			ImGui::ListBoxFooter();
		}
#endif

		ImGui::End();
	}

	void Draw()
	{
		if (!g_sFeatures->Get(patch->name) || !g_sFeatures->Get(patch->name)->enabled)
			return;

		g_mono("entities") = entities;
		Entity* player = g_mono("player");

		g_sRenderer->BeginScene();
		for (int i = 0; i < entities.size(); i++)
		{
			auto ent = entities[i];
			if (!ent)
				continue;

			if (ent->coords.x == 0 || ent->coords.y == 0 || ent->coords.z == 0)
				continue;

			if (player && player == ent)
				continue;

			if (ent->health_readonly <= 0.001)
			{
			/*
				// Garbage Collector
				for (int j = 0; j < entities.size(); j++)
				{
					if (entities[j] == ent)
						entities[j] = nullptr;
				}
			*/
				continue;
			}

			Vector3 screen = { 0.0, 0.0, 0.0 };
			Vector3 screen_head = { 0.0, 0.0, 0.0 };
			Vector3 screen_head_above = { 0.0, 0.0, 0.0 };

			bool valid = Icetrix::Engine::Asura::WorldToScreen(mvp, window, ent->coords, screen, modifier, boundary, NDC);
			bool validHeadAbove = Icetrix::Engine::Asura::WorldToScreen(mvp, window, ent->b3, screen_head_above, modifier, boundary, NDC); //thorax
			bool validHead = Icetrix::Engine::Asura::WorldToScreen(mvp, window, ent->b2, screen_head, modifier, boundary, NDC); // head

			if (valid && validHead && validHeadAbove) //thorax
			{
				if (headEsp)
					g_sRenderer->RenderCircle({ screen_head.x, screen_head.y }, 10.0f, 0xFFFF0000);

				float relativeInfo = 0.f;

				if (nameEsp)
				{
					char entityName[16];
					sprintf(entityName, "Entity [%d]", i);

#ifdef DEBUG
					char entityAddress[16];
					sprintf(entityAddress, "%I64x", (DWORD64)ent);
					g_sRenderer->RenderText(entityAddress, { screen.x, screen.y + relativeInfo }, 15.0f, 0xFFFF0000, true);
					relativeInfo += 15.0f;
#endif

					g_sRenderer->RenderText(entityName, { screen.x, screen.y + relativeInfo }, 15.0f, 0xFFFF0000, true);
					relativeInfo += 15.0f;
				}

				if (healthEsp)
				{
					char entityHealth[17];
					_gcvt_s(entityHealth, sizeof(entityHealth), ent->health_readonly, 8);

					g_sRenderer->RenderText(entityHealth, { screen.x, screen.y + relativeInfo }, 15.0f, 0xFFFF0000, true);
					relativeInfo += 15.0f;
				}

				if (!player)
					continue;

				if (distanceEsp)
				{
					// @TODO: buggy distance calculation
					Vector3 delta = { ent->coords.x - player->coords.x, ent->coords.y - player->coords.y, ent->coords.z - player->coords.z };
					float distance = sqrt(powf(delta.x, 2) + powf(delta.y, 2) + powf(delta.z, 2));
					char entityDistance[17];
					_gcvt_s(entityDistance, sizeof(entityDistance), distance, 8);
					g_sRenderer->RenderText(entityDistance, { screen.x, screen.y + relativeInfo }, 15.0f, 0xFFFF0000, true);
				}
			}
		};
		g_sRenderer->EndScene();
	}

protected:
	virtual Icetrix::Memory::PatternPatch* GetPatch() override { return patch; }
	virtual Icetrix::Layer::Detour* GetHookFunc() override { return Hook; }

	static void Hook(blackbone::IAsmHelper& a)
	{
		Icetrix::Memory::Asm::PushRegisters(a);
		a.GenCall((size_t)&Collect, { a->zbx }, blackbone::eCalligConvention::cc_cdecl);
		Icetrix::Memory::Asm::PopRegisters(a);

		a->subss(asmjit::x86::xmm6, asmjit::host::ptr(a->zbx, 0x44));
		a->subss(asmjit::x86::xmm7, asmjit::host::ptr(a->zbx, 0x48));
	}

	static void Collect(DWORD64 base)
	{
		Entity* ent = (Entity*)(base);
		for (int i = 0; i < entities.size(); i++)
		{
			//0x140A4CED0
			if (*(DWORD64*)base != 0x140A4CED0) // is entity
				break;

			//if (ent->checkptr != nullptr)
				//break;

			if (entities[i] == ent)
				break;

			if (entities[i] != NULL)
				continue;

			if (ent->health_readonly <= 0.001)
				break;

			entities[i] = ent;
			break;
		}
	}
};
