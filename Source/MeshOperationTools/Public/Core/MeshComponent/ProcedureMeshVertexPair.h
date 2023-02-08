//
//  ProcedureMeshVertexPair.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//
#pragma once

#include "CoreMinimal.h"
#include "PrimitiveMeshComponent.h"
#include "ProcedureMeshVertexPair.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESHOPERATIONTOOLS_API UProcedureMeshVertexPair : public UPrimitiveMeshComponent
{
	GENERATED_BODY()

public:
	
	UProcedureMeshVertexPair();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	/* Attributes */
	int32 Vertex0Index;
	int32 Vertex1Index;
	
	/* Initialization */
	void Initializer(const int32& V0, const int32& V1)
	{
		Vertex0Index = V0;
		Vertex1Index = V1;
	}
	
};
