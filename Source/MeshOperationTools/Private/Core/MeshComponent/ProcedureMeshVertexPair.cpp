//
//  ProcedureMeshVertexPair.cpp
//  MeshOperationTools
//
//  Created by NUZEROVI on 2023/1/30.
//

#include "Core/MeshComponent/ProcedureMeshVertexPair.h"

UProcedureMeshVertexPair::UProcedureMeshVertexPair()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UProcedureMeshVertexPair::BeginPlay()
{
	Super::BeginPlay();
}

void UProcedureMeshVertexPair::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

