#include <iostream>
#include<vector>
#include <set>
#include <queue>
#include <deque>
#include <fstream>
using namespace std;

struct automat{
    int nr_stari;
    int nr_caractere;
    char alfabet[100];
    int q0;
    int nr_finale;
    int finale[100];
    int l;
    vector<int> tranzitii[100][100];
};

void afisare(const automat &a){

    cout << "Numarul de stari al automatului este: " << a.nr_stari << '\n';
    cout << "Alfabetul utilizat are " << a.nr_caractere << " caractere" <<'\n';
    cout << "Caracterele utilizate sunt: ";
    for (int i = 0; i < a.nr_caractere; i++)
        cout << a.alfabet[i] << '\n';
    cout << "Starea initiala a automatului este: " << a.q0 << '\n';
    cout << "Automatul are " << a.nr_finale << " stari finale: " << '\n';
    for (int i = 0; i < a.nr_finale; i++)
        cout << a.finale[i] << " ";
    cout << '\n' << "Si " << a.l << " tranzitii, dupa cum arata matricea: " << '\n';
    for (int i = 0; i < a.nr_stari; i++)
        {
            for (int j = 0; j < a.nr_caractere; j++)
            {
                cout << "Starea "<< i << " si caracterul " << a.alfabet[j] << ": \n";
                for(int l : a.tranzitii[i][j+1])
                    cout << l << " ";
                cout <<'\n';
            }
        }
}

int gaseste_poz(const automat& a, char c)
{
    for (int i=0;  i<a.nr_caractere; i++)
        if (a.alfabet[i] == c)
            return i;
    return -1;
}

int e_finala(const automat& a, int stare)
{
    for(int i = 0; i < a.nr_finale; i++)
        if(a.finale[i] == stare)
            return i;
    return -1;
}
int gaseste_starea(const int vec[], int nr, int stare){
    for (int c = 0; c < nr; c++)
        if (vec[c] == stare)
            return c;
    return -2;
}

void citire(automat& a){
    ifstream fin("input.in");
    fin >> a.nr_stari;
    fin >> a.nr_caractere;
    for (int i = 0; i < a.nr_caractere; i++)
        fin >> a.alfabet[i];
    fin >> a.q0;
    fin >> a.nr_finale;
    for (int i = 0; i < a.nr_finale; i++)
        fin >> a.finale[i];
    fin >> a.l;
    for (int i = 0; i < a.l; i++)
    {
        int p; int q; char r;
        fin >> p >> r >> q;
        int s = gaseste_poz(a, r);
        a.tranzitii[p][s+1].push_back(q);
    }
}

