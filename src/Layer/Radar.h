#pragma once

#include "Icetrix.h"
#include <corecrt_math_defines.h>

class Radar
{
public:
	Radar()
	{
		g_sApp.dispatcher.sink<Icetrix::LayerEvent::Attach>().connect<&Radar::Attach>(*this);
		g_sApp.dispatcher.sink<Icetrix::LayerEvent::Detach>().connect<&Radar::Detach>(*this);
		g_sApp.dispatcher.sink<Icetrix::Layer::MenuDrawEvent>().connect<&Radar::Draw>(*this);
		g_sApp.dispatcher.sink<Icetrix::Layer::MenuDrawEvent>().connect<&Radar::Menu>(*this);
	}

private:
	Icetrix::Feature* feature = new Icetrix::Feature{ "Radar" };
	float scale = 1.0f;
	float zoom = 1.0f;

public:
	void Attach() { Icetrix::Features::GetInstance()->Push(feature); }
	void Detach() { delete feature; }

	void Menu(const Icetrix::Layer::MenuDrawEvent& menu)
	{
		if (!Icetrix::Features::GetInstance()->Get(feature->label)->enabled)
			return;

		if (!menu.show)
			return;

		ImGui::Begin("Radar Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SliderFloat("Scale", &scale, -100, +100);
		ImGui::SliderFloat("Zoom", &zoom, -100, +100);
		ImGui::End();
	}

	void Draw()
	{
		if (!Icetrix::Features::GetInstance()->Get(feature->label)->enabled)
			return;

		ImGui::SetNextWindowSize(ImVec2(256, 256), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), SquareConstraint);

		if (ImGui::Begin("Radar", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			ImVec2 winpos = ImGui::GetWindowPos();
			ImVec2 winsize = ImGui::GetWindowSize();

			draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y), ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y), ImColor(70, 70, 70, 255), 1.f);
			draw_list->AddLine(ImVec2(winpos.x, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y + winsize.y * 0.5f), ImColor(70, 70, 70, 255), 1.f);

			draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x, winpos.y), ImColor(90, 90, 90, 255), 1.f);
			draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y), ImColor(90, 90, 90, 255), 1.f);

			Entity* player = g_mono("player");
			std::array<Entity*, 1024> entities = g_mono("entities");

			if (!player)
			{
				ImGui::End();
				return;
			}

			ImVec2 center = { winpos.x + (winsize.x * 0.5f), winpos.y + (winsize.y * 0.5f) };
			draw_list->AddCircleFilled(ImVec2(center.x, center.y), scale, ImColor(255, 255, 255, 255));

			if (entities.size() <= 0)
			{
				ImGui::End();
				return;
			}

			for (auto entity : entities)
			{
				if (!entity || entity == player)
					continue;

				if (entity->health_readonly <= 0.01f)
					continue;

				// calculate delta between entity and local player
				ImVec2 screen = WorldToRadar(entity->coords, player->coords, player->rotation, zoom);
				draw_list->AddCircleFilled(ImVec2(center.x + screen.x, center.y + screen.y), scale, ImColor(0, 255, 0));
			}

			ImGui::End();
		}
	}

	static void SquareConstraint(ImGuiSizeCallbackData* data)
	{
		data->DesiredSize = ImVec2(std::max(data->DesiredSize.x, data->DesiredSize.y), std::max(data->DesiredSize.x, data->DesiredSize.y));
	}

protected:
	struct ImGuiSizeCallbackData
	{
		void* UserData;       // Read-only.   What user passed to SetNextWindowSizeConstraints()
		ImVec2  Pos;            // Read-only.   Window position, for reference.
		ImVec2  CurrentSize;    // Read-only.   Current window size.
		ImVec2  DesiredSize;    // Read-write.  Desired size, based on user's mouse position. Write to this field to restrain resizing.
	};

	ImVec2 WorldToRadar(Vector3 entityCoords, Vector3 playerCoords, Vector3 playerRotation, float zoom)
	{
		Vector3 delta;
		delta.x = entityCoords.x - playerCoords.x;
		//delta.y = entityCoords.y - playerCoords.y;
		delta.y = entityCoords.z - playerCoords.z;

		// scale delta to screen coords
		ImVec2 screen;
		screen.x = delta.x / zoom;
		screen.y = delta.y / zoom;
		
		// rotate screen using rotation matrix
		// convert yaw degree to radian
		float yaw = static_cast<float>((playerRotation.x) * M_PI / 180.0);
		//float yaw = playerRotation.x;

		ImVec2 rotation;
		rotation.x = (screen.x * cos(yaw)) - (screen.y * sin(yaw));
		rotation.y = (-screen.x * sin(yaw)) + (screen.y * cos(yaw));

		return rotation;
	}
};
