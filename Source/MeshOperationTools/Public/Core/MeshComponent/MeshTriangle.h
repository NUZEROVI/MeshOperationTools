//
//  MeshTriangle.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveMeshComponent.h"
#include "Components/ActorComponent.h"
#include "MeshTriangle.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESHOPERATIONTOOLS_API UMeshTriangle : public UActorComponent
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
	int Vertex0Index;
	int Vertex1Index;
	int Vertex2Index;
	bool operator == (const UMeshTriangle &b) const { return Vertex0Index == b.Vertex0Index && Vertex1Index == b.Vertex1Index &&	Vertex2Index == b.Vertex2Index;	}

	/* Initialization */
	void Initializer(AProceduralMesh* Mesh, const int& V0Index, const int& V1Index, const int& V2Index);
	
	/* Getters */
	UPrimitiveMeshComponent* GetMeshComponent() { return  PrimitiveMeshComponent; }
	AProceduralMesh* GetMesh() { return _Mesh; }
	TArray<UMeshEdge*> GetEdge();
	
	/* Setters */
	FORCEINLINE void SetMeshComponent(UPrimitiveMeshComponent* MeshComponent) { PrimitiveMeshComponent = MeshComponent; }
	void Dispose();

private:
	/* Attributes */
	UPrimitiveMeshComponent* PrimitiveMeshComponent;
	AProceduralMesh* _Mesh;
	bool _IsDisposed = false;	
};
