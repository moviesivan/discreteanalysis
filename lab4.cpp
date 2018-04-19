#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

int main(int argc, char **argv)
{
    vector<unsigned int> pattern;
    unsigned int Temp;
    
    string line;
    getline(cin, line);
    istringstream iss(line);
    while ( iss >> Temp) {    
        pattern.push_back(Temp);
    }

    int patternSize = pattern.size();
    vector<int> T(patternSize + 1, -1);
    vector<int*> prevPos(patternSize , NULL);
    for(int i = 1; i <= int(pattern.size()); i++){
        int pos = T[i - 1];
        while(pos != -1 && pattern[pos] != pattern[i - 1]) 
            pos = T[pos];
        T[i] = pos + 1;
    }
    
    int patternPos = 0;
    int prevIndex = 0;
    for (int n1 = 1; getline(cin, line) ; n1++) {
        istringstream iss(line);
        int n2;
        for (n2 = 1; iss >> Temp; n2++) {
            while(patternPos != -1 && (patternPos == int(pattern.size()) || pattern[patternPos] != Temp)) 
                patternPos = T[patternPos];
            patternPos++;
            int *currpos;
            currpos = new int[2];
            currpos[0] = n1; currpos[1] = n2;
            if (prevPos[prevIndex]) 
                delete[] prevPos[prevIndex];
            prevPos[prevIndex] = currpos;
            prevIndex = (prevIndex + 1) % patternSize;
            if(patternPos == int(pattern.size())) {
                printf("%d, %d\n", prevPos[prevIndex][0], prevPos[prevIndex][1]);
            }
        }
    }
    for (vector<int*>::iterator it = prevPos.begin(); it != prevPos.end(); it++) 
        delete[] *it;
    return 0;
}
