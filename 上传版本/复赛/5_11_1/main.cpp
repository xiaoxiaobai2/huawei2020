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
vector<string> ids;
vector<int> use;
vector<edge> data;
bool cmp(array<int, 4> a, array<int, 4> b) { return a[1] < b[1]; }
class Check {
  public:
    vector<bool> vis, able3, able4;
    unordered_map<int, vector<int>> p3;
    unordered_map<int, vector<array<int, 2>>> p4;

    // vector<array<string, 5>> ans;
    int nodeCnt = 0;
    int num;
    int loops = 0;
    int curHead;
    int path[7];
    void operator()(int threadId) {
        extern vector<edge> data;

        vis = vector<bool>(nodeCnt, false);
        able3 = vector<bool>(num, false);
        able4 = vector<bool>(num, false);

        for (int &i : use) {
            curHead = data[i].u_node;
            if ((curHead - threadId) % threadnum != 0) {
                continue;
            }
            // curHead = data[i][3]; //给 curHead 节点找环
            // cout << i << "/" << num << endl;
            prepare(i);
            if (p3.size() == 0) {
                continue;
            }
            d1(i);
            // cout << p4.size() << endl;
            able3 = vector<bool>(num, false);
            able4 = vector<bool>(num, false);
            p3.clear();
            p4.clear();

            // else {
            //     cout << i << "/" << num << endl;
            // }
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
        ids = vector<string>(num);
        for (int i = 0; i < num; i++) {
            ids[i] = to_string(inputs[i][0]);
        }
        auto inputs1 = inputs;
        // data1[i][3]是边在data中的索引
        for (int i = 0; i < num; i++) {
            inputs1[i][3] = i;
        }
        stable_sort(inputs1.begin(), inputs1.end(), cmp);
        Gout = vector<vector<int>>(num);
        Gin = vector<vector<int>>(num);
        int k = 0;
        // data: u-v-c-nodeCnt
        // data1:u-v-c-i
        for (int i = 0; i < num; i++) {
            int v = inputs1[i][1], n = inputs1[i][3];
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
                    if (inputs1[i + 1][1] > v) {
                        // cout << v << "_" << nodeCnt << endl;
                        id_node[v] = nodeCnt++;
                        // cout << v << endl;
                        // nodeCnt++;
                        k = j;
                    }
                    break;
                }
            }
        }
        id_node[inputs1[num - 1][1]] = nodeCnt;
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
        // for (int i = 0; i < num; i++) {
        //     if (data[i].u == 6055) {
        //         cout << data[i].u_node << endl;
        //         cout << data[i].v_node << endl;
        //         cout << data[i].v << endl;

        //         cout << data[2].u_node << endl;
        //         break;
        //     }

        //     /* code */
        // }
        // system("pause");
    }
    void backhome(int cur, int depth, int able) {
        extern vector<edge> data;

        path[depth - 1] = cur;
        string tmp = "";
        for (int i = 0; i < depth; i++) {
            tmp.append(ids[path[i]] + ",");
        }
        switch (able) {
        case 3:
            for (int &u : p3[cur]) {
                // if (!vis[data[u].u_node]) {
                ans[curHead][depth - 2].append(tmp);
                ans[curHead][depth - 2].append(ids[u] + "\n");
                loops++;
                // }
            }
            break;
        case 4:
            for (array<int, 2> &u : p4[cur]) {
                if (!vis[data[u[1]].u_node]) {

                    // if (!vis[data[u[0]].u_node] && !vis[data[u[1]].u_node]) {
                    ans[curHead][depth - 1].append(tmp);
                    ans[curHead][depth - 1].append(ids[u[0]] + ",");
                    ans[curHead][depth - 1].append(ids[u[1]] + "\n");
                    loops++;
                }
            }
            break;
        }
    }
    void d1(int cur) {
        extern vector<edge> data;
        int head = data[cur].u_node;
        vis[head] = true;
        // vis[data[cur].v_node] = true;
        path[0] = cur;
        for (int &v : data[cur].out) {
            if (data[v].v_node <= head) {
                continue;
            }
            if (able3[v]) {
                backhome(v, 2, 3);
            }
            if (p4.size() == 0) {
                continue;
            }
            if (able4[v]) {
                backhome(v, 2, 4);
            }
            dfs(head, v, 2);
            /* code */
        }
        // vis[data[cur].v_node] = false;

        // vis[data[head][3]] = false;
    }
    void dfs(int &head, int cur, int depth) {
        extern vector<edge> data;

        if (depth == 5) {
            return;
        }
        vis[data[cur].u_node] = true;
        path[depth - 1] = cur;
        for (int &v : data[cur].out) {
            if (data[v].v_node < head || vis[data[v].v_node]) {
                continue;
            }
            if (able4[v] && depth < 5) {
                backhome(v, depth + 1, 4);
            }
            dfs(head, v, depth + 1);
        }
        vis[data[cur].u_node] = false;
    }
    void prepare(int i) { // data: u-v-c-nodeCnt u
        extern vector<edge> data;

        int head = data[i].u_node;
        int next;
        array<int, 2> temp;
        for (int &v : data[i].in) {
            if (data[v].u_node > head) {
                temp[1] = v;
                next = data[v].u_node;
                for (int &u : data[v].in) {
                    if (data[u].u_node > head) {
                        // path[1] = data[u][3];
                        p3[u].push_back(v);
                        able3[u] = true;
                        temp[0] = u;
                        for (int &w : data[u].in) {
                            if (data[w].u_node > head &&
                                data[w].u_node != next) {
                                //     if (w > i && w != v) {
                                //         // tep.push_back(w);
                                p4[w].push_back(temp);
                                able4[w] = true;
                                // sort(p4[w].begin(), p4[w].end());
                                //     }
                            }
                        }
                    }
                }
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
