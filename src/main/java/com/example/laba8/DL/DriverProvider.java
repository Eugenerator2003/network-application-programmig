package com.example.laba8.DL;

import java.lang.reflect.InvocationTargetException;
import java.sql.Connection;
import java.sql.Driver;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.HashMap;

public class DriverProvider {
    public static void setDriver(String className) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, InstantiationException, IllegalAccessException {
        if (!driverHashMap.containsKey(className)) {
            Driver driver = (Driver) Class.forName(className).getDeclaredConstructor().newInstance();
            driverHashMap.put(className, driver);
        }
    }

    public static Connection getConnection(String url, String loginOpt, String passwordOpt) throws SQLException {
        if (!connectionHashMap.containsKey(url)) {
            Connection connection = DriverManager.getConnection(url, loginOpt, passwordOpt);
            connectionHashMap.put(url, connection);
        }
        return connectionHashMap.get(url);
    }

    private static HashMap<String, Driver> driverHashMap = new HashMap<>();
    private static HashMap<String, Connection> connectionHashMap = new HashMap<>();
}
