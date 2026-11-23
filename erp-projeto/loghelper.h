#ifndef LOGHELPER_H
#define LOGHELPER_H

#include <QtSql>
#include <QDateTime>
#include "telashelper.h"

//Registra uma acao do usuario na tela de Logs (auditoria do sistema)
inline void registrarLog(const QString &acao, const QString &modulo,
                         const QString &detalhes)
{
    QSqlQuery q;
    q.prepare("INSERT INTO tb_logs (usuario, acao, modulo, detalhes, data, ip) "
              "VALUES (?, ?, ?, ?, ?, ?)");
    q.addBindValue(sessao().nome.isEmpty() ? QString("-") : sessao().nome);
    q.addBindValue(acao);
    q.addBindValue(modulo);
    q.addBindValue(detalhes);
    q.addBindValue(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    q.addBindValue(QString("-"));
    q.exec();
}

#endif // LOGHELPER_H
