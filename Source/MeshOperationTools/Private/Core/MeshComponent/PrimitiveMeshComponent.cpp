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
			 _Vertices[MeshTriangle->Vertex0Index]->CurrentPosition,
			_Vertices[MeshTriangle->Vertex1Index]->CurrentPosition,
			_Vertices[MeshTriangle->Vertex2Index]->CurrentPosition,
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
				
				float Matrix00 = _Vertices[MeshTriangle->Vertex2Index]->CurrentPosition.X - _Vertices[MeshTriangle->Vertex0Index]->CurrentPosition.X;
				float Matrix11 = _Vertices[MeshTriangle->Vertex1Index]->CurrentPosition.Y - _Vertices[MeshTriangle->Vertex0Index]->CurrentPosition.Y;
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

				U = ( P.X - (_Vertices[MeshTriangle->Vertex0Index]->CurrentPosition.X) ) / ( (_Vertices[MeshTriangle->Vertex2Index]->CurrentPosition.X) - (_Vertices[MeshTriangle->Vertex0Index]->CurrentPosition.X));
				V = ( P.Y - (_Vertices[MeshTriangle->Vertex0Index]->CurrentPosition.Y) ) / ( (_Vertices[MeshTriangle->Vertex1Index]->CurrentPosition.Y) - (_Vertices[MeshTriangle->Vertex0Index]->CurrentPosition.Y));
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
	
	UMeshVertex* V0 = _Vertices[ClosestTri->Vertex0Index];
	UMeshVertex* V1 = _Vertices[ClosestTri->Vertex1Index]; 
	UMeshVertex* V2 = _Vertices[ClosestTri->Vertex2Index];

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
				_DragVertexIndex = ClosestTri->Vertex1Index;
				_IsOriVertices = true;
				return V1;
			}
			if (sqrDistanceP0V2 <= SqrSnapDistance && sqrDistanceP0V2 < sqrDistanceP0V1)
			{
				_DragVertexIndex = ClosestTri->Vertex2Index;
				_IsOriVertices = true;
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
				_DragVertexIndex = ClosestTri->Vertex0Index;
				_IsOriVertices = true;
				return V0;
			}
			if (sqrDistanceP1V2 <= SqrSnapDistance && sqrDistanceP1V2 < sqrDistanceP1V0)
			{
				_DragVertexIndex = ClosestTri->Vertex2Index;
				_IsOriVertices = true;
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
				_DragVertexIndex = ClosestTri->Vertex0Index;
				_IsOriVertices = true;
				return V0;
			}
			if (sqrDistanceP2V1 <= SqrSnapDistance && sqrDistanceP2V1 < sqrDistanceP2V0)
			{
				_DragVertexIndex = ClosestTri->Vertex1Index;
				_IsOriVertices = true;
				return V1;
			}
			return CreateVertexOnEdge(V0, V1, t2);
		}
	}

	return NewObject<UMeshVertex>();	
}

