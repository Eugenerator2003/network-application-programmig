package com.example.laba8.DL.Persistance;

import com.example.laba8.DL.Models.Receipt;
import com.example.laba8.DL.Models.ReceiptType;

import javax.swing.text.DateFormatter;
import java.time.format.DateTimeFormatter;

public class MySqlReceiptSqlGenerator implements ReceiptSqlGenerator {
    private static DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");

    @Override
    public String all() {
        return "SELECT * FROM receipts;";
    }

    @Override
    public String create(Receipt receipt) {
        return "INSERT INTO receipts (id, type, date, sum) " +
                "VALUES (" + Long.valueOf(receipt.getId()).toString() + "," +
                        Integer.valueOf(ReceiptType.toInteger(receipt.getReceiptType())).toString() + ",'" +
                        receipt.getDate().format(formatter) + "'," +
                        Double.valueOf(receipt.getSum()) + ");";

    }

    @Override
    public String read(long id) {
        return "SELECT * FROM receipts WHERE id = " + Long.valueOf(id).toString() + ";";
    }

    @Override
    public String update(Receipt receipt) {
        return "UPDATE receipts SET type = " + Long.valueOf(ReceiptType.toInteger(receipt.getReceiptType()))
                                                    .toString() +
                                    ", sum = " + Double.valueOf(receipt.getSum()).toString() +
                                    ", date = '" + receipt.getDate().format(formatter) +
                "' WHERE id = " + Long.valueOf(receipt.getId()).toString() + ";";
    }

    @Override
    public String delete(long id) {
        return "DELETE FROM receipts WHERE id = " + Long.valueOf(id).toString() + ";";
    }

    @Override
    public String nextId() {
        return "SELECT MAX(id) FROM receipts";
    }
}
