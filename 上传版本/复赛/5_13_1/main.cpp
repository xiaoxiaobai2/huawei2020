#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
typedef unsigned int ui;
struct VC
{
    int v;
    int c;
    VC(const int v, const int c) : v(v), c(c) {}
    bool operator<(const VC &vc) const { return v < vc.v; }
};
//  判断 i-u-v  的金额是否符合要求
inline bool COK(int cu, int cv)
{
    long long c = cu;
    long long c1 = cv;

    return 0.2 * c <= c1 && 3 * c >= c1;
}

int threadnum = 8;
vector<int> all_loops;
vector<array<string, 5>> Ans;
vector<vector<VC>> Gout, Gin; // 出入度，存v 和 c
vector<string> ids;
int nodeCnt = 0;

class Check
{
public:
    // vector<int> inDegrees, outDegrees;
    vector<bool> vis, able3, able4;
    unordered_map<int, vector<array<int, 3>>> p3;
    unordered_map<int, vector<array<int, 4>>> p4;
    array<int, 3> path3;
    array<int, 4> path4;
    vector<int> in;
    vector<int> out;
    int path[5]; // 环路径 前5个
    int loops;
    int head, headC; // 当前头结点和第一步的金额
    int maxC, minC;
    /**
     * 启用多线程   按线程id 分任务找环
     * */
    void operator()(int threadId)
    {
        vis = vector<bool>(nodeCnt, false);
        able3 = vector<bool>(nodeCnt, false);
        able4 = vector<bool>(nodeCnt, false);
        // able = vector<bool>(nodeCnt, false);
        for (int i = threadId; i < nodeCnt; i += threadnum)
        {
            // cout << threadId << endl;
            head = i;
            canBack(i);
            prepare();
            if (p3.size() == 0)
            {
                continue;
            }
            dfs();
            p3.clear();
            p4.clear();
            able3 = vector<bool>(nodeCnt, false);
            able4 = vector<bool>(nodeCnt, false);
        }
        all_loops[threadId] = loops;
    }

