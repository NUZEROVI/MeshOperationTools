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
	ProceduralMesh = Mesh;
	UMeshVertex* MeshVertex = NewObject<UMeshVertex>();
	MeshVertex->Initializer(Index , CurrPos, OriPos, FVector2D(CurrPos.X, CurrPos.Y));
	SetVertices(MeshVertex);
}

UMeshTriangle* UPrimitiveMeshComponent::InitializedTriangle(int V0Index, int V1Index, int V2Index)
{
	if((V0Index < 0 || V0Index >= MeshVertices.Num())) return NewObject<UMeshTriangle>();
	if((V1Index < 0 || V1Index >= MeshVertices.Num())) return NewObject<UMeshTriangle>();
	if((V2Index < 0 || V2Index >= MeshVertices.Num())) return NewObject<UMeshTriangle>();

	UMeshTriangle* MeshTriangle = NewObject<UMeshTriangle>();
	MeshTriangle->Initializer(V0Index, V1Index, V2Index);
	
	// Add Triangle (in this Mesh and UMeshVertex)
	SetTriangle(MeshTriangle);
	MeshVertices[V0Index]->SetTriangle(MeshTriangle);
	MeshVertices[V1Index]->SetTriangle(MeshTriangle);
	MeshVertices[V2Index]->SetTriangle(MeshTriangle);

	// Set Edges
	UProcedureMeshVertexPair* Pair1 = NewObject<UProcedureMeshVertexPair>();
	Pair1->Initializer(V0Index, V1Index);
	AddTriangleToEdge(ProceduralMesh, Pair1, MeshTriangle);
	UProcedureMeshVertexPair* Pair2 = NewObject<UProcedureMeshVertexPair>();
	Pair2->Initializer(V1Index, V2Index);
	AddTriangleToEdge(ProceduralMesh, Pair2, MeshTriangle);
	UProcedureMeshVertexPair* Pair3 = NewObject<UProcedureMeshVertexPair>();
	Pair3->Initializer(V2Index, V0Index);
	AddTriangleToEdge(ProceduralMesh, Pair3, MeshTriangle);

	return MeshTriangle;
}

void UPrimitiveMeshComponent::AddTriangleToEdge(AProceduralMesh* Mesh, UProcedureMeshVertexPair* MeshVertexPair, UMeshTriangle* MeshTriangle)
{
	bool ContainKey = false;
	
	// To check if 'MeshEdge' exists.
	for (TPair<int32, TTuple<int32, int32, UMeshEdge*>>& MeshEdge : MeshEdges)
	{
		int32  V0Index = MeshEdge.Value.Get<0>();
		int32  V1Index = MeshEdge.Value.Get<1>();
		if((V0Index == MeshVertexPair->Vertex0Index && V1Index == MeshVertexPair->Vertex1Index))
		{
			ContainKey = true; // 'MeshEdge' exists.
			break;
		}
	}

	// 'MeshEdge' not exists.
	if(ContainKey == false)
	{
		UMeshEdge* Edge = NewObject<UMeshEdge>();
		Edge->Initializer(MeshVertexPair->Vertex0Index, MeshVertexPair->Vertex1Index);
		SetEdges(MeshEdges.Num(), MakeTuple(MeshVertexPair->Vertex0Index, MeshVertexPair->Vertex1Index, Edge));
		Edge->SetTriangle(MeshTriangle);
	}
}

UMeshVertex* UPrimitiveMeshComponent::GetClosestVertexToLineSegment(FVector P, FVector Q, float SnapDist)
{
	FVector ClosestPoint;
	float Dist = 0.0f;
	float U = 0.0f;
	float V = 0.0f;
	UMeshTriangle* ClosestTriangle = GetClosestTriangleToLineSegment(P, Q, ClosestPoint, Dist, U, V);
	if (ClosestTriangle->IsNull) return NewObject<UMeshVertex>();
	
	UMeshVertex* ClosestVertex = GetVertexOnClosestEdge(ClosestTriangle, ClosestPoint, SnapDist);
	if(ClosestVertex->IsNull == false) return ClosestVertex;
	
	return CreateVertexInTriangle(ClosestTriangle, U, V);
}

