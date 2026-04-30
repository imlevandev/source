#pragma once
#include <iostream>

namespace offsets
{
    uintptr_t GWorld                = 0xD4FD2F8; // Off::InSDK::World::GWorld
    uintptr_t GNames                = 0xD224140; // Off::InSDK::NameArray::GNames
    uintptr_t GObjects              = 0xD32C560; // Off::InSDK::ObjArray::GObjects

    uintptr_t OwningGameInstance    = 0x250;     // World::OwningGameInstance
    uintptr_t LocalPlayers          = 0x58;      // GameInstance::LocalPlayers
    uintptr_t PlayerController      = 0x50;      // Player::PlayerController
    uintptr_t PlayerControlRotation = 0x370;     // Controller::ControlRotation

    uintptr_t GroupID               = 0x8C;      // Actor::RayTracingGroupId

    uintptr_t CharacterMesh         = 0x370;     // Character::Mesh
    uintptr_t AcknowledgedPawn      = 0x3A0;     // PlayerController::AcknowledgedPawn
    uintptr_t PlayerCameraManager   = 0x3B0;     // PlayerController::PlayerCameraManager
    uintptr_t CameraCachePrivate    = 0x15A0;    // PlayerCameraManager::CameraCachePrivate

    // Camera / POV / FOV
    uintptr_t ADBDPlayer_CameraOffset                                      = 0xA58;
    uintptr_t APlayerCameraManager_DefaultFOVOffset                        = 0x30C;
    uintptr_t APlayerCameraManager_ViewTargetOffset                        = 0x390;
    uintptr_t APlayerCameraManager_CameraCachePrivateOffset                = 0x15A0;
    uintptr_t FCameraCacheEntry_POVOffset                                  = 0x10;
    uintptr_t FMinimalViewInfo_LocationOffset                              = 0x0;
    uintptr_t FMinimalViewInfo_RotationOffset                              = 0x18;
    uintptr_t FMinimalViewInfo_FOVOffset                                   = 0x30;
    uintptr_t FMinimalViewInfo_DesiredFOVOffset                            = 0x34;
    uintptr_t FMinimalViewInfo_FirstPersonFOVOffset                        = 0x38;
    uintptr_t UCameraComponent_FieldOfViewOffset                           = 0x280;
    uintptr_t UCameraComponent_FirstPersonFieldOfViewOffset                = 0x284;
    uintptr_t UCameraComponent_FirstPersonScaleOffset                      = 0x288;
    uintptr_t UCameraComponent_bEnableFirstPersonFieldOfViewOffset         = 0x2C2;

    uintptr_t PlayerState           = 0x310;     // Pawn::PlayerState
    uintptr_t PlayerStateLocalPlayer= 0x2F8;     // Controller::PlayerState
    uintptr_t RootComponent         = 0x1E0;     // Actor::RootComponent
    uintptr_t RelativeLocation      = 0x178;     // SceneComponent::RelativeLocation
    uintptr_t RelativeRotation      = 0x190;     // SceneComponent::RelativeRotation
    uintptr_t PersistentLevel       = 0x50;      // World::PersistentLevel

    uintptr_t LODData               = 0x678;     // StaticMeshComponent::LODData
    uintptr_t AGenerator_GeneratorChargeable = 0x508; // AGenerator::_generatorChargeable

    uintptr_t PlayerNamePrivate     = 0x390;     // PlayerState::PlayerNamePrivate
    uintptr_t ActorArray            = 0xC0;      // Off::InSDK::ULevel::Actors
    uintptr_t ActorCount            = 0x8;       // TArray::Num, relative to ActorArray
    uintptr_t ActorID               = 0x18;      // Off::UObject::Name

    uintptr_t ComponentToWorld      = 0x1F0;     // SceneComponent::ComponentToWorld
}
