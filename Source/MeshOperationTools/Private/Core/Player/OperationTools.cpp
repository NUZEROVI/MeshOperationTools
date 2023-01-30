//
//  OperationTools.cpp
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#include "Core/Player/OperationTools.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

AOperationTools::AOperationTools()
{
	// Set Defaults
	DefaultCameraArmLength = 0.f;
	DefaultCameraRotation = FRotator(-90.f, 0.f, 0.f);

	// Set Root Component
	CollisionCylinder = GetCapsuleComponent();
	SetRootComponent(CollisionCylinder);
	CollisionCylinder->InitCapsuleSize(40.f, 108.f);
	CollisionCylinder->SetWorldScale3D(FVector(1.f));

	// Default Settings for inheriting controller rotations
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Create CameraArm and Attach to Root
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->bUsePawnControlRotation  = false;
	CameraArm->TargetArmLength = DefaultCameraArmLength;
	CameraArm->TargetOffset = FVector(0.f, 0.f, 0.f);
	CameraArm->SetRelativeRotation(DefaultCameraRotation);
	CameraArm->bDoCollisionTest = true; 
	CameraArm->bEnableCameraLag = true;
	CameraArm->bEnableCameraRotationLag = false; 
	CameraArm->bInheritPitch = false; 

	// Create FirstPersonCamera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FirstPersonCameraComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	FirstPersonCameraComponent->FieldOfView = 15.f;
	
	PrimaryActorTick.bCanEverTick = true;

}

void AOperationTools::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOperationTools::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if(PlayerController)
	{
		PlayerController->bShowMouseCursor = true; 
		PlayerController->bEnableClickEvents = true; 
		PlayerController->bEnableMouseOverEvents = true;
	}
}

void AOperationTools::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AOperationTools::ForcepsToolOn);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AOperationTools::ForcepsToolOff);

}

void AOperationTools::ForcepsToolOn()
{
}

void AOperationTools::ForcepsToolOff()
{
}

void AOperationTools::ScalpelTool()
{
}

