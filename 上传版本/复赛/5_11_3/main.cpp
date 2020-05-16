#include <bits/stdc++.h>
// #include <unistd.h>
using namespace std;
typedef unsigned int ui;

struct edge {
    int u, v, c;
    // int index;
    // bool head = false;
    int u_node = -1, v_node = -1;
    vector<int> in, out;
};
int threadnum = 8;

vector<array<string, 5>> ans;
vector<int> all_loops;
vector<string> ids; // nodeCnt  to  string
vector<int> use;
vector<edge> data;
bool cmp(array<int, 4> a, array<int, 4> b) { return a[1] < b[1]; }

bool COK(int cu, int cv) {
    long long c = cu;
    long long c1 = cv;
    return 0.2 * c <= c1 && 3 * c >= c1;
}

class Check {
  public:
    vector<bool> vis, able4, edge4;
    // unordered_map<int, vector<array<int, 2>>> p4;
    unordered_map<int, vector<array<int, 3>>> p4;

    // vector<array<string, 5>> ans;
    int nodeCnt = 0;
    int num;
    int loops = 0;
    int curHead;
    int path[7];
    string headS;

    void operator()(int threadId) {
        extern vector<edge> data;

        vis = vector<bool>(nodeCnt, false);
        able4 = vector<bool>(num, false);
        edge4 = vector<bool>(num, false);
        for (int &i : use) {
            curHead = data[i].u_node;
            if ((curHead - threadId) % threadnum != 0) {
                continue;
            }
            // curHead = data[i][3]; //给 curHead 节点找环
            // cout << i << "/" << num << endl;
            prepare(i);
            if (p4.size() == 0) {
                continue;
            }
            dfs(i);
            able4 = vector<bool>(nodeCnt, false);
            edge4 = vector<bool>(num, false);
            p4.clear();
        }
        all_loops[threadId] = loops;
    }
    void readInput(string &File) {
        extern vector<edge> data;
        vector<array<int, 4>> inputs;
        unordered_map<int, int> id_node;
        vector<vector<int>> Gout, Gin;

        FILE *file = fopen(File.c_str(), "r");
        ui u, v, c;
        array<int, 4> temp;
        temp[3] = -1;
        while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF) {
            temp[0] = u;
            temp[1] = v;
            temp[2] = c;
            inputs.push_back(temp);
        }
        sort(inputs.begin(), inputs.end(), cmp);
        stable_sort(inputs.begin(), inputs.end());
        num = inputs.size();

        // 本应为nodeCnt 大小，暂时没想好
        // nodeCnt  to  string
        ids = vector<string>(num);
        // for (int i = 0; i < num; i++) {
        //     ids[i] = to_string(inputs[i][0]);
        // }
        auto inputs1 = inputs;
        // data1[i][3]是边在data中的索引
        for (int i = 0; i < num; i++) {
            inputs1[i][3] = i;
        }
        stable_sort(inputs1.begin(), inputs1.end(), cmp);
        Gout = vector<vector<int>>(num);
        Gin = vector<vector<int>>(num);

