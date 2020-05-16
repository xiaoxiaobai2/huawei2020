import java.io.*;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Date;
import java.util.concurrent.CountDownLatch;

public class Main {
    private static ArrayList<ArrayList<ArrayList<Integer>>> circles = new ArrayList<>();
    private static ArrayList<Transaction> data1_1 = new ArrayList<>();
    private static long start = System.currentTimeMillis();
    private static int answer = 0;
    private static int carNumber = 0;


    public static void main(String[] args) {
        System.out.println("开始时间 = " + new Date());
        ArrayList<Transaction> data1 = readFile();
        //初始化circles
        int len = 5;
        while (len > 0) {
            ArrayList<ArrayList<Integer>> ArrayLists = new ArrayList<>();
            circles.add(ArrayLists);
            len--;
        }
        System.out.println("读数据花费： = " + (System.currentTimeMillis() - start) / 1000.0);

        // 按照to增序
        getData1(data1);
        System.out.println("排序花费： = " + (System.currentTimeMillis() - start) / 1000.0);

        //按照  from 增序
        getData1_1(data1_1);

//        //生成新的 data
//        check(data1, data1_1, 2);

        //分割data1
//        int threadNum = 4;
//        CountDownLatch countDownLatch = new CountDownLatch(threadNum);
//        int average = data1.size() / threadNum;
//        for (int j = 0; j < threadNum - 1; j++) {
//            MyHelper myHelper = new MyHelper(new ArrayList<>(data1.subList(j * average, (j + 1) * average)), j + 1, countDownLatch);
//            MultiThread thread = new MultiThread(myHelper);
//            thread.start();
//        }
//
//        //计算剩余部分
//        MyHelper myHelper = new MyHelper(new ArrayList<>(data1.subList((threadNum - 1) * average, data1.size())), threadNum, countDownLatch);
//        MultiThread thread = new MultiThread(myHelper);
//        thread.start();
        check(data1,data1_1,2);
        //对环进行排序

        for (int j = 0; j < circles.size(); j++) {
            answer += circles.get(j).size();
            int size = j + 3;
            while (size > 0) {
                getCircle(circles.get(j), size);
                size--;
            }
        }
        // 生成result
        write();
        System.out.println("总共花费： = " + (System.currentTimeMillis() - start) / 1000.0);
    }


    /*
        遍历 data1_1 和  data 1_0
            判断是否构成新的环
                生成新的转账路径表
     */
    private static ArrayList<Transaction> check(ArrayList<Transaction> data0, ArrayList<Transaction> data1, int num) {
        int maxSize = 10_0000;
        //跳出递归
        System.gc();
        if (num > 7) {
            System.out.println("完成了一部分。。。。。。。");
            return null;
        }
        //存储新的from to path
        ArrayList<Transaction> newData = new ArrayList<>();

        //存储当前边所构成的环,不存储2环
        ArrayList<ArrayList<Integer>> circleN = null;
        if (num > 2) {
            circleN = circles.get(num - 3);
        }
        int k = 0;
        for (int i = 0; i < data0.size(); i++) {
            for (int j = k; j < data1.size(); j++) {
                //  to 和 from 相等，可以连成两节
                if (data0.get(i).getTo() == data1.get(j).getFrom() && data0.get(i).getFrom() <= data1.get(j).getTo()) {
                    //clone 并更新
                    Transaction clone = data0.get(i).clone();

                    clone.getPath().add(data1.get(j).getFrom());
                    if (contains(clone.getPath())) {
                        continue;
                    }
                    clone.setTo(data1.get(j).getTo());
                    //判断是否构成环 去掉8 字循环
                    if (clone.getFrom() == clone.getTo() && num > 2) {
                        circleN.add(clone.getPath());
                    } else {
                        newData.add(clone);
                        //一车满了，先走
                        if (newData.size() > maxSize) {
                            System.out.println("第" + ++carNumber + "辆车出发。。");
                            getData1(newData);
                            check(newData, data1_1, num + 1);
                            newData.clear();
                        }
                    }
                } else if (data1.get(j).getFrom() > data0.get(i).getTo()) {
//                    //下一个不是 to 和当前不一样，则接着当前的j  判断
//                    //  如果相等，则k 不变
                    if (i != data0.size() - 1 && data0.get(i).getTo() != data0.get(i + 1).getTo()) {
                        k = j;
                    }
                    break;
                }
            }
        }
        getData1(newData);
        System.out.println("第" + ++carNumber + "辆车出发。。（没装满）");

        return check(newData, data1, num + 1);
    }


