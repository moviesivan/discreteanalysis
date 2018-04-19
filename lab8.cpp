#include <algorithm>
#include <iostream>
#include <vector>
#include <set>

using namespace std;

struct interval 
{ 
    int l;
    int r;
    int num; 
};


bool intervalless (const interval& a, const interval& b)
{
        return (a.l < b.l) || ( (a.l == b.l) && (a.r < b.r) );
}

struct intervalcmp
{
    bool operator() (const interval& a, const interval& b)
    {
        return a.num < b.num;
    }
};

int main()
{
    int n;
    cin >> n;
    vector<interval> intervals;
    interval tmp;
    for(int i = 0; i < n; i++){
        cin >> tmp.l >> tmp.r;
        tmp.num = i;
        intervals.push_back(tmp);
    }
    sort(intervals.begin(), intervals.end(),intervalless);
    int m;
    cin >> m;
    set<interval, intervalcmp> result;
    int left_bound = 0, right_bound = 0;
    for(vector<interval>::iterator it = intervals.begin(); it != intervals.end() && left_bound < m; it++){
        if( left_bound >= it->r ) 
            continue;
        vector<interval>::iterator max_right = it;
        while( left_bound >= it->l && it != intervals.end() ){
            if( it->r >= right_bound ){
                right_bound = it->r;
                max_right = it;
            }
            it++;
        }
        result.insert( *max_right );
        left_bound = right_bound;
        if( left_bound >= it->l ) 
            it--;
    }
    if( left_bound < m ) { 
        cout << 0 << endl; 
        return 0;
    }
    cout << result.size() << endl;
    for(set<interval>::iterator it = result.begin(); it != result.end(); it++)
        cout << it->l << " " << it->r << endl;	
    return 0;
}