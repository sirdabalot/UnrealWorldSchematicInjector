#include <iostream>
#include <Windows.h>
#include <string>
#include <queue>
#include <fstream>

using namespace std;

void readSchematic(string schemName, queue<char>& fileData) {

	string path = "C:/Schematics/" + schemName;

	ifstream schematic(path);

	if (schematic.is_open()) {
		while (!schematic.fail()) {
			fileData.push(schematic.get());
		}
	}

	schematic.close();
}

byte getIDFromChar(char c) {
	if (c == '=') { //Nwall
		return 196;
	}
	else if (c == '-') { //Swall
		return 208;
	}
	else if (c == '[') { //Wwall
		return 200;
	}
	else if (c == ']') { //Ewall
		return 188;
	}
	else if (c == 'q') { //NWwall
		return 218;
	}
	else if (c == 'e') { //NEwall
		return 191;
	}
	else if (c == 'z') { //SWwall
		return 218;
	}
	else if (c == 'c') { //SEwall
		return 217;
	}
	else if (c == 'i') { //Interior
		return 95;
	}
	else if (c == 'd') { //Door
		return 22;
	}
	else if (c == 'o') { //Shutter
		return 23;
	}
	else if (c == 'f') { //Fireplace
		return 4;
	}
	else if (c == 's') { //Saunastove
		return 5;
	}
	else if (c == 'b') { //Bench
		return 28;
	}
	else if (c == 't') { //Table
		return 240;
	}
	else if (c == 'x') { //Cellar
		return 81;
	}
	else if (c == '*') { //Spruce
		return 13;
	}
	else if (c == '|') { //SleepingBunk
		return 19;
	}
	else {
		return 250;
	}
}

void insertSchematic(queue<char>& fileData, HANDLE& processHandle, LPCVOID origin) {
	int row = 0;
	int col = 0;

	while (!fileData.empty()) {

		char c = fileData.front();
		fileData.pop();

		byte writeByte;

		if (c == '\n') {
			row++;
			col = 0;
		}
		else if (c == '\r') {

		}
		else {
			int offset = (col + (row * 0xC01));
			LPVOID tilePointer = (LPVOID)((byte*)origin + offset);
			writeByte = getIDFromChar(c);
			byte currentTile;
			ReadProcessMemory(&processHandle, tilePointer, &currentTile, sizeof(byte), NULL);
			cout << "Tile at " << col << "," << row << ":" << (int)currentTile << endl;;
			int response = WriteProcessMemory(processHandle, tilePointer, &writeByte, sizeof(byte), NULL);
			col++;
		}
	}
}

void main() {
	LPCVOID tlOriginMemAddr = (LPCVOID) 0x1381710;

	HWND windowHandle = FindWindow(0, "UnReal World");

	queue<char> fileData;

	if (windowHandle == 0) {
		cout << "No window found!" << endl;
		return;
	}
	else {
		DWORD processID;
		GetWindowThreadProcessId(windowHandle, &processID);
		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processID);

		if (!processHandle) {
			cout << "Failed to open process!" << endl;
			return;
		}

		string schematicName;

		cout << "What schematic would you like to use?: ";

		cin >> schematicName;

		readSchematic(schematicName, fileData);
		insertSchematic(fileData, processHandle, tlOriginMemAddr);

		CloseHandle(processHandle);

		cin.get();
	}
}