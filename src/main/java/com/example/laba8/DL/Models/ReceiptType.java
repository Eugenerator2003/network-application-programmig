package com.example.laba8.DL.Models;

public enum ReceiptType {
    Overheads,
    PublicUtilities,
    Education,
    Phone;

    public static ReceiptType fromInteger(int value) {
        switch (value) {
            case (1):
                return PublicUtilities;
            case (2):
                return Education;
            case (3):
                return Phone;
            default:
                return Overheads;
        }
    }

    public static int toInteger(ReceiptType type) {
        if (type == PublicUtilities) return 1;
        if (type == Education) return 2;
        if (type == Phone) return 3;
        return 0;
    }
}