UMeshTriangle* UPrimitiveMeshComponent::GetClosestTriangleToLineSegment(FVector P, FVector Q, FVector& ClosestPoint, float& Dist, float& U, float& V)
{
	ClosestPoint = FVector(0);
	Dist = 0.0f;
	U = 0.0f;
	V = 0.0f;
	UMeshTriangle* ClosestTriangle = NewObject<UMeshTriangle>();
	float ClosestSquareDistance = TNumericLimits<float>::Max();

	for (UMeshTriangle* MeshTriangle : GetTriangles())
	{
		
		bool InsideTriangle;
		FVector IntersectionPoint, ClosestTriNormal;
		InsideTriangle = FMath::SegmentTriangleIntersection(P, Q,
			 MeshVertices[MeshTriangle->Vertex0Index]->CurrentPosition,
			MeshVertices[MeshTriangle->Vertex1Index]->CurrentPosition,
			MeshVertices[MeshTriangle->Vertex2Index]->CurrentPosition,
			IntersectionPoint,
			ClosestTriNormal);

		if(InsideTriangle)
		{
			float SqrDistance = (P - IntersectionPoint).SizeSquared() ;
			if(SqrDistance < ClosestSquareDistance)
			{
				ClosestPoint = IntersectionPoint;
				ClosestTriangle = MeshTriangle;
				ClosestTriangle->IsNull = false;
				ClosestSquareDistance = SqrDistance;
				
				float Matrix00 = MeshVertices[MeshTriangle->Vertex2Index]->CurrentPosition.X - MeshVertices[MeshTriangle->Vertex0Index]->CurrentPosition.X;
				float Matrix11 = MeshVertices[MeshTriangle->Vertex1Index]->CurrentPosition.Y - MeshVertices[MeshTriangle->Vertex0Index]->CurrentPosition.Y;
				float Matrix22 = P.Z - Q.Z;
				float Matrix12 = P.Y - Q.Y;
				float Matrix02 = P.X - Q.X;
				if(FMath::IsNearlyEqual(Matrix00, 0.0f) || FMath::IsNearlyEqual(Matrix11, 0.0f) || FMath::IsNearlyEqual(Matrix22, 0.0f)
					|| FMath::IsNearlyEqual(Matrix12, 0.0f) || FMath::IsNearlyEqual(Matrix02, 0.0f))
				{
					U = 0.0f;
					V = 0.0f;
					return ClosestTriangle;
				}

				U = ( P.X - (MeshVertices[MeshTriangle->Vertex0Index]->CurrentPosition.X) ) / ( (MeshVertices[MeshTriangle->Vertex2Index]->CurrentPosition.X) - (MeshVertices[MeshTriangle->Vertex0Index]->CurrentPosition.X));
				V = ( P.Y - (MeshVertices[MeshTriangle->Vertex0Index]->CurrentPosition.Y) ) / ( (MeshVertices[MeshTriangle->Vertex1Index]->CurrentPosition.Y) - (MeshVertices[MeshTriangle->Vertex0Index]->CurrentPosition.Y));
			}
		}
	}
	
	if(ClosestTriangle->IsNull == false)
	{
		Dist = FMath::Sqrt(ClosestSquareDistance);
	}
	
	return ClosestTriangle;
}

