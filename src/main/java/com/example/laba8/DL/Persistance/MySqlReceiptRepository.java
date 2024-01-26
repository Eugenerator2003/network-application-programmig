package com.example.laba8.DL.Persistance;

import com.example.laba8.DL.Models.Receipt;
import com.example.laba8.DL.Models.ReceiptType;

import java.sql.*;
import java.util.ArrayList;

public class MySqlReceiptRepository implements Repository<Receipt> {

    private ReceiptSqlGenerator sqlGenerator;
    private Connection connection;

    public MySqlReceiptRepository(Connection connection, ReceiptSqlGenerator sqlGenerator) {
        this.connection = connection;
        this.sqlGenerator = sqlGenerator;
    }

    @Override
    public Receipt[] getAll() {
        try {
            String sql = sqlGenerator.all();
            Statement statement = connection.createStatement();
            ArrayList<Receipt> list = new ArrayList<>();
            ResultSet set = statement.executeQuery(sql);
            while (set.next()) {
                Receipt r = readFromResultSet(set);
                list.add(r);
            }
            return list.toArray(new Receipt[0]);
        }
        catch (SQLException ex) {
            return new Receipt[0];
        }
    }

    @Override
    public Receipt get(long id) {
        try{
            String sql = sqlGenerator.read(id);
            Statement statement = connection.createStatement();
            ResultSet set = statement.executeQuery(sql);
            if (set.next()) {
                return readFromResultSet(set);
            }
            else {
                return null;
            }
        }
        catch (SQLException ex) {
            return null;
        }
    }

    @Override
    public long create(Receipt entity) {
        try {
            Long id = getNextId();
            entity.setId(id);
            String sql = sqlGenerator.create(entity);
            Statement statement = connection.createStatement();
            statement.executeUpdate(sql);
            return id;
        }
        catch (SQLException ex) {
            return -1;
        }
    }

    @Override
    public void update(Receipt entity) {
        try {
            String sql = sqlGenerator.update(entity);
            Statement statement = connection.createStatement();
            statement.executeUpdate(sql);
        }
        catch (SQLException ex) {

        }
    }

    @Override
    public void remove(long id) {
        try {
            String sql = sqlGenerator.delete(id);
            Statement statement = connection.createStatement();
            statement.executeUpdate(sql);
        }
        catch (SQLException ex) {

        }
    }

    @Override
    public long getNextId() {
        try{
            String sql = sqlGenerator.nextId();
            Statement statement = connection.createStatement();
            ResultSet set = statement.executeQuery(sql);
            if (set.next()) {
                int id = set.getInt(1);
                return ++id;
            }
            else {
                return 1;
            }
        }
        catch (SQLException ex) {
            return -1;
        }
    }

    protected Receipt readFromResultSet(ResultSet set) throws SQLException {
        Receipt r = new Receipt();
        r.setId(set.getLong("id"));
        r.setSum(set.getDouble("sum"));
        r.setDate(set.getDate("date").toLocalDate());
        r.setReceiptType(ReceiptType.fromInteger(set.getInt("type")));
        return r;
    }
}