void lnfa_to_nfa (automat a, automat &nfa) {
    //calculam lambda-inchiderea
    vector<int> lambda_inchidere[100];
    for (int i = 0; i < a.nr_stari; i++) {
        int vizitat[106];
        for (int j = 0; j <= 100; j++)
            vizitat[j] = 0;
        deque<int> curent;
        curent.push_back(i);
        vizitat[i] = 1;
        while (!curent.empty()) {
            int actual = curent.front();
            curent.pop_front();
            for (int j : a.tranzitii[actual][0])
                if (vizitat[j] == 0) {
                    vizitat[j] = 1;
                    curent.push_back(j);
                }
        }
        for (int j = 0; j < a.nr_stari; j++)
            if (vizitat[j] == 1)
                lambda_inchidere[i].push_back(j);
    }
    //calculul functiei de tranzitie
    vector<int> functie_de_tranzitie[100][100];
    for (int i = 0; i < a.nr_caractere; i++) {
        for (int j = 0; j < a.nr_stari; j++) {
            set<int> litere;
            for (int k : lambda_inchidere[j])
                for (int l : a.tranzitii[k][i + 1]) {
                    litere.insert(l);
                }
            set<int> f;
            for (int x : litere) {
                for (int k : lambda_inchidere[x]) {
                    f.insert(k);
                }
            }
            for (auto x : f)
                functie_de_tranzitie[j][i + 1].push_back(x);
            litere.clear();
            f.clear();
        }
    }

    //starile initiala si finale
    int q01 = a.q0;
    set<int> finale;
    for (int i = 0; i < a.nr_stari; i++) {
        for (unsigned int j = 0; j < lambda_inchidere[i].size(); j++)
            for (int k = 0; k < a.nr_finale; k++)
                if (lambda_inchidere[i][j] == a.finale[k])
                    finale.insert(i);

    }


    //eliminarea starilor redundante

    int viz[106], pus[106];
    for (int i = 0; i < 100; i++){
        viz[i] = 0;
    }
    for (int j = 0; j < 100; j++)
        pus[j] = -1;

    for (auto x : finale)
        viz[x] = 1;
    pus[0] = 0;

    for (int i = 0; i < a.nr_stari; i++) {          //vectorul pus are noul nume al starii
            if (pus[i] == -1)
                pus[i] = i;
            for (int j = i+1; j < a.nr_stari; j++)
            {   int ok = 0;
                if (viz[i] == viz[j]) {
                    for (int k = 0; k < a.nr_caractere && ok == 0; k++)
                        if (functie_de_tranzitie[i][k + 1].size() == functie_de_tranzitie[j][k + 1].size()) {
                            for (unsigned int l = 0; l < functie_de_tranzitie[i][k + 1].size() && ok == 0; l++)
                                if (functie_de_tranzitie[i][k + 1][l] != functie_de_tranzitie[j][k + 1][l])
                                    ok = 1;
                        } else
                            ok = 1;
                } else
                    ok = 1;
            if (ok == 0)
            {
                pus[j] = pus[i];
            }
        }
    }

    set<int> eliminare;

    for (int i = 0; i < a.nr_stari-1; i++)        //inlocuiesc in functia de tranzitie starile care vor disparea
        if (pus[i] != i)
        {
            for (int j = 0; j < a.nr_stari; j++)
                for (int k = 0; k < a.nr_caractere; k++)
                    for (int & l : functie_de_tranzitie[j][k+1])
                        if (l == i)
                            l = pus[i];
            eliminare.insert(i);
        }
    //eliminare posibile duplicate
    for (int i = 0; i < a.nr_stari; i++)
        for (int j = 0; j < a.nr_caractere; j++){
            set<int> duplicate;
            for (int l : functie_de_tranzitie[i][j+1])
                duplicate.insert(l);
            functie_de_tranzitie[i][j+1].clear();
            for (auto x : duplicate)
                functie_de_tranzitie[i][j+1].push_back(x);
            duplicate.clear();
        }


    int el = 0;     //stergere stari redundante din functia de tranzitie
    for (auto x : eliminare)
    {   for (int j = x - el; j < a.nr_stari; j++)
            for (int k = 0; k < a.nr_caractere; k++) {
                functie_de_tranzitie[j][k+1].clear();
                if (!functie_de_tranzitie[j+1][k+1].empty())
                    for (unsigned int l = 0; l < functie_de_tranzitie[j+1][k+1].size(); l++)
                        functie_de_tranzitie[j][k+1].push_back(functie_de_tranzitie[j+1][k+1][l]);
            }
        for(int i = 0; i < a.nr_stari; i++)
             for(int k = 0; k < a.nr_caractere; k++)
                 for(unsigned int l = 0; l < functie_de_tranzitie[i][k+1].size(); l++)
                     if (functie_de_tranzitie[i][k+1][l] >= x-el)
                         functie_de_tranzitie[i][k+1][l]--;
        el++;
    }

    a.nr_stari = a.nr_stari - eliminare.size(); //modificare numar stari
    nfa.nr_stari = a.nr_stari;
    nfa.nr_caractere = a.nr_caractere;
    for (int i = 0; i < a.nr_caractere; i++)
        nfa.alfabet[i] = a.alfabet[i];
    nfa.q0 = q01;

    int p = 0;

    for (auto x : finale)
    {int vf = 0;
     for (auto y : eliminare)
          if (y == x)
              vf = 1;
     if (vf == 0)
     {  nfa.finale[p] = x;
        p++;
     }
    }
    nfa.nr_finale = p;
    for (auto y : eliminare)
        for (int i= 0; i < nfa.nr_finale; i++)
                if (nfa.finale[i] >= y)
                    nfa.finale[i]--;
    nfa.l = 0;
    for (int i = 0; i < nfa.nr_stari; i++)
        for (int j = 0; j < nfa.nr_caractere; j++)
            for (unsigned int l = 0; l < functie_de_tranzitie[i][j+1].size(); l++)
                {nfa.tranzitii[i][j+1].push_back(functie_de_tranzitie[i][j+1][l]);
                nfa.l++;}
}

