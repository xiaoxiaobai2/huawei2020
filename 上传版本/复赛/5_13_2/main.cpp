#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
typedef unsigned int ui;

struct edge {
    int n;
    int c;
    // int index;
    // bool head = false;
    int u = -1, v = -1;
    // vector<int> in, out;
};
struct VC {
    int v;
    int c;
    int edgeNum;
    VC(const int v, const int c, const int num) : v(v), c(c), edgeNum(num) {}
    bool operator<(const VC &vc) const { return v < vc.v; }
};
//  判断 i-u-v  的金额是否符合要求
bool COK(int cu, int cv) {
    long long c = cu;
    long long c1 = cv;

    return 0.2 * c <= c1 && 3 * c >= c1;
}
bool cmp(array<int, 4> a, array<int, 4> b) { return a[1] < b[1]; }

int threadnum = 8;
vector<int> all_loops;
vector<array<string, 5>> Ans;
vector<vector<VC>> Gout, Gin; // 出入度，存v 和 c
vector<vector<edge>> Eout, Ein;

vector<string> ids;
// vector<int> use;
vector<edge> data;

int nodeCnt = 0;
int num;

class Check {
  public:
    // vector<int> inDegrees, outDegrees;
    vector<bool> vis, able3, able4;
    unordered_map<int, vector<array<int, 3>>> p3;
    unordered_map<int, vector<array<int, 4>>> p4;
    array<int, 3> path3;
    array<int, 4> path4;
    int path[5]; // 环路径 前5个
    int loops = 0;
    int head, headC; // 当前头结点和第一步的金额

    /**
     * 启用多线程   按线程id 分任务找环
     * */
    void operator()(int threadId) {
        vis = vector<bool>(nodeCnt, false);
        able3 = vector<bool>(nodeCnt, false);
        able4 = vector<bool>(nodeCnt, false);
        // able = vector<bool>(nodeCnt, false);
        for (int i = threadId; i < nodeCnt; i += threadnum) {
            // cout << threadId << endl;
            if (!Gout[i].empty()) {
                head = i;
                prepare();
                if (p3.size() == 0) {
                    continue;
                }
                dfs();
                p3.clear();
                p4.clear();
                able3 = vector<bool>(nodeCnt, false);
                able4 = vector<bool>(nodeCnt, false);
            }
        }
        all_loops[threadId] = loops;
    }

