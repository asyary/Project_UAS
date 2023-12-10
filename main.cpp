#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>
#include <signal.h>
#include <vector>
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
};

struct MasterData {
	int id;
	string user;
	string hashPin;
};

MasterData* users;

void greet();
void login();
void daftar();
char optionHandler();
bool isQuit = false, doneLoading = false;
int totalUser = 0;

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
	ifstream baca("./data/master.txt");
	if (baca.fail()) {
		return exit(0); // Waduh
	}
	int total;
	baca >> total;
	totalUser = total;
	users = new MasterData[total];
	for (int i = 0; i < total; i++) {
		baca >> users[i].id;
		baca >> users[i].user;
		baca >> users[i].hashPin;
	}
	baca.close();
	return;
}

void ShowConsoleCursor(bool showFlag) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

void init() {
	system("cls");
	ShowConsoleCursor(false);
	thread t1(loadingScr);
	thread t2(readMasterData);
	thread t3([]() { // lambda function
		sleep_for(milliseconds(2000));
		doneLoading = true;
		ShowConsoleCursor(true);
	});
	t1.join();
	t2.join();
	t3.join();
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
	vector<unsigned char> hash(picosha2::k_digest_size);
	picosha2::hash256(toHash.begin(), toHash.end(), hash.begin(), hash.end());
	string hashed = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
	return hashed;
}

int checkUser(string *user) {
	if (user->length() == 0) {
		errorHandler("Username tidak boleh kosong!");
		return -1;
	} else if (user->length() > 20) {
		errorHandler("Username terlalu panjang!");
		return -1;
	}
	for (int i = 0; i < totalUser; i++) {
		if (users[i].user == *user) {
			return i;
		}
	}
	errorHandler("Username tidak ditemukan!");
	return -1;
}

bool validate(string nama, string hash, int check) {
	if (users[check].user == nama && users[check].hashPin == hash) {
		return true;
	} else {
		return false;
	}
}

void login() {
	system("cls");
	cout << "==== Login ====\n\n";
	string user = "";
	cout << "Username : ";
	if (!getline(cin, user)) {
		return;
	}
	int check = checkUser(&user);
	if (check == -1) {
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
		} else if (ch == 3) {
			quit();
		} else if (ch < 48 || ch > 57) {
			continue;
		}
		pass += ch;
		cout << '*';
	}
	string hashed = hashAlgo(&user, &pass);
	if (validate(user, hashed, check)) {
		cout << "gas";
		cin >> ch;
	} else {
		errorHandler("Pin salah!");
		return login();
	}
}

void daftar() {

}

int main() {
	atexit(quit);
	signal(SIGINT, exit);
	init();
}