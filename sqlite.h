#ifndef SQLITE_H
#define SQLITE_H

#include "sqlite_global.h"
// sql
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QList>

typedef struct {
    uint    id;
    QString strategy;
    QString caption;
    int     period;
    QString setting;
    QString symbol;
    QString status;
    QString trade_status;
} StrategyData, *P2StrategyData;

class SQLITESHARED_EXPORT Sqlite
{

public:
    static Sqlite* getInstance() {
        static Sqlite inst;
        return &inst;
    }
    bool init(const QString& dbName, const QString& userName, const QString& password);
    void final();

    bool createStategyTable();

    bool insertIntoStategy(const StrategyData &param);
    bool updateStategy(const StrategyData &param);
    bool syncLocalStrategyList();
    const QList<StrategyData> getStrategyList() const;

    QSqlError lastError() const;
private:
    Sqlite();
    ~Sqlite();
    Sqlite(const Sqlite&);
    Sqlite operator=(const Sqlite&);
private:
    QSqlDatabase m_db;
    QList<StrategyData> m_strategyList;
};

#endif // SQLITE_H
