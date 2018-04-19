#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

vector<int> kmp(vector<unsigned int> S, vector<unsigned int> K)
{
    vector<int> T(K.size() + 1, -1);
    vector<int> matches;

    if(K.size() == 0)
    {
        matches.push_back(0);
        return matches;
    }
    
    for(int i = 1; i <= int(K.size()); i++){
        int pos = T[i - 1];
        while(pos != -1 && K[pos] != K[i - 1]) 
            pos = T[pos];
        T[i] = pos + 1;
    }

    int sp = 0;
    int kp = 0;
    while(sp < int(S.size())){   
        while(kp != -1 && (kp == int(K.size()) || K[kp] != S[sp])) 
            kp = T[kp];
        kp++;
        sp++;
        if(kp == int(K.size())) 
            matches.push_back(sp - K.size() + 1);
    }

    return matches;
}

int main(int argc, char **argv)
{
    vector<unsigned int> pattern;
    vector<unsigned int> search;
    int Temp;
    
    while ((cin.peek()!='\n') && (cin>>Temp)) {
        pattern.push_back(Temp);
    }
    std::cin >> std::ws;
    
    for (int n = 1; cin.peek() != EOF ; n++) {
        search.clear();
        while ((cin.peek()!='\n') && (cin>>Temp)) {
            search.push_back(Temp);
        }
        std::cin >> std::ws;
        vector<int> v = kmp(search, pattern);
        vector<int>::iterator i = v.begin();

        while (i != v.end()) {
            printf("%d, %d\n", n, *i);
            i++;
        }
    
        }
    return 0;
}