    /**
     * 读取数据，生成邻接表，初始化数据
     * */
    void readInput(string &testFile) {
        vector<vector<int>> Tout, Tin;
        // cout << "adf" << endl;
        extern vector<edge> data;
        vector<array<int, 4>> inputs;
        unordered_map<int, int> id_node;

        FILE *file = fopen(testFile.c_str(), "r");
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
        Tout = vector<vector<int>>(num);
        Tin = vector<vector<int>>(num);

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
                        Tout[n].push_back(j);
                        Tin[j].push_back(n);
                    }
                } else if (inputs[j][0] > v) {
                    kk = j;
                    break;
                }
            }
        }
        // cout << nodeCnt << endl;
        // system("pause");
        // id_node[inputs1[num - 1][1]] = nodeCnt;
        data.resize(num);
        Gin = vector<vector<VC>>(nodeCnt);
        Gout = vector<vector<VC>>(nodeCnt);
        // cout << "ADf" << endl;
        for (int i = 0; i < num; i++) {
            if (Tin[i].size() != 0 && Tout[i].size() != 0) {
                // data[i].u = inputs[i][0];
                // data[i].v = inputs[i][1];
                int c = inputs[i][2];
                int u = id_node[inputs[i][0]];
                int v = id_node[inputs[i][1]];
                data[i].c = c;
                data[i].u = u;
                data[i].v = v;
                data[i].n = i;
                Gin[v].push_back(VC(u, c, i));
                Gout[u].push_back(VC(v, c, i));
                // data[i].in = Gin[i];
                // data[i].out = Gout[i];
            }
        }
        Ein = vector<vector<edge>>(num);
        Eout = vector<vector<edge>>(num);
        for (int i = 0; i < num; i++) {
            for (int &j : Tin[i]) {
                // if (data[j].u != -1) {
                Ein[i].push_back(data[j]);
                // }
            }
        }
        for (int i = 0; i < num; i++) {
            for (int &j : Tout[i]) {
                // if (data[j].u != -1) {
                Eout[i].push_back(data[j]);
                // }
            }
        }
    }

    /**
     * 生成3环
     * */
    void backhome3(int cur, int c) {
        string tmp = "";
        for (int i = 0; i < 2; i++) {
            tmp.append(ids[path[i]] + ",");
        }

        // 找3环
        for (array<int, 3> &u : p3[cur]) {
            // 判断是否重复 及 符合金额要求
            if (!vis[u[0]] && COK(c, u[2]) && COK(u[1], headC)) {
                Ans[head][0].append(tmp);
                Ans[head][0].append(ids[u[0]] + "\n");
                loops++;
            }
        }
    }

    // /**
    //  * 生成4 - 7环
    //  * */
    void backhome(int cur, int depth, int c) {
        string tmp = "";
        for (int i = 0; i < depth + 1; i++) {
            tmp.append(ids[path[i]] + ",");
        }
        // 找4-7环
        for (array<int, 4> &u : p4[cur]) {
            // 判断是否重复 及 符合金额要求
            // 会不会出现所有的都 vis过
            if (!vis[u[0]] && !vis[u[1]] && COK(c, u[3]) && COK(u[2], headC)) {
                Ans[head][depth].append(tmp);
                Ans[head][depth].append(ids[u[0]] + ",");
                Ans[head][depth].append(ids[u[1]] + "\n");
                loops++;
            }
        }
    }
    // /**
    //  * 深度遍历找环
    //  * */
    void dfs() {
        extern vector<edge> data;

        vis[head] = true;
        path[0] = head;
        for (VC &v : Gout[head]) {
            if (v.v < head)
                continue;
            headC = v.c;
            path[1] = v.v;
            // 找3环
            if (able3[v.v])
                backhome3(v.v, headC);
            // 不会有符合要求的产生
            if (p4.size() == 0)
                continue;
            // 找4环
            if (able4[v.v])
                backhome(v.v, 1, headC);
            // dfs(v.v, 2, headC);

            vis[v.v] = true;
            for (edge &u : Eout[v.edgeNum]) {

                // 剪枝
                if (u.v <= head || vis[u.v])
                    continue;
                path[2] = u.v;

                // 找5环
                if (able4[u.v])
                    backhome(u.v, 2, u.c);
                vis[u.v] = true;
                for (edge &w : Eout[u.n]) {
                    // 剪枝
                    if (w.v <= head || vis[w.v])
                        continue;
                    path[3] = w.v;
                    // 找6环
                    if (able4[w.v])
                        backhome(w.v, 3, w.c);
                    vis[w.v] = true;
                    for (edge &z : Eout[w.n]) {
                        // 剪枝
                        if (z.v <= head || vis[z.v])
                            continue;
                        path[4] = z.v;
                        // 找7环
                        if (able4[z.v])
                            backhome(z.v, 4, z.c);
                    }
                    vis[w.v] = false;
                }
                vis[u.v] = false;
            }
            vis[v.v] = false;
        }
        // vis[head] = false;
    }

    // /**
    //  * 准备 p3 p4 able3 able4
    //  * */
    void prepare() {
        extern vector<edge> data;

        //  w - u -v -head
        // path3   v-      v.c-u.c
        // path4   u-v     v.c-w.c
        // unordered_map<int, vector<array<int,4>>> p4,
        for (VC &v : Gin[head]) {
            // temp[0] = v;
            path4[1] = v.v;
            path3[0] = v.v;
            vis[v.v] = true;
            if (v.v > head) {
                // c = v.c;
                path4[2] = v.c;
                path3[1] = v.c;
                // cout << v.edgeNum << endl;
                // system("pause");
                for (edge &u : Ein[v.edgeNum]) {
                    if (u.u > head) {
                        // cout << head << endl;
                        // c = u.c;
                        // temp.push_back(u);
                        path3[2] = u.c;
                        p3[u.u].push_back(path3);
                        able3[u.u] = true;
                        path4[0] = u.u;
                        for (edge &w : Ein[u.n]) {
                            if (w.u > head && !vis[w.u]) {
                                // tep.push_back(w);
                                path4[3] = w.c;
                                p4[w.u].push_back(path4);
                                able4[w.u] = true;
                                sort(p4[w.u].begin(), p4[w.u].end());
                            }
                        }
                    }
                }

                // for (VC &u : Gin[v.v]) {
                //     if (u.v> head && COK(u.c, v.c)) {
                //         // c = u.c;
                //         // temp.push_back(u);
                //         path3[2] = u.c;
                //         p3[u.v].push_back(path3);
                //         able3[u.v] = true;
                //         path4[0] = u.v;
                //         for (VC &w : Gin[u.v]) {
                //             if (w.v > head && !vis[w.v] && COK(w.c, u.c)) {
                //                 // tep.push_back(w);
                //                 path4[3] = w.c;
                //                 p4[w.v].push_back(path4);
                //                 able4[w.v] = true;
                //                 sort(p4[w.v].begin(), p4[w.v].end());
                //             }
                //         }
                //     }
                // }
            }
            vis[v.v] = false;
        }
    }

    // /*
    //     fwrite 逐行写
    // */
    void output() {
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
    Check check;
    vector<thread> threadname;
    check.readInput(testFile);
    // cout << clock() - t << endl;
    // t = clock() - t;
    Ans = vector<array<string, 5>>(nodeCnt);
    all_loops = vector<int>(threadnum, 0);
    for (int i = 0; i < threadnum; i++) {
        thread mythread(check, i);
        threadname.push_back(move(mythread));
    }
    for (int i = 0; i < threadnum; i++) {
        threadname[i].join();
    }
    // cout << clock() - t << endl;
    // t = clock() - t;
    check.output();
    // cout << clock() - t << endl;
    // t = clock() - t;
    // system("pause");
    return 0;
}