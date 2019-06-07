#include <string>
#include <iostream>
#include <pthread.h>
#include <list>
#include <mutex>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <cstdlib>

using namespace std;


class Miasto {
	public:
	string nazwa;
	list<int> kolejka_samochodow;
	list<int> przejazdzka_po_miescie;
	vector<int> samochody_w_miescie;


	public:
	void zwieksz_ilosc(int nr_samochodu);
	void zmniejsz_ilosc(int nr_samochodu);
	int podaj_ilosc_samochodow();
	void dodaj_samochod_do_kolejki(int do_dodania);
	void dodaj_do_przejazdzki(int do_dodania);

	private:
	mutex mtx;
};




class Most {
	public:
	int nr_samochodu = 0; //0 means most is empty
	Miasto* obecne_miasto;
	void set_nr_samochodu(int nr_samochodu);
	int get_nr_przejezdzajacego();
	void set_z_miasta(Miasto* miasto_start);
	void zaimij_most();
	private:
	mutex mtx;
};

void Most::zaimij_most() {
	mtx.lock();
	sleep(5);
	mtx.unlock();

}

void Most::set_nr_samochodu(int nr_samochodu) {
	mtx.lock();
	this->nr_samochodu = nr_samochodu;
	mtx.unlock();
}

int Most::get_nr_przejezdzajacego() {
	return this->nr_samochodu;
}

void Most::set_z_miasta(Miasto* miasto_start) {
	mtx.lock();
	this->obecne_miasto = miasto_start;
	mtx.unlock();
}

class Samochod {
	public:
	int numer;
	Miasto* miasto;
	void przejedz_przez_most();
	void przejadzka_po_miescie();
	void pojedz_do_kolejki();
	void run();
	Most* most_pointer;
	Miasto* obecne_miasto;
};

void Samochod::run() {
	int losowa_licza = rand() % 2;
	
	while(1) {
	if (losowa_licza == 0) {
		this->przejadzka_po_miescie();
	} else {
		this-> pojedz_do_kolejki();
	}
	sleep(30);
	}

}

void Samochod::pojedz_do_kolejki() {
	this->obecne_miasto->dodaj_samochod_do_kolejki(this->numer);	
}

void Samochod::przejadzka_po_miescie() {
	//TODO make this remove element thread save
	this->obecne_miasto->kolejka_samochodow.remove(this->numer);
	this->obecne_miasto->dodaj_do_przejazdzki(this->numer);
	sleep(10);
	this->obecne_miasto->przejazdzka_po_miescie.remove(this->numer);
	this->obecne_miasto->dodaj_samochod_do_kolejki(this->numer);
}

void Samochod::przejedz_przez_most() {
	this->most_pointer->set_nr_samochodu(this->numer);
	this->most_pointer->set_z_miasta(this->miasto);
	sleep(3);
	this->obecne_miasto->zmniejsz_ilosc(this->numer);
	//TODO finish this
}



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

void Miasto::zwieksz_ilosc(int nr_samochodu) {
	this->mtx.lock();
	this->samochody_w_miescie.push_back(nr_samochodu);
	this->mtx.unlock();
}

void Miasto::zmniejsz_ilosc(int nr_samochodu) {
	this->mtx.lock();
	this->samochody_w_miescie.erase(std::remove(this->samochody_w_miescie.begin(), this->samochody_w_miescie.end(), nr_samochodu), this->samochody_w_miescie.end());
	this->mtx.unlock();
}

int Miasto::podaj_ilosc_samochodow() {
	cout << "Ilość samochogów " << this->samochody_w_miescie.size() <<endl;
	return this->samochody_w_miescie.size();
}

int main() {

	pthread_t tid[10];

	return 0;
}