UMeshVertex* UPrimitiveMeshComponent::CreateVertexOnEdge(UMeshVertex* V0, UMeshVertex* V1, float T)
{
	bool ContainKey = false;
	int32  V0Index, V1Index;
	for (TTuple<int32, int32, int32, UMeshEdge*> MeshEdge : _Edges)
	{
		V0Index =  MeshEdge.Get<1>();
		V1Index = MeshEdge.Get<2>();
		
		if((V0Index == V0->VertexIndex) && (V1Index == V1->VertexIndex)) //Contain
		{
			ContainKey = true;
			
			bool OriginalEdge = MeshEdge.Get<3>()->OriginalEdge;
			TArray<UMeshTriangle*> TrianglesToReplace =  MeshEdge.Get<3>()->GetTriangles();
			
			int32 NewVertexIndex = _Vertices.Num();
			UMeshVertex* MeshVertex = NewObject<UMeshVertex>();
			MeshVertex->Initializer(
				_Mesh,
				NewVertexIndex,
				(1 - T) * V0->CurrentPosition + T * V1->CurrentPosition, 
				(1 - T) * V0->OriginalPosition + T * V1->OriginalPosition,
				(1 - T) * V0->UVs + T * V1->UVs);
			
			if(_Vertices.Contains(MeshVertex) == false)
			{
				SetVertices(MeshVertex);
			}else
			{
				int32 index;
				_Vertices.Find(MeshVertex, index);
				NewVertexIndex = index ;
				_DragVertexIndex  = index;
				_IsOriVertices = true;
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

			for (TTuple<int32, int32, int32, UMeshEdge*> Edge : _Edges)
			{
				V0Index = Edge.Get<1>();
				V1Index = Edge.Get<2>();
				if((V0Index == V0->VertexIndex) && (V1Index == NewVertexIndex))
				{
					Edge.Get<3>()->SetOriEdge(OriginalEdge);
				}
				if((V0Index == V1->VertexIndex) && (V1Index == NewVertexIndex))
				{
					Edge.Get<3>()->SetOriEdge(OriginalEdge);
				}
			}
			
			_DragVertexIndex = NewVertexIndex;
			_IsOriVertices = false;
			return MeshVertex;
		}
	}
	if(ContainKey == false) return NewObject<UMeshVertex>(); // No Contain Keys

	return NewObject<UMeshVertex>();
}

UMeshVertex* UPrimitiveMeshComponent::CreateVertexInTriangle(UMeshTriangle* MeshTri, float U, float V)
{
	UMeshVertex* V0 = _Vertices[MeshTri->Vertex0Index];
	UMeshVertex* V1 = _Vertices[MeshTri->Vertex1Index];
	UMeshVertex* V2 = _Vertices[MeshTri->Vertex2Index];

	int NewVertexIndex = _Vertices.Num();
	UMeshVertex* MeshVertex = NewObject<UMeshVertex>();
	MeshVertex->Initializer(
				_Mesh,
				NewVertexIndex,
				(1 - U - V) * V0->CurrentPosition + U * V2->CurrentPosition + V * V1->CurrentPosition,
				(1 - U - V) * V0->OriginalPosition + U * V2->OriginalPosition + V * V1->OriginalPosition,
				(1 - U - V) * V0->UVs + U * V2->UVs + V * V1->UVs );
	
	if(_Vertices.Contains(MeshVertex) == false)
	{
		_Vertices.Add(MeshVertex); 
	}else
	{
		int32 index;
		_Vertices.Find(MeshVertex, index);
		NewVertexIndex = index ;
		_DragVertexIndex  = index;
		_IsOriVertices = true;
		return MeshVertex;
	}

	RemoveMeshTriangle(MeshTri);
	CreateNewTriangle(NewVertexIndex, MeshTri->Vertex0Index, MeshTri->Vertex1Index);
	CreateNewTriangle(NewVertexIndex, MeshTri->Vertex1Index, MeshTri->Vertex2Index);
	CreateNewTriangle(NewVertexIndex, MeshTri->Vertex2Index, MeshTri->Vertex0Index);
	RemoveUnusedEdges();

	_DragVertexIndex = NewVertexIndex;
	_IsOriVertices = false;
	
	return MeshVertex;
}

void UPrimitiveMeshComponent::RemoveMeshTriangle(UMeshTriangle* MeshTri)
{
	bool IsRemoved = false;
	IsRemoved = RemoveTriangle(MeshTri);
	if(IsRemoved == false) return;
	
	RemoveTriangleFromEdge(MeshTri->Vertex0Index, MeshTri->Vertex1Index, MeshTri);
	RemoveTriangleFromEdge(MeshTri->Vertex1Index, MeshTri->Vertex2Index, MeshTri);
	RemoveTriangleFromEdge(MeshTri->Vertex2Index, MeshTri->Vertex0Index, MeshTri);

	_Vertices[MeshTri->Vertex0Index]->RemoveTriangle(MeshTri);
	_Vertices[MeshTri->Vertex1Index]->RemoveTriangle(MeshTri);
	_Vertices[MeshTri->Vertex2Index]->RemoveTriangle(MeshTri);

	MeshTri->Dispose();

}

void UPrimitiveMeshComponent::RemoveTriangleFromEdge(int32 V0, int32 V1, UMeshTriangle* MeshTri)
{
	bool IsExist = false;
	
	for (TTuple<int32, int32, int32, UMeshEdge*> MeshEdge : _Edges)
	{
		if(V0 == MeshEdge.Get<1>() && V1 == MeshEdge.Get<2>())
		{
			IsExist = true;
			break;
		}
	}

	if(IsExist)
	{
		for (TTuple<int32, int32, int32, UMeshEdge*> MeshEdge : _Edges)
		{
			int32  V0Index = MeshEdge.Get<1>();
			int32  V1Index = MeshEdge.Get<2>();
			if((V0Index == V0 && V1Index == V1))
			{
				MeshEdge.Get<3>()->RemoveTriangle(MeshTri);
				return;
			}
		}
	}
	
	
}

UMeshTriangle* UPrimitiveMeshComponent::CreateNewTriangle(int32 V0Index, int32 V1Index, int32 V2Index)
{
	if((V0Index < 0 || V0Index >= _Vertices.Num())) return NewObject<UMeshTriangle>();
	if((V1Index < 0 || V1Index >= _Vertices.Num())) return NewObject<UMeshTriangle>();
	if((V2Index < 0 || V2Index >= _Vertices.Num())) return NewObject<UMeshTriangle>();

	UMeshTriangle* MeshTriangle= NewObject<UMeshTriangle>();
	MeshTriangle->Initializer(_Mesh, V0Index, V1Index, V2Index);
	SetTriangle(MeshTriangle);
	
	_Vertices[V0Index]->SetTriangle(MeshTriangle);
	_Vertices[V1Index]->SetTriangle(MeshTriangle);
	_Vertices[V2Index]->SetTriangle(MeshTriangle);
	
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
	
	for (TTuple<int32, int32, int32, UMeshEdge*> MeshEdge : _Edges)
	{
		if(MeshVertexPair->Vertex0Index == MeshEdge.Get<1>() && MeshVertexPair->Vertex1Index == MeshEdge.Get<2>())
		{
			IsExist = true;
			break;
		}
	}

	if(IsExist == false)
	{
		UMeshEdge* MeshEdge = NewObject<UMeshEdge>();
		MeshEdge->Initializer(_Mesh, MeshVertexPair->Vertex0Index, MeshVertexPair->Vertex1Index);
		MeshEdge->SetTriangle(MeshTriangle);
		SetEdges(MakeTuple(_Edges.Num(), MeshVertexPair->Vertex0Index, MeshVertexPair->Vertex1Index, MeshEdge));
	}
}

void UPrimitiveMeshComponent::RemoveUnusedEdges()
{
	for(auto i = _Edges.Num() - 1; i >= 0; i--)
	{
		int j = _Edges[i].Get<3>()->GetTriangles().Num();
		if(j == 0)
		{
			_Edges.RemoveAt(i);
			_Edges[i].Get<3>()->Dispose();
		}
	}
}
