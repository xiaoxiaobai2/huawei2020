#include <bits/stdc++.h>

using namespace std;

#define TEST
int threadnum = 4;
vector<int> all_loops;

typedef unsigned int ui;
vector<array<string, 5>> Ans;
class Solution {
  public:
    vector<vector<int>> G, GF;
    unordered_map<ui, int> idHash; // sorted id to 0...n
    vector<ui> ids;                // 0...n to sorted id
    vector<ui> inputs;             // u-v pairs
    vector<int> inDegrees, outDegrees;
    vector<bool> vis, able4;
    unordered_map<int, vector<array<int, 2>>> p4;

    int nodeCnt;
    int loops;
    void operator()(int threadId) {
        vis = vector<bool>(nodeCnt, false);
        int path[7];
        able4 = vector<bool>(nodeCnt, false);
        for (int i = threadId; i < nodeCnt; i += threadnum) {
            // cout << threadId << endl;
            if (!G[i].empty()) {
                prepare(i);
                if (p4.size() == 0) {
                    continue;
                }
                dfs(i,i,1,path);
                p4.clear();
                able4 = vis;
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
        G = vector<vector<int>>(nodeCnt);
        GF = vector<vector<int>>(nodeCnt);
        for (int i = 0; i < sz; i += 2) {
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]];
            G[u].push_back(v);
            GF[v].push_back(u);
        }

        for (int i = 0; i < nodeCnt; i++) {
            sort(G[i].begin(), G[i].end());
            sort(GF[i].begin(), GF[i].end());
        }
        loops = 0;
    }

    void backhome(int head, int cur, int depth, int *path) {
        path[depth - 1] = cur;
        string tmp = "";
        for (int i = 0; i < depth; i++) {
            tmp.append(to_string(ids[path[i]]) + ",");
        }
        for (array<int, 2> &u : p4[cur]) {
            if (!vis[u[0]] && !vis[u[1]]) {
                Ans[head][depth - 1].append(tmp);
                Ans[head][depth - 1].append(to_string(ids[u[0]]) + ",");
                Ans[head][depth - 1].append(to_string(ids[u[1]]) + "\n");
                loops++;
            }
        }
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
                backhome(head, v, depth + 1, path);
            }
            dfs(head, v, depth + 1, path);
        }
        vis[cur] = false;
    }

    void prepare(int i) {
        array<int, 2> temp2;
        vector<array<int,2>> depth3;
        string ans3="";
        for (int &v : GF[i]) {
            temp2[1] = v;
            if (v > i) {
                for (int &u : GF[v]) {
                    if (u > i) {
                        temp2[0] = u;
                        for (int &w : GF[u]) {
                            if(w!=v){
                                //构成三环
                                if(w==i){
                                    depth3.push_back(temp2);
                                    loops++;
                                }

                                else if (w > i) {
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
        sort(depth3.begin(),depth3.end());
        string head = to_string(ids[i]) + ",";
        for (array<int,2> &u:depth3)
        {
            Ans[i][0].append(head);
            Ans[i][0].append(to_string(ids[u[0]]) + ",");
            Ans[i][0].append(to_string(ids[u[1]]) + "\n");
        }
    }

    /*
        fwrite 逐行写
    */
    void save() {
        // FILE *file = fopen("output.txt", "w");
        FILE *file = fopen("/projects/student/result.txt", "w");
        for (int &u : all_loops) {
            loops += u;
        }
        fprintf(file, "%d\n", loops);
        char *buf;
        for (int i = 0; i < 5; i++) {
            for (int k = 0; k < nodeCnt; k++) {
                buf = new char[Ans[k][i].length() + 1];
                strcpy(buf, Ans[k][i].c_str());
                fwrite(buf, 1, Ans[k][i].length(), file);
            }
        }
        fflush(file);
        fclose(file);
    }
};

int main() {
    extern vector<array<string, 5>> Ans;
    extern vector<int> all_loops;
    // string testFile = "test_data.txt";
    string testFile = "/data/test_data.txt";
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
    return 0;
}