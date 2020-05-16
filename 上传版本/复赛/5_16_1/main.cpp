#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;
typedef unsigned int ui;
struct VC {
    int v;
    double c;
    VC(const int v, const double c) : v(v), c(c) {}
    bool operator<(const VC &vc) const { return v < vc.v; }
};
struct PP {
    array<int, 3> path;
    double cIn;
    double cOut;
    bool operator<(const PP &pp) const {
        if (path[0] != pp.path[0])
            return path[0] < pp.path[0];
        else if (path[1] != pp.path[1])
            return path[1] < pp.path[1];
        else {
            return path[2] < pp.path[2];
        }
    }
};
struct Loc {
    char *l;
    int len;
    // Loc(const char* l, const int len) : l(l), len(len) {}
};
//  判断 i-u-v  的金额是否符合要求
inline bool COK(double cu, double cv) { return 0.2 * cu <= cv && 3 * cu >= cv; }

int threadnum = 8; // 线程数量
int denseNum = 64; // 需要计算的稠密点的数量
vector<int> all_loops;
// vector<array<string, 5>> Ans;
vector<array<Loc, 6>> Ans;

vector<vector<VC>> Gout, Gin; // 出入度，存v 和 c
vector<string> ids;
int nodeCnt = 0;
// vector<int> denses;
// vector<bool> visI;         // 与稠密点相关，已经找过环
// vector<vector<bool>> work; // 给稠密点的任务,找完置为false
class Check {
  public:
    // vector<int> inDegrees, outDegrees;
    vector<bool> vis, able3, able4, able5;
    unordered_map<int, vector<PP>> p3;
    unordered_map<int, vector<PP>> p4;
    unordered_map<int, vector<PP>> p5;
    PP path3, path4, path5;
    vector<int> in;
    vector<int> out;
    int path[5]; // 环路径 前5个
    int loops;
    int head;
    double headC; // 当前头结点和第一步的金额
    // 0  1   2   3``4    5
    // vector<array<int, 6>> d1; // c1 c1  min v1     v1
    // vector<array<int, 6>> d2; // c1 c2  min v1 v2  v2
    // vector<array<int, 6>> d3; // c1 c3  min v1 v2  v3
    // int denseNode;            // 当前正在处理的稠密点
    char *result3, *result4, *result5, *result6, *result7, *result8;
    // char *data3, *data4, *data5, *data6, *data7;
    char *result[6];
    /**
     * 启用多线程   按线程id 分任务找环
     * */
    void operator()(int threadId) {
        result3 = (char *)malloc(100 * 10000 * 10 * 3);
        result4 = (char *)malloc(100 * 10000 * 10 * 4);
        result5 = (char *)malloc(200 * 10000 * 10 * 5);
        result6 = (char *)malloc(500 * 10000 * 10 * 6);
        result7 = (char *)malloc(1000 * 10000 * 10 * 7);

        result8 = (char *)malloc(1000 * 10000 * 10 * 8);

        result[0] = result3;
        result[1] = result4;
        result[2] = result5;
        result[3] = result6;
        result[4] = result7;
        result[5] = result8;

        vis = vector<bool>(nodeCnt, false);
        able3 = vector<bool>(nodeCnt, false);
        able4 = vector<bool>(nodeCnt, false);
        able5 = vector<bool>(nodeCnt, false);
        // able = vector<bool>(nodeCnt, false);

        // for (int i = threadId; i < denseNum; i += threadnum)
        // {
        //     // 当前稠密点
        //     denseNode = denses[i];
        //     denseDFS(denseNode);
        //     for (VC &v : Gin[denseNode])
        //     {

        //         // 判断当前节点是不是你的待完成任务
        //         if (work[threadId][v.v])
        //         {
        //             head = v.v;
        //             // 存储当前节点的每一环的首地址
        //             for (int i = 0; i < 6; i++)
        //             {
        //                 Ans[head][i].l = result[i];
        //             }
        //             work[threadId][v.v] = false;

        //             canBack(head);
        //             prepare();
        //             if (p3.size() == 0)
        //             {
        //                 continue;
        //             }
        //             dfs2();
        //             // 计算当前节点的每一环的长度
        //             for (int i = 0; i < 6; i++)
        //             {
        //                 Ans[head][i].len = result[i] - Ans[head][i].l;
        //             }
        //             p3.clear();
        //             p4.clear();
        //             p5.clear();
        //             able3 = vector<bool>(nodeCnt, false);
        //             able4 = vector<bool>(nodeCnt, false);
        //             able5 = vector<bool>(nodeCnt, false);
        //         }
        //     }
        //     d1.clear();
        //     d2.clear();
        //     d3.clear();
        // }

        for (int i = threadId; i < nodeCnt; i += threadnum) {
            // cout << threadId << endl;
            // 判断该节点是否由稠密点相关完成
            // if (!visI[i])
            // {
            head = i;
            // 存储当前节点的每一环的首地址
            for (int j = 0; j < 6; j++) {
                Ans[head][j].l = result[j];
            }
            // visI[head] = true;
            canBack(i);
            prepare();
            if (p3.size() == 0) {
                continue;
            }
            dfs();
            // 计算当前节点的每一环的长度
            for (int j = 0; j < 6; j++) {
                Ans[head][j].len = result[j] - Ans[head][j].l;
            }
            p3.clear();
            p4.clear();
            p5.clear();
            able3 = vector<bool>(nodeCnt, false);
            able4 = vector<bool>(nodeCnt, false);
            able5 = vector<bool>(nodeCnt, false);
            // }
        }
        all_loops[threadId] = loops;
    }

