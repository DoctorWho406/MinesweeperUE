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

	VerticalBox->AddSlot().FillHeight(1).Padding(8)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot().HAlign(EHorizontalAlignment::HAlign_Center).VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						[
							SNew(STextBlock)
								.Text(LOCTEXT("MinesweeperWindowWidgetSettingHeader", "Settings"))
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
										.Text(LOCTEXT("MinesweeperWindowWidgetSettingHeigth", "Heigth"))
								]
								+ SHorizontalBox::Slot().FillWidth(2).VAlign(EVerticalAlignment::VAlign_Center)
								[
									SNew(SEditableTextBox)
										.OnTextCommitted_Lambda([this](const FText& InValue, ETextCommit::Type& InCommitType) -> void {
										MinesweeperClass->SetHeigth(FCString::Atoi(*InValue.ToString()));
											})
										.OnVerifyTextChanged_Lambda([this](const FText& InValue, FText& InErrorMessage) -> bool {
												if (InValue.IsNumeric()) {
													if (MinesweeperClass->SetHeigth(FCString::Atoi(*InValue.ToString()))) {
														return true;
													} else {
														InErrorMessage = LOCTEXT("MinesweeperWindowWidgetSettingHeigthNegativeError", "Insert positive value");
														return false;
													}
												}
												MinesweeperClass->SetHeigth(0);
												InErrorMessage = LOCTEXT("MinesweeperWindowWidgetSettingHeigthNANError", "Value is not a numeric value");
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
										.Text(LOCTEXT("MinesweeperWindowWidgetSettingWidth", "Width"))
								]
								+ SHorizontalBox::Slot().FillWidth(2).VAlign(EVerticalAlignment::VAlign_Center)
								[
									SNew(SEditableTextBox)
										.OnTextCommitted_Lambda([this](const FText& InValue, ETextCommit::Type& InCommitType) -> void {
										UE_LOG(LogMinesweeperPlugin, Display, TEXT("Commited text %s(%d)"), *InValue.ToString(), InValue.IsNumeric());
										MinesweeperClass->SetWidth(FCString::Atoi(*InValue.ToString()));
											})
										.OnVerifyTextChanged_Lambda([this](const FText& InValue, FText& InErrorMessage) -> bool {
												if (InValue.IsNumeric()) {
													if (MinesweeperClass->SetWidth(FCString::Atoi(*InValue.ToString()))) {
														return true;
													} else {
														InErrorMessage = LOCTEXT("MinesweeperWindowWidgetSettingWidthNegativeError", "Insert positive value");
														return false;
													}
												}
												MinesweeperClass->SetWidth(0);
												InErrorMessage = LOCTEXT("MinesweeperWindowWidgetSettingWidthNANError", "Value is not a numeric value");
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
								.Text(LOCTEXT("MinesweeperWindowWidgetSettingMines", "Number of Mines"))
						]
						+ SHorizontalBox::Slot().FillWidth(2).VAlign(EVerticalAlignment::VAlign_Center)
						[
							SNew(SEditableTextBox)
								.OnTextCommitted_Lambda([this](const FText& InValue, ETextCommit::Type& InCommitType) -> void {
								UE_LOG(LogMinesweeperPlugin, Display, TEXT("Commited text %s(%d)"), *InValue.ToString(), InValue.IsNumeric());
								MinesweeperClass->SetMines(FCString::Atoi(*InValue.ToString()));
									})
								.OnVerifyTextChanged_Lambda([this](const FText& InValue, FText& InErrorMessage) -> bool {
										if (InValue.IsNumeric()) {
											if (MinesweeperClass->SetMines(FCString::Atoi(*InValue.ToString()))) {
												return true;
											} else {
												InErrorMessage = LOCTEXT("MinesweeperWindowWidgetSettingMinesNegativeError", "Insert positive value");
												return false;
											}
										}
										MinesweeperClass->SetMines(0);
										InErrorMessage = LOCTEXT("MinesweeperWindowWidgetSettingMinesNANError", "Value is not a numeric value");
										return false;
									})
						]

				]
				+ SVerticalBox::Slot().VAlign(EVerticalAlignment::VAlign_Center).HAlign(EHorizontalAlignment::HAlign_Center)
				[
					SNew(SButton)
						.Text(LOCTEXT("MinesweeperWindowWidgetSettingConfirm", "Generate grid"))
						.OnClicked_Lambda([this]() -> FReply {
						UE_LOG(LogMinesweeperPlugin, Display, TEXT("Generate new grid"));
						GenerateGrid();
						return FReply::Handled();
							})
				]
		];

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMinesweeperCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FMinesweeperModule::PluginButtonClicked),
		FCanExecuteAction());

	MinesweeperClass = MakeShareable(new MinesweeperData);

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

TSharedRef<SDockTab> FMinesweeperModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs) {
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			VerticalBox
		];
}

