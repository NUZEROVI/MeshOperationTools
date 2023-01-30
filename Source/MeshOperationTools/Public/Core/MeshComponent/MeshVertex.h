//
//  MeshVertex.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveMeshComponent.h"
#include "Components/ActorComponent.h"
#include "MeshVertex.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESHOPERATIONTOOLS_API UMeshVertex : public UActorComponent
{
	GENERATED_BODY()

public:	

	UMeshVertex();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:	
	/* Attributes */
	bool IsNull;
	int VertexIndex;
	FVector CurrentPosition;
	FVector OriginalPosition;
	FVector2D UVs;
	bool operator == (const UMeshVertex &V) const {	return CurrentPosition == V.CurrentPosition && OriginalPosition == V.OriginalPosition;	}
	
	/* Initialization */
	void Initializer(AProceduralMesh* Mesh, const int& Index, const FVector& CurrPos, const FVector& OriPos, const FVector2D& MeshUVs);

	/* Getters */
	UPrimitiveMeshComponent* GetMeshComponent() { return  PrimitiveMeshComponent; }
	AProceduralMesh* GetMesh() { return _Mesh; }
	TArray<UMeshTriangle*> GetTriangles() const { return _Triangles; }
	TArray<UMeshEdge*> GetEdge();
	
	/* Setters */
	FORCEINLINE void SetMeshComponent(UPrimitiveMeshComponent* MeshComponent) { PrimitiveMeshComponent = MeshComponent; }
	FORCEINLINE void SetTriangle(UMeshTriangle* MeshTriangle) { _Triangles.Add(MeshTriangle); }
	FORCEINLINE void RemoveTriangle(UMeshTriangle* MeshTriangle) { _Triangles.Remove(MeshTriangle); }
	
private:
	/* Attributes */
	UPrimitiveMeshComponent* PrimitiveMeshComponent;
	AProceduralMesh* _Mesh;
	TArray<UMeshTriangle*> _Triangles;
};


