#include <bits/stdc++.h>
using namespace std;

#define TEST
// 计算完18.312
//输出完 29.905
// vector < unordered_map<int, vector<vector<int>>>> BFS; BFS[i][j]
typedef unsigned int ui;

struct Path {
    // ID最小的第一个输出；
    //总体按照循环转账路径长度升序排序；
    //同一级别的路径长度下循环转账账号ID序列，按照字典序（ID转为无符号整数后）升序排序
    int length;
    vector<ui> path;
    Path(int length, const vector<ui> &path) : length(length), path(path) {}
    bool operator<(const Path &rhs) const {
        if (length != rhs.length)
            return length < rhs.length;
        for (int i = 0; i < length; i++) {
            if (path[i] != rhs.path[i])
                return path[i] < rhs.path[i];
        }
        return 0;
    }
};

class Solution {
  public:
    // maxN=560000
    // maxE=280000 ~avgN=26000
    // vector<int> *G;
    vector<vector<int>> G, GF;

    unordered_map<ui, int> idHash; // sorted id to 0...n
    vector<ui> ids;                // 0...n to sorted id
    // unordered_map<char, int> idchar;
    vector<ui> inputs; // u-v pairs
    vector<int> inDegrees, outDegrees;
    vector<bool> vis, able3, able4;
    // vector<vector<int>> frmpath1, frmpath2, frmpath3;
    unordered_map<int, vector<vector<int>>> p3, p4;
    vector<Path> ans;
    int nodeCnt;

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
                    ans.emplace_back(Path(depth + 1, tmp));
                }
            }
            break;
        case 4:
            tmp.push_back(0);
            for (vector<int> &u : p4[cur]) {
                if (!vis[u[0]] && !vis[u[1]]) {
                    tmp[depth] = ids[u[1]];
                    tmp[depth + 1] = ids[u[0]];
                    ans.emplace_back(Path(depth + 2, tmp));
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
            temp2[0] = v;
            if (v > i) {
                for (int &u : GF[v]) {
                    if (u > i) {
                        // temp.push_back(u);
                        p3[u].push_back(temp);
                        able3[u] = true;
                        temp2[1] = u;
                        for (int &w : GF[u]) {
                            if (w > i && w != v) {
                                // tep.push_back(w);
                                p4[w].push_back(temp2);
                                able4[w] = true;
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
                // frmpath1.clear();
                // frmpath2.clear();
                // frmpath3.clear();
            }
        }
        // cout << clock() / 1000.0 << endl;
        sort(ans.begin(), ans.end());
        // cout << clock() / 1000.0 << endl;
    }

    void save() {
        // FILE *fp = fopen("/projects/student/result.txt", "w");
        // char s[32];
        // snprintf(s, sizeof(s), "%d", ans.size());
        // fwrite(&s, 1, strlen(s), fp);
        // fwrite("\n", 1, 1, fp);
        // for (auto &x : ans) {
        //     auto path = x.path;
        //     int sz = path.size();
        //     snprintf(s, sizeof(s), "%d", path[0]);
        //     fwrite(&s, 1, strlen(s), fp);
        //     for (int i = 1; i < sz; i++) {
        //         fwrite(",", 1, 1, fp);
        //         snprintf(s, sizeof(s), "%d", path[i]);
        //         fwrite(&s, 1, strlen(s), fp);
        //     }
        //     fwrite("\n", 1, 1, fp);
        // }
        // cout << "fwrite" << endl;
        // fclose(fp);

        string result = "";
        result.append(to_string(ans.size()) + "\n");
        for (auto &x : ans) {
            auto path = x.path;
            int sz = path.size();
            for (int i = 0; i < sz - 1; i++) {
                result.append(to_string(path[i]) + ",");
            }
            result.append(to_string(path[sz - 1]) + "\n");
        }
        char *buf = new char[result.length()];
        strcpy(buf, result.c_str());
        FILE *fp = fopen("/projects/student/result.txt", "w");
        fwrite(buf, 1, result.length(), fp);
        // cout << "fwrite a big String" << endl;
        fclose(fp);
    }
};

int main() {
    string testFile = "/data/test_data.txt";
    Solution solution;
    solution.parseInput(testFile);
    solution.constructGraph();
    // cout << "我要开始了" << endl;
    solution.solve();
    solution.save();
    return 0;
}