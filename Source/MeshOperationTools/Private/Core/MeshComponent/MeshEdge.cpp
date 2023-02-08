//
//  MeshEdge.cpp
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#include "Core/MeshComponent/MeshEdge.h"

#include "Core/MeshComponent/MeshVertex.h"

UMeshEdge::UMeshEdge()
{
	IsNull = true;
}

void UMeshEdge::Initializer(const int& V0, const int& V1)
{
	Vertex0Index = V0;
	Vertex1Index = V1;
	IsNull = false;
}

TArray<UMeshTriangle*> UMeshEdge::GetTriangles() const
{
	return MeshTriangles;
}

float UMeshEdge::GetCurrentLength()
{
	
	FVector V0 = GetVertices()[Vertex0Index]->CurrentPosition;
	FVector V1 = GetVertices()[Vertex1Index]->CurrentPosition;
	
	return (V0 - V1).Size(); 
}

float UMeshEdge::GetInitialLength()
{
	FVector V0 = GetVertices()[Vertex0Index]->OriginalPosition;
	FVector V1 = GetVertices()[Vertex1Index]->OriginalPosition;
	
	return (V0 - V1).Size(); 
}

void UMeshEdge::SetTriangle(UMeshTriangle* MeshTriangle)
{
	MeshTriangles.Add(MeshTriangle);
}

void UMeshEdge::Dispose()
{
	if(IsDisposed == false)
	{
		MeshTriangles.Empty();
		SetMesh(nullptr);
	}
	IsDisposed = true;
}

void UMeshEdge::BeginPlay()
{
	Super::BeginPlay();
}

void UMeshEdge::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


