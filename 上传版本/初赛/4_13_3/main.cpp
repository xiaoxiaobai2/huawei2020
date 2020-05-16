#include <bits/stdc++.h>
using namespace std;

#define TEST
// 计算完18.312
//输出完 29.905
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
bool vector_sort(vector<int> v1, vector<int> v2) {
    return v1[v1.size() - 1] > v2[v1.size() - 1] ? true : false;
}
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
    vector<bool> vis, able;
    vector<vector<int>> frmpath1, frmpath2, frmpath3;
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
    void dfs(int head, int cur, int depth, vector<int> &path) {
        if (depth == 5) {
            // vis[cur] = true;
            if (!able[cur]) {
                return;
            }
            path.push_back(cur);
            for (vector<int> &u : frmpath1) {
                if (u[0] == cur) {
                    // cout << head << "找到循环" << cur << endl;
                    vector<ui> tmp;
                    for (int &x : path)
                        tmp.push_back(ids[x]);
                    ans.emplace_back(Path(depth, tmp));
                }
            }
            for (vector<int> &u : frmpath2) {
                if (u[1] == cur && !vis[u[0]]) {
                    path.push_back(u[0]);
                    vector<ui> tmp;
                    for (int &x : path)
                        tmp.push_back(ids[x]);
                    ans.emplace_back(Path(depth + 1, tmp));
                    path.pop_back();
                }
            }
            for (vector<int> &u : frmpath3) {
                if (u[2] == cur && !vis[u[1]] && !vis[u[0]]) {
                    path.push_back(u[1]);
                    path.push_back(u[0]);
                    vector<ui> tmp;
                    for (int &x : path)
                        tmp.push_back(ids[x]);
                    ans.emplace_back(Path(depth + 2, tmp));
                    path.pop_back();
                    path.pop_back();
                }
            }
            path.pop_back();
        } else {
            vis[cur] = true;
            path.push_back(cur);
            for (int &v : G[cur]) {
                if (depth <= 4 && v == head && depth >= 3) {
                    vector<ui> tmp;
                    for (int &x : path)
                        tmp.push_back(ids[x]);
                    ans.emplace_back(Path(depth, tmp));
                }
                if (depth < 5 && !vis[v] && v > head) {
                    dfs(head, v, depth + 1, path);
                }
            }
            vis[cur] = false;
            path.pop_back();
        }
    }

    void prepare(int i, int depth) {
        // cout << i << "   " << depth << endl;
        switch (depth) {
        case 1: {
            // cout << i << " 进来了，入度为 " << GF[i].size() << endl;
            for (int &v : GF[i]) {
                // cout << GF[i].size() << "   " << i << "大小" << endl;
                if (v > i) {

                    vector<int> temp;
                    temp.push_back(v);
                    frmpath1.push_back(temp);
                    able[v] = true;
                }
            }
            depth++;
            prepare(i, depth);
            break;
        }
        case 2: {
            for (vector<int> &v : frmpath1) {
                for (int &u : GF[v[0]]) {
                    if (u > i) {
                        vector<int> temp;
                        temp.push_back(v[0]);
                        temp.push_back(u);
                        frmpath2.push_back(temp);
                        able[u] = true;
                    }
                    // temp[0] = u;
                }
            }
            depth++;
            prepare(i, depth);
            break;
        }
        case 3: {
            for (vector<int> &v : frmpath2) {
                for (int &u : GF[v[1]]) {
                    if (u > i && u != v[0]) {
                        vector<int> temp;
                        temp.push_back(v[0]);
                        temp.push_back(v[1]);
                        temp.push_back(u);
                        frmpath3.push_back(temp);
                        able[u] = true;
                    }
                    // temp[0] = u;
                }
            }
            break;
        }
        default:
            break;
        }
    }

    // search from 0...n
    //由于要求id最小的在前，因此搜索的全过程中不考虑比起点id更小的节点
    void solve() {
        vis = vector<bool>(nodeCnt, false);
        vector<int> path;
        for (int i = 0; i < nodeCnt; i++) {
            if (!G[i].empty()) {
                // cout << i << "准备" << endl;
                able = vector<bool>(nodeCnt, false);
                prepare(i, 1);
                // sort(frmpath1.begin(), frmpath1.end(), vector_sort);
                // sort(frmpath2.begin(), frmpath2.end(), vector_sort);
                // sort(frmpath3.begin(), frmpath3.end(), vector_sort);
                // cout << i << "准备好了" << endl;
                dfs(i, i, 1, path);
                frmpath1.clear();
                frmpath2.clear();
                frmpath3.clear();
            }
        }
        sort(ans.begin(), ans.end());
    }

    void save(string &outputFile) {
        ofstream out(outputFile);
        out << ans.size() << endl;
        for (auto &x : ans) {
            auto path = x.path;
            int sz = path.size();
            out << path[0];
            for (int i = 1; i < sz; i++)
                out << "," << path[i];
            out << endl;
        }
    }
};

int main() {
    string testFile = "/data/test_data.txt";
    string outputFile = "/projects/student/result.txt";
    auto t = clock();
    //    for(int i=0;i<100;i++){
    Solution solution;
    solution.parseInput(testFile);
    solution.constructGraph();
    // solution.topoSort();
    solution.solve();
    solution.save(outputFile);
    return 0;
}