void nfa_to_dfa(automat a, automat& dfa){
    queue <set<int>> coada;
    set <set<int>> elemente;
    coada.push({a.q0});
    elemente.insert({a.q0});
    vector <int> functie_de_tranzitii[100][100];
   int loc_in_coada = 0;
    while (!coada.empty()){
        set<int> curent = coada.front();
        coada.pop();
        for (int i = 0; i < a.nr_caractere; i++)
        {   set<int> actual;
            for(auto x : curent){
                for (int j : a.tranzitii[x][i+1])
                    actual.insert(j);
            }
            if (!actual.empty())
                for (auto x : actual) {
                    functie_de_tranzitii[loc_in_coada][i + 1].push_back(x);
                    }
            auto poz = elemente.find(actual);
            if (poz == elemente.end() && *poz!=actual && !actual.empty())
            {
                coada.push(actual);
                elemente.insert(actual);
            }
            }
        loc_in_coada++;
        }
    int nr_tranzitii = 0;
    for(int i = 0; i < loc_in_coada; i++)
        for (int j = 0; j < a.nr_caractere; j++)
        {   set<int> verificare;
            for (unsigned int x = 0; x < functie_de_tranzitii[i][j+1].size(); x++)
                verificare.insert(functie_de_tranzitii[i][j+1][x]);
            int loc = 0;
            if (!verificare.empty()) {
                for (const auto& x : elemente) {
                    if (x != verificare)
                        loc++;
                    else
                        break;
                }
                functie_de_tranzitii[i][j+1].clear();
                functie_de_tranzitii[i][j+1].push_back(loc);
                nr_tranzitii++;
            }
            else
            functie_de_tranzitii[i][j+1].clear();

        }

    dfa.nr_stari = loc_in_coada;
    dfa.nr_caractere = a.nr_caractere;
    for (int i = 0; i < a.nr_caractere; i++)
        dfa.alfabet[i] = a.alfabet[i];
    dfa.q0 = a.q0;
    //stari finale
    set<int> finale;
    for (int i = 0; i < a.nr_finale; i++)
    {   int count = 0;
        for(const auto& x : elemente)
        {
            for (auto y : x)
                if (y == a.finale[i])
                    finale.insert(count);
            count++;
        }

    }
    dfa.nr_finale = finale.size();
    int p = 0;
    for (auto x : finale)
        {dfa.finale[p] = x;
        p++;}
    dfa.l = nr_tranzitii;
    for (int i = 0; i < dfa.nr_stari; i++)
        for (int j = 0; j < dfa.nr_caractere; j++)
            for (unsigned int l = 0; l < functie_de_tranzitii[i][j+1].size(); l++)
                dfa.tranzitii[i][j+1].push_back(functie_de_tranzitii[i][j+1][l]);
}



