#include "sqlite.h"
#include <QSqlRecord>
#include <QVariant>
#include <QDir>

#include <QFile>
#include <QTextStream>
#include <string>

Sqlite::Sqlite()
{
}

Sqlite::~Sqlite()
{
    final();
}

bool Sqlite::init(const QString& dbName, const QString& userName, const QString& password)
{
#if 0
    QFile file("C:/wd_filename.txt");
    QString dbNameStr(dbName);
    QString userNameStr(userName);
    QString passwordStr(password);
    if (file.open(QIODevice::Append|QIODevice::Text)) {
        QTextStream in(&file);
        in<< dbName << " " << userName << " " << password <<"\n";
        in<< dbNameStr << " " << userNameStr << " " << passwordStr <<"\n";
        in << "\n";
        file.close();
    }
#endif

    final();

    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(dbName);
        m_db.setUserName(userName);
        m_db.setPassword(password);
    }

    return m_db.open();
}

void Sqlite::final()
{
    m_db.close();
}

bool Sqlite::createStategyTable()
{
    QSqlQuery sqlQuery(m_db);
    QString sqlStr = "CREATE TABLE `t_strategy` ( \
            `id`	INTEGER PRIMARY KEY AUTOINCREMENT, \
            `strategy`	TEXT, \
            `caption`	TEXT, \
            `period`	INTEGER, \
            `setting`	TEXT, \
            `symbol`	TEXT, \
            `status`	TEXT, \
            `trade_status`	TEXT \
        );";
    return sqlQuery.exec(sqlStr);
}

bool Sqlite::insertIntoStategy(const StrategyData& param) {
    QSqlQuery sqlQuery(m_db);
    QString sqlStr = "INSERT INTO `t_strategy` \
            (`strategy`, `caption`, `period`, `setting`, `symbol`, `status`, `trade_status`) \
            values (?, ?, ?, ?, ?, ?, ?)";
    sqlQuery.prepare(sqlStr);
    sqlQuery.addBindValue(param.strategy);
    sqlQuery.addBindValue(param.caption);
    sqlQuery.addBindValue(param.period);
    sqlQuery.addBindValue(param.setting);
    sqlQuery.addBindValue(param.symbol);
    sqlQuery.addBindValue(param.status);
    sqlQuery.addBindValue(param.trade_status);
    return sqlQuery.exec();
}

bool Sqlite::updateStategy(const StrategyData& param) {
    QSqlQuery sqlQuery(m_db);
    QString sqlStr = "UPDATE `t_strategy` \
            SET `strategy`= :strategy, `caption`= :caption, `period`= :period, \
            `setting`= :setting, `symbol` = :symbol, `status` = :status, `trade_status = :trade_status`) \
            WHERE `id` = :id";
    sqlQuery.prepare(sqlStr);
    sqlQuery.bindValue(":strategy", param.strategy);
    sqlQuery.bindValue(":caption", param.caption);
    sqlQuery.bindValue(":period", param.period);
    sqlQuery.bindValue(":setting", param.setting);
    sqlQuery.bindValue(":symbol", param.symbol);
    sqlQuery.bindValue(":status", param.status);
    sqlQuery.bindValue(":trade_status", param.trade_status);
    sqlQuery.bindValue(":id", param.id);
    return sqlQuery.exec();
}

bool Sqlite::syncLocalStrategyList()
{
    QList<StrategyData> strategies;
    QSqlQuery sqlQuery(m_db);
    QString sqlStr = "SELECT `id`, `strategy`, `caption`, `period`, `setting`, `symbol`, `status`, `trade_status` \
                    FROM `t_strategy`";
    if(!sqlQuery.exec(sqlStr)) {
        return false;
    }
    while(sqlQuery.next()) {
        StrategyData strategy;
        strategy.id = sqlQuery.value(0).toUInt();
        strategy.strategy = sqlQuery.value(1).toString();
        strategy.caption = sqlQuery.value(2).toString();
        strategy.period = sqlQuery.value(3).toInt();
        strategy.setting = sqlQuery.value(4).toString();
        strategy.symbol = sqlQuery.value(5).toString();
        strategy.status = sqlQuery.value(6).toString();
        strategy.trade_status = sqlQuery.value(7).toString();

        strategies.append(strategy);
    }

    m_strategyList = strategies;
    return true;
}

const QList<StrategyData> Sqlite::getStrategyList() const {
    return m_strategyList;
}

QSqlError Sqlite::lastError() const
{
    return m_db.lastError();
}

