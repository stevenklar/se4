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
	float scale = 2.25f;
	float zoom = 1.0f;
	DWORD64 baseAddress = g_dMainModuleBase;

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
		ImGui::SliderFloat("Scale", &scale, 0, 10);
		ImGui::SliderFloat("Zoom", &zoom, 0.5, 100);
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

				DWORD64 pitch = (DWORD64)baseAddress + 0xC1E4B0;
				float yaw = *(float*)(baseAddress + 0xC1E4B4);

				// calculate delta between entity and local player
				ImVec2 screen = WorldToRadar(entity->coords, player->coords, yaw, zoom, false);
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

	#define URotationToRadians( URotation )		( ( URotation ) * ( M_PI / 32768.0f ) ) 
	#define URotationToDegree( URotation )		( ( URotation ) * ( 360.0f / 65536.0f ) ) 
	#define DegreeToURotation( Degree )			( ( Degree ) * ( 65536.0f / 360.0f ) )
	#define DegreeToRadian( Degree )			( ( Degree ) * ( M_PI / 180.0f ) )
	#define RadianToURotation( URotation )		( ( URotation ) * ( 32768.0f / M_PI ) ) 
	#define RadianToDegree( Radian )			( ( Radian ) * ( 180.0f / M_PI ) )

	ImVec2 WorldToRadar(Vector3 entityCoords, Vector3 playerCoords, float yaw, float zoom, bool degreeToRadian = false)
	{
		Vector2 delta;
		delta.x = entityCoords.x - playerCoords.x;
		delta.y = entityCoords.z - playerCoords.z;

		// scale delta to screen coords
		ImVec2 screen;
		screen.x = delta.x * zoom;
		screen.y = delta.y * zoom;
		
		if (degreeToRadian)
			yaw = DegreeToRadian(yaw);

		ImVec2 rotation;
		float adjust = DegreeToRadian(180);
		rotation.x = -screen.x * (cos(yaw + adjust)) + screen.y * (sin(yaw + adjust));
		rotation.y = screen.x * (sin(yaw + adjust)) + screen.y * (cos(yaw + adjust));

		return rotation;
	}
};
