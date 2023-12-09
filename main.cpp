#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>
#include <signal.h>
#include <iomanip>
#include "include/sha256.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

struct UserData {
	int id;
	string nama;
	string user;
	string hashPin;
	string level;
	double saldo;
	double pinjaman;
};

struct MasterData {
	int id;
	string user;
	string hashPin;
};

MasterData* user;

void greet();
void login();
void daftar();
char optionHandler();
bool isQuit = false, doneLoading = false;

void loadingScr() {
	char spinner[4] = {'|', '/', '-', '\\'};
	int counter  = 0;
	cout << "Loading...  ";
	while(!doneLoading) {
		cout << '\b' << spinner[counter];
		counter = (counter+1) % 4;
		sleep_for(milliseconds(200));
	}
}

void readMasterData() {

}

void ShowConsoleCursor(bool showFlag) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

void init() {
	ShowConsoleCursor(false);
	thread t1(loadingScr);
	thread t2(readMasterData);
	t1.join();
	t2.join();
	ShowConsoleCursor(true);
	return greet();
}

void quit() {
	isQuit = true;
	system("cls");
	cout << "Terima kasih sudah menggunakan layanan Bank ABC.\n\n";
	exit(0);
}

void menu(int num) {
	switch (num) { // main menu
		case 1:
			system("cls");
			string mainMenu = "==== Selamat Datang ====\n\n1. Login\n2. Daftar\n0. Keluar\n\n";
			cout << mainMenu << "Masukkan pilihan : ";
			char pil = '\0';
			pil = optionHandler();
			switch (pil) {
				case '1':
					login();
					break;

				case '2':
					daftar();
					break;

				case '0':
					quit();
				case '\0':
					return;
				
				default:
					cout << "Pilihan invalid!\n";
					system("pause");
					menu(1);
					break;
			}
			break;


	}
	system("cls");
	return;
}

void greet() {
	system("cls");
	string banner = R"(
 /$$$$$$$                      /$$              /$$$$$$  /$$$$$$$   /$$$$$$ 
| $$__  $$                    | $$             /$$__  $$| $$__  $$ /$$__  $$
| $$  \ $$  /$$$$$$  /$$$$$$$ | $$   /$$      | $$  \ $$| $$  \ $$| $$  \__/
| $$$$$$$  |____  $$| $$__  $$| $$  /$$/      | $$$$$$$$| $$$$$$$ | $$      
| $$__  $$  /$$$$$$$| $$  \ $$| $$$$$$/       | $$__  $$| $$__  $$| $$      
| $$  \ $$ /$$__  $$| $$  | $$| $$_  $$       | $$  | $$| $$  \ $$| $$    $$
| $$$$$$$/|  $$$$$$$| $$  | $$| $$ \  $$      | $$  | $$| $$$$$$$/|  $$$$$$/
|_______/  \_______/|__/  |__/|__/  \__/      |__/  |__/|_______/  \______/ 
)";
	cout << banner << "\nSelamat datang di aplikasi Bank ABC!\n\n";
	system("pause");
	menu(1);
}

void errorHandler(string err) {
	if (isQuit) {
		return;
	}
	system("cls");
	cout << "==== ERROR ====\n\n" + err + "\n\n";
	system("pause");
}

char optionHandler() {
	char pil;
	pil = _getch();
	if (pil == 3) {
		quit();
	}
	return pil;
};

string hashAlgo(string *user, string *pass) {
	minstd_rand generator(stoi(*pass));
	string toHash = *user + *pass + to_string(generator());
	cout << toHash << "\n";
	// Implementasi SHA256 ini breaks down pada 55 character
	string hashed = sha256(toHash);
	return hashed;
}

bool checkUser(string *user) {
	if (*user == "\0") {
		exit(0);
	}
	if (user->length() == 0) {
		errorHandler("Username tidak boleh kosong!");
		return false;
	} else if (user->length() > 20) {
		errorHandler("Username terlalu panjang!");
		return false;
	}
}

bool validate(string nama, string hash) {

}

void login() {
	system("cls");
	cout << "==== Login ====\n\n";
	string user = "\0";
	cout << "Username : ";
	getline(cin, user);
	if (!checkUser(&user)) {
		return login();
	}
	string pass = "";
	char ch;
	cout << "Password : ";
	while (pass.length() < 6) {
		ch = _getch();
		if (ch == 8 && pass.length() > 0) {
			cout << "\b \b";
			if (!pass.empty()) {
				pass.pop_back();
			}
			continue;
		} else if (ch < 48 || ch > 57) {
			continue;
		}
		pass += ch;
		cout << '*';
	}
	string hashed = hashAlgo(&user, &pass);
	cout << hashed;
	cin >> ch;
	validate(user, hashed);
}

void daftar() {

}

int main() {
	atexit(quit);
	signal(SIGINT, exit);
	greet();
}