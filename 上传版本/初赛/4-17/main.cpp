#include <bits/stdc++.h>
using namespace std;

#define TEST
int threadnum = 8;
vector<int> all_loops;

typedef unsigned int ui;
vector<vector<vector<vector<ui>>>> Ans;
class Solution {
  public:
    // maxN=560000
    // maxE=280000 ~avgN=26000
    // vector<int> *G;
    vector<vector<int>> G, GF;
    unordered_map<ui, int> idHash; // sorted id to 0...n
    vector<ui> ids;                // 0...n to sorted id
    vector<ui> inputs;             // u-v pairs
    vector<int> inDegrees, outDegrees;
    vector<bool> vis, able3, able4;
    // vector<vector<int>> frmpath1, frmpath2, frmpath3;
    unordered_map<int, vector<vector<int>>> p3, p4;
    // vector<vector<vector<vector<ui>>>> ans;
    int nodeCnt;
    int loops;
    void operator()(int threadId) {
        vis = vector<bool>(nodeCnt, false);
        vector<int> path;
        able3 = vector<bool>(nodeCnt, false);
        able4 = vector<bool>(nodeCnt, false);
        // able = vector<bool>(nodeCnt, false);
        for (int i = threadId; i < nodeCnt; i += threadnum) {
            // cout << threadId << endl;
            if (!G[i].empty()) {
                prepare(i, 1);
                d1(i, i, 1, &path);
                p3.clear();
                p4.clear();
                able3 = vis;
                able4 = vis;
                // frmpath1.clear();
                // frmpath2.clear();
                // frmpath3.clear();
            }
        }
        all_loops[threadId] = loops;
    }

