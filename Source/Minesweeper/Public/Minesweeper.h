// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "MinesweeperData.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMinesweeperPlugin, Log, All);

class FToolBarBuilder;
class FMenuBuilder;

class FMinesweeperModule : public IModuleInterface {
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	void GenerateGrid();

	void OnButtonPressed(const int x, const int y);
	void RevealFieldRecursive(const int x, const int y, TMap<int, bool>& discovered);

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	TSharedRef<class SVerticalBox> VerticalBox = SNew(SVerticalBox);

	TSharedPtr<MinesweeperData> MinesweeperClass;
};
