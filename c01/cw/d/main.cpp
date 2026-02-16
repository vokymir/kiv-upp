#include <iostream>
#include <thread>
#include <chrono>

void vlakna_priklad__basic() {

	// Co je vysledkem? Obcas je to spousty A, ktere nasleduje spousty B. Obcas je to spousta B, ktere nasleduje spousta A. Obcas je to smisene. Je to nepredvidatelne.
	// proc tomu tak je? Operacni system totiz planuje vlakna a muze se stat, ze vlakno, ktere vypisuje A, bude mit delsi cas CPU nez vlakno, ktere vypisuje B. Nebo naopak. Nebo se mohou stridat velmi rychle.
	// jinymi slovy: planovani vlaken je NEDETERMINISTICKE z pohledu programatora (OS samozrejme vi, kdy ktere vlakno planovat, ale programator to nevidi a nemuze to ovlivnit jinak, nez konfiguracne napr. nastavenim priority vlakna, ale to je jiz pokrocile tema).

	std::thread t([] {
		for (int i = 0; i < 1000; ++i) {
			std::cout << "A";
		}
	});

	for (int i = 0; i < 1000; ++i) {
		std::cout << "B";
	}

	// zkuste zakomentovat tento radek, zkuste ho take potom presunout pred cyklus
	t.join();
}

void vlakna_priklad__parametrizace() {

	// vlastne totez, co v predchozim prikladu, ale s pouzitim lambda funkce a parametru funkce dodanym pri vytvareni vlakna.

	auto vlaknova_funkce = [](char znak) {
		for (int i = 0; i < 1000; ++i) {
			std::cout << znak;
		}
	};

	std::thread t1(vlaknova_funkce, 'A');
	vlaknova_funkce('B');

	t1.join();
}

void vlakna_priklad__spanek() {

	// cekani lze zaridit i pomoci funkce std::this_thread::sleep_for, ktera zablokuje vlakno na zadany cas
	// remarks:
	//		- sleep je nepresny, muze se stat, ze vlakno bude probuzeno pozdeji, nez je zadano (ale nikdy drive)
	//		- sleep nepouzivejte jako synchronizacni mechanismus (na to se pozdeji naucime pouzivat mutexy, podminkove promenne, atd.)

	std::thread t([] {
		for (int i = 0; i < 10; ++i) {
			std::cout << "A";
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	});

	for (int i = 0; i < 10; ++i) {
		std::cout << "B";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	t.join();
}

void vlakna_priklad__vicevlaken() {

	// zde je zaruceno, ze START! a KONEC! budou vzdy na zacatku a konci vypisu, ale mezi nimi se bude vzdy vyskytovat 1000 A, 1000 B a 1000 C, ale poradi se bude menit (nepredvidatelne)

	std::cout << "START!" << std::endl;

	std::thread t1([] {
		for (int i = 0; i < 1000; ++i) {
			std::cout << "A";
		}
	});
	std::thread t2([] {
		for (int i = 0; i < 1000; ++i) {
			std::cout << "B";
		}
	});
	std::thread t3([] {
		for (int i = 0; i < 1000; ++i) {
			std::cout << "C";
		}
	});
	t1.join();
	t2.join();
	t3.join();

	std::cout << "KONEC!" << std::endl;
}

int main(int argc, char** argv) {

	std::cout << "Priklad 1" << std::endl;
	std::cout << "---------" << std::endl;
	vlakna_priklad__basic();
	std::cout << "\n---------" << std::endl;

	std::cout << "Priklad 2" << std::endl;
	std::cout << "---------" << std::endl;
	vlakna_priklad__parametrizace();
	std::cout << "\n---------" << std::endl;

	std::cout << "Priklad 3" << std::endl;
	std::cout << "---------" << std::endl;
	vlakna_priklad__spanek();
	std::cout << "\n---------" << std::endl;

	std::cout << "Priklad 4" << std::endl;
	std::cout << "---------" << std::endl;
	vlakna_priklad__vicevlaken();
	std::cout << "\n---------" << std::endl;

	return 0;
}
