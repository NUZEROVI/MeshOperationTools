//
//  OperationTools.cpp
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#include "Core/Player/OperationTools.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Core/MeshComponent/MeshTriangle.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	
	// Create ProceduralMesh (Constructor -> PostInitialize -> BeginPlay)
	UWorld* const World = GetWorld();
	if(World)
	{
		ProceduralMesh = GetWorld()->SpawnActor<AProceduralMesh>(AProceduralMesh::StaticClass(), FVector(0), FRotator(0), FActorSpawnParameters());
		PrimitiveMeshComponent = ProceduralMesh->GetPrimitiveMeshComponent();
	}
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	if(ProceduralMesh == nullptr)
	{
		for (AActor* Actor : Actors)
		{
			ProceduralMesh = Cast<AProceduralMesh>(Actor);
			if(ProceduralMesh)
			{
				//UE_LOG(LogTemp, Log, TEXT("Cast Actor: %s"), *ProceduralMesh->GetName());
				break;
			}
		}
	}	
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

	int32 ScaleX, ScaleY;
	FVector2D TickCursor, NewCursorPos;
	PlayerController->GetViewportSize(ScaleX, ScaleY);
	PlayerController->GetMousePosition(TickCursor.X, TickCursor.Y);

	if(CursorPos.X != -1000.0f && CursorPos.Y != -1000.f)
	{
		if(TickCursor.X != OriCursorPos.X && TickCursor.Y != OriCursorPos.Y)
		{
			IsMove = true;
		}
	}
	
	NewCursorPos.X = ((TickCursor.X / ScaleX) - 0.5f) * 2;
	NewCursorPos.Y = ((TickCursor.Y / ScaleY) - 0.5f) * 2;
	
	TArray<FVector> Vertices;
	TArray<FVector2D> UVs;
	TArray<int> Triangles;

	if(IsDrag)
	{
		if(!DragVertex->IsNull)
		{
			if(DraggingHeight < MaxHeight)
			{
				DraggingHeight = FMath::Min(MaxHeight, DraggingHeight + DeltaTime * MaxHeight);
			}
			
			FVector NewPosition = FVector(-NewCursorPos.Y, NewCursorPos.X,  (OriginalVertexHeight + DraggingHeight)); 
			
			FVector VertexForce = FVector(0);
			VertexForce.Normalize();
			FVector NewForce = NewPosition - DragVertex->CurrentPosition;
	
			float NewForceProjectedMagnitude = FVector::DotProduct(NewForce, VertexForce);
	
			if(NewForceProjectedMagnitude < 0.0f)
			{
				float Multiplier = FMath::Clamp(0.5f - 0.33f * UKismetMathLibrary::Atan(VertexForce.Size() - MaximumPullForce), 0.0f , 1.0f);
				NewPosition = DragVertex->CurrentPosition + NewForce * Multiplier;
			}
	
			TArray<int32> NewTriangles;
			if(Vertices.Num() == 0 || Vertices.Num() < PrimitiveMeshComponent->GetVertices().Num())
			{
				Vertices.Init(FVector(), PrimitiveMeshComponent->GetVertices().Num());
			}
			if(UVs.Num() == 0 || UVs.Num() < PrimitiveMeshComponent->GetVertices().Num())
			{
				UVs.Init(FVector2D(), PrimitiveMeshComponent->GetVertices().Num());
			}
			
			for(int i = 0; i < PrimitiveMeshComponent->GetVertices().Num(); ++i)
			{
				if(PrimitiveMeshComponent->GetVertices()[i]->IsNull) continue;
				if(i == PrimitiveMeshComponent->GetDragVertexIndex() && (PrimitiveMeshComponent->GetIsOriVertices() == false || IsMove == true)) // Drag Vertex
				{
					Vertices[i] = NewPosition;
					PrimitiveMeshComponent->GetVertices()[i]->CurrentPosition = Vertices[i]; 
					
				}else
				{
					Vertices[i] = PrimitiveMeshComponent->GetVertices()[i]->CurrentPosition; 
				}
				
				UVs[i] = PrimitiveMeshComponent->GetVertices()[i]->UVs;
				NewTriangles.Add(PrimitiveMeshComponent->GetVertices()[i]->VertexIndex);
			}
	
			Triangles.Init(int32(), PrimitiveMeshComponent->GetTriangles().Num() * 3);
			for(int j = 0; j < PrimitiveMeshComponent->GetTriangles().Num(); ++j)
			{
				Triangles[3 * j + 0] = PrimitiveMeshComponent->GetTriangles()[j]->Vertex0Index;;
				Triangles[3 * j + 1] = PrimitiveMeshComponent->GetTriangles()[j]->Vertex1Index;
				Triangles[3 * j + 2] = PrimitiveMeshComponent->GetTriangles()[j]->Vertex2Index;
			}
			
			ProceduralMesh->UpdateNewMeshArrays(Vertices, UVs, Triangles);
		
		}
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
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if(PlayerController)
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if(LocalPlayer && LocalPlayer->ViewportClient)
		{
			if(PlayerController->DeprojectMousePositionToWorld(Pos, Dir))
			{
				FVector StartLocation = Pos;
				FVector EndLocation = StartLocation + (Dir * 10000);
		 		
				UE_LOG(LogTemp, Log, TEXT("Pos location: %s"), *Pos.ToString());
		 	
				FHitResult HitResult(ForceInit);
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);
		 		
				GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);
				if(Cast<AActor>(HitResult.GetActor()))
				{
					if(HitResult.GetActor()->GetName() == ProceduralMesh->GetName())
					{
						int32 ScaleX, ScaleY;
						PlayerController->GetViewportSize(ScaleX, ScaleY);
						PlayerController->GetMousePosition(CursorPos.X, CursorPos.Y);
						OriCursorPos = FVector2D(CursorPos.X, CursorPos.Y);
						IsClick = true;
						IsDrag = true;
						DragVertex = NewObject<UMeshVertex>();
						if(DragVertex->IsNull)
						{
							DragVertex = PrimitiveMeshComponent->GetClosestVertexToLineSegment(StartLocation, EndLocation, SnapDistance);

							if(!DragVertex->IsNull)
							{
								OriginalVertexHeight = DragVertex->CurrentPosition.Z;
							}
							DraggingHeight = 0.0f;
						}
					}else
					{
						DragVertex = NewObject<UMeshVertex>();
					}
		 	
		 		
		 			
				}
			}
		}
	}
}

void AOperationTools::ForcepsToolOff()
{
	CursorPos = FVector2D(-1000.0f, -1000.0f);
	IsClick = false;
	IsMove = false;
	IsDrag = false;
	DragVertex = NewObject<UMeshVertex>();
}

void AOperationTools::ScalpelTool()
{
}

