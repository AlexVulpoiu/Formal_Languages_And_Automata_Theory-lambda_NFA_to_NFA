#include <fstream>
#include <set>
#include <vector>

#define lambda int('$')

using namespace std;

int n, m, q0, k, l, l_nfa, n_nfa, poz_nfa[1005];
char alfabet[260];
bool elim[1005], viz[1005];

vector<int> v;

set<int> A, finale, tranzitie[1005][260], inchidere[1005], tranzitie_NFA[1005][260], finale_NFA;
set<int>::iterator it, itr;

set<pair<int, int>> B;
set<pair<int, int>>::iterator itb;

void citire_automat()
{
    ifstream f("automat.in");
    int i, x, y;
    char c;

    f >> n;
    f >> m;
    for(i = 0; i < m; i++)
        f >> alfabet[i];
    f >> q0;
    f >> k;
    for(i = 0; i < k; i++)
    {
        f >> x;
        finale.insert(x);
    }
    f >> l;
    for(i = 0; i < l; i++)
    {
        f >> x >> c >> y;
        tranzitie[x][int(c)].insert(y);
    }
    f.close();
}

void calculare_inchidere_stari_finale()
{
    int i, j, p;

    for(i = 0; i < n; i++)
    {
        inchidere[i].insert(i);             /// starea i face parte din propria lambda-inchidere
        v.clear();
        for(j = 0; j < n; j++)
            viz[j] = false;
        viz[i] = true;
        for(it = tranzitie[i][lambda].begin(); it != tranzitie[i][lambda].end(); it++)
        {
            v.push_back(*it);
            viz[*it] = true;
        }
        /// v = multimea de stari in care ajungem cu $ din i
        for(p = 0; p < v.size(); p++)
        {
            inchidere[i].insert(v[p]);       /// adaugam la inchiderea lui i starile din v
            for(it = tranzitie[v[p]][lambda].begin(); it != tranzitie[v[p]][lambda].end(); it++)
                if(!viz[*it])
                {
                    viz[*it] = true;
                    v.push_back(*it);        /// adaugam in v starile in care ajungem cu $ din starea pe care am verificat-o
                }
        }

        for(it = inchidere[i].begin(); it != inchidere[i].end(); it++)
            if(finale.find(*it) != finale.end())
            {   /// starea i va fi stare finala in NFA daca exista o stare finala din lambda-NFA care se afla in inchiderea starii i
                finale_NFA.insert(i);
                break;
            }
    }
}

void construire_NFA()
{
    int i, j;

    for(i = 0; i < n; i++)
        for(j = 0; j < m; j++)
        {
            A.clear();
            for(it = inchidere[i].begin(); it != inchidere[i].end(); it++)
                for(itr = tranzitie[*it][int(alfabet[j])].begin(); itr != tranzitie[*it][int(alfabet[j])].end(); itr++)
                    A.insert(*itr);    /// A = starile in care ajung cu caracterul alfabet[j] din starile care se afla in inchiderea starii i
            for(it = A.begin(); it != A.end(); it++)
                tranzitie_NFA[i][int(alfabet[j])].insert(inchidere[*it].begin(), inchidere[*it].end());
            /// tranzitia in NFA de la starea i cu caracterul alfabet[j] contine reuniunea inchiderilor starilor obtinute in multimea A
        }
}

void eliminare_stari_identice()
{
    int i, j, p;

    for(i = 0; i < n; i++)
        poz_nfa[i] = -1;
    n_nfa = 0;  /// numarul de stari din NFA
    for(i = 0; i < n; i++)
        if(!elim[i])
        {
            poz_nfa[i] = n_nfa++;   /// noua pozitie din NFA a starii i (de exemplu, daca elimin starea 3, starile 4, 5, ..., n vor veni cu o
            for(j = i + 1; j < n; j++)                   /// pozitie mai in fata in NFA)
                if(!elim[j])
                {
                    for(p = 0; p < m; p++)
                        if(tranzitie_NFA[i][int(alfabet[p])] != tranzitie_NFA[j][int(alfabet[p])])
                            break;
                    if(p == m)
                    {
                        B.insert(make_pair(j, i));    /// B contine perechi (j, i), adica aparitiile starii j vor fi inlocuite cu starea i
                        elim[j] = true;     /// starea j este eliminata
                    }
                }
        }

    for(i = 0; i < n; i++)
        if(!elim[i])
            for(j = 0; j < m; j++)
                for(itb = B.begin(); itb != B.end(); itb++)
                {
                    it = tranzitie_NFA[i][int(alfabet[j])].find(itb->first);    /// inlocuim starile eliminate
                    if(it != tranzitie_NFA[i][int(alfabet[j])].end())
                    {
                        tranzitie_NFA[i][int(alfabet[j])].erase(it);
                        tranzitie_NFA[i][int(alfabet[j])].insert(itb->second);
                    }
                }
    for(itb = B.begin(); itb != B.end(); itb++)
    {
        it = finale_NFA.find(itb->first);
        if(it != finale_NFA.end())
        {
            finale_NFA.erase(it);
            finale_NFA.insert(itb->second);
        }
    }
}

void afisare_NFA()
{
    ofstream g("automat.out");
    int i, j;

    g << "NFA:\n" << n_nfa << '\n';
    for(i = 0; i < n; i++)
        if(poz_nfa[i] != -1)
            g << "starea " << i << " din $-NFA a devenit starea " << poz_nfa[i] << " in NFA\n";
        else
            g << "starea " << i << " din $-NFA a fost eliminata\n";
    g << m << '\n';
    for(i = 0; i < m; i++)
        g << alfabet[i] << ' ';
    g << '\n';
    g << q0 << '\n';
    g << finale_NFA.size() << '\n';
    for(it = finale_NFA.begin(); it != finale_NFA.end(); it++)
        g << poz_nfa[*it] << ' ';
    g << '\n';
    l_nfa = 0;
    for(i = 0; i < n; i++)
        if(!elim[i])
            for(j = 0; j < m; j++)
                l_nfa += tranzitie_NFA[i][int(alfabet[j])].size();
    g << l_nfa << '\n';
    for(i = 0; i < n; i++)
        if(!elim[i])
            for(j = 0; j < m; j++)
                for(it = tranzitie_NFA[i][int(alfabet[j])].begin(); it != tranzitie_NFA[i][int(alfabet[j])].end(); it++)
                    g << poz_nfa[i] << ' ' << alfabet[j] << ' ' << poz_nfa[*it] << '\n';
    g.close();
}

int main()
{
    citire_automat();
    calculare_inchidere_stari_finale();
    construire_NFA();
    eliminare_stari_identice();
    afisare_NFA();
    return 0;
}
