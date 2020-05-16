#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#define TEST
int threadnum = 4;
vector<int> all_loops;
typedef unsigned int ui;
vector<array<string, 5>> Ans;
class Solution {
  public:
    // maxN=560000
    // maxE=280000 ~avgN=26000
    // vector<int> *G;
    vector<vector<int>> G, GF;
    unordered_map<ui, int> idHash; // sorted id to 0...n
    vector<ui> ids;                // 0...n to sorted id
    vector<ui> inputs;             // u-v pairs
    // vector<int> inDegrees, outDegrees;
    vector<bool> vis, able3, able4;
    // vector<vector<int>> frmpath1, frmpath2, frmpath3;
    unordered_map<int, vector<int>> p3;
    unordered_map<int, vector<array<int, 2>>> p4;

    // vector<vector<vector<vector<ui>>>> ans;
    int nodeCnt;
    int loops;
    void operator()(int threadId) {
        vis = vector<bool>(nodeCnt, false);
        int path[7];
        able3 = vector<bool>(nodeCnt, false);
        able4 = vector<bool>(nodeCnt, false);
        // able = vector<bool>(nodeCnt, false);
        for (int i = threadId; i < nodeCnt; i += threadnum) {
            // cout << threadId << endl;
            if (!G[i].empty()) {
                prepare(i);
                if (p3.size() == 0) {
                    continue;
                }
                d1(i, path);
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
        // inDegrees = vector<int>(nodeCnt, 0);
        // outDegrees = vector<int>(nodeCnt, 0);
        for (int i = 0; i < sz; i += 2) {
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]];
            G[u].push_back(v);
            GF[v].push_back(u);
            // ++inDegrees[v];
            // ++outDegrees[u];
        }

        for (int i = 0; i < nodeCnt; i++) {
            sort(G[i].begin(), G[i].end());
            sort(GF[i].begin(), GF[i].end());
        }
        loops = 0;
    }

    void backhome(int head, int cur, int depth, int able, int *path) {
        path[depth - 1] = cur;
        string tmp = "";
        for (int i = 0; i < depth; i++) {
            tmp.append(to_string(ids[path[i]]) + ",");
        }
        switch (able) {
        case 3:
            for (int &u : p3[cur]) {
                if (!vis[u]) {
                    Ans[head][depth - 2].append(tmp);
                    Ans[head][depth - 2].append(to_string(ids[u]) + "\n");
                    loops++;
                }
            }
            break;
        case 4:
            for (array<int, 2> &u : p4[cur]) {
                if (!vis[u[0]] && !vis[u[1]]) {
                    Ans[head][depth - 1].append(tmp);
                    Ans[head][depth - 1].append(to_string(ids[u[0]]) + ",");
                    Ans[head][depth - 1].append(to_string(ids[u[1]]) + "\n");
                    loops++;
                }
            }
            break;
        }
    }
    void d1(int head, int *path) {
        vis[head] = true;
        path[0] = head;
        for (int &v : G[head]) {
            if (v < head) {
                continue;
            }
            if (able3[v]) {
                backhome(head, v, 2, 3, path);
            }
            if (p4.size() == 0) {
                continue;
            }
            if (able4[v]) {
                backhome(head, v, 2, 4, path);
            }
            dfs(head, v, 2, path);
        }
        vis[head] = false;
    }

    void dfs(int head, int cur, int depth, int *path) {
        if (depth == 5) {
            return;
        }
        vis[cur] = true;
        path[depth - 1] = cur;
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
    }
    // unordered_map<int, vector<vector<int>>> p3,
    void prepare(int i) {
        array<int, 2> temp2;
        // temp = vector<int>(1);
        // temp2 = vector<int>(2);
        for (int &v : GF[i]) {
            // temp[0] = v;
            temp2[1] = v;
            if (v > i) {
                for (int &u : GF[v]) {
                    if (u > i) {
                        // temp.push_back(u);
                        p3[u].push_back(v);
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

    /*
        fwrite 逐行写
    */
    void save() {
        // int f = open("output.txt",O_RDWR | O_CREAT,0666);
        int f = open("/projects/student/result.txt",O_RDWR | O_CREAT,0666);
        // FILE *file = fopen("/projects/student/result.txt", "w");
        for (int &u : all_loops) {
            loops += u;
        }
        string result = to_string(loops)+'\n';

        // fprintf(file, "%d\n", loops);
        for (int i = 0; i < 5; i++) {
            for (int k = 0; k < nodeCnt; k++) {
                result.append(Ans[k][i]);
            }
        }
        char *buf;
        buf = new char[result.length()];
        strcpy(buf, result.c_str());
        write(f, &buf[0],result.length());
        close(f);
    }
};

int main() {
    extern vector<array<string, 5>> Ans;
    extern vector<int> all_loops;
    // string testFile = "test_data.txt";
    string testFile = "/data/test_data.txt";
    // auto t = clock();
    Solution solution;
    vector<thread> threadname;
    solution.parseInput(testFile);
    solution.constructGraph();
    Ans = vector<array<string, 5>>(solution.nodeCnt);
    all_loops = vector<int>(threadnum, 0);
    for (int i = 0; i < threadnum; i++) {
        thread mythread(solution, i);
        threadname.push_back(move(mythread));
    }
    for (int i = 0; i < threadnum; i++) {
        threadname[i].join();
    }
    solution.save();
    // cout << clock() - t << endl;
    // system("pause");
    return 0;
}