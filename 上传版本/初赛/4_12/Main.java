import java.io.*;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.CountDownLatch;

/*
    多线程 深度优先遍历
        3738：
            解决数据花费 ：3.072ms
            answer.size = 3738
            写入数据花费 ：3.443ms
        58284:  (15条线程)
            解决数据花费 ：12.109ms
            answer.size = 58284
            写入数据花费 ：13.196ms
        1004812: (10 条线程)
            解决数据花费 ：783.897ms
            answer.size = 1004812
            写入数据花费 ：787.632ms
 */
public class MultiThread {
    private static int[][] read;//二维数组，表示当前节点被访问的状态
    private static ArrayList<ArrayList<Integer>> graph;
    private static ArrayList<ArrayList<Integer>> result;
    private static ArrayList<Integer> data;// index to id
    private static int nodeCnt;//账户数
    private static final int THREAD_NUM = 20;//线程数量

    public static void main(String[] args) {
        long start = System.currentTimeMillis();
        // 读取数据到  input  1列 双数为  from ，单数为 to;
        ArrayList<Integer> input = readFile();
        createGraph(input);
        input.clear();
        //线程计数器
        CountDownLatch countDownLatch = new CountDownLatch(THREAD_NUM);
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
                check(helperId);
            } finally {
                //线程结束时，将计时器减一
                countDownLatch.countDown();
            }
        }
    }


    /*
        创建 邻接表 graph
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

        //创建 graph  和 map_index;
        graph = new ArrayList<>();
        // id to index
        Map<Integer, Integer> map_index = new HashMap<>();
        for (Integer integer : data) {
            map_index.put(integer, nodeCnt++);
            graph.add(new ArrayList<>());
        }

        //记录入读数，删除入读为0的节点
        int[] inDegrees = new int[nodeCnt];
        //初始化 graph
        for (int i = 0; i < input.size(); i += 2) {
            int from = map_index.get(input.get(i));
            int to = map_index.get(input.get(i + 1));
            graph.get(from).add(to);
            inDegrees[to]++;
        }
        for (int i = 0; i < graph.size(); i++) {
            if (inDegrees[i] == 0)
                graph.set(i, new ArrayList<>());
        }
        //初始化 节点状态列表 和 结果集
        read = new int[nodeCnt][THREAD_NUM];
        result = new ArrayList<>();
    }

    /*
        深度优先遍历 每一个节点 判断是否构成环，并且换的长度 大于三，小于 7
     */
    private static void DFS(int from, int cur, int depth, ArrayList<Integer> path, int threadId) {
        // 更新当前节点为已读
        read[cur][threadId] = 1;
        // 添加到路径
        path.add(cur);
        //遍历 当前节点可到达的节点
        for (Integer v : graph.get(cur)) {
            //判断是否构成环，且符合要求
            if (v == from && depth >= 3 && depth <= 7) {
                //生成环
                ArrayList<Integer> temp = new ArrayList<>();
                for (Integer integer : path) {
                    temp.add(data.get(integer));
                }
                synchronized (result) {
                    result.add(temp);
                }
            }
            // 剪枝 继续递归
            if (depth < 7 && read[v][threadId] != 1 && v > from)
                DFS(from, v, depth + 1, path, threadId);
        }
        //重置当前节点状态
        read[cur][threadId] = 0;
        //移除最后一个节点
        path.remove(path.size() - 1);
    }

    /*
        遍历所有节点
     */
    private static void check(int threadId) {
        ArrayList<Integer> path = new ArrayList<>();
        for (int i = threadId; i < nodeCnt; i += THREAD_NUM) {
            DFS(i, i, 1, path, threadId);
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
        写 出 结果
     */
    private static void write() {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter("/projects/student/result.txt"));
            writer.write(result.size() + "");
            writer.newLine();
            //按照 长度短的优先   长度一样 按 字典序排序
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
            //写出结果
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