UMeshVertex* UPrimitiveMeshComponent::GetVertexOnClosestEdge(UMeshTriangle* ClosestTri, FVector ClosestPoint, float SnapDist)
{
	float SqrSnapDistance = SnapDist * SnapDist;
	
	UMeshVertex* V0 = MeshVertices[ClosestTri->Vertex0Index];
	UMeshVertex* V1 = MeshVertices[ClosestTri->Vertex1Index]; 
	UMeshVertex* V2 = MeshVertices[ClosestTri->Vertex2Index];

	float t0, t1, t2;
	FVector P0 = FMath::ClosestPointOnSegment(ClosestPoint, V1->CurrentPosition, V2->CurrentPosition);
	t0  = FVector::Distance(ClosestPoint, P0);
	FVector P1 = FMath::ClosestPointOnSegment(ClosestPoint, V2->CurrentPosition, V0->CurrentPosition); 
	t1  = FVector::Distance(ClosestPoint, P1);
	FVector P2 = FMath::ClosestPointOnSegment(ClosestPoint, V0->CurrentPosition, V1->CurrentPosition); 
	t2  = FVector::Distance(ClosestPoint, P2);

	float SqrDistanceToP0 = (ClosestPoint - P0).SizeSquared() ;
	float SqrDistanceToP1 = (ClosestPoint - P1).SizeSquared() ;
	float SqrDistanceToP2 = (ClosestPoint - P2).SizeSquared() ;
	
	float MinSqrDistance = FMath::Min(SqrDistanceToP0, SqrDistanceToP1);
	MinSqrDistance = FMath::Min(MinSqrDistance, SqrDistanceToP2);
	
	if (MinSqrDistance <= SqrSnapDistance)
	{
		if (FMath::IsNearlyEqual((MinSqrDistance - SqrDistanceToP0), 0.0f))
		{
			float sqrDistanceP0V1 = (P0 - V1->CurrentPosition).SizeSquared() ;
			float sqrDistanceP0V2 = (P0 - V2->CurrentPosition).SizeSquared() ;
			if (sqrDistanceP0V1 <= SqrSnapDistance && sqrDistanceP0V1 < sqrDistanceP0V2)
			{
				DragVertexIndex = ClosestTri->Vertex1Index;
				IsOriVertices = true;
				return V1;
			}
			if (sqrDistanceP0V2 <= SqrSnapDistance && sqrDistanceP0V2 < sqrDistanceP0V1)
			{
				DragVertexIndex = ClosestTri->Vertex2Index;
				IsOriVertices = true;
				return V2;
			}
			return CreateVertexOnEdge(V1, V2, t0);
		}
		
		if (FMath::IsNearlyEqual((MinSqrDistance - SqrDistanceToP1), 0.0f))
		{
			float sqrDistanceP1V0 = (P1 - V0->CurrentPosition).SizeSquared() ;
			float sqrDistanceP1V2 = (P1 - V2->CurrentPosition).SizeSquared() ;
			if (sqrDistanceP1V0 <= SqrSnapDistance && sqrDistanceP1V0 < sqrDistanceP1V2)
			{
				DragVertexIndex = ClosestTri->Vertex0Index;
				IsOriVertices = true;
				return V0;
			}
			if (sqrDistanceP1V2 <= SqrSnapDistance && sqrDistanceP1V2 < sqrDistanceP1V0)
			{
				DragVertexIndex = ClosestTri->Vertex2Index;
				IsOriVertices = true;
				return V2;
			}
			return CreateVertexOnEdge(V2, V0, t1);
		}
	
		if (FMath::IsNearlyEqual((MinSqrDistance - SqrDistanceToP2), 0.0f))
		{
			float sqrDistanceP2V0 = (P2 - V0->CurrentPosition).SizeSquared() ;
			float sqrDistanceP2V1 = (P2 - V1->CurrentPosition).SizeSquared() ;
			if (sqrDistanceP2V0 <= SqrSnapDistance && sqrDistanceP2V0 < sqrDistanceP2V1)
			{
				DragVertexIndex = ClosestTri->Vertex0Index;
				IsOriVertices = true;
				return V0;
			}
			if (sqrDistanceP2V1 <= SqrSnapDistance && sqrDistanceP2V1 < sqrDistanceP2V0)
			{
				DragVertexIndex = ClosestTri->Vertex1Index;
				IsOriVertices = true;
				return V1;
			}
			return CreateVertexOnEdge(V0, V1, t2);
		}
	}

	return NewObject<UMeshVertex>();	
}

