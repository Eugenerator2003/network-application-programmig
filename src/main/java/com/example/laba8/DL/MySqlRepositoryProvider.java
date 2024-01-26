package com.example.laba8.DL;

import com.example.laba8.DL.Models.Receipt;
import com.example.laba8.DL.Persistance.MySqlReceiptRepository;
import com.example.laba8.DL.Persistance.MySqlReceiptSqlGenerator;
import com.example.laba8.DL.Persistance.Repository;

import java.lang.reflect.InvocationTargetException;
import java.sql.Connection;
import java.sql.SQLException;

public class MySqlRepositoryProvider implements RepositoryProvider {
    public static Repository<Receipt> getReceiptRepository()
            throws ClassNotFoundException, InvocationTargetException,
            NoSuchMethodException, InstantiationException, IllegalAccessException, SQLException {
        if (repository == null) {
            DriverProvider.setDriver("com.mysql.jdbc.Driver");
            Connection connection = DriverProvider.getConnection("jdbc:mysql://localhost:3306/laba8",
                                "root",
                             "zalupa228");
            repository = new MySqlReceiptRepository(connection, new MySqlReceiptSqlGenerator());
        }
        return repository;
    }

    private static Repository<Receipt> repository;
}
