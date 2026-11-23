#ifndef BANCO_H
#define BANCO_H

#include <QtSql>

//Garante que a coluna existe na tabela (adiciona se faltar em bancos antigos)
inline void garantirColuna(QSqlDatabase &db, const QString &tabela,
                           const QString &coluna, const QString &tipo)
{
    QSqlQuery q(db);
    if (!q.exec(QString("PRAGMA table_info(%1)").arg(tabela)))
        return;
    bool existe = false;
    while (q.next()) {
        if (q.value(1).toString().compare(coluna, Qt::CaseInsensitive) == 0) {
            existe = true;
            break;
        }
    }
    if (!existe)
        q.exec(QString("ALTER TABLE %1 ADD COLUMN %2 %3").arg(tabela, coluna, tipo));
}

//Cria todas as tabelas do ERP se nao existirem (roda toda vez que abre - e idempotente).
//Assim qualquer banco (novo, antigo ou vazio) fica completo.
inline void criarTabelasBanco(QSqlDatabase &db)
{
    QSqlQuery q(db);

    //--------- cadastros ---------
    q.exec("CREATE TABLE IF NOT EXISTS tb_usuarios ("
           "id_usuario INTEGER PRIMARY KEY AUTOINCREMENT,"
           "nome TEXT, login TEXT, senha TEXT,"
           "ativo INTEGER DEFAULT 1, nivel INTEGER DEFAULT 3, ultimo_login TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_funcionarios ("
           "id_funcionario INTEGER PRIMARY KEY AUTOINCREMENT,"
           "nome TEXT, cpf TEXT, cargo TEXT, telefone TEXT, email TEXT,"
           "salario REAL, data_adimissao TEXT, ativo INTEGER DEFAULT 1)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_empresa ("
           "id_empresa INTEGER PRIMARY KEY AUTOINCREMENT,"
           "razao_social TEXT, nome_fantasia TEXT, cnpj TEXT, telefone TEXT,"
           "email TEXT, endereco TEXT, cidade TEXT, estado TEXT, cep TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_clientes ("
           "id_cliente INTEGER PRIMARY KEY AUTOINCREMENT,"
           "nome TEXT, cpf_cnpj TEXT, telefone TEXT, celular TEXT, email TEXT,"
           "endereco TEXT, numero TEXT, bairro TEXT, cidade TEXT, estado TEXT,"
           "cep TEXT, data_cadastro TEXT, ativo INTEGER DEFAULT 1)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_fornecedores ("
           "id_fornecedor INTEGER PRIMARY KEY AUTOINCREMENT,"
           "nome TEXT, cnpj TEXT, telefone TEXT, celular TEXT, email TEXT,"
           "endereco TEXT, bairro TEXT, cidade TEXT, estado TEXT, cep TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_categorias ("
           "id_categoria INTEGER PRIMARY KEY AUTOINCREMENT,"
           "nome TEXT, descricao TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_produtos ("
           "id_produtos INTEGER PRIMARY KEY AUTOINCREMENT,"
           "codigo TEXT, descricao TEXT, id_categoria INTEGER, marca TEXT,"
           "modelo TEXT, custo REAL, preco REAL, estoque_minimo INTEGER,"
           "unidade TEXT, codigo_barras TEXT, ativo INTEGER DEFAULT 1)");

    //--------- estoque ---------
    q.exec("CREATE TABLE IF NOT EXISTS tb_estoque ("
           "id_estoque INTEGER PRIMARY KEY AUTOINCREMENT,"
           "id_produto INTEGER, quantidade INTEGER, localizacao TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_movimentacao_estoque ("
           "id_mov INTEGER PRIMARY KEY AUTOINCREMENT,"
           "id_produto INTEGER, tipo TEXT, quantidade INTEGER,"
           "data TEXT, usuario TEXT, observacao TEXT)");

    //--------- compras e vendas ---------
    q.exec("CREATE TABLE IF NOT EXISTS tb_compras ("
           "id_compra INTEGER PRIMARY KEY AUTOINCREMENT,"
           "id_fornecedor INTEGER, id_usuario INTEGER, data TEXT,"
           "valor_total REAL, status TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_itens_compra ("
           "id_item INTEGER PRIMARY KEY AUTOINCREMENT,"
           "id_compra INTEGER, id_produto INTEGER, quantidade INTEGER,"
           "valor_unitario REAL, subtotal REAL)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_vendas ("
           "id_venda INTEGER PRIMARY KEY AUTOINCREMENT,"
           "id_cliente INTEGER, id_usuario INTEGER, data TEXT,"
           "subtotal REAL, desconto REAL, total REAL,"
           "forma_pagamento TEXT, status TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_itens_venda ("
           "id_item INTEGER PRIMARY KEY AUTOINCREMENT,"
           "id_venda INTEGER, id_produto INTEGER, quantidade INTEGER,"
           "valor_unitario REAL, subtotal REAL)");

    //--------- financeiro ---------
    q.exec("CREATE TABLE IF NOT EXISTS tb_caixa ("
           "id_caixa INTEGER PRIMARY KEY AUTOINCREMENT,"
           "tipo TEXT, descricao TEXT, valor REAL, data TEXT, usuario TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_receber ("
           "id_receber INTEGER PRIMARY KEY AUTOINCREMENT,"
           "id_cliente INTEGER, categoria TEXT, valor REAL,"
           "data_emissao TEXT, data_vencimento TEXT, data_pagamento TEXT,"
           "status TEXT DEFAULT 'pendente', forma_pagamento TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_pagar ("
           "id_pagar INTEGER PRIMARY KEY AUTOINCREMENT,"
           "id_fornecedor INTEGER, categoria TEXT, valor REAL,"
           "data_emissao TEXT, data_vencimento TEXT, data_pagamento TEXT,"
           "status TEXT DEFAULT 'pendente', forma_pagamento TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_financeiro ("
           "id_financeiro INTEGER PRIMARY KEY AUTOINCREMENT,"
           "tipo TEXT, origem TEXT, descricao TEXT, valor REAL, data TEXT)");

    //--------- sistema ---------
    q.exec("CREATE TABLE IF NOT EXISTS tb_permissoes ("
           "id_permissao INTEGER PRIMARY KEY AUTOINCREMENT, nome TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_usuario_permissao ("
           "id_usuario INTEGER, id_permissao INTEGER,"
           "PRIMARY KEY (id_usuario, id_permissao))");

    q.exec("CREATE TABLE IF NOT EXISTS tb_logs ("
           "id_log INTEGER PRIMARY KEY AUTOINCREMENT,"
           "usuario TEXT, acao TEXT, modulo TEXT, detalhes TEXT,"
           "data TEXT, ip TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS tb_config ("
           "chave TEXT PRIMARY KEY, valor TEXT)");

    //colunas que faltavam em bancos antigos (as telas precisam delas)
    garantirColuna(db, "tb_clientes", "celular", "TEXT");
    garantirColuna(db, "tb_fornecedores", "email", "TEXT");
    garantirColuna(db, "tb_fornecedores", "bairro", "TEXT");
    garantirColuna(db, "tb_fornecedores", "celular", "TEXT");
    garantirColuna(db, "tb_fornecedores", "cep", "TEXT");
    garantirColuna(db, "tb_compras", "nota_fiscal", "TEXT");

    //usuario padrao: se o banco nao tiver ninguem, cria o administrador
    {
        QSqlQuery u(db);
        u.exec("SELECT COUNT(*) FROM tb_usuarios");
        if (u.next() && u.value(0).toInt() == 0)
            u.exec("INSERT INTO tb_usuarios (nome, login, senha, ativo, nivel) "
                   "VALUES ('Administrador', 'admin', 'admin', 1, 1)");
    }

    //permissoes padrao (os 7 modulos da tela de permissoes)
    {
        QSqlQuery p(db);
        p.exec("SELECT COUNT(*) FROM tb_permissoes");
        if (p.next() && p.value(0).toInt() == 0) {
            const QStringList nomes = {"Vendas", "Compras", "Estoque", "Financeiro",
                                       "Cadastros", "Relatórios", "Configurações"};
            for (const QString &n : nomes) {
                QSqlQuery i(db);
                i.prepare("INSERT INTO tb_permissoes (nome) VALUES (?)");
                i.addBindValue(n);
                i.exec();
            }
        }
    }
}

#endif // BANCO_H