UMeshVertex* UPrimitiveMeshComponent::CreateVertexOnEdge(UMeshVertex* V0, UMeshVertex* V1, float T)
{
	bool ContainKey;
	ContainKey = false;
	int32  V0Index, V1Index;
	for (TPair<int32, TTuple<int32, int32, UMeshEdge*>>& Edge : MeshEdges)
	{
		V0Index = Edge.Value.Get<0>();
		V1Index = Edge.Value.Get<1>();
		
		if((V0Index == V0->VertexIndex) && (V1Index == V1->VertexIndex)) //Contain
		{
			ContainKey = true;
			
			bool OriginalEdge = Edge.Value.Get<2>()->OriginalEdge;
			TArray<UMeshTriangle*> TrianglesToReplace =  Edge.Value.Get<2>()->GetTriangles();
			
			int32 NewVertexIndex = MeshVertices.Num();
			UMeshVertex* MeshVertex = NewObject<UMeshVertex>();
			MeshVertex->Initializer(
				NewVertexIndex,
				(1 - T) * V0->CurrentPosition + T * V1->CurrentPosition, 
				(1 - T) * V0->OriginalPosition + T * V1->OriginalPosition,
				(1 - T) * V0->UVs + T * V1->UVs);
			
			if(MeshVertices.Contains(MeshVertex) == false)
			{
				SetVertices(MeshVertex);
			}else
			{
				int32 index;
				MeshVertices.Find(MeshVertex, index);
				NewVertexIndex = index ;
				DragVertexIndex  = index;
				IsOriVertices = true;
				return MeshVertex;
			}
	
			for (UMeshTriangle* MeshTriangleToReplace : TrianglesToReplace)
			{
				MeshTriangleToReplace->ShuffleVertexIndices(V0Index, V1Index);
				RemoveMeshTriangle(MeshTriangleToReplace);
				CreateNewTriangle(NewVertexIndex, MeshTriangleToReplace->Vertex1Index, MeshTriangleToReplace->Vertex2Index);
				CreateNewTriangle(NewVertexIndex, MeshTriangleToReplace->Vertex2Index, MeshTriangleToReplace->Vertex0Index);
			}
			RemoveUnusedEdges();

			for (TPair<int32, TTuple<int32, int32, UMeshEdge*>>& MeshEdge : MeshEdges)
			{
				V0Index = MeshEdge.Value.Get<0>();
				V1Index = MeshEdge.Value.Get<1>();
				if((V0Index == V0->VertexIndex) && (V1Index == NewVertexIndex))
				{
					MeshEdge.Value.Get<2>()->SetOriEdge(OriginalEdge);
				}
				if((V0Index == V1->VertexIndex) && (V1Index == NewVertexIndex))
				{
					MeshEdge.Value.Get<2>()->SetOriEdge(OriginalEdge);
				}
			}
			
			DragVertexIndex = NewVertexIndex;
			IsOriVertices = false;
			return MeshVertex;
		}
	}
	if(ContainKey == false) return NewObject<UMeshVertex>(); // No Contain Keys

	return NewObject<UMeshVertex>();
}

UMeshVertex* UPrimitiveMeshComponent::CreateVertexInTriangle(UMeshTriangle* MeshTri, float U, float V)
{
	UMeshVertex* V0 = MeshVertices[MeshTri->Vertex0Index];
	UMeshVertex* V1 = MeshVertices[MeshTri->Vertex1Index];
	UMeshVertex* V2 = MeshVertices[MeshTri->Vertex2Index];

	int NewVertexIndex = MeshVertices.Num();
	UMeshVertex* MeshVertex = NewObject<UMeshVertex>();
	MeshVertex->Initializer(
				NewVertexIndex,
				(1 - U - V) * V0->CurrentPosition + U * V2->CurrentPosition + V * V1->CurrentPosition,
				(1 - U - V) * V0->OriginalPosition + U * V2->OriginalPosition + V * V1->OriginalPosition,
				(1 - U - V) * V0->UVs + U * V2->UVs + V * V1->UVs );
	
	if(MeshVertices.Contains(MeshVertex) == false)
	{
		MeshVertices.Add(MeshVertex); 
	}else
	{
		int32 index;
		MeshVertices.Find(MeshVertex, index);
		NewVertexIndex = index ;
		DragVertexIndex  = index;
		IsOriVertices = true;
		return MeshVertex;
	}

	RemoveMeshTriangle(MeshTri);
	CreateNewTriangle(NewVertexIndex, MeshTri->Vertex0Index, MeshTri->Vertex1Index);
	CreateNewTriangle(NewVertexIndex, MeshTri->Vertex1Index, MeshTri->Vertex2Index);
	CreateNewTriangle(NewVertexIndex, MeshTri->Vertex2Index, MeshTri->Vertex0Index);
	RemoveUnusedEdges();

	DragVertexIndex = NewVertexIndex;
	IsOriVertices = false;
	
	return MeshVertex;
}

void UPrimitiveMeshComponent::RemoveMeshTriangle(UMeshTriangle* MeshTri)
{
	bool IsRemoved;
	IsRemoved = IsRemoveTriangle(MeshTri);
	if(IsRemoved == false) return;
	
	RemoveTriangleFromEdge(MeshTri->Vertex0Index, MeshTri->Vertex1Index, MeshTri);
	RemoveTriangleFromEdge(MeshTri->Vertex1Index, MeshTri->Vertex2Index, MeshTri);
	RemoveTriangleFromEdge(MeshTri->Vertex2Index, MeshTri->Vertex0Index, MeshTri);

	MeshVertices[MeshTri->Vertex0Index]->RemoveTriangle(MeshTri);
	MeshVertices[MeshTri->Vertex1Index]->RemoveTriangle(MeshTri);
	MeshVertices[MeshTri->Vertex2Index]->RemoveTriangle(MeshTri);

	MeshTri->Dispose();

}