    void parseInput(string &testFile) {
        FILE *file = fopen(testFile.c_str(), "r");
        ui u, v, c;
        while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF) {
            inputs.push_back(u);
            inputs.push_back(v);
        }
    }

    void constructGraph() {
        auto tmp = inputs;
        sort(tmp.begin(), tmp.end());
        tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
        nodeCnt = tmp.size();
        ids = tmp;
        nodeCnt = 0;
        for (ui &x : tmp) {
            idHash[x] = nodeCnt++;
        }
        int sz = inputs.size();
        // G=new vector<int>[nodeCnt];
        G = vector<vector<int>>(nodeCnt);
        GF = vector<vector<int>>(nodeCnt);
        inDegrees = vector<int>(nodeCnt, 0);
        outDegrees = vector<int>(nodeCnt, 0);
        for (int i = 0; i < sz; i += 2) {
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]];
            G[u].push_back(v);
            GF[v].push_back(u);
            ++inDegrees[v];
            ++outDegrees[u];
        }

        for (int i = 0; i < nodeCnt; i++) {
            sort(G[i].begin(), G[i].end());
            sort(GF[i].begin(), GF[i].end());
        }
        loops = 0;
    }

    void backhome(int head, int cur, int depth, int able, vector<int> *path) {
        path->push_back(cur);
        vector<ui> tmp;
        for (vector<int>::iterator it = path->begin(); it != path->end(); it++)
            tmp.push_back(ids[*it]);
        tmp.push_back(0);
        switch (able) {
        case 3:
            for (vector<int> &u : p3[cur]) {
                if (!vis[u[0]]) {
                    tmp[depth] = ids[u[0]];
                    Ans[head][depth - 2].push_back(tmp);
                    loops++;
                }
            }
            break;
        case 4:
            tmp.push_back(0);
            for (vector<int> &u : p4[cur]) {
                if (!vis[u[0]] && !vis[u[1]]) {
                    tmp[depth] = ids[u[0]];
                    tmp[depth + 1] = ids[u[1]];
                    Ans[head][depth - 1].push_back(tmp);
                    loops++;
                }
            }
            break;
        }
        path->pop_back();
    }
    void d1(int head, int cur, int depth, vector<int> *path) {
        vis[cur] = true;
        path->push_back(cur);
        for (int &v : G[cur]) {
            if (v < head) {
                continue;
            }
            if (able3[v]) {
                backhome(head, v, depth + 1, 3, path);
            }
            if (able4[v]) {
                backhome(head, v, depth + 1, 4, path);
            }
            dfs(head, v, depth + 1, path);
        }
        vis[cur] = false;
        path->pop_back();
    }

    void dfs(int head, int cur, int depth, vector<int> *path) {
        if (depth == 5) {
            return;
        }
        vis[cur] = true;
        path->push_back(cur);
        for (int &v : G[cur]) {
            // 剪枝
            if (v <= head || vis[v]) {
                continue;
            }
            if (able4[v] && depth < 5) {
                backhome(head, v, depth + 1, 4, path);
            }
            dfs(head, v, depth + 1, path);
        }
        vis[cur] = false;
        path->pop_back();
    }
    // unordered_map<int, vector<vector<int>>> p3,
    void prepare(int i, int depth) {
        vector<int> temp, temp2;
        temp = vector<int>(1);
        temp2 = vector<int>(2);
        for (int &v : GF[i]) {
            temp[0] = v;
            temp2[1] = v;
            if (v > i) {
                for (int &u : GF[v]) {
                    if (u > i) {
                        // temp.push_back(u);
                        p3[u].push_back(temp);
                        able3[u] = true;
                        temp2[0] = u;
                        for (int &w : GF[u]) {
                            if (w > i && w != v) {
                                // tep.push_back(w);
                                p4[w].push_back(temp2);
                                able4[w] = true;
                                sort(p4[w].begin(), p4[w].end());
                            }
                        }
                    }
                }
            }
        }
    }

    // search from 0...n
    //由于要求id最小的在前，因此搜索的全过程中不考虑比起点id更小的节点
    void solve() {
        vis = vector<bool>(nodeCnt, false);
        vector<int> path;
        able3 = vector<bool>(nodeCnt, false);
        able4 = vector<bool>(nodeCnt, false);

        // able = vector<bool>(nodeCnt, false);
        for (int i = 0; i < nodeCnt; i++) {
            if (!G[i].empty()) {
                // cout << i << "准备" << endl;
                prepare(i, 1);
                d1(i, i, 1, &path);
                p3.clear();
                p4.clear();
                able3 = vis;
                able4 = vis;
            }
        }
    }

    void save() {
        // FILE *file = fopen("output.txt", "w");
        FILE *file = fopen("/projects/student/result.txt", "w");
        for (int &u : all_loops) {
            loops += u;
        }
        fprintf(file, "%d\n", loops);
        for (int i = 0; i < 5; i++) {
            for (int k = 0; k < nodeCnt; k++) {
                for (vector<ui> &x : Ans[k][i]) {
                    for (int j = 0; j < i + 2; j++) {
                        fprintf(file, "%u,", x[j]);
                    }
                    fprintf(file, "%u\n", x[i + 2]);
                }
            }
        }
    }
};

int main() {
    extern vector<vector<vector<vector<ui>>>> Ans;
    extern vector<int> all_loops;
    // string testFile = "test_data.txt";
    string testFile = "/data/test_data.txt";

    string outputFile = "output.txt";
    // auto t = clock();
    // t = clock();
    Solution solution;
    vector<thread> threadname;
    solution.parseInput(testFile);
    solution.constructGraph();
    Ans = vector<vector<vector<vector<ui>>>>(solution.nodeCnt,
                                             vector<vector<vector<ui>>>(5));
    all_loops = vector<int>(threadnum, 0);
    for (int i = 0; i < threadnum; i++) {
        thread mythread(solution, i);
        threadname.push_back(move(mythread));
    }
    for (int i = 0; i < threadnum; i++) {
        threadname[i].join();
    }
    // solution.solve();
    solution.save();
    return 0;
}