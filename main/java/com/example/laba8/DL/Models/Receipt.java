package com.example.laba8.DL.Models;

import java.time.LocalDate;
import java.util.Objects;

public class Receipt {
    private long id;
    private ReceiptType receiptType;
    private LocalDate date;
    private double sum;

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    public ReceiptType getReceiptType() {
        return receiptType;
    }

    public void setReceiptType(ReceiptType receiptType) {
        this.receiptType = receiptType;
    }

    public LocalDate getDate() {
        return date;
    }

    public void setDate(LocalDate date) {
        this.date = date;
    }

    public double getSum() {
        return sum;
    }

    public void setSum(double sum) {
        this.sum = sum;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Receipt receipt = (Receipt) o;
        return id == receipt.id && Double.compare(sum, receipt.sum) == 0 && receiptType == receipt.receiptType && Objects.equals(date, receipt.date);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, receiptType, date, sum);
    }
}
