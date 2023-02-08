//
//  MeshTriangle.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveMeshComponent.h"
#include "MeshTriangle.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESHOPERATIONTOOLS_API UMeshTriangle : public UPrimitiveMeshComponent
{
	GENERATED_BODY()

public:	

	UMeshTriangle();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/* Attributes */
	bool IsNull;
	int32 Vertex0Index;
	int32 Vertex1Index;
	int32 Vertex2Index;
	bool operator == (const UMeshTriangle &b) const { return Vertex0Index == b.Vertex0Index && Vertex1Index == b.Vertex1Index &&	Vertex2Index == b.Vertex2Index;	}

	/* Initialization */
	void Initializer(const int32& V0Index, const int32& V1Index, const int32& V2Index);
	
	/* Getters */
	TArray<UMeshEdge*> GetEdgeFromTriangle();
	
	/* Setters */
	void ShuffleVertexIndices(const int32& V0Index, const int32& V1Index);
	void Dispose();

private:
	/* Attributes */
	bool IsDisposed = false;	
};