    /**
     * 读取数据，生成邻接表，初始化数据
     * */
    void readInput(string &testFile) {
        vector<ui> inputs;
        vector<double> counts;
        FILE *file = fopen(testFile.c_str(), "r");
        ui u, v;
        double c;
        while (fscanf(file, "%u,%u,%lf", &u, &v, &c) != EOF) {
            inputs.push_back(u);
            inputs.push_back(v);
            counts.push_back(c);
        }

        unordered_map<ui, int> idHash;
        auto tmp = inputs;

        sort(tmp.begin(), tmp.end());
        tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
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
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]];
            double c = counts[i / 2];
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

        // denses = vector<int>(denseNum);
        // // 找到稠密点
        // getDenses();
        // // 分配任务
        // distruWork();
    }

    /**
     * 多线程分配任务
     * */
    // void distruWork() {
    //     visI = vector<bool>(nodeCnt, false);
    //     work = vector<vector<bool>>(threadnum, vector<bool>(nodeCnt, false));
    //     for (int i = 0; i < denseNum; i++) {
    //         // cout << "index:" << ids[denses[i]] << endl;
    //         int j = i % threadnum;
    //         for (VC &v : Gin[denses[i]]) {
    //             // 当前节点没有被分配出去，则分配给你
    //             if (!visI[v.v]) {
    //                 visI[v.v] = true;
    //                 work[j][v.v] = true;
    //             }
    //         }
    //     }
    // }
    // /**
    //  * 建堆
    //  * */
    // void heapify(vector<array<int, 2>> *v, int cur, int size) {

    //     if (cur < size) {
    //         //左子树和右字数的位置
    //         int left = 2 * cur + 1;
    //         int right = 2 * cur + 2;
    //         //把当前父节点位置看成是最大的
    //         int max = cur;
    //         if (left < size)
    //             //如果比当前根元素要大，记录它的位置
    //             if (v->at(max)[0] < v->at(left)[0])
    //                 max = left;
    //         if (right < size)
    //             //如果比当前根元素要大，记录它的位置
    //             if (v->at(max)[0] < v->at(right)[0])
    //                 max = right;
    //         //如果最大的不是根元素位置，那么就交换
    //         if (max != cur) {
    //             array<int, 2> temp = v->at(max);
    //             v->at(max) = v->at(cur);
    //             v->at(cur) = temp;
    //             //继续比较，直到完成一次建堆
    //             heapify(v, max, size);
    //         }
    //     }
    // }

    // /**
    //  * 获取菊花点
    //  * */
    // void getDenses() {
    //     vector<array<int, 2>> v = vector<array<int, 2>>(nodeCnt);
    //     array<int, 2> tep;
    //     for (int i = 0; i < nodeCnt; i++) {
    //         tep[0] = min(Gout[i].size(), Gin[i].size());
    //         tep[1] = i;
    //         v[i] = tep;
    //     }

    //     for (int i = 1; i <= denseNum; i++) {
    //         for (int j = nodeCnt - i; j >= 0; j--) {
    //             heapify(&v, j, nodeCnt - i + 1);
    //         }
    //         denses[i - 1] = v[0][1];
    //         v[0] = v[nodeCnt - i];
    //     }
    // }

    /**
     * 生成3环
     * */
    void backhome3(int cur, double c) {
        // 找3环

        for (PP &u : p3[cur]) {
            // 判断是否重复 及 符合金额要求
            if (!vis[u.path[0]] && COK(c, u.cOut) && COK(u.cIn, headC)) {
                for (int i = 0; i < 2; i++) {
                    memcpy(result[0], ids[path[i]].c_str(),
                           ids[path[i]].size());
                    result[0] += ids[path[i]].size();
                    *result[0] = ',';
                    result[0]++;
                }
                memcpy(result[0], ids[u.path[0]].c_str(),
                       ids[u.path[0]].size());
                result[0] += ids[u.path[0]].size();
                *result[0] = '\n';
                result[0]++;

                loops++;
            }
        }
    }
    /**
     * 生成4环
     * */
    void backhome4(int cur, double c) {

        for (PP &u : p4[cur]) {
            // 判断是否重复 及 符合金额要求
            if (!vis[u.path[0]] && !vis[u.path[1]] && COK(c, u.cOut) &&
                COK(u.cIn, headC)) {
                for (int i = 0; i < 2; i++) {
                    memcpy(result[1], ids[path[i]].c_str(),
                           ids[path[i]].size());
                    result[1] += ids[path[i]].size();
                    *result[1] = ',';
                    result[1]++;
                }
                memcpy(result[1], ids[u.path[0]].c_str(),
                       ids[u.path[0]].size());
                result[1] += ids[u.path[0]].size();
                *result[1] = ',';
                result[1]++;

                memcpy(result[1], ids[u.path[1]].c_str(),
                       ids[u.path[1]].size());
                result[1] += ids[u.path[1]].size();
                *result[1] = '\n';
                result[1]++;

                loops++;
            }
        }
    }

    /**
     * 生成5 - 8环
     * */
    void backhome(int cur, int depth, double c) {
        for (PP &u : p5[cur]) {
            // 判断是否重复 及 符合金额要求
            // 会不会出现所有的都 vis过
            if (!vis[u.path[0]] && !vis[u.path[1]] && !vis[u.path[2]] &&
                COK(c, u.cOut) && COK(u.cIn, headC)) {
                for (int i = 0; i < depth; i++) {
                    // cout << path[i] << endl;

                    memcpy(result[depth], ids[path[i]].c_str(),
                           ids[path[i]].size());
                    // cout << path[i] << endl;

                    result[depth] += ids[path[i]].size();
                    *result[depth] = ',';
                    result[depth]++;
                }
                memcpy(result[depth], ids[u.path[0]].c_str(),
                       ids[u.path[0]].size());
                result[depth] += ids[u.path[0]].size();
                *result[depth] = ',';
                result[depth]++;

                memcpy(result[depth], ids[u.path[1]].c_str(),
                       ids[u.path[1]].size());
                result[depth] += ids[u.path[1]].size();
                *result[depth] = ',';
                result[depth]++;

                memcpy(result[depth], ids[u.path[2]].c_str(),
                       ids[u.path[2]].size());
                result[depth] += ids[u.path[2]].size();
                *result[depth] = '\n';
                result[depth]++;

                loops++;
            }
        }
    }

    int getLowerIndex(vector<VC> *vc) {
        int left = 0;
        int right = vc->size() - 1;
        int mid;
        while (left <= right) {
            mid = (left + right) / 2;
            if (vc->at(mid).v == head)
                return mid;
            if (vc->at(mid).v > head)
                right = mid - 1;
            else
                left = mid + 1;
        }
        return left;
    }
    void canBack(int i) {
        in.clear();
        out.clear();
        // cout << "here" << endl;
        int m = getLowerIndex(&Gin[i]);
        int size1 = Gin[i].size();
        int n = getLowerIndex(&Gout[i]);
        int size2 = Gout[i].size();
        vector<bool> v = vector<bool>(size2, false);
        for (int j = m; j < size1; j++)
            for (int k = n; k < size2; k++)
                if (COK(Gin[i][j].c, Gout[i][k].c)) {
                    in.push_back(j);
                    v[k] = true;
                    break;
                }
        for (int j = n; j < size2; j++) {
            if (v[j]) {
                out.push_back(j);
                continue;
            }
            for (int k = m; k < size1; k++)
                if (COK(Gin[i][k].c, Gout[i][j].c)) {
                    out.push_back(j);
                    break;
                }
        }
    }

    /*
                                  // 0  1   2   3``4    5
        vector<array<int, 6>> d1; // c1         v1
        vector<array<int, 6>> d2; // c1 c2  min v1 v2
        vector<array<int, 6>> d3; // c1 c3  min v1 v2  v3
    */
    // int k1, k2, k3;
    // void denseDFS(int node) {
    //     k2 = 0, k3 = 0, k1 = 0;

    //     array<int, 6> temp;
    //     for (VC &v1 : Gout[node]) {
    //         temp[0] = v1.c;
    //         temp[3] = v1.v;
    //         d1.push_back(temp);
    //         for (VC &v2 : Gout[v1.v]) {
    //             if (v2.v != node && COK(v1.c, v2.c)) {
    //                 temp[1] = v2.c;
    //                 temp[4] = v2.v;
    //                 temp[2] = min(temp[3], temp[4]);
    //                 int xiao = temp[2];
    //                 d2.push_back(temp);
    //                 for (VC &v3 : Gout[v2.v]) {
    //                     if (v3.v != node && v3.v != v1.v && COK(v2.c, v3.c))
    //                     {
    //                         temp[1] = v3.c;
    //                         temp[5] = v3.v;
    //                         temp[2] = min(xiao, temp[5]);
    //                         d3.push_back(temp);
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }

    // void denseBack() {
    //     for (int i = k1; i < d1.size(); i++)
    //         if (head > d1[i][3])
    //             k1++;
    //         else
    //             break;
    //     for (int i = k1; i < d1.size(); i++) {
    //         if (head < d1[i][3] && COK(headC, d1[i][0]) && able4[d1[i][3]]) {
    //             path[2] = d1[i][3];
    //             backhome(d1[i][3], 2, d1[i][0]);
    //         }
    //     }
    //     for (int i = k2; i < d2.size(); i++)
    //         if (head > d2[i][3])
    //             k2++;
    //         else
    //             break;
    //     for (int i = k2; i < d2.size(); i++) {
    //         if (head < d2[i][2] && COK(headC, d2[i][0]) && able4[d2[i][4]]) {
    //             path[2] = d2[i][3];
    //             path[3] = d2[i][4];
    //             vis[d2[i][3]] = true;
    //             backhome(d2[i][4], 3, d2[i][1]);
    //             vis[d2[i][3]] = false;
    //         }
    //     }

    //     /*
    //                               // 0  1   2   3``4    5
    //     vector<array<int, 6>> d1; // c1         v1
    //     vector<array<int, 6>> d2; // c1 c2  min v1 v2
    //     vector<array<int, 6>> d3; // c1 c3  min v1 v2  v3
    // */
    //     for (int i = k3; i < d3.size(); i++)
    //         if (head > d3[i][3])
    //             k3++;
    //         else
    //             break;
    //     for (int i = k3; i < d3.size(); i++) {
    //         if (head < d3[i][2] && COK(headC, d3[i][0]) && able4[d3[i][5]]) {
    //             path[2] = d3[i][3];
    //             path[3] = d3[i][4];
    //             path[4] = d3[i][5];
    //             vis[d3[i][3]] = true;
    //             vis[d3[i][4]] = true;
    //             backhome(d3[i][5], 4, d3[i][1]);
    //             vis[d3[i][3]] = false;
    //             vis[d3[i][4]] = false;
    //         }
    //     }
    // }
    /**
     * 深度遍历找环
     * */
    // void dfs2() {
    //     vis[head] = true;
    //     path[0] = head;
    //     for (int &i : out) {
    //         VC v = Gout[head][i];
    //         headC = v.c;
    //         path[1] = v.v;
    //         // 找3环

    //         if (able3[v.v])
    //             backhome3(v.v, headC);
    //         // 不会有符合要求的产生
    //         if (p4.size() == 0)
    //             continue;
    //         // 找4环
    //         if (able4[v.v])
    //             backhome(v.v, 1, headC);
    //         // dfs(v.v, 2, headC);

    //         vis[v.v] = true;
    //         if (v.v == denseNode) {
    //             denseBack();
    //             vis[v.v] = false;
    //             continue;
    //         }

    //         for (VC &u : Gout[v.v]) {
    //             // 剪枝
    //             if (u.v <= head || vis[u.v] || !COK(headC, u.c))
    //                 continue;
    //             path[2] = u.v;

    //             // 找5环
    //             if (able4[u.v])
    //                 backhome(u.v, 2, u.c);

    //             vis[u.v] = true;
    //             for (VC &w : Gout[u.v]) {
    //                 // 剪枝
    //                 if (w.v <= head || vis[w.v] || !COK(u.c, w.c))
    //                     continue;
    //                 path[3] = w.v;
    //                 // 找6环
    //                 if (able4[w.v])
    //                     backhome(w.v, 3, w.c);
    //                 vis[w.v] = true;
    //                 for (VC &z : Gout[w.v]) {
    //                     // 剪枝
    //                     if (z.v <= head || vis[z.v] || !COK(w.c, z.c))
    //                         continue;
    //                     path[4] = z.v;
    //                     // 找7环
    //                     if (able4[z.v])
    //                         backhome(z.v, 4, z.c);
    //                 }
    //                 vis[w.v] = false;
    //             }
    //             vis[u.v] = false;
    //         }
    //         vis[v.v] = false;
    //     }
    //     vis[head] = false;
    // }

    /**
     * 深度遍历找环
     * */
    void dfs() {
        vis[head] = true;
        path[0] = head;
        for (int &i : out) {
            VC v = Gout[head][i];
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
                backhome4(v.v, headC);
            // dfs(v.v, 2, headC);

            if (able5[v.v])
                backhome(v.v, 2, headC);
            if (p5.size() == 0)
                continue;
            /*
            if(isDense()){
                denseBack();
                continue
            }
            */

            vis[v.v] = true;
            for (VC &u : Gout[v.v]) {
                // 剪枝
                if (u.v <= head || !COK(headC, u.c))
                    continue;
                path[2] = u.v;

                // 找5环
                if (able5[u.v])
                    backhome(u.v, 3, u.c);

                vis[u.v] = true;
                for (VC &w : Gout[u.v]) {
                    // 剪枝
                    if (w.v <= head || vis[w.v] || !COK(u.c, w.c))
                        continue;
                    path[3] = w.v;
                    // 找6环
                    if (able5[w.v])
                        backhome(w.v, 4, w.c);
                    vis[w.v] = true;
                    for (VC &z : Gout[w.v]) {
                        // 剪枝
                        if (z.v <= head || vis[z.v] || !COK(w.c, z.c))
                            continue;
                        path[4] = z.v;
                        // 找7环
                        if (able5[z.v])
                            backhome(z.v, 5, z.c);
                    }
                    vis[w.v] = false;
                }
                vis[u.v] = false;
            }
            vis[v.v] = false;
        }
        // vis[head] = false;
    }

    /**
     * 准备 p3 p4 able3 able4
     * */
    void prepare() {
        //  w - u -v -head
        // path3   v-        v.c-u.c
        // path4   u-v       v.c-w.c
        // path4   w-u-v     v.c-z.c
        // unordered_map<int, vector<array<int,4>>> p4,
        // unordered_map<int, vector<array<int,5>>> p4,
        for (int &i : in) {
            VC v = Gin[head][i];
            // temp[0] = v;
            path5.path[2] = v.v;
            path4.path[1] = v.v;
            path3.path[0] = v.v;
            vis[v.v] = true;

            // c = v.c;
            path5.cIn = v.c;
            path4.cIn = v.c;
            path3.cIn = v.c;
            // if (v.c > maxC)
            //     maxC = v.c;
            // if (v.c < minC)
            //     minC = v.c;
            for (VC &u : Gin[v.v]) {
                if (u.v > head && COK(u.c, v.c)) {
                    // c = u.c;
                    // temp.push_back(u);
                    path3.cOut = u.c;
                    p3[u.v].push_back(path3);
                    able3[u.v] = true;
                    path4.path[0] = u.v;
                    path5.path[1] = u.v;

                    vis[u.v] = true;
                    for (VC &w : Gin[u.v]) {
                        if (w.v > head && !vis[w.v] && COK(w.c, u.c)) {
                            // tep.push_back(w);
                            path4.cOut = w.c;
                            p4[w.v].push_back(path4);
                            able4[w.v] = true;
                            sort(p4[w.v].begin(), p4[w.v].end());

                            path5.path[0] = w.v;

                            for (VC &z : Gin[w.v]) {
                                if (z.v > head && !vis[z.v] && COK(z.c, w.c)) {
                                    // tep.push_back(w);
                                    path5.cOut = z.c;
                                    p5[z.v].push_back(path5);
                                    able5[z.v] = true;
                                    stable_sort(p5[z.v].begin(), p5[z.v].end());
                                }
                            }
                        }
                    }
                    vis[u.v] = false;
                }
            }
            vis[v.v] = false;
        }
    }
};
//     void output() {
//         // FILE *file = fopen("output.txt", "w");
//         // FILE *file = fopen("/projects/student/result.txt", "w");
//         int f = open("output.txt", O_RDWR | O_CREAT, 0666);
//         // int f = open("/projects/student/result.txt", O_RDWR | O_CREAT,
//         0666); for (int &u : all_loops) {
//             loops += u;
//         }
//         // fprintf(file, "%d\n", loops);
//         string cirecels = to_string(loops) + "\n";
//         write(f, cirecels.c_str(), cirecels.length());
//         // char *buf;
//         // string result;
//         // result.reserve()
//         for (int i = 0; i < 6; i++) {
//             for (int k = 0; k < nodeCnt; k++) {
//                 // buf = new char[Ans[k][i].length() + 1];
//                 // strcpy(buf, Ans[k][i].c_str());
//                 write(f, Ans[k][i].l, Ans[k][i].len);
//                 // fwrite(Ans[k][i].l, 1, Ans[k][i].len, file);
//                 // result.append(Ans[k][i]);
//             }
//         }
//         close(f);
//         // write(f, result.c_str(), result.length());
//         // fwrite(result.c_str(), 1, result.length(), file);
//         // fflush(file);
//         // fclose(file);
//     }
// };