void FMinesweeperModule::GenerateGrid() {
	if (!MinesweeperClass->AreSettingsValid()) {
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MinesweeperWindowWidgetDialogueNonValid", "Setting not valid"));
		return;
	} else {
		UE_LOG(LogMinesweeperPlugin, Display, TEXT("Spawning mines"));
		MinesweeperClass->SpawnMines();
		// Remove old Grid
		while (VerticalBox->NumSlots() >= 2) {
			VerticalBox->RemoveSlot(VerticalBox->GetSlot(1).GetWidget());
		}
		auto GridSlot = VerticalBox->AddSlot();
		TSharedRef<SVerticalBox> GridVertical = SNew(SVerticalBox);
		for (int y = 0; y < MinesweeperClass->GetHeigth(); ++y) {
			auto GridRowSlot = GridVertical->AddSlot();
			TSharedRef<SHorizontalBox> GridHorizontal = SNew(SHorizontalBox);
			for (int x = 0; x < MinesweeperClass->GetWidth(); ++x) {
				GridHorizontal->AddSlot()[
					SNew(SButton)
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.OnClicked_Lambda([this, x, y]() -> FReply {
						if (!MinesweeperClass->GetGameOver()) {
							FMinesweeperModule::OnButtonPressed(x, y);
						}
						return FReply::Handled();
							})
				];
			}
			GridRowSlot.AttachWidget(GridHorizontal);
		}
		GridSlot.Padding(8).AttachWidget(GridVertical);
		VerticalBox->Invalidate(EInvalidateWidgetReason::ChildOrder);
	}
}

void FMinesweeperModule::OnButtonPressed(const int InX, const int InY) {
	UE_LOG(LogMinesweeperPlugin, Display, TEXT("Clicked button at (%d, %d)"), InX, InY);

	// Disable button
	TSharedRef<SButton> CurrentButton = StaticCastSharedRef<SButton>(StaticCastSharedRef<SHorizontalBox>(StaticCastSharedRef<SVerticalBox>(VerticalBox->GetSlot(VerticalBox->NumSlots() > 2 ? 2 : 1).GetWidget())->GetSlot(InY).GetWidget())->GetSlot(InX).GetWidget());
	CurrentButton->SetEnabled(false);

	// Clicked on Mine
	if (MinesweeperClass->ThereIsMineAt(InX, InY)) {
		//Game Over
		MinesweeperClass->SetGameOver();
		CurrentButton->SetContent(
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("X")))
		);
		VerticalBox->InsertSlot(1).AutoHeight().VAlign(EVerticalAlignment::VAlign_Center).HAlign(EHorizontalAlignment::HAlign_Center).AttachWidget(
			SNew(STextBlock)
			.Text(LOCTEXT("MinesweeperWindowWidgetGameOverLose", "Game Over"))
		);
		VerticalBox->Invalidate(EInvalidateWidgetReason::ChildOrder);
	} else {
		//Count Nearest Mines
		int NearestMines = MinesweeperClass->CountNearestMines(InX, InY);
		if (NearestMines == 0) {
			//Iterative Search
			TMap<int, bool> Discovered = TMap<int, bool>();
			Discovered.Add(MinesweeperClass->IndexOf(InX, InY), false);
			for (int NearY = -1; NearY <= 1; ++NearY) {
				for (int NearX = -1; NearX <= 1; ++NearX) {
					if (NearY == 0 && NearX == 0) {
						continue;
					}
					RevealFieldRecursive(InX + NearX, InY + NearY, Discovered);
				}
			}
		} else {
			MinesweeperClass->AddDiscoveredField();
			//Write number of nearest Mines
			CurrentButton->SetContent(
				SNew(STextBlock)
				.Text(FText::AsNumber(NearestMines))
			);
		}
		if (MinesweeperClass->GetGameOver()) {
			//Discovered all fields. I win
			VerticalBox->InsertSlot(1).AutoHeight().VAlign(EVerticalAlignment::VAlign_Center).HAlign(EHorizontalAlignment::HAlign_Center).AttachWidget(
				SNew(STextBlock)
				.Text(LOCTEXT("MinesweeperWindowWidgetGameOverWin", "You win"))
			);
			VerticalBox->Invalidate(EInvalidateWidgetReason::ChildOrder);
		}
	}
}

void FMinesweeperModule::RevealFieldRecursive(const int InX, const int InY, TMap<int, bool>& InDiscovered) {
	int CurrentIndex = MinesweeperClass->IndexOf(InX, InY);
	UE_LOG(LogMinesweeperPlugin, Display, TEXT("Recursive call on (%d, %d)[%d]"), InX, InY, CurrentIndex);

	//Out of Grid
	if (MinesweeperClass->IsOutOfGrid(InX, InY)) {
		return;
	}

	//Set current field as discovered
	InDiscovered.Add(CurrentIndex, MinesweeperClass->ThereIsMineAt(InX, InY));
	//I'm a bomb
	if (InDiscovered[CurrentIndex]) {
		return;
	}

	//Count Nearest Mines
	int NearestMines = MinesweeperClass->CountNearestMines(InX, InY);

	// Disable button
	TSharedRef<SButton> CurrentButton = StaticCastSharedRef<SButton>(StaticCastSharedRef<SHorizontalBox>(StaticCastSharedRef<SVerticalBox>(VerticalBox->GetSlot(VerticalBox->NumSlots() > 2 ? 2 : 1).GetWidget())->GetSlot(InY).GetWidget())->GetSlot(InX).GetWidget());
	CurrentButton->SetEnabled(false);

	if (NearestMines == 0) {
		//Iterative Search
		for (int NearY = -1; NearY <= 1; ++NearY) {
			for (int NearX = -1; NearX <= 1; ++NearX) {
				//I can not skip the current field because already added to the discovered
				if (!InDiscovered.Contains(MinesweeperClass->IndexOf(InX + NearX, InY + NearY))) {
					RevealFieldRecursive(InX + NearX, InY + NearY, InDiscovered);
				}
			}
		}
	} else {
		//Write number of nearest Mines
		CurrentButton->SetContent(
			SNew(STextBlock)
			.Text(FText::AsNumber(NearestMines))
		);
	}
	MinesweeperClass->AddDiscoveredField();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMinesweeperModule, Minesweeper)