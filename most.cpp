#include <iostream>
#include <pthread.h>
#include <list>
#include <mutex>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <cstdlib>
#include <string>

using namespace std;


class Miasto {
	public:
	Miasto(string nazwa): nazwa{"None"}{this->nazwa = nazwa;};
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
	Samochod(int numer, Most* most_pointer, Miasto* obecne_miasto, vector<Miasto*> &miasta);
	int numer;
	void przejedz_przez_most();
	void przejadzka_po_miescie();
	void pojedz_do_kolejki();
	void run();
	Most* most_pointer;
	Miasto* obecne_miasto;
	vector<Miasto*> miasta_copie;
};

Samochod::Samochod(int numer, Most* most_pointer, Miasto* obecne_miasto, vector<Miasto*> &miasta) {
	this->numer = numer;
	this->most_pointer = most_pointer;
	this->obecne_miasto = obecne_miasto;
	this->obecne_miasto->zwieksz_ilosc(this->numer);
	this->miasta_copie = miasta;
}

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
	if (this->numer == this->obecne_miasto->kolejka_samochodow.front()) { //Jeżeli to jest następny samochód do przejechania przez most nie rezygnuje z kolejki
		return;
	}
	//TODO make this remove element thread save
	this->obecne_miasto->kolejka_samochodow.remove(this->numer);
	this->obecne_miasto->dodaj_do_przejazdzki(this->numer);
	sleep(10);
	this->obecne_miasto->przejazdzka_po_miescie.remove(this->numer);
	this->obecne_miasto->dodaj_samochod_do_kolejki(this->numer);
}

void Samochod::przejedz_przez_most() {
	this->most_pointer->set_nr_samochodu(this->numer);
	this->most_pointer->set_z_miasta(this->obecne_miasto);
	sleep(3);
	this->obecne_miasto->zmniejsz_ilosc(this->numer);
	
	//TODO finish this
	for(std::vector<Miasto*>::iterator it = this->miasta_copie.begin(); it != this->miasta_copie.end(); ++it) {
		cout << "Compering names" << (*it)->nazwa << " with " << this->obecne_miasto->nazwa << endl;
		if ((*it)->nazwa != this->obecne_miasto->nazwa) {
			this->obecne_miasto = *it;
			break;
		}
	}
	this->most_pointer->set_nr_samochodu(0); //0 means most is empty
	this->obecne_miasto->zwieksz_ilosc(this->numer);
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

void wyswietl_stan(Most* most, vector<Miasto*> &miasta) {

	cout << "Status mostu: " << most->nr_samochodu << ";" << endl;
	for(std::vector<Miasto*>::iterator it = miasta.begin(); it != miasta.end(); ++it) {
	cout << (*it)->nazwa << endl;
	cout << (*it)->podaj_ilosc_samochodow() << endl;
	}
}

int main() {
	Most most;
	vector<Miasto*> miasta;
	string A = "A";
	string B = "B";
	
	//Creation of miasta vector
	miasta.push_back(new Miasto("A"));
	miasta.push_back(new Miasto("B"));

	Most* pointer_most = &most;

	wyswietl_stan(pointer_most, miasta);
	Samochod testowy(5, pointer_most, miasta[1], miasta);
	testowy.przejadzka_po_miescie();
	wyswietl_stan(pointer_most, miasta);
	testowy.pojedz_do_kolejki();
	testowy.przejedz_przez_most();

	//testowy.przejadzka_po_miescie();
	sleep(10);
	wyswietl_stan(pointer_most, miasta);
	testowy.przejedz_przez_most();
	wyswietl_stan(pointer_most, miasta);

	
	return 0;
}
