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

		g_overlay->draw_text(5, 140, D2D1::ColorF(0, 255, 0, 255), "LEFT/RIGHT Camera FOV = ");
		g_overlay->draw_text(190, 140, D2D1::ColorF(0, 0, 255, 255), std::to_string(int(FOV)).c_str());
	}
}

static bool WriteFloat(uintptr_t address, float value)
{
	if (!DBD || !DRV::IsValidUserAddress(address, sizeof(float)))
		return false;

	return DBD->WriteRaw(address, &value, sizeof(value));
}

static void ApplyThirdPersonFOV()
{
	if (!DBD || process_base == 0)
		return;

	float targetFOV = FOV;
	if (targetFOV < 40.0f)
		targetFOV = 40.0f;
	else if (targetFOV > 170.0f)
		targetFOV = 170.0f;

	auto readPtr = [](uintptr_t address) -> uintptr_t {
		if (!DRV::IsValidUserAddress(address, sizeof(uintptr_t)))
			return 0;
		return DBD->rpm<uintptr_t>(address);
	};

	const uintptr_t currentWorld = readPtr(process_base + offsets::GWorld);
	if (!DRV::IsValidUserAddress(currentWorld))
		return;

	const uintptr_t currentGameInstance = readPtr(currentWorld + offsets::OwningGameInstance);
	if (!DRV::IsValidUserAddress(currentGameInstance))
		return;

	const uintptr_t currentLocalPlayerPtr = readPtr(currentGameInstance + offsets::LocalPlayers);
	const uintptr_t currentLocalPlayer = readPtr(currentLocalPlayerPtr);
	const uintptr_t currentPlayerController = readPtr(currentLocalPlayer + offsets::PlayerController);
	if (!DRV::IsValidUserAddress(currentPlayerController))
		return;

	const uintptr_t currentPawn = readPtr(currentPlayerController + offsets::AcknowledgedPawn);
	const uintptr_t currentCameraManager = readPtr(currentPlayerController + offsets::PlayerCameraManager);

	if (DRV::IsValidUserAddress(currentCameraManager))
	{
		WriteFloat(currentCameraManager + offsets::APlayerCameraManager_DefaultFOVOffset, targetFOV);

		const uintptr_t cachePOV = currentCameraManager
			+ offsets::APlayerCameraManager_CameraCachePrivateOffset
			+ offsets::FCameraCacheEntry_POVOffset;
		WriteFloat(cachePOV + offsets::FMinimalViewInfo_FOVOffset, targetFOV);
		WriteFloat(cachePOV + offsets::FMinimalViewInfo_DesiredFOVOffset, targetFOV);

		const uintptr_t viewTargetPOV = currentCameraManager
			+ offsets::APlayerCameraManager_ViewTargetOffset
			+ offsets::FCameraCacheEntry_POVOffset;
		WriteFloat(viewTargetPOV + offsets::FMinimalViewInfo_FOVOffset, targetFOV);
		WriteFloat(viewTargetPOV + offsets::FMinimalViewInfo_DesiredFOVOffset, targetFOV);
	}

	const uintptr_t cameraComponent = readPtr(currentPawn + offsets::ADBDPlayer_CameraOffset);
	if (DRV::IsValidUserAddress(cameraComponent))
	{
		WriteFloat(cameraComponent + offsets::UCameraComponent_FieldOfViewOffset, targetFOV);
	}
}

