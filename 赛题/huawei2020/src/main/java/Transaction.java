/**
  * @author:  Zhang
  * @description:
  *     交易记录
 *     from：出账户  to:入账户  money：转账金额
  *
 **/
public class Transaction {
    private int from;
    private String to;
    private String money;

    @Override
    public String toString() {
        return "Transaction{" +
                "from='" + from + '\'' +
                ", to='" + to + '\'' +
                ", money='" + money + '\'' +
                '}';
    }

    public String getFrom() {
        return from;
    }

    public void setFrom(String from) {
        this.from = from;
    }

    public String getTo() {
        return to;
    }

    public void setTo(String to) {
        this.to = to;
    }

    public String getMoney() {
        return money;
    }

    public void setMoney(String money) {
        this.money = money;
    }
}