        // data: u-v-c-nodeCnt
        // data1:u-v-c-i
        int k = 0, pre = -1, kk = 0;
        for (int i = 0; i < num; i++) {
            int v = inputs1[i][1], n = inputs1[i][3];
            if (v > pre) {
                pre = v;
                ids[nodeCnt] = to_string(v);
                id_node[v] = nodeCnt++;
                k = kk;
            }
            long long c = inputs1[i][2];
            for (int j = k; j < num; j++) {
                if (inputs[j][0] == v) {
                    long long c1 = inputs[j][2];
                    if (0.2 * c <= c1 && 3 * c >= c1) {
                        // inputs[j][3] = nodeCnt;
                        Gout[n].push_back(j);
                        Gin[j].push_back(n);
                    }
                } else if (inputs[j][0] > v) {
                    kk = j;
                    break;
                }
            }
        }
        // id_node[inputs1[num - 1][1]] = nodeCnt;
        data.resize(num);
        for (int i = 0; i < num; i++) {
            if (Gin[i].size() != 0 && Gout[i].size() != 0) {
                data[i].u = inputs[i][0];
                data[i].v = inputs[i][1];
                data[i].c = inputs[i][2];
                data[i].u_node = id_node[data[i].u];
                data[i].v_node = id_node[data[i].v];
                data[i].in = Gin[i];
                data[i].out = Gout[i];
            }
        }
        for (int i = 0; i < num; i++) {
            if (data[i].v_node > data[i].u_node) {
                use.push_back(i);
            }
        }
    }
    void backhome7(int &j, int depth, int &c) {
        extern vector<edge> data;

        path[depth] = j;
        string tmp = "";
        for (int i = 0; i < depth + 1; i++) {
            tmp.append(ids[path[i]] + ",");
        }
        for (array<int, 3> &u : p4[j]) {
            if (!vis[u[0]] && !vis[u[1]] && COK(c, u[2])) {
                // if (!vis[data[u[0]].u_node] && !vis[data[u[1]].u_node]) {
                ans[curHead][depth].append(tmp);
                ans[curHead][depth].append(ids[u[0]] + ",");
                ans[curHead][depth].append(ids[u[1]] + "\n");
                loops++;
            }
        }
    }
    void backhome(int &j, int &v, int depth) {
        extern vector<edge> data;

        path[depth + 1] = v;
        string tmp = "";
        for (int i = 0; i < depth + 2; i++) {
            tmp.append(ids[path[i]] + ",");
        }
        for (array<int, 3> &u : p4[j]) {
            if (u[0] == v && !vis[u[1]]) {
                ans[curHead][depth].append(tmp);
                ans[curHead][depth].append(ids[u[1]] + "\n");
                loops++;
            }
        }
    }

    void dfs(int i) {
        extern vector<edge> data;
        path[0] = curHead;
        path[1] = data[i].v_node;
        vis[data[i].v_node] = true;
        for (int &v : data[i].out) {
            if (data[v].v_node <= curHead) {
                continue;
            }
            vis[data[v].v_node] = true;
            path[2] = data[v].v_node;
            for (int &u : data[v].out) {
                if (data[u].v_node <= curHead || vis[data[u].v_node])
                    continue;
                // 五环
                if (edge4[u]) {
                    backhome(data[u].u_node, data[u].v_node, 2);
                }

                vis[data[u].v_node] = true;
                path[3] = data[u].v_node;
                for (int &w : data[u].out) {
                    if (data[w].v_node <= curHead || vis[data[w].v_node])
                        continue;
                    // 六环
                    if (edge4[w]) {
                        backhome(data[w].u_node, data[w].v_node, 3);
                    }
                    if (able4[data[w].v_node]) {
                        // 七环
                        backhome7(data[w].v_node, 4, data[w].c);
                    }
                }
                vis[data[u].v_node] = false;
            }
            vis[data[v].v_node] = false;
            // }
        }
        vis[data[i].v_node] = false;
    }

    void prepare(int i) {
        // data: u-v-c-nodeCnt u
        extern vector<edge> data;

        int head = data[i].u_node;
        int j = data[i].v_node;
        int c = data[i].c;

        vis[head] = true;
        // vis[j] = true;

        headS = ids[head] + "," + ids[j] + ",";

        string rear = "";
        int next_v, next_u, next_w;
        // array<int, 2> temp;
        array<int, 3> temp;
        vector<array<int, 3>> circle4;
        for (int &v : data[i].in) {
            next_v = data[v].u_node;
            if (next_v > head && next_v != j) {
                temp[1] = next_v;
                rear = ids[next_v] + "\n";
                for (int &u : data[v].in) {
                    // rear = ids[data[u].u_node] + "," + rear;
                    next_u = data[u].u_node;
                    if (next_u == j) {
                        if (COK(c, data[u].c)) {
                            ans[head][0].append(headS);
                            ans[head][0].append(ids[next_v] + "\n");
                            loops++;
                        }
                    } else if (next_u > head) {
                        // path[1] = data[u][3];
                        temp[0] = next_u;
                        for (int &w : data[u].in) {
                            next_w = data[w].u_node;
                            // 找4环
                            if (next_w == j) {
                                if (COK(c, data[w].c)) {
                                    circle4.push_back(temp);
                                }
                            } else if (next_w > head && next_w != next_v) {
                                // 4边
                                temp[2] = data[w].c;
                                p4[next_w].push_back(temp);
                                sort(p4[next_w].begin(), p4[next_w].end());
                                able4[next_w] = true;
                                edge4[w] = true;
                            }
                        }
                    }
                }
            }
        }
        sort(circle4.begin(), circle4.end());
        for (array<int, 3> &tem : circle4) {
            ans[head][1].append(headS);
            ans[head][1].append(ids[tem[0]] + ",");
            ans[head][1].append(ids[tem[1]] + "\n");
            loops++;
        }
    }
    void save() {
        // FILE *file = fopen("output.txt", "w");
        FILE *file = fopen("/projects/student/result.txt", "w");
        for (int &u : all_loops) {
            loops += u;
        }
        fprintf(file, "%d\n", loops);
        // cout<< "loops : " << loops << endl;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < nodeCnt; j++) {
                fwrite(ans[j][i].c_str(), 1, ans[j][i].length(), file);
            }
        }
        fflush(file);
        fclose(file);
    }
};
int main() {
    extern vector<array<string, 5>> ans;
    extern vector<int> all_loops;
    // string testFile = "test_data.txt";
    string testFile = "/data/test_data.txt";

    Check check;
    // auto t = clock();
    check.readInput(testFile);
    // cout << clock() - t << endl;
    vector<thread> threadname;
    ans = vector<array<string, 5>>(check.nodeCnt);
    all_loops = vector<int>(threadnum, 0);

    for (int i = 0; i < threadnum; i++) {
        thread mythread(check, i);
        threadname.push_back(move(mythread));
    }
    for (int i = 0; i < threadnum; i++) {
        threadname[i].join();
    }

    // check.solve();
    // cout << clock() - t << endl;

    check.save();
    // cout << clock() - t << endl;

    // system("pause");
    return 0;
}
