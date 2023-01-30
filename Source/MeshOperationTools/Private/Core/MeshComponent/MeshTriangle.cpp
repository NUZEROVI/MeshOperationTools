//
//  MeshTriangle.cpp
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#include "Core/MeshComponent/MeshTriangle.h"

UMeshTriangle::UMeshTriangle()
{
	IsNull = true;
}

void UMeshTriangle::Initializer(AProceduralMesh* Mesh, const int& V0Index, const int& V1Index, const int& V2Index)
{
	_Mesh = Mesh;
	Vertex0Index = V0Index;
	Vertex1Index = V1Index;
	Vertex2Index = V2Index;
	IsNull = false;
}

TArray<UMeshEdge*> UMeshTriangle::GetEdge()
{
	TArray<UMeshEdge*> Edges;
	UMeshEdge* Edges0;
	UMeshEdge* Edges1;
	UMeshEdge* Edges2;
	
	for (TTuple<int32, int32, int32, UMeshEdge*> EdgesData : GetMeshComponent()->GetEdges())
	{
		if(EdgesData.Get<1>() == Vertex0Index && EdgesData.Get<2>() == Vertex1Index) { Edges0 = EdgesData.Get<3>(); }
		if(EdgesData.Get<1>() == Vertex1Index && EdgesData.Get<2>() == Vertex2Index) { Edges1 = EdgesData.Get<3>(); }
		if(EdgesData.Get<1>() == Vertex2Index && EdgesData.Get<2>() == Vertex0Index) { Edges2 = EdgesData.Get<3>(); }
	}
	Edges.Add(Edges0);
	Edges.Add(Edges1);
	Edges.Add(Edges2);
	return Edges;
}

void UMeshTriangle::Dispose()
{
	if(_IsDisposed == false)
	{
		_Mesh = nullptr;
	}
	_IsDisposed = true;
}

void UMeshTriangle::BeginPlay()
{
	Super::BeginPlay();
}

void UMeshTriangle::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}



