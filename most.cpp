#include <string>
#include <iostream>
#include <pthread.h>
#include <list>
#include <mutex>
#include <vector>

using namespace std;

class Samochod {
	public:
		int numer;
	void przejedz_przez_most();
	void przejadzka_po_miescie();
	void pojedz_do_kolejki();
	void run();
};

void Samochod::run() {
	

}

class Most {
	public:
	int nr_samochodu = 0; //0 means most is empty
	string z_miasta;

	void set_nr_samochodu();
	int get_nr_przejezdzajacego();
	void set_z_miasta();

	private:
	mutex mtx;
};

class Miasto {
	public:
	string nazwa;
	int ilosc_samochodow;
	list<int> kolejka_samochodow;
	list<int> przejazdzka_po_miescie;
	vector<int> samochody_w_miescie;


	public:
	void zwieksz_ilosc();
	void zmniejsz_ilosc();
	void podaj_ilosc_samochodow();
	void dodaj_samochod_do_kolejki(int do_dodania);
	void dodaj_do_przejazdzki(int do_dodania);

	private:
	mutex mtx;
};

void Miasto::dodaj_samochod_do_kolejki(int do_dodania) {
	this->mtx.lock();
	this->kolejka_samochodow.push_back(do_dodania);
	this->mtx.unlock();
}

void Miasto::dodaj_do_przejazdzki(int do_dodania) {
	this->mtx.lock();
	this->przejazdzka_po_miescie.push_back(do_dodania);
	this->mtx.unlock();
}

void Miasto::zwieksz_ilosc() {
	this->ilosc_samochodow++;
}

void Miasto::zmniejsz_ilosc() {
	this->ilosc_samochodow--;
}

void Miasto::podaj_ilosc_samochodow() {
	cout << "Ilość samochogów " << this->ilosc_samochodow <<endl;
}

void Samochod::przejadzka_po_miescie() {
	//TODO place this car in Miasto array
	
	//TODO remove this car from Miasto array
}

int main() {
	Miasto A;
	A.nazwa = "A";
	A.ilosc_samochodow = 10;

	A.zwieksz_ilosc();
	A.podaj_ilosc_samochodow();

	pthread_t tid[10];

	return 0;
}
