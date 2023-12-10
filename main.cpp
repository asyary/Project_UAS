#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>
#include <signal.h>
#include <vector>
#include <cmath>
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
} currentUser;

struct UserLog {
	string dari;
	double jumlah;
};

struct MasterData {
	int id;
	string user;
	string hashPin;
};

MasterData* users;
UserLog* currentLog;

void greet();
void login();
void daftar();
void menu(int num);
void errorHandler(string err);
char optionHandler();
bool isQuit = false, doneLoading = false;
int totalUser = 0;
double bunga;

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
	baca >> bunga;
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

void readUserData(string user) {
	ifstream baca("./userdata/" + user + "/data.txt");
	ifstream bacaLog("./userdata/" + user + "/log.txt");
	if (baca.fail()) {
		return errorHandler("Error membaca data user!");
	}
	baca >> currentUser.id >> currentUser.nama >> currentUser.user
	>> currentUser.hashPin >> currentUser.level >> currentUser.saldo;
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

void treatAngka(double saldo, string* saldoStr, int* desimal) {
	int newSaldo = saldo; // reminder to always round by 2 decimal places
	*saldoStr = to_string(newSaldo); // always setw(2) << setfill('0')
	int saldoLen = saldoStr->length();
	int jarak = 3;
	while (saldoLen > jarak) {
		saldoStr->insert(saldoLen - jarak, 1, '.');
		jarak += 4; saldoLen += 1;
	}
	*desimal = (double)round((saldo - newSaldo)*100);
	return;
}

void cekSaldo() {
	system("cls");
	// Do some math demi keliatan bagus angkanya
	double saldo = currentUser.saldo;
	string saldoStr;
	int desimal;
	treatAngka(saldo, &saldoStr, &desimal);
	cout << "==== Saldo ====\n\nRp. " << saldoStr << "," << setw(2) << setfill('0') << desimal << "\n\n";
	system("pause");
	menu(2);
}

double compound(double saldo, double interest, int month) {
	if (month == 0) {
		return saldo;
	}
	saldo += (saldo*interest);
	return compound(saldo, interest, month-1);
}

void bungaSaldo() {
	system("cls");
	string bungaStr = to_string(bunga*100);
	bungaStr.replace(bungaStr.find('.'), 1, ",");
	bungaStr.erase(bungaStr.find_last_not_of('0')+1);
	cout << "==== Perkiraan Saldo ====\n\nBunga kami sebesar " + bungaStr + "% per bulan\n\n";
	cout << "Masukkan jumlah waktu (dalam bulan) : ";
	string bulan;
	char num = 0;
	while (num != 13) {
		num = optionHandler();
		if (num == 8 && bulan.length() > 0) {
			cout << "\b \b";
			if (!bulan.empty()) {
				bulan.pop_back();
			}
			continue;
		} else if (num < 48 || num > 57) {
			continue;
		}
		if (bulan.length() < 4) {
			bulan += num;
			cout << num;
		}
	}
	system("cls");
	double currentSaldo = currentUser.saldo;
	double hasil = compound(currentSaldo, bunga, stoi(bulan));
	hasil = round(hasil*100)/100; // round 2 decimal digits
	string strHasil, strTambah, strSaldo;
	int desimalHasil, desimalTambah, desimalSaldo;
	treatAngka(hasil, &strHasil, &desimalHasil);
	treatAngka(currentSaldo, &strSaldo, &desimalSaldo);
	treatAngka(hasil - currentSaldo, &strTambah, &desimalTambah);
	cout << "==== Perkiraan Saldo ====\n\nDalam " + bulan + " bulan, saldo anda akan menjadi Rp. " + strHasil + "," 
	<< setw(2) << setfill('0') << desimalHasil << "\nBertambah Rp. " << strTambah + "," << desimalTambah << " dari saldo utama (Rp. "
	<< strSaldo + "," << desimalSaldo << ")\n\n";
	system("pause");
	menu(2);
}

void logTransaksi() {

}

void deposit() {

}

void withdraw() {

}

void kirimUang() {

}

void menu(int num) {
	switch (num) {
		case 1: { // main menu
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
			}}
			break;

		case 2: { // menu after logged in
			system("cls");
			cout << "==== Selamat datang, " + currentUser.nama + " ====\n\n";
			cout << "1. Informasi akun\n2. Transaksi\n0. Keluar\n\nMasukkan pilihan : ";
			char pil = '\0';
			pil = optionHandler();
			switch (pil) {
				case '1':
					menu(3);
					break;

				case '2':
					menu(4);
					break;

				case '0':
					quit();
				case '\0':
					return;
				
				default:
					cout << "Pilihan invalid!\n";
					system("pause");
					menu(2);
					break;
			}}
			break;

		case 3: { // menu informasi
			system("cls");
			cout << "==== Informasi Akun ====\n\n1. Jumlah saldo\n2. Perkiraan saldo\n3. Log transaksi\n0. Kembali";
			cout << "\n\nMasukkan pilihan : ";
			char pil = '\0';
			pil = optionHandler();
			switch (pil) {
				case '1':
					cekSaldo();
					break;

				case '2':
					bungaSaldo();
					break;

				case '3':
					logTransaksi();
					break;

				case '0':
					menu(2);
					break;
				
				case '\0':
					return;
				
				default:
					cout << "Pilihan invalid!\n";
					system("pause");
					menu(3);
					break;
			}}
			break;

		case 4: { // menu informasi
			system("cls");
			cout << "==== Transaksi ====\n\n1. Deposit\n2. Withdraw\n3. Kirim uang\n0. Kembali";
			cout << "\n\nMasukkan pilihan : ";
			char pil = '\0';
			pil = optionHandler();
			switch (pil) {
				case '1':
					deposit();
					break;

				case '2':
					withdraw();
					break;

				case '3':
					kirimUang();
					break;

				case '0':
					menu(2);
					break;
				
				case '\0':
					return;
				
				default:
					cout << "Pilihan invalid!\n";
					system("pause");
					menu(4);
					break;
			}}
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
		readUserData(user);
		return menu(2);
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