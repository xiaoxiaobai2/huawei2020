#include <bits/stdc++.h>
using namespace std;
typedef unsigned int ui;
ui input[280000 * 2];
map<ui, int> id;
unordered_map<int, ui> ids;
array<array<int, 50>, 140000> G;
array<array<int, 255>, 140000> GF;
array<bool, 140000> vis, able3, able4;
// bool vis[30000] = {false};

// bool able3[30000] = {false};
// bool able4[30000] = {false};
int idnum = 0;
int num = 0; // num<300,000
unordered_map<int, vector<int>> p3;
unordered_map<int, vector<array<int, 2>>> p4;
int ans3[3 * 500000];
int ans4[4 * 500000];
int ans5[5 * 1000000];
int ans6[6 * 2000000];
int ans7[7 * 3000000];
int ansNum[5];
int *ans[] = {ans3, ans4, ans5, ans6, ans7};
int Ginout[140000][2];
void inputs(string &testFile) {
    FILE *file = fopen(testFile.c_str(), "r");
    ui u, v, c;
    while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF) {
        input[2 * num] = u;
        input[2 * num + 1] = v;
        id[u] = 0;
        id[v] = 0;//???????
        num++;
    }
    for (map<ui, int>::iterator iter = id.begin(); iter != id.end(); iter++) {
        id[iter->first] = idnum;
        ids[idnum] = iter->first;
        idnum++;
    }
    for (int i = 0; i < num; i++) {
        int u = id[input[2 * i]];
        int v = id[input[2 * i + 1]];
        G[u][Ginout[u][0]++] = v;
        GF[v][Ginout[v][1]++] = u;
        // G[u][0]++;
        // GF[v][0]++;
    }
    for (int i = 0; i < idnum; i++) {
        sort(G[i].begin(), G[i].begin() + Ginout[i][0]);
        sort(GF[i].begin(), GF[i].begin() + Ginout[i][1]);
    }
    // for (int i = 0; i < Ginout[0][0]; i++) {
    //     cout << G[0][i] << endl;
    // }
}
void backhome(int head, int cur, int depth, int able, int *path) {
    path[depth - 1] = cur;
    switch (able) {
    case 3:
        for (int &u : p3[cur]) {
            if (!vis[u]) {
                path[depth] = u;
                // 当前ans的起始index
                int index = ansNum[depth - 2]++ * (depth + 1);
                for (int i = 0; i < depth + 1; i++) {
                    ans[depth - 2][index + i] = ids[path[i]];
                }
            }
        }
        break;
    case 4:
        for (array<int, 2> &a : p4[cur]) {
            if (!vis[a[0]] && !vis[a[1]]) {
                path[depth] = a[0];
                path[depth + 1] = a[1];
                int index = ansNum[depth - 1]++ * (depth + 2);
                for (int i = 0; i < depth + 2; i++) {
                    ans[depth - 1][index + i] = ids[path[i]];
                }
            }
        }
        break;
    }
}

// // unordered_map<int, int> P3;
// // unordered_map<int, vector<int[2]>> P4;
void prepare(int i) {
    int temp;
    array<int, 2> temp2;
    for (int u = 0; u < Ginout[i][1]; u++) {
        temp = GF[i][u];
        temp2[1] = temp;
        if (temp > i) {
            // GF[temp2[0]][W]-(GF[temp][v])-temp(GF[i][u])-i
            for (int v = 0; v < Ginout[temp][1]; v++) {
                if (GF[temp][v] > i) {
                    p3[GF[temp][v]].push_back(temp);
                    able3[GF[temp][v]] = true;
                    temp2[0] = GF[temp][v];
                    for (int w = 0; w < Ginout[GF[temp][v]][1]; w++) {
                        if (GF[temp2[0]][w] > i && GF[temp2[0]][w] != temp) {
                            p4[GF[temp2[0]][w]].push_back(temp2);
                            able4[GF[temp2[0]][w]] = true;
                            sort(p4[GF[temp2[0]][w]].begin(),
                                 p4[GF[temp2[0]][w]].end());
                        }
                    }
                }
            }
        }
    }
}
void dfs(int head, int cur, int depth, int *path) {
    if (depth == 5) {
        return;
    }
    vis[cur] = true;
    path[depth - 1] = cur;
    for (int u = 0; u < Ginout[cur][0]; u++) {
        int v = G[cur][u];
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
void d1(int head, int *path) {
    vis[head] = true;
    path[0] = head;
    for (int u = 0; u < Ginout[head][0]; u++) {
        int v = G[head][u];
        if (v < head) {
            continue;
        }
        if (able3[v]) {
            backhome(head, v, 2, 3, path);
        }
        if (able4[v]) {
            backhome(head, v, 2, 4, path);
        }
        // cout << "没毛病" << endl;
        dfs(head, v, 2, path);
    }
    vis[head] = false;
}
void solve() {
    int path[7];
    for (int i = 0; i < idnum; i++) {
        prepare(i);
        // for (unordered_map<int, vector<int>>::iterator iter = p3.begin();
        //      iter != p3.end(); iter++) {
        //     cout << iter->first;
        //     for (auto &u : iter->second) {
        //         cout << "  " << u;
        //     }
        //     cout << endl;
        // // }
        // system("pause");

        d1(i, path);
        p3.clear();
        p4.clear();
        able3 = vis;
        able4 = vis;
    }
}
void save() {
    FILE *file = fopen("/projects/student/result.txt", "w");
    // FILE *file = fopen("output.txt", "w");
    int loops = 0;
    loops = ansNum[0] + ansNum[1] + ansNum[2] + ansNum[3] + ansNum[4];
    fprintf(file, "%d\n", loops);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < ansNum[i]; j++) {
            int index = j * (i + 3);
            for (int k = 0; k < i + 2; k++) {
                // cout <<
                fprintf(file, "%d,", ans[i][index + k]);
            }
            fprintf(file, "%d\n", ans[i][index + i + 2]);
        }
    }
}

int main() {
    string testFile = "/data/test_data.txt";
    // string testFile = "test_data.txt";
    // string outputFile = "output.txt";
    inputs(testFile);
    
    // for (int i = 0; i < Ginout[4999][0]; i++)
    // {
    //     /* code */
    //     cout<< ids[G[4999][i]] << endl;
    // }
    
    solve();
    save();
    // cout << idnum << endl;
    // cout << num << endl;
    // cout << "结束" << endl;
    return 0;
}
