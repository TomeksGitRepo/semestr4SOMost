#include <string>
#include <iostream>
#include <pthread.h>
#include <list>
#include <mutex>
#include <vector>
#include <unistd.h>

using namespace std;


class Miasto {
	public:
	string nazwa;
	list<int> kolejka_samochodow;
	list<int> przejazdzka_po_miescie;
	vector<int> samochody_w_miescie;


	public:
	void zwieksz_ilosc();
	void zmniejsz_ilosc();
	int podaj_ilosc_samochodow();
	void dodaj_samochod_do_kolejki(int do_dodania);
	void dodaj_do_przejazdzki(int do_dodania);

	private:
	mutex mtx;
};




class Most {
	public:
	int nr_samochodu = 0; //0 means most is empty
	string z_miasta;
	Miasto* obecne_miasto;
	void set_nr_samochodu(int nr_samochodu);
	int get_nr_przejezdzajacego();
	void set_z_miasta(string miasto_start);

	private:
	mutex mtx;
};



class Samochod {
	public:
	int numer;
	string miasto;
	void przejedz_przez_most();
	void przejadzka_po_miescie();
	void pojedz_do_kolejki();
	void run();
	Most* most_pointer;
	Miasto* obecne_miasto;
};

void Samochod::run() {
	

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

void Miasto::zwieksz_ilosc() {
	this->ilosc_samochodow++;
}

void Miasto::zmniejsz_ilosc(int nr_samochodu) {
	this->samochody_w_miescie.remove(nr_samochodu);
}

int Miasto::podaj_ilosc_samochodow() {
	cout << "Ilość samochogów " << this->samochody_w_miescie.size() <<endl;
	return this->samochody_w_miescie.size();
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
