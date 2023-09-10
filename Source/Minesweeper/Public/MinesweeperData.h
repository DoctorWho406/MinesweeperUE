// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class MINESWEEPER_API MinesweeperData {
public:
	MinesweeperData();
	~MinesweeperData();

	bool SetWidth(const int InWidth);
	int GetWidth() const;
	bool SetHeigth(const int InHeigth);
	int GetHeigth() const;
	bool SetMines(int InMines);
	int GetMines() const;
	void SetGameOver();
	bool GetGameOver() const;

	bool AreSettingsValid() const;

	void SpawnMines();

	int IndexOf(const int InX, const int InY) const;
	bool IsOutOfGrid(const int InX, const int InY) const;
	bool ThereIsMineAt(const int InX, const int InY) const;
	int CountNearestMines(const int InX, const int InY) const;

	void AddDiscoveredField();
private:
	int GridSize() const;
private:
	bool GameOver = false;

	int Width = 0;
	int Heigth = 0;
	int Mines = 0;

	int DiscoveredField;

	TSet<int> MinesPosition;
};
