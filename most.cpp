#include <string>
#include <iostream>

using namespace std;

class Miasto {
	public:
	string nazwa;
	int ilosc_samochodow;

	public:
	void zwieksz_ilosc();
	void zmniejsz_ilosc();
	void podaj_ilosc_samochodow();
};

void Miasto::zwieksz_ilosc() {
	this->ilosc_samochodow++;
}

void Miasto::zmniejsz_ilosc() {
	this->ilosc_samochodow--;
}

void Miasto::podaj_ilosc_samochodow() {
	cout << "Ilość samochogów " << this->ilosc_samochodow <<endl;
}

int main() {
	Miasto A;
	A.nazwa = "A";
	A.ilosc_samochodow = 10;

	A.zwieksz_ilosc();
	A.podaj_ilosc_samochodow();
}
