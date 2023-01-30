//
//  ProceduralMesh.cpp
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#include "Core/Mesh/ProceduralMesh.h"

// Constructor -> PostInitialize -> BeginPlay
AProceduralMesh::AProceduralMesh()
{

	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
	RootComponent->SetWorldScale3D(FVector(1.f));

	// For 01_PlaneMesh Demo Material
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/DefaultResources/PlaneMesh.PlaneMesh"));
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"), NULL, LOAD_None, NULL);

	// Create Procedural Mesh and Attach to Root
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	if (StaticMesh.Succeeded()) {
		ProceduralMesh->SetMaterial(0, Material);
	}
	Mesh = StaticMesh.Object;
	ProceduralMesh->bUseAsyncCooking = true;
	ProceduralMesh->SetupAttachment(RootComponent);

}

void AProceduralMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProceduralMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProceduralMesh::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Load mesh data from Resource.
	LoadMeshVertices();
}

void AProceduralMesh::LoadMeshVertices()
{
	TArray<FLinearColor> VertexColors;
	TArray<FVector> Normals;

	FStaticMeshLODResources& Resource = Mesh->RenderData->LODResources[0];
	
	if (IsValid(Mesh))
	{
		TArray<uint32> _Indices;
		Resource.IndexBuffer.GetCopy(_Indices);
		Indices.Append(_Indices);
	}
	
	Vertices.AddUninitialized(Resource.VertexBuffers.PositionVertexBuffer.GetNumVertices());
	VertexColors.AddUninitialized(Vertices.Num());
	for(int i = 0; i < Vertices.Num(); i++)
	{
		// Original Mesh Vertices
		Vertices[i] = Resource.VertexBuffers.PositionVertexBuffer.VertexPosition(i);
	}

	ProceduralMesh->CreateMeshSection_LinearColor(
		0,
		Vertices,
		Indices,
		TArray<FVector>(),
		TArray<FVector2D>(),
		TArray<FLinearColor>(),
		TArray<FProcMeshTangent>(),
		true
	);
	ProceduralMesh->ContainsPhysicsTriMeshData(true);
}

