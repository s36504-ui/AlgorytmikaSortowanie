#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct ZestawDanych {
    std::string nazwa;
    std::vector<int> liczby;
};

// wczytuje liczby przed uruchomieniem stopera
bool wczytaj_dane(const std::string& nazwa, std::vector<int>& liczby) {
    std::ifstream plik(nazwa);
    if (!plik) {
        std::cerr << "Nie mozna otworzyc pliku: " << nazwa << '\n';
        return false;
    }

    int liczba;
    while (plik >> liczba) {
        liczby.push_back(liczba);
    }

    if (!plik.eof() || liczby.empty()) {
        std::cerr << "Plik jest pusty albo zawiera bledne dane: " << nazwa << '\n';
        return false;
    }

    return true;
}

// sortuje rekurencyjnie przez scalanie
void scal(std::vector<int>& liczby, std::vector<int>& pomocnicze,
          std::size_t poczatek, std::size_t srodek, std::size_t koniec) {
    std::size_t lewy = poczatek;
    std::size_t prawy = srodek;
    std::size_t indeks = poczatek;

    while (lewy < srodek && prawy < koniec) {
        if (liczby[lewy] <= liczby[prawy]) {
            pomocnicze[indeks++] = liczby[lewy++];
        } else {
            pomocnicze[indeks++] = liczby[prawy++];
        }
    }
    while (lewy < srodek) {
        pomocnicze[indeks++] = liczby[lewy++];
    }
    while (prawy < koniec) {
        pomocnicze[indeks++] = liczby[prawy++];
    }
    for (std::size_t i = poczatek; i < koniec; ++i) {
        liczby[i] = pomocnicze[i];
    }
}

void sortuj_rekurencyjnie(std::vector<int>& liczby, std::vector<int>& pomocnicze,
                          std::size_t poczatek, std::size_t koniec) {
    if (koniec - poczatek < 2) {
        return;
    }

    const std::size_t srodek = poczatek + (koniec - poczatek) / 2;
    sortuj_rekurencyjnie(liczby, pomocnicze, poczatek, srodek);
    sortuj_rekurencyjnie(liczby, pomocnicze, srodek, koniec);
    scal(liczby, pomocnicze, poczatek, srodek, koniec);
}

void mergesort(std::vector<int>& liczby) {
    std::vector<int> pomocnicze(liczby.size());
    sortuj_rekurencyjnie(liczby, pomocnicze, 0, liczby.size());
}

// przywraca wlasnosc kopca w wybranym fragmencie
void przesiej_w_dol(std::vector<int>& liczby, std::size_t poczatek,
                    std::size_t rozmiar, std::size_t korzen) {
    while (korzen < rozmiar / 2) {
        std::size_t najwiekszy = korzen;
        const std::size_t lewy = 2 * korzen + 1;
        const std::size_t prawy = lewy + 1;

        if (liczby[poczatek + lewy] > liczby[poczatek + najwiekszy]) {
            najwiekszy = lewy;
        }
        if (prawy < rozmiar &&
            liczby[poczatek + prawy] > liczby[poczatek + najwiekszy]) {
            najwiekszy = prawy;
        }
        if (najwiekszy == korzen) {
            return;
        }

        std::swap(liczby[poczatek + korzen], liczby[poczatek + najwiekszy]);
        korzen = najwiekszy;
    }
}

void heapsort_fragment(std::vector<int>& liczby, std::size_t poczatek,
                       std::size_t koniec) {
    const std::size_t rozmiar = koniec - poczatek;
    for (std::size_t i = rozmiar / 2; i > 0; --i) {
        przesiej_w_dol(liczby, poczatek, rozmiar, i - 1);
    }
    for (std::size_t i = rozmiar; i > 1; --i) {
        std::swap(liczby[poczatek], liczby[poczatek + i - 1]);
        przesiej_w_dol(liczby, poczatek, i - 1, 0);
    }
}

// sortuje caly wektor przez kopcowanie
void heapsort(std::vector<int>& liczby) {
    heapsort_fragment(liczby, 0, liczby.size());
}

int mediana(int a, int b, int c) {
    if (a < b) {
        return b < c ? b : (a < c ? c : a);
    }
    return a < c ? a : (b < c ? c : b);
}

