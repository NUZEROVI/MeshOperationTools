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
	int Vertex0Index;
	int Vertex1Index;
	bool OriginalEdge;
	bool operator == (const UMeshEdge &b) const { return Vertex0Index == b.Vertex0Index && Vertex1Index == b.Vertex1Index; }

	/* Initialization */
	void Initializer(const int& V0, const int& V1);
	
	/* Getters */
	float GetCurrentLength();
	float GetInitialLength();
	
	/* Setters */
	FORCEINLINE void SetOriEdge(bool OriEdge) { OriginalEdge = OriEdge; }
	FORCEINLINE void RemoveTriangle(UMeshTriangle* MeshTriangle) { _Triangles.Remove(MeshTriangle); }
	void Dispose();
	
private:
	/* Attributes */
	TArray<UMeshTriangle*> _Triangles;
	bool _IsDisposed = false;		
};