void dfa_to_dfa(automat a, automat&dfa_minimal){

    for (int i = 0; i < a.nr_stari; i++)                //in caz ca nu e completata, o completez
        for (int j = 0; j < a.nr_caractere; j++)
            if (a.tranzitii[i][j+1].empty())
                a.tranzitii[i][j+1].push_back(i);


    int matrice_echivalenta[100][100];

    for (int i = 0; i < a.nr_stari; i++)     //aici formez matricea de echivalenta
        for (int j = 0; j < a.nr_stari; j++)
        {
            matrice_echivalenta[i][j] = -1; //pas general
            if (i > j) {
                matrice_echivalenta[i][j] = 1; //pas 1
                if ((e_finala(a, i) == -1 && e_finala(a,j)!=-1) || (e_finala(a, j) == -1 && e_finala(a,i)!=-1))  //pas2
                    matrice_echivalenta[i][j] = 0;
                }
        }

    int ok = 1;
    while (ok == 1){
        ok = 0;
        for (int i = 0; i < a.nr_caractere; i++)
        for (int j = 0; j < a.nr_stari; j++)
            for (int k = 0; k < j; k++){
                int s = a.tranzitii[j][i+1][0];
                int t = a.tranzitii[k][i+1][0];
                if ((matrice_echivalenta[s][t] == 0 || matrice_echivalenta[t][s] == 0) && (matrice_echivalenta[j][k] == 1))
                {
                    ok = 1;
                    matrice_echivalenta[j][k] = 0;
                }
            }
    }
    int grupare_stari[100];
    vector <int> functie_de_tranzitie[100][100];
    int nrcrt = 0;
    for (int i = 0; i < a.nr_stari; i++)
        grupare_stari[i] = -1;

    for (int i = 0; i < a.nr_stari; i++ ) {
        for (int j = 0; j < i; j++)
            if (grupare_stari[j] != -1 && matrice_echivalenta[i][j] == 1) {
                grupare_stari[i] = grupare_stari[j];
                break;
            }
        if (grupare_stari[i] == -1)
        {
            grupare_stari[i] = nrcrt;
            nrcrt++;
        }
    }

    for (int i = 0; i < nrcrt; i++) //creaza matricea functie de tranzitie dupa poz din tranzitiile automatului
        for (int j = 0; j < a.nr_caractere; j++)
            if (functie_de_tranzitie[i][j+1].empty())
                functie_de_tranzitie[i][j+1].push_back(grupare_stari[a.tranzitii[gaseste_starea(grupare_stari, a.nr_stari, i)][j+1][0]]);

    int q01;
    q01 = grupare_stari[a.q0];
    set<int> finale;                        //starile initiala+finale noi
    for (int i = 0; i < a.nr_finale; i++)
        finale.insert(grupare_stari[a.finale[i]]);

    set <int> eliminare;         //vectorul care pastreaza starile care vor fi eliminate

    for(int i = 0; i < nrcrt; i++){                 //eliminarea starilor dead-end
        int vizitat[100];
        for (int j = 0; j <=nrcrt; j++)
            vizitat[j] = 0;
        deque<int> lista;
        lista.push_back(i);
        vizitat[i] = 1;
        while (!lista.empty()){
            int actual = lista[0];
            lista.pop_front();
            for (int j = 0; j < a.nr_caractere; j++)
                if (vizitat[functie_de_tranzitie[actual][j+1][0]] == 0)
                {
                    lista.push_back(functie_de_tranzitie[actual][j+1][0]);
                    vizitat[functie_de_tranzitie[actual][j+1][0]] = 1;
                }
        }
        int ok = 0;

        for (int j = 0; j < nrcrt && ok == 0; j++)
            for (auto x : finale)
                if (vizitat[x] == 1)
                    ok = 1;
        if (ok == 0)
            eliminare.insert(i);
    }

    deque <int> inaccesibile;
    inaccesibile.push_back(q01);
    int vizitat[100];
    for (int i = 0; i < nrcrt; i++)
        vizitat[i] = 0;
    vizitat[q01] = 1;
    while (!inaccesibile.empty()){
        int curent = inaccesibile[0];
        inaccesibile.pop_front();
        for (int j = 0; j < a.nr_caractere; j++)
            if (!functie_de_tranzitie[curent][j+1].empty() && vizitat[functie_de_tranzitie[curent][j+1][0]] == 0)
            {
                inaccesibile.push_back(functie_de_tranzitie[curent][j+1][0]);
                vizitat[functie_de_tranzitie[curent][j+1][0]] = 1;
            }

    }

    for (int i = 0; i < nrcrt; i++)
        if (vizitat[i] == 0)
            eliminare.insert(i);

    for (auto x : eliminare){           //elimin din functia de tranzitie toate aparitiile starilor eliminate
        for (int i = 0; i < nrcrt; i++)
            for (int j = 0; j < a.nr_caractere; j++)
                if (!functie_de_tranzitie[i][j+1].empty() && functie_de_tranzitie[i][j+1][0] == x)
                    {functie_de_tranzitie[i][j+1].clear();
                    functie_de_tranzitie[i][j+1].push_back(i);
    }}
    int am_eliminat = 0;
    for (auto x : eliminare){
        for (int i = x-am_eliminat; i < nrcrt-1; i++)
            for (int j = 0; j < a.nr_caractere; j++)
                for (unsigned int l = 0; l < functie_de_tranzitie[i][j+1].size();l++)
                    {functie_de_tranzitie[i][j+1].clear();
                    functie_de_tranzitie[i][j+1].push_back(functie_de_tranzitie[i+1][j+1][l]);
                    }
    nrcrt--;
    am_eliminat++;
    }

    dfa_minimal.nr_stari = nrcrt;
    dfa_minimal.nr_caractere = a.nr_caractere;

    for (int i = 0; i < a.nr_caractere; i++)
        dfa_minimal.alfabet[i] = a.alfabet[i];
    dfa_minimal.q0 = q01;
    dfa_minimal.nr_finale = finale.size();

    int i = 0;
    for (auto x : finale)
    {
        dfa_minimal.finale[i] = x;
        i++;

    }
   dfa_minimal.l = nrcrt*dfa_minimal.nr_caractere;

     for (int i = 0; i < dfa_minimal.nr_stari; i++)
        for (int j = 0; j < a.nr_caractere; j++)
            for (unsigned int l = 0; l < functie_de_tranzitie[i][j+1].size(); l++)
                dfa_minimal.tranzitii[i][j+1].push_back(functie_de_tranzitie[i][j+1][l]);
}

int main() {
    automat aut, dfa, a, minimal;
    citire(aut);
    lnfa_to_nfa(aut, a);
    //nfa_to_dfa(a, dfa);
    //dfa_to_dfa(dfa, minimal);
    afisare(a);

    return 0;
}
