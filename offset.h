#pragma once
#include <iostream>

namespace offsets
{
    uintptr_t GWorld                = 0xD4FC2F8; // Off::InSDK::World::GWorld
    uintptr_t GNames                = 0xD223140; // Off::InSDK::NameArray::GNames
    uintptr_t GObjects              = 0xD32B560; // Off::InSDK::ObjArray::GObjects

    uintptr_t OwningGameInstance    = 0x250;     // World::OwningGameInstance
    uintptr_t LocalPlayers          = 0x58;      // GameInstance::LocalPlayers
    uintptr_t PlayerController      = 0x50;      // Player::PlayerController
    uintptr_t PlayerControlRotation = 0x370;     // Controller::ControlRotation

    // MISSING GroupID (Actor::GroupId|GroupID|GroupIDComponent|Groups)

    uintptr_t CharacterMesh         = 0x370;     // Character::Mesh
    uintptr_t AcknowledgedPawn      = 0x3A0;     // PlayerController::AcknowledgedPawn
    uintptr_t PlayerCameraManager   = 0x3B0;     // PlayerController::PlayerCameraManager
    uintptr_t CameraCachePrivate    = 0x15A0;    // PlayerCameraManager::CameraCachePrivate

    uintptr_t PlayerState           = 0x310;     // Pawn::PlayerState
    uintptr_t PlayerStateLocalPlayer= 0x2F8;     // Controller::PlayerState
    uintptr_t RootComponent         = 0x1E0;     // Actor::RootComponent
    uintptr_t RelativeLocation      = 0x178;     // SceneComponent::RelativeLocation
    uintptr_t RelativeRotation      = 0x190;     // SceneComponent::RelativeRotation
    uintptr_t PersistentLevel       = 0x50;      // World::PersistentLevel

    // MISSING LODData (MeshComponent::LODData)

    uintptr_t PlayerNamePrivate     = 0x390;     // PlayerState::PlayerNamePrivate
    uintptr_t ActorArray            = 0xC0;      // Off::InSDK::ULevel::Actors
    uintptr_t ActorCount            = 0x8;       // TArray::Num, relative to ActorArray
    uintptr_t ActorID               = 0x18;      // Off::UObject::Name

    // MISSING ComponentToWorld (SceneComponent::ComponentToWorld) thay offset mới vào 
}
