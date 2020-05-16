#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
typedef unsigned int ui;
struct VC {
    int v;
    int c;
    VC(const int v, const int c) : v(v), c(c) {}
    bool operator<(const VC &vc) const { return v < vc.v; }
};
//  判断 i-u-v  的金额是否符合要求
bool COK(int cu, int cv) {
    long long c = cu;
    long long c1 = cv;

    return 0.2 * c <= c1 && 3 * c >= c1;
}
/*
	改成员变量为全局变量，减少多线程拷贝 9.8s 到8.8s
*/
int threadnum = 8;
vector<int> all_loops;
vector<array<string, 5>> Ans;
vector<vector<VC>> Gout, Gin; // 出入度，存v 和 c
vector<string> ids;
int nodeCnt = 0;

class Check {
  public:
    // vector<int> inDegrees, outDegrees;
    vector<bool> vis, able3, able4;
    unordered_map<int, vector<array<int, 3>>> p3;
    unordered_map<int, vector<array<int, 4>>> p4;
    array<int, 3> path3;
    array<int, 4> path4;
    int path[5]; // 环路径 前5个
    int loops;
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
                d1();
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
        vector<ui> inputs;
        vector<ui> counts;

        FILE *file = fopen(testFile.c_str(), "r");
        ui u, v, c;
        while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF) {
            inputs.push_back(u);
            inputs.push_back(v);
            counts.push_back(c);
        }

        unordered_map<ui, int> idHash;
        auto tmp = inputs;

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

        Gout = vector<vector<VC>>(nodeCnt);
        Gin = vector<vector<VC>>(nodeCnt);
        // inDegrees = vector<int>(nodeCnt, 0);
        // outDegrees = vector<int>(nodeCnt, 0);
        for (int i = 0; i < sz; i += 2) {
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]],
                c = counts[i / 2];
            Gout[u].push_back(VC(v, c));
            Gin[v].push_back(VC(u, c));
            // ++inDegrees[v];
            // ++outDegrees[u];
        }

        for (int i = 0; i < nodeCnt; i++) {
            sort(Gout[i].begin(), Gout[i].end());
            sort(Gin[i].begin(), Gin[i].end());
        }
        loops = 0;
    }

    /**
     * 生成环
     * */
    void backhome(int head, int cur, int depth, int able, int c) {

        path[depth] = cur;
        string tmp = "";
        int sz = depth - 1;
        for (int i = 0; i < depth + 1; i++) {
            tmp.append(ids[path[i]] + ",");
        }
        switch (able) {
        case 3:
            // 找3环
            for (array<int, 3> &u : p3[cur]) {
                // 判断是否重复 及 符合金额要求
                if (!vis[u[0]] && COK(c, u[2]) && COK(u[1], headC)) {
                    Ans[head][sz].append(tmp);
                    Ans[head][sz].append(ids[u[0]] + "\n");
                    loops++;
                }
            }
            break;
        case 4:
            // 找4-7环
            for (array<int, 4> &u : p4[cur]) {
                // 判断是否重复 及 符合金额要求
                if (!vis[u[0]] && !vis[u[1]] && COK(c, u[3]) &&
                    COK(u[2], headC)) {
                    Ans[head][depth].append(tmp);
                    Ans[head][depth].append(ids[u[0]] + ",");
                    Ans[head][depth].append(ids[u[1]] + "\n");
                    loops++;
                }
            }
            break;
        }
    }
    /**
     * 深度遍历找3 环 和4环
     * */
    void d1() {
        vis[head] = true;
        path[0] = head;
        for (VC &v : Gout[head]) {
            if (v.v < head) {
                continue;
            }
            headC = v.c;
            if (able3[v.v]) {
                backhome(head, v.v, 1, 3, headC);
            }
            if (p4.size() == 0) {
                continue;
            }
            if (able4[v.v]) {
                backhome(head, v.v, 1, 4, headC);
            }
            dfs(v.v, 2, headC);
        }
        // vis[head] = false;
    }

    /**
     * 深度遍历找5-7环
     * */
    void dfs(int cur, int depth, int c) {
        if (depth == 5) {
            return;
        }
        vis[cur] = true;
        path[depth - 1] = cur;
        for (VC &v : Gout[cur]) {
            // 剪枝
            if (v.v <= head || vis[v.v] || !COK(c, v.c)) {
                continue;
            }
            if (able4[v.v] && depth < 5) {
                backhome(head, v.v, depth, 4, v.c);
            }
            dfs(v.v, depth + 1, v.c);
        }
        vis[cur] = false;
    }

    /**
     * 准备 p3 p4 able3 able4
     * */
    void prepare() {
        //  w - u -v -head
        // path3   v-      v.c-u.c
        // path4   u-v     v.c-w.c
        // unordered_map<int, vector<array<int,4>>> p4,
        for (VC &v : Gin[head]) {
            // temp[0] = v;
            path4[1] = v.v;
            path3[0] = v.v;
            if (v.v > head) {
                // c = v.c;
                path4[2] = v.c;
                path3[1] = v.c;
                for (VC &u : Gin[v.v]) {
                    if (u.v > head && COK(u.c, v.c)) {
                        // c = u.c;
                        // temp.push_back(u);
                        path3[2] = u.c;
                        p3[u.v].push_back(path3);
                        able3[u.v] = true;
                        path4[0] = u.v;
                        for (VC &w : Gin[u.v]) {
                            if (w.v > head && w.v != v.v && COK(w.c, u.c)) {
                                // tep.push_back(w);
                                path4[3] = w.c;
                                p4[w.v].push_back(path4);
                                able4[w.v] = true;
                                sort(p4[w.v].begin(), p4[w.v].end());
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