extern "C" {
SQLITESHARED_EXPORT int initSqlite(const char* dbName, const char* userName, const char* password) {
    Sqlite* inst = Sqlite::getInstance();
    bool ret = inst->init(dbName, userName, password);
    return ret?0:-1;
}

SQLITESHARED_EXPORT int closeSqlite() {
    Sqlite* inst = Sqlite::getInstance();
    inst->final();
    return 0;
}

SQLITESHARED_EXPORT int createStrategyTable() {
    Sqlite* inst = Sqlite::getInstance();
    bool ret = inst->createStategyTable();
    return ret?0:-1;
}

SQLITESHARED_EXPORT int getLastErrorString(char* pstr) {
    QString str = Sqlite::getInstance()->lastError().text();
    size_t length = str.length()+1;
    memcpy(pstr, str.toLatin1().data(), length);
    return 0;
}

SQLITESHARED_EXPORT int currentPath(char* ppath) {
    QString path = QDir::currentPath();
    size_t lenth = path.length()+1;
    memcpy(ppath, path.toLatin1().data(), lenth);
    return 0;
}

SQLITESHARED_EXPORT int supportDrivers(char* pdrivers) {
    QStringList drivers = QSqlDatabase::drivers();
    QString driverJoins = drivers.join(",");
    size_t length = driverJoins.length() + 1;
    memcpy(pdrivers, driverJoins.toLatin1().data(), length);
    return 0;
}

SQLITESHARED_EXPORT int insertIntoStragegy(const char* name,
                            const char* caption, int period, const char* setting,
                            const char* symbol, const char* status, const char* trade_status) {
    Sqlite* inst = Sqlite::getInstance();
    StrategyData stragegy;
    stragegy.strategy = QString(name);
    stragegy.caption = QString(caption);
    stragegy.period = period;
    stragegy.setting = QString(setting);
    stragegy.symbol = QString(symbol);
    stragegy.status = QString(status);
    stragegy.trade_status = QString(trade_status);
    return inst->insertIntoStategy(stragegy)?0:-1;
}

SQLITESHARED_EXPORT int updateStragegy(uint id, const char* name,
                            const char* caption, int period, const char* setting,
                            const char* symbol, const char* status, const char* trade_status) {
    Sqlite* inst = Sqlite::getInstance();
    StrategyData stragegy;
    stragegy.id = id;
    stragegy.strategy = QString(name);
    stragegy.caption = QString(caption);
    stragegy.period = period;
    stragegy.setting = QString(setting);
    stragegy.symbol = QString(symbol);
    stragegy.status = QString(status);
    stragegy.trade_status = QString(trade_status);
    return inst->updateStategy(stragegy)?0:-1;
}

SQLITESHARED_EXPORT int syncLocalStrategyList() {
    Sqlite* inst = Sqlite::getInstance();
    bool ret = inst->syncLocalStrategyList();
    return ret?0:-1;
}

SQLITESHARED_EXPORT int getStragegyLength(int *plen) {
    Sqlite* inst = Sqlite::getInstance();
    QList<StrategyData> strategies = inst->getStrategyList();
    int lenth = strategies.length();
    *plen = lenth;
    return 0;
}

SQLITESHARED_EXPORT int getStragegyItem(int row, uint* id, char* name,
                            char* caption, int* period, char* setting,
                            char* symbol, char* status, char* trade_status) {
    Sqlite* inst = Sqlite::getInstance();
    QList<StrategyData> strategies = inst->getStrategyList();
    if (row >= strategies.length() || row < 0) {
        return -1;
    }

    StrategyData strategy = strategies[row];
    *id = strategy.id;
    memcpy(name, strategy.strategy.toLatin1().data(), strategy.strategy.length()+1);
    memcpy(caption, strategy.caption.toLatin1().data(), strategy.caption.length()+1);
    *period = strategy.period;
    memcpy(setting, strategy.setting.toLatin1().data(), strategy.setting.length()+1);
    memcpy(symbol, strategy.symbol.toLatin1().data(), strategy.symbol.length()+1);
    memcpy(status, strategy.status.toLatin1().data(), strategy.status.length()+1);
    memcpy(trade_status, strategy.trade_status.toLatin1().data(), strategy.trade_status.length()+1);

    return 0;
}

SQLITESHARED_EXPORT int openTestFile(const char* filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
        return -1;
    }

    QTextStream out(&file);
    out << QString("log11111") << endl;
    file.close();
    return 0;
}

}

