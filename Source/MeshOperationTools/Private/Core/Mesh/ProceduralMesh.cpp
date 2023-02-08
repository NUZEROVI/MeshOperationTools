//
//  ProceduralMesh.cpp
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#include "Core/Mesh/ProceduralMesh.h"

#include "Core/MeshComponent/MeshEdge.h"
#include "Core/MeshComponent/PrimitiveMeshComponent.h"

// Constructor -> PostInitialize -> BeginPlay
AProceduralMesh::AProceduralMesh()
{

	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
	RootComponent->SetWorldScale3D(FVector(1.f));

	// For 01_PlaneMesh Demo Material
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/DefaultResources/PlaneMesh.PlaneMesh"));
	// UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"), NULL, LOAD_None, NULL);

	// For 02_HeartMesh Demo (FBX 3D Model)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/DefaultResources/Heart.Heart"));
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/DefaultResources/Heart_Material.Heart_Material"), NULL, LOAD_None, NULL);

	// Create Procedural Mesh and Attach to Root
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	if (StaticMesh.Succeeded()) {
		ProceduralMesh->SetMaterial(0, Material);
	}
	Mesh = StaticMesh.Object;
	ProceduralMesh->bUseAsyncCooking = true;
	ProceduralMesh->SetupAttachment(RootComponent);
	
	PrimitiveMeshComponent = CreateDefaultSubobject<UPrimitiveMeshComponent>(TEXT("MeshComponent"));
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
	for(int32 i = 0; i < Vertices.Num(); i++)
	{
		// Original Mesh Vertices
		Vertices[i] = Resource.VertexBuffers.PositionVertexBuffer.VertexPosition(i);

		// Initialized UMeshVertex
		PrimitiveMeshComponent->InitializedVertices(this, i , Vertices[i], Vertices[i], FVector2D(Vertices[i].X, Vertices[i].Y));
	}
	
	// Initialized UMeshTriangle and MeshEdges InitializedTriangle()
	for(int32 i = 0; i < Indices.Num(); i += 3) { PrimitiveMeshComponent->InitializedTriangle(Indices[i], Indices[i + 1], Indices[i + 2]);	}
	
	// Set OriginalEdge
	for (TPair<int32, TTuple<int32, int32, UMeshEdge*>>& MeshEdge : PrimitiveMeshComponent->GetEdges()) { MeshEdge.Value.Get<2>()->SetOriEdge(true); }

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

void AProceduralMesh::UpdateNewMeshArrays(TArray<FVector> NewMeshVertices, TArray<FVector2D> NewMeshUVs, TArray<int32> NewMeshTriangles)
{
	NewVertices = NewMeshVertices;
	NewIndices = NewMeshTriangles;
	
	TArray<FVector> NewNormals;
	TArray<FVector> Normals;
	TArray<FLinearColor> VertexColors;

	VertexColors.AddUninitialized(NewVertices.Num());
	for (int32 i = 0; i < NewIndices.Num(); i += 3)
	{
		VertexColors[NewIndices[i]] = FLinearColor::Gray;
		VertexColors[NewIndices[i + 1]] = FLinearColor::Gray;
		VertexColors[NewIndices[i + 2]] = FLinearColor::Gray;
		NewNormals.Add(FVector::CrossProduct(NewVertices[NewIndices[i + 2]] - NewVertices[NewIndices[i]], NewVertices[NewIndices[i + 1]] - NewVertices[NewIndices[i]]).GetSafeNormal());
	}
	
	ProceduralMesh->ClearMeshSection(0);
	ProceduralMesh->CreateMeshSection_LinearColor(0, NewVertices, NewIndices, NewNormals, NewMeshUVs, VertexColors, TArray<FProcMeshTangent>(), true);
	ProceduralMesh->ContainsPhysicsTriMeshData(true);
}
