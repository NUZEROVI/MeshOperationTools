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

void UMeshEdge::Initializer(AProceduralMesh* Mesh, const int& V0, const int& V1)
{
	_Mesh = Mesh;
	Vertex0Index = V0;
	Vertex1Index = V1;
	IsNull = false;
}

float UMeshEdge::GetCurrentLength()
{
	
	FVector V0 = GetMeshComponent()->GetVertices()[Vertex0Index]->CurrentPosition;
	FVector V1 = GetMeshComponent()->GetVertices()[Vertex1Index]->CurrentPosition;
	
	return (V0 - V1).Size(); 
}

float UMeshEdge::GetInitialLength()
{
	FVector V0 = GetMeshComponent()->GetVertices()[Vertex0Index]->OriginalPosition;
	FVector V1 = GetMeshComponent()->GetVertices()[Vertex1Index]->OriginalPosition;
	
	return (V0 - V1).Size(); 
}

void UMeshEdge::Dispose()
{
	if(_IsDisposed == false)
	{
		_Triangles.Empty();
		_Mesh = nullptr;
	}
	_IsDisposed = true;
}

void UMeshEdge::BeginPlay()
{
	Super::BeginPlay();
}

void UMeshEdge::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


