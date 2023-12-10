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
#include <regex>
#include "include/sha256.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

struct MasterData {
	int id;
	string user;
	string hashPin;
};

struct UserLog {
	string dari;
	double jumlah;
};

struct UserData {
	int id;
	string nama;
	string user;
	string hashPin;
	string level;
	double saldo;
	int jmlLog;
	UserLog* log;
} currentUser;

MasterData* users;
UserLog* logs;

void greet();
void login();
void daftar(string nama = "", string user = "", string level = "user");
void menu(int num);
void errorHandler(string err);
char optionHandler();
bool isQuit = false, doneLoading = false, doneReading = false;
int totalUser, totalLog;
double bunga;

void loadingScr() {
	char spinner[4] = {'|', '/', '-', '\\'};
	int counter  = 0;
	cout << "Loading...  ";
	while(!doneLoading || !doneReading) {
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
	users = new MasterData[total+10];
	baca.close();
	ifstream bacaUser("./data/usermaster.txt");
	for (int i = 0; i < total; i++) {
		bacaUser >> users[i].id;
		bacaUser >> users[i].user;
		bacaUser >> users[i].hashPin;
	}
	bacaUser.close();
	ifstream bacaLog("./data/log.txt");
	bacaLog >> totalLog;
	logs = new UserLog[totalLog+10];
	for (int i = 0; i < totalLog; i++) {
		bacaLog >> logs[i].dari;
		bacaLog >> logs[i].jumlah;
	}
	bacaLog.close();
	doneReading = true;
	return;
}

void readUserData(string user) {
	ifstream baca("./userdata/" + user + "/data.txt");
	ifstream bacaLog("./userdata/" + user + "/log.txt");
	if (baca.fail() || bacaLog.fail()) {
		return errorHandler("Error membaca data user!");
	}
	baca >> currentUser.id >> currentUser.nama >> currentUser.user
	>> currentUser.hashPin >> currentUser.level >> currentUser.saldo;
	bacaLog >> currentUser.jmlLog;
	currentUser.log = new UserLog[currentUser.jmlLog+10];
	for (int i = 0; i < currentUser.jmlLog; i++) {
		bacaLog >> currentUser.log[i].dari;
		bacaLog >> currentUser.log[i].jumlah;
	}
	baca.close();
	bacaLog.close();
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
	});
	t1.join();
	t2.join();
	t3.join();
	ShowConsoleCursor(true);
	doneLoading = false; doneReading = false;
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
	bool isNegatif = false;
	if ((*saldoStr)[0] == '-') {
		isNegatif = true;
		saldoStr->erase(0, 1);
	}
	int saldoLen = saldoStr->length();
	int jarak = 3;
	while (saldoLen > jarak) {
		saldoStr->insert(saldoLen - jarak, 1, '.');
		jarak += 4; saldoLen += 1;
	}
	if (isNegatif) {
		saldoStr->insert(0, "-");
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
	system("cls");
	cout << "==== Log Transaksi ====\n\n";
	if (currentUser.jmlLog == 0) {
		cout << "Tidak ada record!\n\n";
	}
	for (int i = 0; i < currentUser.jmlLog; i++) {
		string jumlah;
		int jumlahDesimal;
		treatAngka(currentUser.log[i].jumlah, &jumlah, &jumlahDesimal);
		cout << currentUser.log[i].dari << "\nRp. " + jumlah + "," << setw(2) << setfill('0') << jumlahDesimal << "\n\n";
	}
	system("pause");
	return menu(2);
}

void deposit() {
	system("cls");
	// For every transaksi, jangan lupa tambahin data ke semua variable (termasuk in program)

}

void withdraw() {
	system("cls");

}

