package com.example.laba8.DL.Persistance;

public interface Repository<T> {
    T[] getAll();
    T get(long id);
    long create(T entity);
    void update(T entity);
    void remove(long id);
    long getNextId();
}
