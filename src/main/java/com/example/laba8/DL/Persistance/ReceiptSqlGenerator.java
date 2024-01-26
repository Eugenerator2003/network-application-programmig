package com.example.laba8.DL.Persistance;

import com.example.laba8.DL.Models.Receipt;

public interface ReceiptSqlGenerator {
    String all();
    String create(Receipt receipt);
    String read(long id);
    String update(Receipt receipt);
    String delete(long id);
    String nextId();
}
