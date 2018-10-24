/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * This database initialization file is modeled after the Qt Company's Book
 * demonstration application.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef INITDB_H
#define INITDB_H

#include <QtSql>

constexpr auto DB_TYPE = "QSQLITE";
constexpr auto DB_NAME = "petshop.db";

QVariant add_product(QSqlQuery& q, int id, const QString& desc, double price)
{
    q.addBindValue(id);
    q.addBindValue(desc);
    q.addBindValue(price);
    q.exec();
    return q.lastInsertId();
}

void add_inventory(QSqlQuery& q, const QVariant& product_id, int qty)
{
    q.addBindValue(product_id);
    q.addBindValue(qty);
    q.exec();
}

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(DB_TYPE);
    db.setDatabaseName(DB_NAME);

    if (!db.open())
        return db.lastError();

    // early exit if tables exist already
    QStringList tables = db.tables();
    if (tables.contains("products", Qt::CaseInsensitive) &&
            tables.contains("inventory", Qt::CaseInsensitive))
        return QSqlError();

    // initialize tables
    QSqlQuery q;
    if (!q.exec(QLatin1String("CREATE TABLE products("
                              "id INTEGER PRIMARY KEY,"
                              "desc TEXT,"
                              "price REAL);")))
        return q.lastError();
    if (!q.exec(QLatin1String("CREATE TABLE inventory("
                              "id INTEGER PRIMARY KEY,"
                              "qty INTEGER DEFAULT 0);")))
        return q.lastError();

    // populate tables with initial data
    if (!q.prepare(QLatin1String("INSERT INTO products(id, desc, price)"
                                 "VALUES (?, ?, ?);")))
        return q.lastError();
    QVariant tuna_id   = add_product(q, 1001, QLatin1String("Cat 12x156g Tuna Food"), 5.99);
    QVariant kibble_id = add_product(q, 1002, QLatin1String("Cat 1.5kg Kibble Food"), 12.99);
    QVariant treat_id  = add_product(q, 1003, QLatin1String("Cat 150g Cheezy Treats"), 3.29);

    if (!q.prepare(QLatin1String("INSERT INTO inventory(id, qty)"
                                 "VALUES (?, ?);")))
        return q.lastError();
    add_inventory(q, tuna_id, 20);
    add_inventory(q, kibble_id, 20);
    add_inventory(q, treat_id, 50);

    return QSqlError();
}

#endif // INITDB_H
