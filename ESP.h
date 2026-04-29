#include "global.h"
#include "util.h"
#include "offset.h"
#include <cmath>
// ###################################################### Some useful stuff (I was lazy) ###################################################### \\


HWND hwnd = NULL;
HWND hwnd_active = NULL;
HWND OverlayWindow = NULL;
auto CrosshairColor = D2D1::ColorF(0, 100, 255, 255);
auto Color = D2D1::ColorF(255, 0, 0, 255);

FCameraCacheEntry cameraCache;
uintptr_t uWorld;
uintptr_t gameInstance;
uintptr_t persistentLevel;
uintptr_t localPlayerPtr;
uintptr_t localPlayer;
uintptr_t playerController;
uintptr_t Localpawn;
uintptr_t Localteam;
uintptr_t cameraManager;
uintptr_t actorsArray;
uintptr_t PlayerStateLocalPlayer;
uintptr_t PlayerState;
int actorsCount;
std::string nameS;

// ###################################################### ESP Features ###################################################### \\

void ESPLoop() {

	hwnd = FindWindowA("UnrealWindow", NULL); // Target Window With his lpClassName ( UnrealWindow )
	OverlayWindow = FindWindow("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");
	hwnd_active = GetForegroundWindow();

	if (hwnd_active == hwnd) {

		ShowWindow(OverlayWindow, SW_SHOW);
	}
	else
	{
		ShowWindow(OverlayWindow, SW_HIDE);
	}
}

void RenderMenu()
{
	if (showmenu && rendering)
	{
		g_overlay->draw_text(5, 5, D2D1::ColorF(255, 20, 20, 255), "SHOW/HIDE [INSERT] Zangous mapped");

		if (esp)
			g_overlay->draw_text(5, 20, D2D1::ColorF(0, 255, 0, 255), "F1 Esp : ON");
		else
			g_overlay->draw_text(5, 20, D2D1::ColorF(255, 0, 0, 255), "F1 Esp : OFF");

		if (distanceESp)
			g_overlay->draw_text(5, 40, D2D1::ColorF(0, 255, 0, 255), "F2 Distance ESP : ON");
		else
			g_overlay->draw_text(5, 40, D2D1::ColorF(255, 0, 0, 255), "F2 Distance ESP : OFF");

		if (espLine)
			g_overlay->draw_text(5, 60, D2D1::ColorF(0, 255, 0, 255), "F4 ESPLine : ON");
		else
			g_overlay->draw_text(5, 60, D2D1::ColorF(255, 0, 0, 255), "F4 ESPLine : OFF");

		if (name)
			g_overlay->draw_text(5, 80, D2D1::ColorF(0, 255, 0, 255), "F5 NAME : ON");
		else
			g_overlay->draw_text(5, 80, D2D1::ColorF(255, 0, 0, 255), "F5 NAME: OFF");

		if (mate)
			g_overlay->draw_text(5, 100, D2D1::ColorF(0, 255, 0, 255), "F6 TEAM : ON");
		else
			g_overlay->draw_text(5, 100, D2D1::ColorF(255, 0, 0, 255), "F6 TEAM : OFF");

		g_overlay->draw_text(5, 120, D2D1::ColorF(0, 255, 0, 255), "UP/Down DistanceMax = ");
		g_overlay->draw_text(160, 120, D2D1::ColorF(0, 0, 255, 255), std::to_string(int(distanceMax)).c_str());
	}
}