// int main() {
//     extern vector<array<Loc, 6>> Ans;
//     extern vector<int> all_loops;
//     string testFile = "test_data.txt";
//     // string testFile = "/data/test_data.txt";
//     // auto t = clock();
//     Check check;
//     vector<thread> threadname;
//     check.readInput(testFile);
//     // system("pause");
//     Ans = vector<array<Loc, 6>>(nodeCnt);
//     all_loops = vector<int>(threadnum, 0);
//     for (int i = 0; i < threadnum; i++) {
//         thread mythread(check, i);
//         threadname.push_back(move(mythread));
//     }
//     for (int i = 0; i < threadnum; i++) {
//         threadname[i].join();
//     }
//     // ui ti = clock() - t;
//     // 80000000
//     // usleep(ti * 10);
//     check.output();
//     // cout << clock() - t << endl;
//     // system("pause");
//     return 0;
// }

vector<array<int, 6>> location;
string sloops;
long int result_length;
void getlocation() {
    int loops = 0;
    location = vector<array<int, 6>>(nodeCnt);

    for (int &u : all_loops) {
        loops += u;
    }
    sloops = to_string(loops) + "\n";
    result_length = sloops.length();

    for (int i = 0; i < 6; i++) {
        for (int k = 0; k < nodeCnt; k++) {
            location[k][i] = result_length;
            result_length += Ans[k][i].len;
        }
    }
}
void output(const string &file_name, int pid) {

    int fd = open(file_name.c_str(), O_RDWR | O_CREAT, 0666);
    char *buf = (char *)mmap(NULL, result_length, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);
    close(fd);
    char *start;
    if (pid == 0) {
        memcpy(buf, sloops.c_str(), sloops.length());
    }

    for (int i = pid; i < nodeCnt; i += 4) {
        for (int j = 0; j < 6; j++) {
            start = buf + location[i][j];
            memcpy(start, Ans[i][j].l, Ans[i][j].len);
        }
    }
    munmap(buf, result_length);
}
int main() {
    extern vector<array<Loc, 6>> Ans;
    extern vector<int> all_loops;
    // string testFile = "test_data.txt";
    // string testFile = "test_data.txt";
    string testFile = "/data/test_data.txt";
    string resultFile = "/projects/student/result.txt";
    // auto t = clock();
    Check check;
    vector<thread> threadname;
    check.readInput(testFile);
    // system("pause");
    Ans = vector<array<Loc, 6>>(nodeCnt);
    all_loops = vector<int>(threadnum, 0);
    for (int i = 0; i < threadnum; i++) {
        thread mythread(check, i);
        threadname.push_back(move(mythread));
    }
    for (int i = 0; i < threadnum; i++) {
        threadname[i].join();
    }

    getlocation();
    // cout << result_length << endl;
    // system("pause");

    int fd = open(resultFile.c_str(), O_RDWR | O_CREAT, 0666);
    lseek(fd, result_length - 1, SEEK_SET);
    write(fd, " ", 1);
    close(fd);

    //  mmap多进程写
    int id;
    pid_t pid;
    vector<pid_t> pids;
    for (int i = 1; i < 4; i++) {
        id = i;
        pid = fork();
        if (pid <= 0)
            break;
        pids.push_back(pid);
    }

    if (pid == -1) {
        cerr << "startup process failed" << endl;
    } else {
        if (pid == 0) {
            output(resultFile, id); //  子进程
            exit(0);
        } else {
            output(resultFile, 0); //  main进程
        }
    }
    exit(0);
}