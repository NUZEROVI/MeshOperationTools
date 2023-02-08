//
//  MeshEdge.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveMeshComponent.h"
#include "MeshEdge.generated.h"

class AProceduralMesh;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESHOPERATIONTOOLS_API UMeshEdge : public UPrimitiveMeshComponent
{
	GENERATED_BODY()

public:	

	UMeshEdge();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/* Attributes */
	bool IsNull;
	int32 Vertex0Index;
	int32 Vertex1Index;
	bool OriginalEdge;
	bool operator == (const UMeshEdge &b) const { return Vertex0Index == b.Vertex0Index && Vertex1Index == b.Vertex1Index; }

	/* Initialization */
	void Initializer(const int32& V0, const int32& V1);
	
	/* Getters */
	virtual TArray<UMeshTriangle*> GetTriangles() const override;
	float GetCurrentLength();
	float GetInitialLength();
	
	/* Setters */
	virtual void SetTriangle(UMeshTriangle* MeshTriangle) override;
	FORCEINLINE void SetOriEdge(bool OriEdge) { OriginalEdge = OriEdge; }
	void Dispose();
	
private:
	/* Attributes */
	TArray<UMeshTriangle*> MeshTriangles;
	bool IsDisposed = false;		
};
