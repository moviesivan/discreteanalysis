
#include <iostream>
#include <limits.h>
#include <string.h>
#include <queue>
#include <vector>
using namespace std;

 
bool bfs(vector<vector<int> >& rGraph, int s, int t, int parent[], int n)
{
    bool visited[n];
    memset(visited, 0, sizeof(visited));
 
    queue <int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;
 
    while (!q.empty() && visited[t] != true){
        int u = q.front();
        q.pop();
 
        for (int v=0; v<n; v++){
            if (visited[v]==false && rGraph[u][v] > 0){
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }
    return (visited[t] == true);
}
 
int fordFulkerson(vector<vector<int> >& graph, int s, int t, int n)
{
    int u, v;
 
    vector<vector<int> > rGraph(n, vector<int>(n));  
    for (u = 0; u < n; u++)
        for (v = 0; v < n; v++)
             rGraph[u][v] = graph[u][v];
 
    int parent[n];  
 
    int max_flow = 0; 
 
    while (bfs(rGraph, s, t, parent,n)){
        int path_flow = INT_MAX;
        for (v=t; v!=s; v=parent[v]){
            u = parent[v];
            path_flow = min(path_flow, rGraph[u][v]);
        }
 
        for (v=t; v != s; v=parent[v]){
            u = parent[v];
            rGraph[u][v] -= path_flow;
            rGraph[v][u] += path_flow;
        }
 
        max_flow += path_flow;
    }
 
    return max_flow;
}
 
int main()
{
    int n,m;
    cin >> n >> m;
    vector<vector<int> > graph(n, vector<int>(n));
    int f,t,v;
    for (int i = 0; i < m; ++i){
        cin >> f >> t >> v;
        graph[f-1][t-1] = v;
    }
 
    cout << "The maximum possible flow is " << fordFulkerson(graph, 0, n-1, n);
    
    return 0;
}