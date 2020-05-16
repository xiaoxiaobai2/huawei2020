import java.io.*;
import java.util.*;
import java.util.concurrent.CountDownLatch;

/**
 * @author: Zhang
 * @description: 多线程 + 广度优先遍历 + 深度优先遍历  +  拓扑排序 + 减枝
 **/
public class Main {
    private static int[][] read;//二维数组，表示当前节点被访问的状态
    private static ArrayList<ArrayList<Integer>> graphTo;
    private static ArrayList<ArrayList<Integer>> graphFrom;
    private static ArrayList<ArrayList<ArrayList<Integer>>> answer;
    private static ArrayList<ArrayList<Integer>> result;
    private static ArrayList<Integer> data;// index to id
    private static int nodeCnt;
    private static final int THREAD_NUM = 300;//线程数量

    public static void main(String[] args) {
        // 读取数据到  input  1列 双数为  from ，单数为 to;
        createGraph(readFile());
        CountDownLatch countDownLatch = new CountDownLatch(THREAD_NUM);
        result = new ArrayList<>();
        for (int i = 0; i < THREAD_NUM; i++) {
            new Thread(new MyHelper(i, countDownLatch)).start();
        }
        //等待所有线程结束
        try {
            countDownLatch.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        write();
    }

    static class MyHelper implements Runnable {
        private int helperId;
        private CountDownLatch countDownLatch;

        MyHelper(int helperId, CountDownLatch countDownLatch) {
            this.helperId = helperId;
            this.countDownLatch = countDownLatch;
        }

        @Override
        public void run() {
            try {
                solve(helperId);
            } finally {
                synchronized (result) {
                    result.addAll(answer.get(helperId));
                }
                //线程结束时，将计时器减一
                countDownLatch.countDown();
            }
        }
    }

    /*
        读取文件，存成一列
     */
    private static ArrayList<Integer> readFile() {
        ArrayList<Integer> input = new ArrayList<>();
        BufferedReader bufferedReader = null;
        try {
            bufferedReader = new BufferedReader(new FileReader("/data/test_data.txt"));

            String line;
            while ((line = bufferedReader.readLine()) != null) {

                String[] split = line.split(",");
                input.add(Integer.parseInt(split[0]));
                input.add(Integer.parseInt(split[1]));
            }
        } catch (FileNotFoundException e) {
            System.err.println("文件不存在");
        } catch (IOException e) {
            System.err.println("文件读取失败！");
        } finally {
            if (bufferedReader != null) {
                try {
                    bufferedReader.close();
                } catch (IOException e) {
                    System.err.println("关闭失败！");
                }
            }
        }
        return input;
    }

    /*
        创建
            graphTO     : To的邻接表
            graphFrom   : from的邻接表
            data        : index   To   account
            index_map   : account To   index
            nodeCnt     : 账户数量
            read[][]    : 当前账户的状态
     */
    private static void createGraph(ArrayList<Integer> input) {
        //排序去重
        ArrayList<Integer> temp = new ArrayList<>(input);
        temp.sort(Comparator.comparingInt(o -> o));
        data = new ArrayList<>();
        //判断上一个是否已经存在
        int front = -1;
        for (Integer integer : temp) {
            if (front != integer) {
                data.add(integer);
                front = integer;
            }
        }
        //清除销毁 临时 列表
        temp.clear();

        nodeCnt = 0;

        //创建 graphTo  和 map_index;
        graphTo = new ArrayList<>();
        graphFrom = new ArrayList<>();
        // id to index
        Map<Integer, Integer> map_index = new HashMap<>();
        for (Integer integer : data) {
            map_index.put(integer, nodeCnt++);
            graphTo.add(new ArrayList<>());
            graphFrom.add(new ArrayList<>());
        }

        // 出入度列表，可够成环的
        int[] inDegrees = new int[nodeCnt];
        int[] outDegrees = new int[nodeCnt];
        //初始化 graphTo 和 graphFrom
        for (int i = 0; i < input.size(); i += 2) {
            int from = map_index.get(input.get(i));
            int to = map_index.get(input.get(i + 1));
            graphTo.get(from).add(to);
            graphFrom.get(to).add(from);
            inDegrees[to]++;
            outDegrees[from]++;
        }
        //拓扑排序
        topologicalSort(inDegrees, outDegrees);
        //初始化 节点状态列表 和 结果集
        read = new int[nodeCnt][THREAD_NUM];
        //每个线程的结果集分开，简化加锁
        answer = new ArrayList<>();
        for (int i = 0; i < THREAD_NUM; i++) {
            answer.add(new ArrayList<>());
        }
    }

    /*
        拓扑排序 ， 置空 入度 和 出度为 0 的账户节点
     */
    private static void topologicalSort(int[] inDegrees, int[] outDegrees) {
//        //待更新节点
        Stack<Integer> updateInDegrees = new Stack<>();
        Stack<Integer> updateOutDegrees = new Stack<>();
        for (int i = 0; i < inDegrees.length; i++) {
            if (inDegrees[i] == 0)
                updateInDegrees.push(i);
            if (outDegrees[i] == 0)
                updateOutDegrees.push(i);
        }
        while (!updateInDegrees.isEmpty()) {
            Integer node = updateInDegrees.pop();
            //更新 删除 节点的 入读，并判断是否 小于0；加入待更新列表
            ArrayList<Integer> nodeTo = graphTo.get(node);
            for (Integer to : nodeTo) {
                //更新 和该节点相关的 入读
                graphFrom.get(to).remove(node);
                if (--inDegrees[to] <= 0)
                    updateInDegrees.push(to);
            }
            graphTo.set(node, new ArrayList<>());
        }
        while (updateOutDegrees.isEmpty()) {
            Integer node = updateOutDegrees.pop();
            ArrayList<Integer> nodeFrom = graphFrom.get(node);
            for (Integer from : nodeFrom) {
                graphTo.get(from).remove(node);
                if (--outDegrees[from] <= 0)
                    updateInDegrees.push(from);
            }
            graphFrom.set(node, new ArrayList<>());
        }
    }

    /*
        深度优先遍历 每一个节点 判断是否构成环，并且换的长度 大于三，小于 7
     */
    private static void DFS(int from, int cur, int depth, ArrayList<Integer> path, int threadId,
                            ArrayList<ArrayList<ArrayList<Integer>>> nodeFromPath, int[] able) {
        //深度小于 5 时，进行深度搜索
        if (depth < 5) {
            // 更新当前节点为已读
            read[cur][threadId] = 1;
            // 添加到路径
            path.add(cur);
            //遍历 当前节点可到达的节点
            for (Integer v : graphTo.get(cur)) {
                //判断是否构成环，且符合要求
                if (v == from && depth >= 3) {
                    //生成环
                    ArrayList<Integer> temp = new ArrayList<>();
                    for (Integer integer : path) {
                        temp.add(data.get(integer));
                    }
                    answer.get(threadId).add(temp);
                }
                // 剪枝 继续递归
                if (read[v][threadId] != 1 && v > from) {
                    DFS(from, v, depth + 1, path, threadId, nodeFromPath, able);
                }
            }
            //重置当前节点状态
            read[cur][threadId] = 0;
            //移除最后一个节点
            path.remove(depth - 1);
        } else if (depth <= 7 && able[cur] == 1) {
            /*
                深度大于五时连接前面
             */
            //添加当前节点到路径
            path.add(cur);
            for (ArrayList<Integer> fromPath : nodeFromPath.get(depth - 5)) {
                if (fromPath.get(0) == cur) {
                    boolean flag = true;
                    for (int i = 1; i < fromPath.size(); i++) {
                        path.add(fromPath.get(i));
                        //判断节点是否已读
                        if (read[fromPath.get(i)][threadId] == 1)
                            flag = false;
                    }
                    //生成环 ，，已读则不重复添加环
                    if (flag) {
                        ArrayList<Integer> temp = new ArrayList<>();
                        for (Integer integer : path) {
                            temp.add(data.get(integer));
                        }
                        answer.get(threadId).add(temp);
                    }
                    //移除节点
                    for (int i = 1; i < fromPath.size(); i++) {
                        path.remove(path.size() - 1);
                    }
                }
            }
            path.remove(4);
            DFS(from, cur, depth + 1, path, threadId, nodeFromPath, able);
        }
    }

    private static ArrayList<ArrayList<ArrayList<Integer>>> BFSgetFromPath(int cur, int[] able) {

        //生成深度 为 1 的广度列表
        ArrayList<ArrayList<Integer>> fromPath1 = new ArrayList<>();
        for (Integer integer : graphFrom.get(cur)) {
            if (integer > cur) {
                ArrayList<Integer> path1 = new ArrayList<>();
                path1.add(integer);
                fromPath1.add(path1);
                able[integer] = 1;
            }
        }

        //生成深度 为 2 的广度列表
        ArrayList<ArrayList<Integer>> fromPath2 = new ArrayList<>();
        for (ArrayList<Integer> arrayList : fromPath1) {
            for (Integer integer : graphFrom.get(arrayList.get(0))) {
                if (integer > cur) {
                    ArrayList<Integer> path2 = new ArrayList<>();
                    path2.add(integer);
                    path2.addAll(arrayList);
                    fromPath2.add(path2);
                    able[integer] = 1;
                }
            }
        }

        //生成深度 为 3 的广度列表
        ArrayList<ArrayList<Integer>> fromPath3 = new ArrayList<>();
        for (ArrayList<Integer> arrayList : fromPath2) {
            for (Integer integer : graphFrom.get(arrayList.get(0))) {
                if (integer > cur && !integer.equals(arrayList.get(1))) {
                    ArrayList<Integer> path3 = new ArrayList<>();
                    path3.add(integer);
                    path3.addAll(arrayList);
                    fromPath3.add(path3);
                    able[integer] = 1;
                }
            }
        }
        //节点  i 的  fromPath
        ArrayList<ArrayList<ArrayList<Integer>>> fromPath = new ArrayList<>();
        fromPath.add(fromPath1);
        fromPath.add(fromPath2);
        fromPath.add(fromPath3);
        return fromPath;
    }


    /*
        遍历所有节点
     */
    private static void solve(int threadId) {
        ArrayList<Integer> path = new ArrayList<>();
        for (int i = threadId; i < nodeCnt; i += THREAD_NUM) {
            int[] able = new int[nodeCnt];
            DFS(i, i, 1, path, threadId, BFSgetFromPath(i, able), able);
        }
    }

    /*
        写 出 结果
     */
    private static void write() {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter("/projects/student/result.txt"));
            writer.write(result.size() + "");
            writer.newLine();
            result.sort((o1, o2) -> {
                if (o1.size() != o2.size()) {
                    return o1.size() > o2.size() ? 1 : -1;
                } else {
                    for (int i = 0; i < o1.size(); i++) {
                        if (!o1.get(i).equals(o2.get(i)))
                            return o1.get(i) > o2.get(i) ? 1 : -1;
                    }
                }
                return 0;
            });
            for (ArrayList<Integer> circle : result) {
                String s = circle.toString();
                writer.write(s.substring(1, s.length() - 1).replace(" ", ""));
                writer.newLine();
            }
            writer.flush();
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
