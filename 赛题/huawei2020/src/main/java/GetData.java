import java.io.*;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

/*
    读取测试数据文件
 */
class GetData {
    public static List<Transaction> readFile(String filePath){
        List<Transaction> list = new ArrayList<Transaction>();
        BufferedReader bufferedReader = null;
        String path = GetData.class.getClassLoader().getResource(filePath).getPath();
        try {
            bufferedReader = new BufferedReader(new FileReader(new File(path)));
            while (-1!=bufferedReader.read()){
                String line = bufferedReader.readLine();
                String[] split = line.split(",");
                Transaction transaction = new Transaction();
                transaction.setFrom(split[0]);
                transaction.setTo(split[1]);
                transaction.setMoney(split[2]);
                list.add(transaction);
            }
        } catch (FileNotFoundException e) {
            System.err.println("文件不存在");
        } catch (IOException e) {
            System.err.println("文件读取失败！");
        }finally {
            if (bufferedReader!=null) {
                try {
                    bufferedReader.close();
                } catch (IOException e) {
                    System.err.println("关闭失败！");
                }
            }
        }
        return list;
    }
}