    private static boolean contains(ArrayList<Integer> path) {
        for (int i = 0; i < path.size() - 1; i++)
            if (path.get(path.size() - 1).equals(path.get(i)))
                return true;
        return false;
    }

    /*
        按第一列排序，生成data1_0
     */
    private static void getData1(ArrayList<Transaction> data) {
        data.sort(Comparator.comparingInt(Transaction::getTo));
    }

    private static void getCircle(ArrayList<ArrayList<Integer>> circle, int index) {
        circle.sort(Comparator.comparing(o -> o.get(index - 1)));
    }

    /*
        按第二列排序，生成data1_1
     */
    private static void getData1_1(ArrayList<Transaction> data) {
        data.sort(Comparator.comparingInt(Transaction::getFrom));
    }

    /*
        读取文件
     */
    private static ArrayList<Transaction> readFile() {

        ArrayList<Transaction> data = new ArrayList<>();
        BufferedReader bufferedReader = null;
        try {
            bufferedReader = new BufferedReader(new FileReader("C:\\Users\\Administrator\\Desktop\\huawei2020\\untitled\\src\\data\\test_data.txt"));

            String line = null;
            while ((line = bufferedReader.readLine()) != null) {

                String[] split = line.split(",");
                ArrayList<Integer> cross = new ArrayList<>();

                Transaction transaction = new Transaction();
                transaction.setFrom(Integer.parseInt(split[0]));
                transaction.setTo(Integer.parseInt(split[1]));
                cross.add(Integer.parseInt(split[0]));
                transaction.setPath(cross);

                Transaction transaction2 = new Transaction();
                transaction2.setFrom(Integer.parseInt(split[0]));
                transaction2.setTo(Integer.parseInt(split[1]));
                if (transaction.getFrom() < transaction.getTo()) {
                    data.add(transaction);
                }
                data1_1.add(transaction2);
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
        return data;
    }

    private static void write() {
        try {
//            BufferedWriter writer = new BufferedWriter(new FileWriter("result.txt"));
            BufferedWriter writer = new BufferedWriter(new FileWriter("src/result/result.txt"));
            System.out.println("answer = " + answer);
            writer.write(answer + "");
            writer.newLine();
            for (int i = 0; i < circles.size(); i++) {
                System.out.println("circle" + (i + 1) + ":");
                for (ArrayList<Integer> arrayList : circles.get(i)) {
                    String s = arrayList.toString();
                    writer.write(s.substring(1, s.length() - 1).replace(" ", ""));
                    writer.newLine();
                    System.out.println("arrayList = " + arrayList);
                }
            }
            writer.flush();
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static class MyHelper extends MultiThread {
        private ArrayList<Transaction> data;
        private int num;
        private CountDownLatch countDownLatch;

        public MyHelper(ArrayList<Transaction> data, int num, CountDownLatch countDownLatch) {
            this.data = data;
            this.num = num;
            this.countDownLatch = countDownLatch;
        }

        public void run() {
            try {
                System.out.println("\t\t\t\t\t\t\t\t线程" + num + "开始。");
                long s = System.currentTimeMillis();
                check(data, data1_1, 2);
                long e = System.currentTimeMillis();
                System.out.println("\t\t\t\t\t\t\t\t线程 " + num + " 完成花费：" + (e - s) / 1000.0);
            } finally {
                //线程结束时，将计时器减一
                countDownLatch.countDown();
            }
        }
    }
}


/*
    交易记录  from to  经过的路径
 */
class Transaction implements Cloneable {
    private int from;
    private int to;
    private ArrayList<Integer> path;


    int getFrom() {
        return from;
    }

    void setFrom(int from) {
        this.from = from;
    }

    int getTo() {
        return to;
    }

    void setTo(int to) {
        this.to = to;
    }

    ArrayList<Integer> getPath() {
        return path;
    }

    void setPath(ArrayList<Integer> path) {
        this.path = path;
    }

    @Override
    public String toString() {
        return "Transaction{" +
                "from=" + from +
                ", to=" + to +
                ", path=" + path +
                '}';
    }

    @Override
    protected Transaction clone() {
        Transaction transaction = null;
        try {
            transaction = (Transaction) super.clone();
            transaction.path = (ArrayList<Integer>) path.clone();
        } catch (CloneNotSupportedException e) {
            e.printStackTrace();
        }
        return transaction;
    }
}