    /**
     * 读取数据，生成邻接表，初始化数据
     * */
    void readInput(string &testFile)
    {
        vector<ui> inputs;
        vector<ui> counts;
        // auto t = clock();
        FILE *file = fopen(testFile.c_str(), "r");
        ui u, v, c;
        while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF)
        {
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
        for (ui &x : tmp)
        {
            ids[nodeCnt] = to_string(x);
            idHash[x] = nodeCnt++;
        }

        int sz = inputs.size();

        Gout = vector<vector<VC>>(nodeCnt);
        Gin = vector<vector<VC>>(nodeCnt);
        // inDegrees = vector<int>(nodeCnt, 0);
        // outDegrees = vector<int>(nodeCnt, 0);
        for (int i = 0; i < sz; i += 2)
        {
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]],
                c = counts[i / 2];
            Gout[u].push_back(VC(v, c));
            Gin[v].push_back(VC(u, c));
            // ++inDegrees[v];
            // ++outDegrees[u];
        }
        // cout << "con :" << clock() - t << endl;
        // system("pause");

        for (int i = 0; i < nodeCnt; i++)
        {
            sort(Gout[i].begin(), Gout[i].end());
            sort(Gin[i].begin(), Gin[i].end());
        }
        loops = 0;
    }

    /**
     * 生成3环
     * */
    void backhome3(int cur, int c)
    {
        string tmp = "";
        for (int i = 0; i < 2; i++)
        {
            tmp.append(ids[path[i]] + ",");
        }

        // 找3环
        for (array<int, 3> &u : p3[cur])
        {
            // 判断是否重复 及 符合金额要求
            if (!vis[u[0]] && COK(c, u[2]) && COK(u[1], headC))
            {
                Ans[head][0].append(tmp);
                Ans[head][0].append(ids[u[0]] + "\n");
                loops++;
            }
        }
    }
    int getLowerIndex(vector<VC> *vc)
    {
        int left = 0;
        int right = vc->size() - 1;
        int mid;
        while (left <= right)
        {
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
    void canBack(int i)
    {
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
                if (COK(Gin[i][j].c, Gout[i][k].c))
                {
                    in.push_back(j);
                    v[k] = true;
                    break;
                }
        for (int j = n; j < size2; j++)
        {
            if (v[j])
            {
                out.push_back(j);
                continue;
            }
            for (int k = m; k < size1; k++)
                if (COK(Gin[i][k].c, Gout[i][j].c))
                {
                    out.push_back(j);
                    break;
                }
        }
    }
    /**
     * 生成4 - 7环
     * */
    void backhome(int cur, int depth, int c)
    {
        string tmp = "";
        for (int i = 0; i < depth + 1; i++)
        {
            tmp.append(ids[path[i]] + ",");
        }
        // 找4-7环
        for (array<int, 4> &u : p4[cur])
        {
            // 判断是否重复 及 符合金额要求
            // 会不会出现所有的都 vis过
            if (!vis[u[0]] && !vis[u[1]] && COK(c, u[3]) &&
                COK(u[2], headC))
            {
                Ans[head][depth].append(tmp);
                Ans[head][depth].append(ids[u[0]] + ",");
                Ans[head][depth].append(ids[u[1]] + "\n");
                loops++;
            }
        }
    }

    // bool canBack()
    // {
    //     long long c = headC;
    //     long long max = maxC;
    //     long long min = minC;

    //     return 5 * c < min || 3 * max < c;
    // }
    /**
     * 深度遍历找环
     * */
    void dfs()
    {
        vis[head] = true;
        path[0] = head;
        for (int &i : out)
        {
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
                backhome(v.v, 1, headC);
            // dfs(v.v, 2, headC);

            vis[v.v] = true;
            for (VC &u : Gout[v.v])
            {
                // 剪枝
                if (u.v <= head || vis[u.v] || !COK(headC, u.c))
                    continue;
                path[2] = u.v;

                // 找5环
                if (able4[u.v])
                    backhome(u.v, 2, u.c);
                vis[u.v] = true;
                for (VC &w : Gout[u.v])
                {
                    // 剪枝
                    if (w.v <= head || vis[w.v] || !COK(u.c, w.c))
                        continue;
                    path[3] = w.v;
                    // 找6环
                    if (able4[w.v])
                        backhome(w.v, 3, w.c);
                    vis[w.v] = true;
                    for (VC &z : Gout[w.v])
                    {
                        // 剪枝
                        if (z.v <= head || vis[z.v] || !COK(w.c, z.c))
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

    /**
     * 准备 p3 p4 able3 able4
     * */
    void prepare()
    {
        //  w - u -v -head
        // path3   v-      v.c-u.c
        // path4   u-v     v.c-w.c
        // unordered_map<int, vector<array<int,4>>> p4,
        // maxC = Gin[head][0].c;
        // minC = maxC;
        for (int &i : in)
        {
            VC v = Gin[head][i];
            // temp[0] = v;
            path4[1] = v.v;
            path3[0] = v.v;
            vis[v.v] = true;

            // c = v.c;
            path4[2] = v.c;
            path3[1] = v.c;
            // if (v.c > maxC)
            //     maxC = v.c;
            // if (v.c < minC)
            //     minC = v.c;
            for (VC &u : Gin[v.v])
            {
                if (u.v > head && COK(u.c, v.c))
                {
                    // c = u.c;
                    // temp.push_back(u);
                    path3[2] = u.c;
                    p3[u.v].push_back(path3);
                    able3[u.v] = true;
                    path4[0] = u.v;
                    for (VC &w : Gin[u.v])
                    {
                        if (w.v > head && !vis[w.v] && COK(w.c, u.c))
                        {
                            // tep.push_back(w);
                            path4[3] = w.c;
                            p4[w.v].push_back(path4);
                            able4[w.v] = true;
                            sort(p4[w.v].begin(), p4[w.v].end());
                        }
                    }
                }
            }

            vis[v.v] = false;
        }
    }

    /*
        fwrite 逐行写
    */
    void output()
    {
        // FILE *file = fopen("output.txt", "w");
        // int f = open("output.txt", O_RDWR | O_CREAT, 0666);
        // int f = open("/projects/student/result.txt", O_RDWR | O_CREAT, 0666);

        FILE *file = fopen("/projects/student/result.txt", "w");
        for (int &u : all_loops)
        {
            loops += u;
        }
        fprintf(file, "%d\n", loops);
        // string cirecels = to_string(loops) + "\n";
        // write(f, cirecels.c_str(), cirecels.length());
        // cout << loops << endl;
        // char *buf;
        // string result;
        // result.reserve()
        for (int i = 0; i < 5; i++)
        {
            for (int k = 0; k < nodeCnt; k++)
            {
                // buf = new char[Ans[k][i].length() + 1];
                // strcpy(buf, Ans[k][i].c_str());
                // write(f, Ans[k][i].c_str(), Ans[k][i].length());
                fwrite(Ans[k][i].c_str(), 1, Ans[k][i].length(), file);
                // result.append(Ans[k][i]);
            }
        }
        // close(f);
        // write(f, result.c_str(), result.length());
        // fwrite(result.c_str(), 1, result.length(), file);
        fflush(file);
        fclose(file);
    }
};

int main()
{
    extern vector<array<string, 5>> Ans;
    extern vector<int> all_loops;
    // string testFile = "test_data.txt";
    string testFile = "/data/test_data.txt";
    // auto t = clock();
    Check check;
    vector<thread> threadname;
    check.readInput(testFile);
    // cout << clock() - t << endl;
    // system("pause");
    // t = clock() - t;

    Ans = vector<array<string, 5>>(nodeCnt);
    all_loops = vector<int>(threadnum, 0);
    for (int i = 0; i < threadnum; i++)
    {
        thread mythread(check, i);
        threadname.push_back(move(mythread));
    }
    for (int i = 0; i < threadnum; i++)
    {
        threadname[i].join();
    }
    // cout << clock() - t << endl;
    check.output();
    // cout << clock() - t << endl;
    // system("pause");
    return 0;
}