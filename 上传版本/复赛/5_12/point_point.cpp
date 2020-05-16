#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#define TEST
int threadnum = 4;
vector<int> all_loops;
typedef unsigned int ui;
vector<array<string, 5>> Ans;
struct VC {
    int v;
    int c;
    VC(const int v, const int c) : v(v), c(c) {}
    bool operator<(const VC &vc) const { return v < vc.v; }
};
bool COK(int cu, int cv) {
    long long c = cu;
    long long c1 = cv;

    return 0.2 * c <= c1 && 3 * c >= c1;
}
/*
    经小学弟提醒 ， 由边该回点，加if判断   从35S 到 9.5S 
        总结  花里胡哨，自作聪明！！！！
*/
class Solution {
  public:
    // maxN=560000
    // maxE=280000 ~avgN=26000
    // vector<int> *G;
    vector<vector<VC>> G, GF;
    unordered_map<ui, int> idHash; // sorted id to 0...n
    vector<string> ids;            // 0...n to sorted id
    vector<ui> inputs;             // u-v pairs
    vector<ui> counts;
    // vector<int> inDegrees, outDegrees;
    vector<bool> vis, able3, able4;
    // vector<vector<int>> frmpath1, frmpath2, frmpath3;
    unordered_map<int, vector<array<int, 3>>> p3;
    unordered_map<int, vector<array<int, 4>>> p4;