inline PVOID BaseThread2()
{
	while (true)
	{
		if (esp)
		{
			std::vector<EntityList> tmpList;

			uWorld = DBD->rpm<uintptr_t>(process_base + offsets::GWorld);
			gameInstance = DBD->rpm<uintptr_t>(uWorld + offsets::OwningGameInstance);
			persistentLevel = DBD->rpm<uintptr_t>(uWorld + offsets::PersistentLevel);
			localPlayerPtr = DBD->rpm<uintptr_t>(gameInstance + offsets::LocalPlayers);
			localPlayer = DBD->rpm<uintptr_t>(localPlayerPtr);
			playerController = DBD->rpm<uintptr_t>(localPlayer + offsets::PlayerController);
			PlayerStateLocalPlayer = DBD->rpm<uintptr_t>(playerController + offsets::PlayerStateLocalPlayer);
			Localpawn = DBD->rpm<uintptr_t>(playerController + offsets::AcknowledgedPawn);
			cameraManager = DBD->rpm<uintptr_t>(playerController + offsets::PlayerCameraManager);
			cameraCache = DBD->rpm<FCameraCacheEntry>(cameraManager + offsets::CameraCachePrivate);
			actorsArray = DBD->rpm<uintptr_t>(persistentLevel + offsets::ActorArray);
			actorsCount = DBD->rpm<int>(persistentLevel + offsets::ActorCount);

			for (int i = 0; i < actorsCount; i++)
			{
				UPlayer player;
				uintptr_t actor = DBD->rpm<uintptr_t>(actorsArray + i * 0x8);
				if (!actor)
					continue;
				player.instance = actor;
				player.objectId = DBD->rpm<int>(actor + offsets::ActorID);

				if (GetNameById(player.objectId).find("BP_CamperMale") != std::string::npos || GetNameById(player.objectId).find("BP_CamperFemale") != std::string::npos || GetNameById(player.objectId).find("BP_Slasher") != std::string::npos)
				{
					player.PlayerState = DBD->rpm<uintptr_t>(actor + offsets::PlayerState);
					player.Pawn = DBD->rpm<uintptr_t>(actor + offsets::AcknowledgedPawn);
					player.root_component = DBD->rpm<uintptr_t>(player.instance + offsets::RootComponent);
					player.origin = DBD->rpm<Vector3>(player.root_component + offsets::RelativeLocation);
					player.TopLocation = Vector3{ player.origin.x, player.origin.y, player.origin.z + 125 };
					float DistM = ToMeters(cameraCache.POV.Location.DistTo(player.origin)) - 6;

					if (DistM > distanceMax || player.PlayerState == PlayerStateLocalPlayer)
						continue;

					if (GetNameById(player.objectId).find("BP_CamperMale") != std::string::npos)
						nameS = "MaleSurvivor";

					if (GetNameById(player.objectId).find("BP_CamperFemale") != std::string::npos)
						nameS = "FemaleSurvivor";

					if (GetNameById(player.objectId).find("BP_Slasher") != std::string::npos)
						nameS = "Killer";

					EntityList Entity{ };
					Entity.instance = player.instance;
					Entity.objectId = player.objectId;
					Entity.PlayerState = player.PlayerState;
					Entity.health = player.health;
					Entity.root_component = player.root_component;
					Entity.origin = player.origin;
					Entity.TopLocation = player.TopLocation;
					Entity.dist = DistM;
					Entity.name = nameS;
					tmpList.push_back(Entity);

				}
				else
					continue;
			}
			entityList = tmpList;
			Sleep(0);
		}
	}
}

void RenderESP()
{
	if (esp)
	{
		auto EntityList_Copy = entityList;
		for (size_t i = 0; i < EntityList_Copy.size(); i++)
		{
			auto Entity = EntityList_Copy[i];
			Vector3 Screen = WorldToScreen(cameraCache.POV, Entity.origin);
			Vector3 head = WorldToScreen(cameraCache.POV, Entity.TopLocation);
			if (!mate & Entity.name == "MaleSurvivor" || !mate & Entity.name == "FemaleSurvivor")
				continue;

			if (Screen.x && head.x)
			{
				const float screenX = static_cast<float>(Screen.x);
				const float screenY = static_cast<float>(Screen.y);

				if (espLine)
					g_overlay->draw_line(ScreenCenterX, 1080.0f, screenX, screenY + 25.0f, Color);

				if (name)
				{
					if(Entity.name == "Killer")
						g_overlay->draw_text(screenX - 50.0f, screenY + 50.0f, D2D1::ColorF(255, 0, 0, 255), Entity.name.c_str());
					else
						g_overlay->draw_text(screenX - 50.0f, screenY + 50.0f, D2D1::ColorF(0, 255, 0, 255), Entity.name.c_str());
				}

				if (distanceESp)
				{
					g_overlay->draw_text(screenX - 23.0f, screenY + 15.0f, D2D1::ColorF(255, 255, 255, 255), "[");
					g_overlay->draw_text(screenX - 15.0f, screenY + 15.0f, D2D1::ColorF(255, 255, 255, 255), std::to_string(int(Entity.dist)).c_str());
					g_overlay->draw_text(screenX + 5.0f, screenY + 15.0f, D2D1::ColorF(255, 255, 255, 255), "]m");
				}
			}

		}

	}
}
