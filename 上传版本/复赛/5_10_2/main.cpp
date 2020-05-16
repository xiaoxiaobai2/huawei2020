#include <bits/stdc++.h>
// #include <unistd.h>
using namespace std;
typedef unsigned int ui;

vector<array<string, 5>> ans;

int threadnum = 8;
vector<int> all_loops;

bool cmp(array<int, 4> a, array<int, 4> b) { return a[1] < b[1]; }
class Check {
  public:
    vector<array<int, 4>> data;
    vector<vector<int>> Gout, Gin;
    // vector<bool> vis;
    vector<string> ids;
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
        vis = vector<bool>(nodeCnt, false);
        able3 = vector<bool>(num, false);
        able4 = vector<bool>(num, false);
        for (int i = 0; i < num; i++) {
            curHead = data[i][3];
            if (curHead == -1 || (curHead - threadId) % threadnum != 0) {
                continue;
            }
            if (data[i][1] > data[i][0]) {
                curHead = data[i][3]; //给 curHead 节点找环
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
            }
            // else {
            //     cout << i << "/" << num << endl;
            // }
        }
        all_loops[threadId] = loops;
    }
    void readInput(string &File) {
        FILE *file = fopen(File.c_str(), "r");
        ui u, v, c;
        array<int, 4> temp;
        temp[3] = -1;
        while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF) {
            temp[0] = u;
            temp[1] = v;
            temp[2] = c;
            data.push_back(temp);
        }
        sort(data.begin(), data.end(), cmp);
        sort(data.begin(), data.end());
        num = data.size();
        ids = vector<string>(num);
        for (int i = 0; i < num; i++) {
            ids[i] = to_string(data[i][0]);
        }
        auto data1 = data;
        // data1[i][3]是边在data中的索引
        for (int i = 0; i < num; i++) {
            data1[i][3] = i;
        }
        sort(data1.begin(), data1.end(), cmp);
        Gout = vector<vector<int>>(num);
        Gin = vector<vector<int>>(num);
        int k = 0;
        // data: u-v-c-nodeCnt
        // data1:u-v-c-i
        for (int i = 0; i < num; i++) {
            int v = data1[i][1], n = data1[i][3];
            long long c = data1[i][2];
            for (int j = k; j < num; j++) {
                if (data[j][0] == v) {
                    long long c1 = data[j][2];
                    if (0.2 * c <= c1 && 3 * c >= c1) {
                        data[j][3] = nodeCnt;
                        Gout[n].push_back(j);
                        Gin[j].push_back(n);
                    }
                } else if (data[j][0] > v) {
                    if (data1[i + 1][1] > v) {
                        nodeCnt++;
                        k = j;
                    }
                    break;
                }
            }
        }
        ans = vector<array<string, 5>>(nodeCnt);
        // cout << nodeCnt << endl;
        // cout << num << endl;
    }
    void backhome3(int cur, int depth) {
        // cout << "ahahahah" << endl;
        path[depth] = cur;
        string tmp = "";
        int size = depth + 1;
        for (int i = 0; i < size; i++) {
            tmp.append(ids[path[i]] + ",");
        }
        size = depth - 1;
        for (int &u : p3[cur]) {
            if (!vis[data[u][3]]) {
                ans[curHead][size].append(tmp);
                ans[curHead][size].append(ids[u] + "\n");
                loops++;
            }
        }
    }

    void backhome4(int cur, int depth) {
        path[depth] = cur;
        string tmp = "";
        int size = depth + 1;
        for (int i = 0; i < size; i++) {
            tmp.append(ids[path[i]] + ",");
        }
        for (array<int, 2> &u : p4[cur]) {
            if (!vis[data[u[0]][3]] && !vis[data[u[1]][3]]) {
                ans[curHead][depth].append(tmp);
                ans[curHead][depth].append(ids[u[0]] + ",");
                ans[curHead][depth].append(ids[u[1]] + "\n");
                loops++;
            }
        }
    }

    void d1(int head) {
        vis[data[head][3]] = true;
        path[0] = head;
        for (int &v : Gout[head]) {
            if (data[v][1] < data[head][0] || data[v][3] == -1) {
                continue;
            }
            if (able3[v]) {
                backhome3(v, 1);
            }
            if (p4.size() == 0) {
                continue;
            }
            if (able4[v]) {
                backhome4(v, 1);
            }
            dfs(head, v);
            /* code */
        }
        vis[data[head][3]] = false;
    }

    void dfs(int head, int cur) {
        vis[data[cur][3]] = true;
        path[1] = cur;
        for (int &v : Gout[cur]) {
            if (data[v][1] < data[head][0] || data[v][3] == -1 ||
                vis[data[v][3]]) {
                continue;
            }
            if (able4[v]) {
                backhome4(v, 2);
            }
            vis[data[v][3]] = true;
            path[2] = v;
            for (int &v2 : Gout[v]) {
                if (data[v2][1] < data[head][0] || data[v2][3] == -1 ||
                    vis[data[v2][3]]) {
                    continue;
                }
                if (able4[v2]) {
                    backhome4(v2, 3);
                }
                vis[data[v2][3]] = true;
                path[3] = v2;
                for (int &v3 : Gout[v2]) {
                    if (data[v3][1] < data[head][0] || data[v3][3] == -1 ||
                        vis[data[v3][3]]) {
                        continue;
                    }
                    if (able4[v3]) {
                        backhome4(v3, 4);
                    }
                }
                vis[data[v2][3]] = false;
            }
            vis[data[v][3]] = false;
        }
        vis[data[cur][3]] = false;
    }

    void prepare(int i) { // data: u-v-c-nodeCnt u
        int head = data[i][3];
        int next;
        array<int, 2> temp;
        for (int &v : Gin[i]) {
            if (data[v][3] != -1 && data[v][3] > head) {
                temp[1] = v;
                next = data[v][3];
                for (int &u : Gin[v]) {
                    if (data[u][3] != -1 && data[u][3] > head) {
                        // path[1] = data[u][3];
                        p3[u].push_back(v);
                        able3[u] = true;
                        temp[0] = u;
                        for (int &w : Gin[u]) {
                            if (data[w][3] != -1 && data[w][3] > head &&
                                data[w][3] != next) {
                                //     if (w > i && w != v) {
                                //         // tep.push_back(w);
                                p4[w].push_back(temp);
                                able4[w] = true;
                                sort(p4[w].begin(), p4[w].end());
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