    // vector<vector<vector<vector<ui>>>> ans;
    int nodeCnt = 0;
    int loops;
    int headC;
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
            counts.push_back(c);
        }
        // cout << inputs.size() << endl;
    }

    void constructGraph() {
        auto tmp = inputs;
        // auto t = clock();
        // array<bool, 150000> bit;
        // for (ui &x : tmp) {
        //     bit[x] = true;
        // }
        sort(tmp.begin(), tmp.end());
        tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
        // cout << clock() - t << endl;
        nodeCnt = tmp.size();

        ids = vector<string>(nodeCnt);
        nodeCnt = 0;
        for (ui &x : tmp) {
            ids[nodeCnt] = to_string(x);
            idHash[x] = nodeCnt++;
        }

        int sz = inputs.size();
        // cout << sz << endl;
        // cout << nodeCnt << endl;
        // system("pause");
        // G=new vector<int>[nodeCnt];
        G = vector<vector<VC>>(nodeCnt);
        GF = vector<vector<VC>>(nodeCnt);
        // inDegrees = vector<int>(nodeCnt, 0);
        // outDegrees = vector<int>(nodeCnt, 0);
        for (int i = 0; i < sz; i += 2) {
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]],
                c = counts[i / 2];
            G[u].push_back(VC(v, c));
            GF[v].push_back(VC(u, c));
            // ++inDegrees[v];
            // ++outDegrees[u];
        }

        for (int i = 0; i < nodeCnt; i++) {
            sort(G[i].begin(), G[i].end());
            sort(GF[i].begin(), GF[i].end());
        }
        loops = 0;
        // for (VC &vc : G[idHash[982]]) {
        //     cout << ids[vc.v] << "," << vc.c << endl;
        // }
        // system("pause");
    }

    void backhome(int head, int cur, int depth, int able, int *path, int c) {
        path[depth - 1] = cur;
        string tmp = "";
        for (int i = 0; i < depth; i++) {
            tmp.append(ids[path[i]] + ",");
        }
        switch (able) {
        case 3:
            for (array<int, 3> &u : p3[cur]) {
                if (!vis[u[0]] && COK(c, u[2]) && COK(u[1], headC)) {
                    Ans[head][depth - 2].append(tmp);
                    Ans[head][depth - 2].append(ids[u[0]] + "\n");
                    loops++;
                }
            }
            break;
        case 4:
            for (array<int, 4> &u : p4[cur]) {
                if (!vis[u[0]] && !vis[u[1]] && COK(c, u[3]) &&
                    COK(u[2], headC)) {
                    Ans[head][depth - 1].append(tmp);
                    Ans[head][depth - 1].append(ids[u[0]] + ",");
                    Ans[head][depth - 1].append(ids[u[1]] + "\n");
                    loops++;
                }
            }
            break;
        }
    }
    void d1(int head, int *path) {
        vis[head] = true;
        path[0] = head;
        for (VC &v : G[head]) {
            if (v.v < head) {
                continue;
            }
            headC = v.c;
            if (able3[v.v]) {
                backhome(head, v.v, 2, 3, path, headC);
            }
            if (p4.size() == 0) {
                continue;
            }
            if (able4[v.v]) {
                backhome(head, v.v, 2, 4, path, headC);
            }
            dfs(head, v.v, 2, path, headC);
        }
        vis[head] = false;
    }

    void dfs(int head, int cur, int depth, int *path, int c) {
        if (depth == 5) {
            return;
        }
        vis[cur] = true;
        path[depth - 1] = cur;
        for (VC &v : G[cur]) {
            // 剪枝
            if (v.v <= head || vis[v.v] || !COK(c, v.c)) {
                continue;
            }
            if (able4[v.v] && depth < 5) {
                backhome(head, v.v, depth + 1, 4, path, v.c);
            }
            dfs(head, v.v, depth + 1, path, v.c);
        }
        vis[cur] = false;
    }
    // unordered_map<int, vector<vector<int>>> p3,
    void prepare(int i) {
        array<int, 4> temp2;
        array<int, 3> temp1;
        // temp = vector<int>(1);
        // temp2 = vector<int>(2);
        // int c;
        for (VC &v : GF[i]) {
            // temp[0] = v;
            temp2[1] = v.v;
            temp1[0] = v.v;
            if (v.v > i) {
                // c = v.c;
                temp2[2] = v.c;
                temp1[1] = v.c;
                for (VC &u : GF[v.v]) {
                    if (u.v > i && COK(u.c, v.c)) {
                        // c = u.c;
                        // temp.push_back(u);
                        temp1[2] = u.c;
                        p3[u.v].push_back(temp1);
                        able3[u.v] = true;
                        temp2[0] = u.v;
                        for (VC &w : GF[u.v]) {
                            if (w.v > i && w.v != v.v && COK(w.c, u.c)) {
                                // tep.push_back(w);
                                temp2[3] = w.c;
                                p4[w.v].push_back(temp2);
                                able4[w.v] = true;
                                sort(p4[w.v].begin(), p4[w.v].end());
                            }
                        }
                    }
                }
            }
        }
        // for (unordered_map<int, vector<array<int, 2>>>::iterator iter =
        //          p4.begin();
        //      iter != p4.end();iter++){
        //     sort(p4[iter->first].begin(), p4[iter->first].end());
        //      }
    }

    /*
        fwrite 逐行写
    */
    void save() {
        // FILE *file = fopen("output.txt", "w");
        // int f = open("output.txt", O_RDWR | O_CREAT, 0666);

        FILE *file = fopen("/projects/student/result.txt", "w");
        for (int &u : all_loops) {
            loops += u;
        }
        fprintf(file, "%d\n", loops);
        // cout << loops << endl;
        // char *buf;
        // string result;
        // result.reserve()
        for (int i = 0; i < 5; i++) {
            for (int k = 0; k < nodeCnt; k++) {
                // buf = new char[Ans[k][i].length() + 1];
                // strcpy(buf, Ans[k][i].c_str());
                fwrite(Ans[k][i].c_str(), 1, Ans[k][i].length(), file);
                // result.append(Ans[k][i]);
            }
        }
        // write(f, result.c_str(), result.length());
        // fwrite(result.c_str(), 1, result.length(), file);
        fflush(file);
        fclose(file);
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

    // cout << clock() - t << endl;
    // t = clock() - t;
    Ans = vector<array<string, 5>>(solution.nodeCnt);
    all_loops = vector<int>(threadnum, 0);
    for (int i = 0; i < threadnum; i++) {
        thread mythread(solution, i);
        threadname.push_back(move(mythread));
    }
    for (int i = 0; i < threadnum; i++) {
        threadname[i].join();
    }
    // cout << clock() - t << endl;
    // t = clock() - t;
    solution.save();
    // cout << clock() - t << endl;
    // t = clock() - t;
    // system("pause");
    return 0;
}