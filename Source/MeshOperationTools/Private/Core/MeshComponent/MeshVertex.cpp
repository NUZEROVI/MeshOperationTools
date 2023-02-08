//
//  MeshVertex.cpp
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#include "Core/MeshComponent/MeshVertex.h"

#include "Core/MeshComponent/MeshEdge.h"
#include "Core/MeshComponent/MeshTriangle.h"

UMeshVertex::UMeshVertex()
{
	IsNull = true;
}

void UMeshVertex::Initializer(const int& Index, const FVector& CurrPos, const FVector& OriPos, const FVector2D& MeshUVs)
{
	VertexIndex = Index;
	CurrentPosition = CurrPos;
	OriginalPosition = OriPos;
	UVs = MeshUVs;
	IsNull = false;
}

TArray<UMeshTriangle*> UMeshVertex::GetTriangles() const
{
	return MeshTriangles;
}

TArray<UMeshEdge*> UMeshVertex::GetEdgeFromTriangle()
{
	TArray<UMeshEdge*> Edges;
		
	for (UMeshTriangle* MeshTriangle : GetTriangles())
	{
		for (UMeshEdge* MeshEdge : MeshTriangle->GetEdgeFromTriangle())
		{
			if(MeshEdge->Vertex0Index == VertexIndex || MeshEdge->Vertex1Index == VertexIndex)
			{
				Edges.Add(MeshEdge);
			}
		}
	}
	return Edges;
}

void UMeshVertex::SetTriangle(UMeshTriangle* MeshTriangle)
{
	MeshTriangles.Add(MeshTriangle);
}

void UMeshVertex::BeginPlay()
{
	Super::BeginPlay();
}

void UMeshVertex::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


