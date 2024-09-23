#pragma once

#include "CoreMinimal.h"
#include "BaseData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDataChanged);

UCLASS()
class UBaseData : public UObject
{
	GENERATED_BODY()
public:
	UBaseData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString DataID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 DataVersion;

	UPROPERTY(Transient)
	bool bIsDirty;

	UPROPERTY(BlueprintAssignable, Category = "Data")
	FOnDataChanged OnDataChanged;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Data")
	void InitializeData();
	virtual void InitializeData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Data")
	bool ValidateData();
	virtual bool ValidateData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Data")
	void ResetData();
	virtual void ResetData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Data")
	void SaveData();
	virtual void SaveData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Data")
	void LoadData();
	virtual void LoadData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Data")
	UBaseData* Clone();
	virtual UBaseData* Clone_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Data")
	virtual FName GetDataType() const;

protected:
	virtual void MarkDirty();
};
