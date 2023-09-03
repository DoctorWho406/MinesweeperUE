// Fill out your copyright notice in the Description page of Project Settings.


#include "MinesweeperData.h"

MinesweeperData::MinesweeperData() {}

MinesweeperData::~MinesweeperData() {}

bool MinesweeperData::SetWidth(const int InWidth) {
	Width = FMath::Max(0, InWidth);
	return InWidth > 0;
}

int MinesweeperData::GetWidth() const {
	return Width;
}

bool MinesweeperData::SetHeigth(int InHeigth) {
	Heigth = FMath::Max(0, InHeigth);
	return InHeigth > 0;
}

int MinesweeperData::GetHeigth() const {
	return Heigth;
}

bool MinesweeperData::SetMines(int InMines) {
	Mines = FMath::Max(0, InMines);
	return InMines > 0;
}

int MinesweeperData::GetMines() const {
	return Mines;
}

void MinesweeperData::SetGameOver() {
	GameOver = true;
}

bool MinesweeperData::GetGameOver() const {
	return GameOver;
}

bool MinesweeperData::AreSettingsValid() const {
	return Width > 0 && Heigth > 0 && Mines > 0 && Mines <= GridSize();
}

void MinesweeperData::SpawnMines() {
	GameOver = false;
	for (int i = 0; i < Mines; ++i) {
		bool AlreadyInSet;
		do {
			MinesPosition.Add(FMath::RandRange(0, GridSize() - 1), &AlreadyInSet);
		} while (AlreadyInSet);
	}
}

int MinesweeperData::IndexOf(const int InX, const int InY) const {
	return InY * Width + InX;
}

bool MinesweeperData::IsOutOfGrid(const int InX, const int InY) const {
	return InX < 0 || InX >= Width || InY < 0 || InY >= Heigth;
}

bool MinesweeperData::ThereIsMineAt(const int InX, const int InY) const {
	return MinesPosition.Contains(IndexOf(InX, InY));
}

int MinesweeperData::CountNearestMines(const int InX, const int InY) const {
	int NearesthBomb = 0;
	for (int NearY = -1; NearY <= 1; ++NearY) {
		for (int NearX = -1; NearX <= 1; ++NearX) {
			if (IsOutOfGrid(InX + NearX, InY + NearY)) {
				continue;
			}
			if (MinesPosition.Contains(IndexOf(InX + NearX, InY + NearY))) {
				NearesthBomb++;
			}
		}
	}
	return NearesthBomb;
}

int MinesweeperData::GridSize() const {
	return Width * Heigth;
}