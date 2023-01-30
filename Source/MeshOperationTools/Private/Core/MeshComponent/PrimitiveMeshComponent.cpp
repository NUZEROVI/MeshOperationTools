//
//  PrimitiveMeshComponent.cpp
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#include "Core/MeshComponent/PrimitiveMeshComponent.h"
#include "Core/MeshComponent/MeshVertex.h"
#include "Core/MeshComponent/MeshTriangle.h"
#include "Core/MeshComponent/MeshEdge.h"
#include "Core/MeshComponent/ProcedureMeshVertexPair.h"

UPrimitiveMeshComponent::UPrimitiveMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	PrimitiveMeshVertex = CreateDefaultSubobject<UMeshVertex>(TEXT("PrimitiveMeshVertex"));
	PrimitiveMeshVertex->SetMeshComponent(this);
	
	PrimitiveMeshTriangle = CreateDefaultSubobject<UMeshTriangle>(TEXT("PrimitiveMeshTriangle"));
	PrimitiveMeshTriangle->SetMeshComponent(this);
	
	PrimitiveMeshEdge = CreateDefaultSubobject<UMeshEdge>(TEXT("PrimitiveMeshEdge"));
	PrimitiveMeshEdge->SetMeshComponent(this);
}

void UPrimitiveMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPrimitiveMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPrimitiveMeshComponent::InitializedVertices(AProceduralMesh* Mesh, const int& Index, const FVector& CurrPos, const FVector& OriPos, const FVector2D& MeshUVs)
{
	_Mesh = Mesh;
	UMeshVertex* MeshVertex = NewObject<UMeshVertex>();
	MeshVertex->Initializer(Mesh, Index , CurrPos, OriPos, FVector2D(CurrPos.X, CurrPos.Y));
	SetVertices(MeshVertex);
}

UMeshTriangle* UPrimitiveMeshComponent::InitializedTriangle(int V0Index, int V1Index, int V2Index)
{
	if((V0Index < 0 || V0Index >= _Vertices.Num())) return NewObject<UMeshTriangle>();
	if((V1Index < 0 || V1Index >= _Vertices.Num())) return NewObject<UMeshTriangle>();
	if((V2Index < 0 || V2Index >= _Vertices.Num())) return NewObject<UMeshTriangle>();

	UMeshTriangle* MeshTriangle = NewObject<UMeshTriangle>();
	MeshTriangle->Initializer(_Mesh, V0Index, V1Index, V2Index);
	
	// Add Triangle (in this Mesh and UMeshVertex)
	SetTriangle(MeshTriangle);
	_Vertices[V0Index]->SetTriangle(MeshTriangle);
	_Vertices[V1Index]->SetTriangle(MeshTriangle);
	_Vertices[V2Index]->SetTriangle(MeshTriangle);

	// Set Edges
	UProcedureMeshVertexPair* Pair1 = NewObject<UProcedureMeshVertexPair>();
	Pair1->Initializer(V0Index, V1Index);
	AddTriangleToEdge(_Mesh, Pair1, MeshTriangle);
	UProcedureMeshVertexPair* Pair2 = NewObject<UProcedureMeshVertexPair>();
	Pair2->Initializer(V1Index, V2Index);
	AddTriangleToEdge(_Mesh, Pair2, MeshTriangle);
	UProcedureMeshVertexPair* Pair3 = NewObject<UProcedureMeshVertexPair>();
	Pair3->Initializer(V2Index, V0Index);
	AddTriangleToEdge(_Mesh, Pair3, MeshTriangle);

	return MeshTriangle;
}

void UPrimitiveMeshComponent::AddTriangleToEdge(AProceduralMesh* Mesh, UProcedureMeshVertexPair* MeshVertexPair, UMeshTriangle* MeshTriangle)
{
	bool ContainKey = false;
	
	// To check if 'MeshEdge' exists.
	for (TTuple<int32, int32, int32, UMeshEdge*> MeshEdge : GetEdges())
	{
		int32  V0Index = MeshEdge.Get<1>();
		int32  V1Index = MeshEdge.Get<2>();
		if((V0Index == MeshVertexPair->Vertex0Index && V1Index == MeshVertexPair->Vertex1Index))
		{
			ContainKey = true; // 'MeshEdge' exists.
			break;
		}
	}

	// 'MeshEdge' not exists.
	if(ContainKey == false)
	{
		UMeshEdge* MeshEdges = NewObject<UMeshEdge>();
		MeshEdges->Initializer(Mesh, MeshVertexPair->Vertex0Index, MeshVertexPair->Vertex1Index);
		SetEdges(MakeTuple(_Edges.Num(), MeshVertexPair->Vertex0Index, MeshVertexPair->Vertex1Index, MeshEdges));
		MeshEdges->SetTriangle(MeshTriangle);
	}
}