void UPrimitiveMeshComponent::RemoveTriangleFromEdge(int32 V0, int32 V1, UMeshTriangle* MeshTri)
{
	bool IsExist = false;
	
	for (TPair<int32, TTuple<int32, int32, UMeshEdge*>>& MeshEdge : MeshEdges)
	{
		if(V0 == MeshEdge.Value.Get<0>() && V1 == MeshEdge.Value.Get<1>())
		{
			IsExist = true;
			break;
		}
	}

	if(IsExist)
	{
		for (TPair<int32, TTuple<int32, int32, UMeshEdge*>>& MeshEdge : MeshEdges)
		{
			int32  V0Index = MeshEdge.Value.Get<0>();
			int32  V1Index = MeshEdge.Value.Get<1>();
			if((V0Index == V0 && V1Index == V1))
			{
				MeshEdge.Value.Get<2>()->RemoveTriangle(MeshTri);
				return;
			}
		}
	}
	
	
}

UMeshTriangle* UPrimitiveMeshComponent::CreateNewTriangle(int32 V0Index, int32 V1Index, int32 V2Index)
{
	if((V0Index < 0 || V0Index >= MeshVertices.Num())) return NewObject<UMeshTriangle>();
	if((V1Index < 0 || V1Index >= MeshVertices.Num())) return NewObject<UMeshTriangle>();
	if((V2Index < 0 || V2Index >= MeshVertices.Num())) return NewObject<UMeshTriangle>();

	UMeshTriangle* MeshTriangle= NewObject<UMeshTriangle>();
	MeshTriangle->Initializer(V0Index, V1Index, V2Index);
	SetTriangle(MeshTriangle);
	
	MeshVertices[V0Index]->SetTriangle(MeshTriangle);
	MeshVertices[V1Index]->SetTriangle(MeshTriangle);
	MeshVertices[V2Index]->SetTriangle(MeshTriangle);
	
	UProcedureMeshVertexPair* Pair1 = NewObject<UProcedureMeshVertexPair>();
	Pair1->Initializer(V0Index, V1Index);
	AddTriangleToEdge(Pair1, MeshTriangle);
	UProcedureMeshVertexPair* Pair2 = NewObject<UProcedureMeshVertexPair>();
	Pair2->Initializer(V1Index, V2Index);
	AddTriangleToEdge(Pair2, MeshTriangle);
	UProcedureMeshVertexPair* Pair3 = NewObject<UProcedureMeshVertexPair>();
	Pair3->Initializer(V2Index, V0Index);
	AddTriangleToEdge(Pair3, MeshTriangle);

	return MeshTriangle;
}

void UPrimitiveMeshComponent::AddTriangleToEdge(UProcedureMeshVertexPair* MeshVertexPair, UMeshTriangle* MeshTriangle)
{
	bool IsExist = false;
	
	for (TPair<int32, TTuple<int32, int32, UMeshEdge*>>& MeshEdge : MeshEdges)
	{
		
		if(MeshVertexPair->Vertex0Index == MeshEdge.Value.Get<0>() && MeshVertexPair->Vertex1Index == MeshEdge.Value.Get<1>())
		{
			IsExist = true;
			break;
		}
	}

	if(IsExist == false)
	{
		UMeshEdge* Edge = NewObject<UMeshEdge>();
		Edge->Initializer(MeshVertexPair->Vertex0Index, MeshVertexPair->Vertex1Index);
		Edge->SetTriangle(MeshTriangle);
		SetEdges(MeshEdges.Num(), MakeTuple(MeshVertexPair->Vertex0Index, MeshVertexPair->Vertex1Index, Edge));
	}
}

void UPrimitiveMeshComponent::RemoveUnusedEdges()
{
	for(int i = MeshEdges.Num() - 1; i >= 0; i--)
	{
		int j = MeshEdges[i].Get<2>()->GetTriangles().Num();
		if(j == 0)
		{
			MeshEdges.Remove(i);
			MeshEdges[i].Get<2>()->Dispose();
		}
	}
}
