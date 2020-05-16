#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;



#define TEST
int threadnum = 4;
vector<int> all_loops;

typedef unsigned int ui;
vector<vector<string>> Ans;
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
    vector<bool> vis, able3, able4;
    // vector<vector<int>> frmpath1, frmpath2, frmpath3;
    unordered_map<int, vector<vector<int>>> p3, p4;
    // vector<vector<vector<vector<ui>>>> ans;
    int nodeCnt;
    int loops;
    void operator()(int threadId) {
        vis = vector<bool>(nodeCnt, false);
        int path[7];
        able3 = vector<bool>(nodeCnt, false);
        able4 = vector<bool>(nodeCnt, false);
        // able = vector<bool>(nodeCnt, false);
        for (int i = threadId; i < nodeCnt; i += threadnum) {
            // cout << threadId << endl;
            if (!G[i].empty()) {
                prepare(i);
                d1(i,path);
                p3.clear();
                p4.clear();
                able3 = vis;
                able4 = vis;
                // frmpath1.clear();
                // frmpath2.clear();
                // frmpath3.clear();
            }
        }
        all_loops[threadId] = loops;
    }

    void parseInput(string &testFile) {
        FILE *file = fopen(testFile.c_str(), "r");
        fseek(file,0,SEEK_END);//偏移到最后，获取长度
        int len = ftell(file);
        fseek(file,0,SEEK_SET);//偏移回去
        char* buf = new char[len]; 
        fread(buf,1,len,file);//读取内容到buf
        ui node;
        int pre=0;
        int rear=0;
        char temp[33];
        int num=0;
        // stringstream stream;
        for (int i = 0; i < len; i++)
        {
            if(buf[i]==',' || buf[i]=='\n'){
                num++;
                pre = i;
                if(num%3!=0){
                    pre=i;
                    strncpy(temp,buf+rear,pre-rear);
                    temp[pre-rear]='\0';
                    // stream << temp;
                    // stream >> node ;
                    // stream.clear();
                    sscanf(temp,"%ui",&node);
                    inputs.push_back(node);
                }
                pre++;
                rear = pre;
            }
        }
        fclose(file);
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

        for (int i = 0; i < nodeCnt; i++) {
            sort(G[i].begin(), G[i].end());
            sort(GF[i].begin(), GF[i].end());
        }
        loops = 0;
    }

    void backhome(int head, int cur, int depth, int able, int *path) {
        path[depth-1] = cur;
        string tmp="";
        for(int i=0;i<depth;i++){
            tmp.append(to_string(ids[path[i]]) + ",");
        }
        switch (able) {
        case 3:
            for (vector<int> &u : p3[cur]) {
                if (!vis[u[0]]) {
                    Ans[head][depth - 2].append(tmp);
                    Ans[head][depth - 2].append(to_string(ids[u[0]]) + "\n");
                    loops++;
                }
            }
            break;
        case 4:
            for (vector<int> &u : p4[cur]) {
                if (!vis[u[0]] && !vis[u[1]]) {
                    Ans[head][depth - 1].append(tmp);
                    Ans[head][depth - 1].append(to_string(ids[u[0]]) + ",");
                    Ans[head][depth - 1].append(to_string(ids[u[1]]) + "\n");
                    loops++;
                }
            }
            break;
        }
    }
    void d1(int head, int *path) {
        vis[head] = true;
        path[0] =head;
        for (int &v : G[head]) {
            if (v < head) {
                continue;
            }
            if (able3[v]) {
                backhome(head, v, 2, 3, path);
            }
            if (able4[v]) {
                backhome(head, v, 2, 4, path);
            }
            dfs(head, v, 2, path);
        }
        vis[head] = false;
    }

    void dfs(int head, int cur, int depth, int *path) {
        if (depth == 5) {
            return;
        }
        vis[cur] = true;
        path[depth-1] = cur;
        for (int &v : G[cur]) {
            // 剪枝
            if (v <= head || vis[v]) {
                continue;
            }
            if (able4[v] && depth < 5) {
                backhome(head, v, depth + 1, 4, path);
            }
            dfs(head, v, depth + 1, path);
        }
        vis[cur] = false;
    }
    // unordered_map<int, vector<vector<int>>> p3,
    void prepare(int i) {
        vector<int> temp, temp2;
        temp = vector<int>(1);
        temp2 = vector<int>(2);
        for (int &v : GF[i]) {
            temp[0] = v;
            temp2[1] = v;
            if (v > i) {
                for (int &u : GF[v]) {
                    if (u > i) {
                        // temp.push_back(u);
                        p3[u].push_back(temp);
                        able3[u] = true;
                        temp2[0] = u;
                        for (int &w : GF[u]) {
                            if (w > i && w != v) {
                                // tep.push_back(w);
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
                buf = new char[Ans[k][i].length()+1];
                strcpy(buf, Ans[k][i].c_str());
                fwrite(buf, 1, Ans[k][i].length(), file);
            }
        }
        fflush(file);
        fclose(file);
    }
};

int main() {
    extern vector<vector<string>> Ans;
    extern vector<int> all_loops;
    // string testFile = "test_data.txt";
    string testFile = "/data/test_data.txt";

    Solution solution;
    vector<thread> threadname;
    solution.parseInput(testFile);
    solution.constructGraph();
    Ans = vector<vector<string>>(solution.nodeCnt,
                                             vector<string>(5));
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