void sortuj_wstawianiem(std::vector<int>& liczby, std::size_t poczatek,
                        std::size_t koniec) {
    for (std::size_t i = poczatek + 1; i < koniec; ++i) {
        const int wartosc = liczby[i];
        std::size_t j = i;
        while (j > poczatek && liczby[j - 1] > wartosc) {
            liczby[j] = liczby[j - 1];
            --j;
        }
        liczby[j] = wartosc;
    }
}

// dzieli fragment na wartosci mniejsze, rowne i wieksze od pivota
std::pair<std::size_t, std::size_t> podziel_na_trzy(
    std::vector<int>& liczby, std::size_t poczatek, std::size_t koniec) {
    const std::size_t srodek = poczatek + (koniec - poczatek) / 2;
    const int pivot = mediana(liczby[poczatek], liczby[srodek], liczby[koniec - 1]);
    std::size_t mniejsze = poczatek;
    std::size_t biezacy = poczatek;
    std::size_t wieksze = koniec;

    while (biezacy < wieksze) {
        if (liczby[biezacy] < pivot) {
            std::swap(liczby[mniejsze++], liczby[biezacy++]);
        } else if (liczby[biezacy] > pivot) {
            std::swap(liczby[biezacy], liczby[--wieksze]);
        } else {
            ++biezacy;
        }
    }
    return {mniejsze, wieksze};
}

void introsort_fragment(std::vector<int>& liczby, std::size_t poczatek,
                        std::size_t koniec, int limit_glebokosci) {
    constexpr std::size_t prog_wstawiania = 16;

    while (koniec - poczatek > prog_wstawiania) {
        if (limit_glebokosci == 0) {
            heapsort_fragment(liczby, poczatek, koniec);
            return;
        }
        --limit_glebokosci;

        const auto granice = podziel_na_trzy(liczby, poczatek, koniec);
        if (granice.first - poczatek < koniec - granice.second) {
            introsort_fragment(liczby, poczatek, granice.first, limit_glebokosci);
            poczatek = granice.second;
        } else {
            introsort_fragment(liczby, granice.second, koniec, limit_glebokosci);
            koniec = granice.first;
        }
    }

    sortuj_wstawianiem(liczby, poczatek, koniec);
}

// laczy szybkie sortowanie, kopcowanie i sortowanie przez wstawianie
void introsort(std::vector<int>& liczby) {
    const int limit_glebokosci =
        2 * static_cast<int>(std::log2(static_cast<double>(liczby.size())));
    introsort_fragment(liczby, 0, liczby.size(), limit_glebokosci);
}

using FunkcjaSortujaca = void (*)(std::vector<int>&);

struct Algorytm {
    const char* nazwa;
    FunkcjaSortujaca sortuj;
};

int main() {
    std::array<ZestawDanych, 4> zestawy{{
        {"danelosowe.txt", {}},
        {"daneprawielosowe.txt", {}},
        {"danepowtarzane.txt", {}},
        {"dane2xsort.txt", {}},
    }};

    // wczytuje wszystkie pliki przed pomiarem
    for (ZestawDanych& zestaw : zestawy) {
        if (!wczytaj_dane(zestaw.nazwa, zestaw.liczby)) {
            return 1;
        }
    }

    const std::array<Algorytm, 3> algorytmy{{
        {"sortowanie rekurencyjne (mergesort)", mergesort},
        {"heapsort", heapsort},
        {"introsort", introsort},
    }};

    std::cout << std::fixed << std::setprecision(9);
    for (const Algorytm& algorytm : algorytmy) {
        std::cout << algorytm.nazwa << '\n';
        for (const ZestawDanych& zestaw : zestawy) {
            // kopiuje dane przed wlaczeniem stopera
            std::vector<int> kopia = zestaw.liczby;
            const auto poczatek = std::chrono::steady_clock::now();
            algorytm.sortuj(kopia);
            const auto koniec = std::chrono::steady_clock::now();
            const std::chrono::duration<double> czas = koniec - poczatek;

            if (!std::is_sorted(kopia.begin(), kopia.end())) {
                std::cerr << "Blad sortowania: " << algorytm.nazwa << ", "
                          << zestaw.nazwa << '\n';
                return 1;
            }

            std::cout << zestaw.nazwa << ' ' << czas.count() << " s\n";
        }
        std::cout << '\n';
    }

    return 0;
}