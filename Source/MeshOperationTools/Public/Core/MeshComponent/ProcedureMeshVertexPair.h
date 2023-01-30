//
//  ProcedureMeshVertexPair.h
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProcedureMeshVertexPair.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESHOPERATIONTOOLS_API UProcedureMeshVertexPair : public UActorComponent
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
	int Vertex0Index;
	int Vertex1Index;
	
	/* Initialization */
	void Initializer(const int& V0, const int& V1)
	{
		Vertex0Index = V0;
		Vertex1Index = V1;
	}
	
};
