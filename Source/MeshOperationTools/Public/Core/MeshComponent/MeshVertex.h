//
//  MeshVertex.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveMeshComponent.h"
#include "MeshVertex.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESHOPERATIONTOOLS_API UMeshVertex : public UPrimitiveMeshComponent
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
	void Initializer(const int& Index, const FVector& CurrPos, const FVector& OriPos, const FVector2D& MeshUVs);

	/* Getters */
	TArray<UMeshEdge*> GetEdge();
	
	/* Setters */
	FORCEINLINE void RemoveTriangle(UMeshTriangle* MeshTriangle) { _Triangles.Remove(MeshTriangle); }
	
private:
	/* Attributes */
	TArray<UMeshTriangle*> _Triangles;
};


