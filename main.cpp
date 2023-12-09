#include <iostream>
#include <fstream>
#include <random>
#include <conio.h>
#include <signal.h>
#include "sha256.h"

using namespace std;

struct {
	int id;
	string user;
	string nama;
	string hashPin;
} user;

void greet();
void login();
void daftar();
char optionHandler();
bool isQuit = false;

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
			char pil = optionHandler();
			switch (pil) {
				case '1':
					login();
					break;

				case '2':
					daftar();
					break;

				case '0':
					quit();
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

bool checkUser(string nama) {

}

bool validate(string nama, string hash) {

}

void login() {
	system("cls");
	cout << "==== Login ====\n\n";
	string user;
	cout << "Username : ";
	cin.ignore();
	getline(cin, user);
	if (user.length() == 0) {
		errorHandler("Username tidak boleh kosong!");
		return login();
	} else if (user.length() > 20) {
		errorHandler("Username terlalu panjang!");
		return login();
	}
	if (!checkUser(user)) {

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
	minstd_rand generator(stoi(pass));
	string toHash = user + pass + to_string(generator());
	// Implementasi SHA256 ini breaks down pada 55 character
	string hash = sha256(toHash);
	validate(user, hash);
}

void daftar() {

}

int main() {
	atexit(quit);
	signal(SIGINT, exit);
	greet();
}