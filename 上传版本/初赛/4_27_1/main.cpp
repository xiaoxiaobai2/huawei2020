#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

int threadnum = 8;
vector<int> all_loops;

typedef unsigned int ui;
vector<array<string, 5>> Ans;
vector<vector<int>> G, GF;
unordered_map<ui, int> idHash; // sorted id to 0...n
vector<string> ids;            // 0...n to sorted id

int nodeCnt;
class Solution {
  public:
    vector<bool> vis, able4;
    unordered_map<int, vector<array<int, 2>>> p4;
    int loops;

    void operator()(int threadId) {

        vis = vector<bool>(nodeCnt, false);
        int path[7];
        able4 = vector<bool>(nodeCnt, false);
        int task = nodeCnt / threadnum;
        int end;
        if (threadId == threadnum - 1) {
            end = nodeCnt;
        } else {
            end = (threadId + 1) * task;
        }

        for (int i = threadId * task; i < end; i++) {
            // cout << threadId << endl;
            prepare(i);
            if (p4.size() == 0) {
                continue;
            }
            dfs(i, i, 1, path);
            p4.clear();
            able4 = vis;
        }
        all_loops[threadId] = loops;
    }

    void parseInput(string &testFile) {
        vector<ui> inputs;
        FILE *file = fopen(testFile.c_str(), "r");
        ui u, v, c;
        while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF) {
            inputs.push_back(u);
            inputs.push_back(v);
        }
        vector<ui> tmp = inputs;
        sort(tmp.begin(), tmp.end());
        tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
        // cout << clock() - t << endl;
        nodeCnt = tmp.size();
        nodeCnt = 0;
        for (ui &x : tmp) {
            ids.push_back(to_string(x));
            idHash[x] = nodeCnt++;
        }

        int sz = inputs.size();
        G = vector<vector<int>>(nodeCnt);
        GF = vector<vector<int>>(nodeCnt);
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

    void backhome(int &head, int &cur, int depth, int *path) {
        path[depth - 1] = cur;
        string tmp = "";
        for (int i = 0; i < depth; i++) {
            tmp.append(ids[path[i]] + ",");
        }
        for (array<int, 2> &u : p4[cur]) {
            if (!vis[u[0]] && !vis[u[1]]) {
                Ans[head][depth - 1].append(tmp);
                Ans[head][depth - 1].append(ids[u[0]] + ",");
                Ans[head][depth - 1].append(ids[u[1]] + "\n");
                loops++;
            }
        }
    }

    void dfs(int &head, int &cur, int depth, int *path) {
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
                backhome(head, v, depth + 1, path);
            }
            dfs(head, v, depth + 1, path);
        }
        vis[cur] = false;
    }
    // unordered_map<int, vector<vector<int>>> p3,
    void prepare(int i) {
        array<int, 2> temp2;
        vector<array<int, 2>> p3;
        for (int &v : GF[i]) {
            temp2[1] = v;
            if (v > i) {
                for (int &u : GF[v]) {
                    if (u > i) {
                        temp2[0] = u;
                        p3.push_back(temp2);
                    }
                }
            }
        }
        // string head = ids[i] + ",";
        sort(p3.begin(), p3.end());
        for (array<int, 2> &u : p3) {
            for (int &w : GF[u[0]]) {
                if (w == i) {
                    Ans[i][0] += ids[i] + ",";
                    Ans[i][0] += ids[u[0]] + ",";
                    Ans[i][0] += ids[u[1]] + "\n";
                    loops++;
                    continue;
                } else if (w > i && w != u[1]) {
                    p4[w].push_back(u);
                    able4[w] = true;
                }
            }
        }
    }

    /*
        fwrite 逐行写
    */
    void save() {

        // FILE *file = fopen("output.txt", "w");
        int f = open("/projects/student/result.txt", O_RDWR | O_CREAT, 0666);
        // FILE *file = fopen("/projects/student/result.txt", "w");
        for (int &u : all_loops) {
            loops += u;
        }
        string result = to_string(loops) + '\n';
        write(f, result.c_str(), result.length());

        for (int i = 0; i < 5; i++) {
            for (int k = 0; k < nodeCnt; k++) {
                write(f, Ans[k][i].c_str(), Ans[k][i].length());

                // result.append(Ans[k][i]);
            }
        }
        // cout << clock() - t << endl;
        // t = clock();
        // write(f, result.c_str(), result.length());
        // auto t = clock();
        // fwrite(result.c_str(), 1, result.length(), file);
        // cout << clock() - t << endl;
        close(f);
        // fflush(file);
        // fclose(file);
    }
};

int main() {
    extern vector<array<string, 5>> Ans;
    extern vector<int> all_loops;
    string testFile = "/data/test_data.txt";
    Solution solution;
    vector<thread> threadname;
    solution.parseInput(testFile);
    Ans = vector<array<string, 5>>(nodeCnt);
    all_loops = vector<int>(threadnum, 0);

    for (int i = 0; i < threadnum; i++) {
        thread mythread(solution, i);
        threadname.push_back(move(mythread));
    }
    for (int i = 0; i < threadnum; i++) {
        threadname[i].join();
    }
    solution.save();
    return 0;
}