void kirimUang() {
	system("cls");

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

bool checkNameAvailability(string user) {
	ifstream baca("./userdata/" + user + "/data.txt");
	if (baca.fail()) {
		return true;
	} else {
		return false;
	}
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

void initUser(string nama, string user, string hashPin, string level = "user") {
	// update master, mkdir, create data and log
	users[totalUser].id = totalUser+1;
	users[totalUser].user = user;
	users[totalUser].hashPin = hashPin;
	totalUser += 1;
	ofstream tulis("./data/master.txt", ios::trunc);
	tulis << totalUser << "\n" << bunga;
	tulis.close();
	ofstream tulisUser("./data/usermaster.txt", ios::app);
	tulisUser << "\n\n" << totalUser << "\n" << user << "\n" << hashPin;
	tulisUser.close();
	ofstream tulisUserData("./userdata/" + user + "/data.txt");
	ofstream tulisUserLog("./userdata/" + user + "/log.txt");
	tulisUserData << totalUser << "\n" << nama << "\n" << user << "\n" << hashPin
	<< "\n" << level << "\n" << 0;
	tulisUserLog << 0;
	tulisUserData.close();
	tulisUserLog.close();
}

void daftar(string nama, string user, string level) {
	system("cls");
	cout << "==== Daftar ====\n\n";
	cout << "Masukkan nama \t\t\t\t: ";
	if (nama.length() > 0) {
		cout << nama << "\n";
	} else {
		if (!getline(cin, nama)) {
			return;
		}
		if (nama.length() == 0) {
			errorHandler("Nama tidak boleh kosong!");
			return daftar("", "", level);
		}
		if (nama.length() > 50) { // idk, I'd just use 50
			errorHandler("Nama terlalu panjang!");
			return daftar("", "", level);
		}
		if (!regex_match(nama, regex("^[A-Za-z ']+$")) || regex_match(nama, regex("^ +$"))) {
			errorHandler("Nama hanya boleh berisi alfabet dan spasi!");
			return daftar("", "", level);
		};
	}
	char ch;
	cout << "\nMasukkan username (lowercase) \t\t: ";
	if (user.length() > 0) {
		cout << user;
	} else {
		while (ch != 13) {
			ch = optionHandler();
			if (ch == 8 && user.length() > 0) {
				cout << "\b \b";
				if (!user.empty()) {
					user.pop_back();
				}
				continue;
			} else if (ch < 97 || ch > 122) {
				continue;
			}
			if (user.length() < 20) {
				user += ch;
				cout << ch;
			}
		}
		if (user.length() == 0) {
			errorHandler("Username tidak boleh kosong!");
			return daftar(nama, "", level);
		}
		if (!checkNameAvailability(user)) {
			errorHandler("Username sudah diambil!");
			return daftar(nama, "", level);
		}
	}
	cout << "\n\nMasukkan password (6 digit pin) \t: ";
	string pass = "";
	char chPass;
	while (pass.length() < 6) {
		chPass = _getch();
		if (chPass == 8 && pass.length() > 0) {
			cout << "\b \b";
			if (!pass.empty()) {
				pass.pop_back();
			}
			continue;
		} else if (chPass == 3) {
			quit();
		} else if (chPass < 48 || chPass > 57) {
			continue;
		}
		pass += chPass;
		cout << '*';
	}
	cout << "\n\nMasukkan ulang password (6 digit pin) \t: ";
	string pass2 = "";
	char chPass2;
	while (pass2.length() < 6) {
		chPass2 = _getch();
		if (chPass2 == 8 && pass2.length() > 0) {
			cout << "\b \b";
			if (!pass2.empty()) {
				pass2.pop_back();
			}
			continue;
		} else if (chPass2 == 3) {
			quit();
		} else if (chPass2 < 48 || chPass2 > 57) {
			continue;
		}
		pass2 += chPass2;
		cout << '*';
	}
	if (pass != pass2) {
		errorHandler("Password tidak sama!");
		return daftar(nama, user, level);
	}
	string hashed = hashAlgo(&user, &pass);
	system("cls");
	ShowConsoleCursor(false);
	thread i1(loadingScr);
	thread i2(initUser, nama, user, hashed, level);
	thread i3([]() { // lambda function
		sleep_for(milliseconds(2000));
		doneLoading = true;
	});
	i1.join();
	i2.join();
	i3.join();
	ShowConsoleCursor(true);
}

int main() {
	atexit(quit);
	signal(SIGINT, exit);
	init();
}