import java.util.List;

public class Main {
    public static void main(String[] args) {
        String pathFie = "data/test_data.txt";
        List<Transaction> transactions = GetData.readFile(pathFie);
        for (Transaction transaction : transactions) {
            System.out.println("transaction = " + transaction);
        }
    }
}
