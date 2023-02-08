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

void UMeshTriangle::Initializer(const int& V0Index, const int& V1Index, const int& V2Index)
{
	Vertex0Index = V0Index;
	Vertex1Index = V1Index;
	Vertex2Index = V2Index;
	IsNull = false;
}

TArray<UMeshEdge*> UMeshTriangle::GetEdgeFromTriangle()
{
	TArray<UMeshEdge*> Edges;
	UMeshEdge* Edges0;
	UMeshEdge* Edges1;
	UMeshEdge* Edges2;
	
	for (TPair<int32, TTuple<int32, int32, UMeshEdge*>>& MeshEdge : GetEdges())
	{
		if(MeshEdge.Value.Get<0>() == Vertex0Index && MeshEdge.Value.Get<1>() == Vertex1Index) { Edges0 = MeshEdge.Value.Get<2>(); }
		if(MeshEdge.Value.Get<0>() == Vertex1Index && MeshEdge.Value.Get<1>() == Vertex2Index) { Edges1 = MeshEdge.Value.Get<2>(); }
		if(MeshEdge.Value.Get<0>() == Vertex2Index && MeshEdge.Value.Get<1>() == Vertex0Index) { Edges2 = MeshEdge.Value.Get<2>(); }
	}
	Edges.Add(Edges0);
	Edges.Add(Edges1);
	Edges.Add(Edges2);
	return Edges;
}

void UMeshTriangle::ShuffleVertexIndices(const int& V0Index, const int& V1Index)
{
	if(Vertex1Index == V0Index && Vertex2Index == V1Index)
	{
		int32 Temp = Vertex1Index;
		Vertex1Index = Vertex2Index;
		Vertex2Index  = Vertex0Index;
		Vertex0Index = Temp; 
	}
	else if (Vertex0Index == V0Index && Vertex2Index == V1Index)
	{
		int32 Temp = Vertex2Index;
		Vertex2Index = Vertex1Index;
		Vertex1Index  = Vertex0Index;
		Vertex0Index = Temp; 
	};
}

void UMeshTriangle::Dispose()
{
	if(IsDisposed == false)
	{
		SetMesh(nullptr);
	}
	IsDisposed = true;
}

void UMeshTriangle::BeginPlay()
{
	Super::BeginPlay();
}

void UMeshTriangle::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}



