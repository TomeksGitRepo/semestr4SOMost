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

class ThreadClass {
	public:
		ThreadClass() {}
		virtual ~ThreadClass() {}
		bool StartInternalThread() {
			return (pthread_create(&_thread, NULL, InternalThreadEntryFunc, this) == 0);
		}

		void WaitForInternalThreadToExit() {
			(void) pthread_join(_thread, NULL);
		}

	protected:
		virtual void InternalThreadEntry() = 0;

	private:
		static void* InternalThreadEntryFunc(void* This) {((ThreadClass*)This)->InternalThreadEntry(); return NULL; }
		pthread_t _thread;
};



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
	Most(vector<Miasto*> &miasta);
	int nr_samochodu = 0; //0 means most is empty
	Miasto* obecne_miasto;
	vector<Miasto*> miasta_copie;
	void set_nr_samochodu(int nr_samochodu);
	int get_nr_przejezdzajacego();
	void set_z_miasta(Miasto* miasto_start);
	void zaimij_most();

	private:
	mutex mtx;
};

Most::Most(vector<Miasto*> &miasta) {
	this->miasta_copie = miasta;
}

		


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

class Samochod : public ThreadClass {
	public:
	Samochod(int numer, Most* most_pointer, Miasto* obecne_miasto, vector<Miasto*> &miasta);
	int numer;
	void przejedz_przez_most();
	void przejadzka_po_miescie();
	void pojedz_do_kolejki();
	void run();
	void InternalThreadEntry();
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
	
	while(1) {
		//TODO add check if can take bridge and if can run thru
		if(this->numer == this->obecne_miasto->kolejka_samochodow.front()) {
			this->przejedz_przez_most();
		}
	int losowa_licza = rand() % 2;
	if (losowa_licza == 0) {
		this->przejadzka_po_miescie();
	} else {
		this-> pojedz_do_kolejki();
	}
	sleep(10);
	}
	

}

void Samochod::InternalThreadEntry() {
	this->run();
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
	std::list<int>::iterator iter = std::find (this->kolejka_samochodow.begin(), this->kolejka_samochodow.end(), do_dodania);	
	if (iter == this->kolejka_samochodow.end()) {
	this->kolejka_samochodow.push_back(do_dodania);
	}
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
	vector<Miasto*> miasta;	
	
	Most most(miasta);
	string A = "A";
	string B = "B";
	
	//Creation of miasta vector
	miasta.push_back(new Miasto("A"));
	miasta.push_back(new Miasto("B"));

	Most* pointer_most = &most;
	
	Samochod testowy(5, pointer_most, miasta[1], miasta);
	vector<Samochod*> samochody;
	samochody.push_back(&testowy);
	testowy.StartInternalThread();
	

	while(1)
		wyswietl_stan(pointer_most, miasta);

	
	return 0;
}
