package com.example.laba8.DL.Persistance;

import com.example.laba8.DL.Models.Receipt;
import com.example.laba8.DL.Models.ReceiptType;
import jakarta.json.*;

import java.io.BufferedReader;
import java.io.StringReader;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

public class JsonSerializer {
    private static DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");
    public JsonObject serialize(Receipt[] receipts) {
        JsonArrayBuilder arrayBuilder = Json.createArrayBuilder();
        for (Receipt r:
             receipts) {
            arrayBuilder.add(serialize(r));
        }
        JsonObjectBuilder objectBuilder = Json.createObjectBuilder();
        objectBuilder.add("data", arrayBuilder);
        return objectBuilder.build();
    }


    public JsonObject serialize(Receipt receipt) {
        JsonObjectBuilder builder = Json.createObjectBuilder();
        if (receipt != null) {
            builder.add("id", receipt.getId());
            builder.add("type", receipt.getReceiptType().name());
            builder.add("date", receipt.getDate().format(formatter));
            builder.add("sum", receipt.getSum());
        }
        return builder.build();
    }

    public Receipt deserialize(String string) {
        JsonReader reader = Json.createReader(new StringReader(string));
        JsonObject data = reader.readObject();
        reader.close();
        Receipt receipt = new Receipt();
        receipt.setId(data.getInt("id"));
        receipt.setReceiptType(ReceiptType.valueOf(data.getString("type")));
        receipt.setDate(LocalDate.parse(data.getString("date"), formatter));
        receipt.setSum(Double.parseDouble(data.getString("sum")));
        return receipt;
    }
}
