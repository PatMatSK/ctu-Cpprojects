#include <cassert>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>

using Place = size_t;

struct Map {
    size_t places;
    Place start, end;
    std::vector<std::pair<Place, Place>> connections;
    std::vector<std::vector<Place>> items;
};

template < typename F, typename S >
struct std::hash<std::pair<F, S>> {
    std::size_t operator () (const std::pair<F, S> &p) const noexcept {
        // something like boost::combine would be much better
        return std::hash<F>()(p.first) ^ (std::hash<S>()(p.second) << 1);
    }
};

using namespace std;

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
int pow ( size_t x )
{
    int res = 1;
    for ( size_t i = 0 ; i < x ; i++ )
        res = res * 2;
    return res;
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
int find_items( int found , Place dude , list<set<Place>> & items  ) {
    int right_here = found;
    int tmp = 0;
    for (const auto &i: items ) {
        if ( i.find(dude) != i.end() )
            right_here = right_here | pow(tmp);
        tmp++;
    }
    return right_here;
}


//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
vector<list<Place>> make_my_map ( const Map &graph )
{
    vector<list<Place>> cons(graph.places);
    cons.reserve( graph.places );
    for ( Place i  = 0; i < graph.places ; i++ )
        cons[i] = {};

    for ( const auto & i : graph.connections )
    {
        if (cons[i.first].empty() )
            cons[i.first] = {i.second};
        else
            cons[i.first].push_back(i.second);
        if (cons[i.second].empty() )
            cons[i.second] = {i.first};
        else
            cons[i.second].push_back(i.first);
    }
    return cons;
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
list<set<Place>> set_items ( const Map &graph )
{
    list<set<Place>> items;
    for ( const auto & i : graph.items )
    {
        set<Place> node = {};
        for ( const auto & j : i )
            node.insert(j);
        items.push_back(node);
    }
    return items;
}


//-----------------------------------------BACK-TRACKING-----------------------------------------------
//-----------------------------------------------------------------------------------------------------
list<Place> build_path (const Map & graph, vector<unordered_map<Place,pair<Place,int>>> & visited ,
                        int cnt_map , list<set<Place>> & items )
{
    list<Place> result  {graph.end};
    Place       target = graph.end;
    Place       prev_target = target;
    int         instart = find_items(0,graph.start,items);  // layer at start
    int         cnt = 0;

    target = visited[cnt_map-1][graph.end].first;
    int         lvl = visited[cnt_map-1][graph.end].second ;
    while ( (target != graph.start || lvl != instart ) && cnt < 100000  )
    {
        cnt++;
        result.push_front(target);
        prev_target = target;
        target = visited[lvl][target].first;
        lvl = visited[lvl][prev_target].second;
    }
    result.push_front(target);
    return result;
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
std::list<Place> find_path( const Map &graph )
{
    vector<list<Place>>     cons = make_my_map(graph);
    list<set<Place>>        items = set_items(graph);
    queue<pair<Place,int>>  toVisit;
    int                     cnt_map = pow(graph.items.size());
    int                     found = find_items(0,graph.start,items);

    // -----------edge-case-------------
    if ( graph.start == graph.end
    && graph.items.size() == (size_t)found )
        return {graph.start};

    toVisit.push(make_pair(graph.start, found));
    vector<unordered_map<Place,pair<Place,int>>> visited(cnt_map);         // <from,to>
    visited[found].insert({ graph.start, {1000000,found} } );


//--------------------------------------BFS-----------------------------------------

    bool    flag = false;
    Place   dude;
    while ( !toVisit.empty() )
    {
        dude = toVisit.front().first;
        found =  find_items( toVisit.front().second,dude,items );

        for ( const auto & i : cons[dude] )
        {
            if ( visited[found].find(i) != visited[found].end() )
                continue;

            int ttt = find_items(found,i,items);
            visited[ttt].insert({ i, {dude, found} });

            if ( i == graph.end && find_items( ttt,i,items ) == cnt_map-1 )
            {
                flag = true;
                break;
            }
            toVisit.push(make_pair(i,ttt));
        }
        if ( flag )
            break;
        toVisit.pop();
    }

    if ( toVisit.empty() )
    {
        return {};
    }

    return build_path(graph, visited, cnt_map, items);
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------

using TestCase = std::pair<size_t, Map>;

const std::array examples = {

        TestCase{ 1, Map{ 2, 0, 0,
                          { { 0, 1 } },
                          { { 0 } }
        }},
        TestCase{ 3, Map{ 2, 0, 0,
                          { { 0, 1 } },
                          { { 1 } }
        }},
        TestCase{ 3, Map{ 4, 0, 1,
                          { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
                          {}
        }},
        TestCase{ 4, Map{ 4, 0, 1,
                          { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
                          { { 2 } }
        }},
        TestCase{ 0, Map{ 4, 0, 1,
                          { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
                          { { 2 }, {} }
        }},
        TestCase{ 0, Map{ 5, 4, 0,
                             { { 0, 1 }, {2,3} },
                             { { 3 } }
        }},
        TestCase{ 0, Map{ 5, 4, 0,
                          {  },
                          { { 3 } }
        }},
        TestCase{ 2, Map{ 5, 4, 0,
                          { { 4, 0 }, {2,3} },
                          { { 4 } }
        }},

        TestCase{ 2, Map{ 5, 0, 1,
                          { { 0, 1 }, {2,3} },
                          { { 1 } }
        }},
        TestCase{ 4, Map{ 5, 0, 3,
                          { { 0, 1 }, {1,2},{2,3} },
                          { { 3 } }
        }},
        TestCase{ 4, Map{ 5, 0, 3,
                          { { 0, 1 }, {1,2},{2,3} },
                          { { 3 },{2} }
        }},

        TestCase{ 1, Map{ 10, 0, 0,
                          { { 0, 1 }, {2,1} },
                          {  }
        }},
        TestCase{ 11, Map{ 10, 0, 0,
                          { { 0, 1 }, {2,1} ,{2,3},
                            {4,3}, {0,4},{1,6},{6,7},{2,5} },
                          { {7}, {5} }
        }},

        TestCase{ 5, Map{ 10, 0, 0,
                          { { 0, 1 }, {1,2} },
                          { {2} }
        }},
        TestCase{ 11, Map{ 10, 0, 0,
                           { { 0, 1 }, {1,2} ,{1,3},{2,3},
                             {3,4},{4,5},{5,6},{6,7},{7,8},{2,8} },
                           { {2}, {4}, {5}, {6} }
        }},
        TestCase{ 10, Map{ 10, 0, 0,
                           { { 0, 1 }, {1,2} ,{1,3},{2,3},
                             {3,4},{4,5},{5,6},{6,7},{7,8},{2,8},{3,9} },
                           { {2}, {4}, {5,9}, {6,9} }
        }},
        TestCase{ 17, Map{ 10, 0, 0,
                           { { 0, 1 }, {0,2} ,{0,3},{0,4},
                             {1,5},{2,6},{3,7},{4,8} },
                           { {5}, {6}, {7}, {8} }
        }},
        TestCase{ 13, Map{ 10, 0, 0,
                           { { 0, 1 }, {0,2} ,{0,3},{1,4},
                             {2,5},{3,6} },
                           { {5}, {6}, {4},  }
        }},

        TestCase{ 4, Map{ 4, 0, 1,
                          { { 0, 1 }, {1,2} },
                          { {2} }
        }},
        TestCase{ 6, Map{ 6, 0, 5,
                          { { 0, 1 }, {1,2} ,
                            {2,3},{3,4}, {4,5} },
                          { {2}, {3},{4} }
        }},
};

int main() {

    int fail = 0;
    for (size_t i = 0; i < examples.size(); i++)
    {
        auto sol = find_path(examples[i].second);
        if ( sol.size() != examples[i].first)
        {
            std::cout << "Wrong anwer for map" << i << "     " << sol.size() << " - " << examples[i].first << std::endl;
            fail++;
        }
    }

    if (fail) std::cout << "Failed " << fail << " tests" << std::endl;
    else std::cout << "All tests completed" << std::endl;

    return 0;
}