inline PVOID BaseThread2()
{
	while (true)
	{
		if (!esp)
		{
			Sleep(10);
			continue;
		}

		auto clearEntities = []() {
			std::lock_guard<std::mutex> lock(entityListMutex);
			entityList.clear();
		};

		if (!DBD || process_base == 0)
		{
			clearEntities();
			Sleep(50);
			continue;
		}

		auto readPtr = [](uintptr_t address) -> uintptr_t {
			if (!DRV::IsValidUserAddress(address, sizeof(uintptr_t)))
				return 0;
			return DBD->rpm<uintptr_t>(address);
		};

		std::vector<EntityList> tmpList;

		uWorld = readPtr(process_base + offsets::GWorld);
		if (!DRV::IsValidUserAddress(uWorld))
		{
			clearEntities();
			Sleep(50);
			continue;
		}

		gameInstance = readPtr(uWorld + offsets::OwningGameInstance);
		persistentLevel = readPtr(uWorld + offsets::PersistentLevel);
		if (!DRV::IsValidUserAddress(gameInstance) || !DRV::IsValidUserAddress(persistentLevel))
		{
			clearEntities();
			Sleep(50);
			continue;
		}

		localPlayerPtr = readPtr(gameInstance + offsets::LocalPlayers);
		localPlayer = readPtr(localPlayerPtr);
		playerController = readPtr(localPlayer + offsets::PlayerController);
		if (!DRV::IsValidUserAddress(playerController))
		{
			clearEntities();
			Sleep(50);
			continue;
		}

		PlayerStateLocalPlayer = readPtr(playerController + offsets::PlayerStateLocalPlayer);
		Localpawn = readPtr(playerController + offsets::AcknowledgedPawn);
		cameraManager = readPtr(playerController + offsets::PlayerCameraManager);
		if (!DRV::IsValidUserAddress(cameraManager))
		{
			clearEntities();
			Sleep(50);
			continue;
		}

		FCameraCacheEntry currentCameraCache = DBD->rpm<FCameraCacheEntry>(cameraManager + offsets::CameraCachePrivate);
		currentCameraCache.POV.FOV = FOV;
		actorsArray = readPtr(persistentLevel + offsets::ActorArray);
		actorsCount = DBD->rpm<int>(persistentLevel + offsets::ActorArray + offsets::ActorCount);
		if (!DRV::IsValidUserAddress(actorsArray) || actorsCount <= 0 || actorsCount > 20000)
		{
			clearEntities();
			Sleep(50);
			continue;
		}

		for (int i = 0; i < actorsCount; i++)
		{
			const uintptr_t actorAddress = actorsArray + static_cast<uintptr_t>(i) * sizeof(uintptr_t);
			if (!DRV::IsValidUserAddress(actorAddress, sizeof(uintptr_t)))
				continue;

			UPlayer player;
			uintptr_t actor = readPtr(actorAddress);
			if (!DRV::IsValidUserAddress(actor))
				continue;

			player.instance = actor;
			player.objectId = DBD->rpm<int>(actor + offsets::ActorID);

			const std::string actorName = GetNameById(player.objectId);
			const bool isSurvivor = actorName.find("BP_Camper") != std::string::npos;
			const bool isKiller = actorName.find("BP_Slasher") != std::string::npos;
			if (isSurvivor || isKiller)
			{
				player.PlayerState = readPtr(actor + offsets::PlayerState);
				player.Pawn = readPtr(actor + offsets::AcknowledgedPawn);
				player.root_component = readPtr(player.instance + offsets::RootComponent);
				if (!DRV::IsValidUserAddress(player.root_component + offsets::RelativeLocation, sizeof(Vector3)))
					continue;

				player.origin = DBD->rpm<Vector3>(player.root_component + offsets::RelativeLocation);
				player.TopLocation = Vector3{ player.origin.x, player.origin.y, player.origin.z + 125 };
				float DistM = ToMeters(currentCameraCache.POV.Location.DistTo(player.origin)) - 6;

				if (DistM > distanceMax || player.PlayerState == PlayerStateLocalPlayer)
					continue;

				if (isKiller)
					nameS = "Killer";
				else
					nameS = "Survivor";

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
		}

		{
			std::lock_guard<std::mutex> lock(entityListMutex);
			cameraCache = currentCameraCache;
			entityList = tmpList;
		}

		Sleep(5);
	}
}

void RenderESP()
{
	if (esp)
	{
		std::vector<EntityList> EntityList_Copy;
		FCameraCacheEntry cameraCacheCopy;
		{
			std::lock_guard<std::mutex> lock(entityListMutex);
			EntityList_Copy = entityList;
			cameraCacheCopy = cameraCache;
		}
		for (size_t i = 0; i < EntityList_Copy.size(); i++)
		{
			auto Entity = EntityList_Copy[i];
			Vector3 Screen = WorldToScreen(cameraCacheCopy.POV, Entity.origin);
			Vector3 head = WorldToScreen(cameraCacheCopy.POV, Entity.TopLocation);
			if (!mate && Entity.name != "Killer")
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
