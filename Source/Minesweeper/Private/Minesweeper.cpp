// Copyright Epic Games, Inc. All Rights Reserved.

#include "Minesweeper.h"
#include "MinesweeperStyle.h"
#include "MinesweeperCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName MinesweeperTabName("Minesweeper");

#define LOCTEXT_NAMESPACE "FMinesweeperModule"

DEFINE_LOG_CATEGORY(LogMinesweeperPlugin);

void FMinesweeperModule::StartupModule() {
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FMinesweeperStyle::Initialize();
	FMinesweeperStyle::ReloadTextures();

	FMinesweeperCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMinesweeperCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FMinesweeperModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMinesweeperModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MinesweeperTabName, FOnSpawnTab::CreateRaw(this, &FMinesweeperModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FMinesweeperTabTitle", "Minesweeper"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMinesweeperModule::ShutdownModule() {
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMinesweeperStyle::Shutdown();

	FMinesweeperCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MinesweeperTabName);
}

TSharedRef<SDockTab> FMinesweeperModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs) {
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FMinesweeperModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("Minesweeper.cpp"))
	);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot().FillHeight(1).Padding(8)
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot().HAlign(EHorizontalAlignment::HAlign_Center).VAlign(EVerticalAlignment::VAlign_Center)
						[
							SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								[
									SNew(STextBlock)
										.Text(LOCTEXT("WindowWidgetSettingHeader", "Settings"))
								]
						]
						+ SVerticalBox::Slot().VAlign(EVerticalAlignment::VAlign_Center)
						[
							SNew(SHorizontalBox)
								+ SHorizontalBox::Slot().FillWidth(1).Padding(8).VAlign(EVerticalAlignment::VAlign_Center)
								[
									SNew(SHorizontalBox)
										+ SHorizontalBox::Slot().FillWidth(1).VAlign(EVerticalAlignment::VAlign_Center)
										[
											SNew(STextBlock)
												.Text(LOCTEXT("WindowWidgetSettingHeigth", "Heigth"))
										]
										+ SHorizontalBox::Slot().FillWidth(2).VAlign(EVerticalAlignment::VAlign_Center)
										[
											SNew(SEditableTextBox)
												.OnTextCommitted_Lambda([this](const FText& InValue, ETextCommit::Type& InCommitType) -> void {
												UE_LOG(LogMinesweeperPlugin, Display, TEXT("Commited text %s(%d)"), *InValue.ToString(), InValue.IsNumeric());
												if (InValue.IsNumeric()) {
													Heigth = FCString::Atoi(*InValue.ToString());
												} else {
													Heigth = 0;
												}
												UE_LOG(LogMinesweeperPlugin, Display, TEXT("Read %d"), Heigth);
													})
												.OnVerifyTextChanged_Lambda([](const FText& InValue, FText& InErrorMessage) -> bool {
														if (InValue.IsNumeric()) {
															return true;
														}
														InErrorMessage = LOCTEXT("WindowWidgetSettingHeigthError", "Insert value is not a numeric value");
														return false;
													})
										]
								]
								+ SHorizontalBox::Slot().FillWidth(1).Padding(8).VAlign(EVerticalAlignment::VAlign_Center)
								[
									SNew(SHorizontalBox)
										+ SHorizontalBox::Slot().FillWidth(1).VAlign(EVerticalAlignment::VAlign_Center)
										[
											SNew(STextBlock)
												.Text(LOCTEXT("WindowWidgetSettingWidth", "Width"))
										]
										+ SHorizontalBox::Slot().FillWidth(2).VAlign(EVerticalAlignment::VAlign_Center)
										[
											SNew(SEditableTextBox)
												.OnTextCommitted_Lambda([this](const FText& InValue, ETextCommit::Type& InCommitType) -> void {
												UE_LOG(LogMinesweeperPlugin, Display, TEXT("Commited text %s(%d)"), *InValue.ToString(), InValue.IsNumeric());
												if (InValue.IsNumeric()) {
													Width = FCString::Atoi(*InValue.ToString());
												} else {
													Width = 0;
												}
												UE_LOG(LogMinesweeperPlugin, Display, TEXT("Read %d"), Width);
													})
												.OnVerifyTextChanged_Lambda([](const FText& InValue, FText& InErrorMessage) -> bool {
														if (InValue.IsNumeric()) {
															return true;
														}
														InErrorMessage = LOCTEXT("WindowWidgetSettingHeigthError", "Insert value is not a numeric value");
														return false;
													})
										]
								]
						]
						+ SVerticalBox::Slot().VAlign(EVerticalAlignment::VAlign_Center)
						[

							SNew(SHorizontalBox)
								+ SHorizontalBox::Slot().FillWidth(1).VAlign(EVerticalAlignment::VAlign_Center)
								[
									SNew(STextBlock)
										.Text(LOCTEXT("WindowWidgetSettingMines", "Number of Mines"))
								]
								+ SHorizontalBox::Slot().FillWidth(2).VAlign(EVerticalAlignment::VAlign_Center)
								[
									SNew(SEditableTextBox)
										.OnTextCommitted_Lambda([this](const FText& InValue, ETextCommit::Type& InCommitType) -> void {
										UE_LOG(LogMinesweeperPlugin, Display, TEXT("Commited text %s(%d)"), *InValue.ToString(), InValue.IsNumeric());
										if (InValue.IsNumeric()) {
											Mines = FCString::Atoi(*InValue.ToString());
										} else {
											Mines = 0;
										}
										UE_LOG(LogMinesweeperPlugin, Display, TEXT("Read %d"), Mines);
											})
										.OnVerifyTextChanged_Lambda([](const FText& InValue, FText& InErrorMessage) -> bool {
												if (InValue.IsNumeric()) {
													return true;
												}
												InErrorMessage = LOCTEXT("WindowWidgetSettingHeigthError", "Insert value is not a numeric value");
												return false;
											})
								]

						]
				]
		];
}

void FMinesweeperModule::PluginButtonClicked() {
	FGlobalTabmanager::Get()->TryInvokeTab(MinesweeperTabName);
}

void FMinesweeperModule::RegisterMenus() {
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("Extra");
			Section.AddMenuEntryWithCommandList(FMinesweeperCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Extra");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMinesweeperCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMinesweeperModule, Minesweeper)