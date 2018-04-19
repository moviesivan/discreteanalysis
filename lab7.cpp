#include <exception>
#include <new>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>

int max(int a, int b) { return (a > b)? a : b; }

using namespace std;

void path( int i,  int j,  int k,  vector<int>& m, vector<vector<vector<int> > >& K, vector<int>& choices) {
    if( !K[i][j][k] ) return;
    if( K[i][j][k] == K[i-1][j][k] ) {
        path(i-1, j, k, m, K, choices);
    } else {
        path(i-1, j-1, k-m[i-1], m, K, choices);
        choices.push_back(i);
    }
}

int main()
{
	int n;
	int m;

	scanf("%d %d", &n, &m);

	vector<int> val(n), wt(n);
	vector<vector<vector<int> > > K(n + 1, vector<vector<int> >(n + 1, vector<int>(m + 1)));

	for (int i = 0; i < n; ++i) {
		scanf("%d %d", &wt[i], &val[i]);
	}
    
    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= i; j++) {
            for(int k = 1; k <= m; k++) {
                if( wt[i-1] <= k ) {
                    K[i][j][k] = max(K[i-1][j][k], K[i-1][j-1][k-wt[i-1]] + val[i-1]);
                    if( !K[i-1][j-1][k-wt[i-1]] && j != 1 && K[i][j][k] == val[i-1])
                        K[i][j][k] = K[i-1][j][k];
                } else {
                    K[i][j][k] = K[i-1][j][k];
                }
            }
        }
    }
    int ans = 0, ansInd = 0;
    for(int i = 0; i <= n; i++)
        if( K[n][i][m] * i > ans )
            ans = K[n][i][m] * i, ansInd = i;
    vector<int> choices;
    path(n, ansInd, m, wt, K, choices);
    printf("%d\n", ans);
    for (vector<int>::iterator it=choices.begin(); it<choices.end(); it++)
        printf("%d%s",*it, ( it+1 == choices.end()? "\n" : " "));

    

	return 0;
}
