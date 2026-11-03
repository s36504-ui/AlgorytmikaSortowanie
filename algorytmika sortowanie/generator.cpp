#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

constexpr int liczba_elementow = 10000;

// zapisuje liczby, po jednej w kazdym wierszu
bool zapisz_dane(const std::string& nazwa, const std::vector<int>& dane) {
    std::ofstream plik(nazwa);
    if (!plik) {
        std::cerr << "Nie mozna utworzyc pliku: " << nazwa << '\n';
        return false;
    }

    for (int liczba : dane) {
        plik << liczba << '\n';
    }

    return static_cast<bool>(plik);
}

int main() {
    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> losowa_liczba(-1000000, 1000000);

    // tworzy zbior w pelni losowy
    std::vector<int> losowe(liczba_elementow);
    for (int& liczba : losowe) {
        liczba = losowa_liczba(generator);
    }

    // tworzy 9000 liczb posortowanych i 1000 losowych na koncu
    std::vector<int> prawie_losowe(liczba_elementow);
    for (int i = 0; i < 9000; ++i) {
        prawie_losowe[i] = losowa_liczba(generator);
    }
    std::sort(prawie_losowe.begin(), prawie_losowe.begin() + 9000);
    for (int i = 9000; i < liczba_elementow; ++i) {
        prawie_losowe[i] = losowa_liczba(generator);
    }

    // powtarza kazda z 2000 liczb piec razy
    std::vector<int> powtarzane;
    powtarzane.reserve(liczba_elementow);
    for (int wartosc = 0; wartosc < 2000; ++wartosc) {
        for (int powtorzenie = 0; powtorzenie < 5; ++powtorzenie) {
            powtarzane.push_back(wartosc);
        }
    }
    std::shuffle(powtarzane.begin(), powtarzane.end(), generator);

    // sortuje te same dane dwa razy
    std::vector<int> dwa_razy_sortowane = losowe;
    std::sort(dwa_razy_sortowane.begin(), dwa_razy_sortowane.end());
    std::sort(dwa_razy_sortowane.begin(), dwa_razy_sortowane.end());

    const bool zapisano_wszystkie =
        zapisz_dane("danelosowe.txt", losowe) &&
        zapisz_dane("daneprawielosowe.txt", prawie_losowe) &&
        zapisz_dane("danepowtarzane.txt", powtarzane) &&
        zapisz_dane("dane2xsort.txt", dwa_razy_sortowane);

    if (!zapisano_wszystkie) {
        return 1;
    }

    std::cout << "Utworzono 4 pliki po " << liczba_elementow
              << " liczb w biezacym folderze.\n";
    return 0;
}