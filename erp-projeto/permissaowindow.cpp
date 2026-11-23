#include "permissaowindow.h"
#include "ui_permissaowindow.h"
#include "loghelper.h"
#include <QtSql>
#include <QMessageBox>

PermissaoWindow::PermissaoWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PermissaoWindow)
{
    ui->setupUi(this);

    //ao terminar de digitar o nome (ou sair do campo), carrega as permissoes
    connect(ui->lineEdit, &QLineEdit::editingFinished,
            this, &PermissaoWindow::carregarPermissoes);
}

PermissaoWindow::~PermissaoWindow()
{
    delete ui;
}

//nome digitado -> id do usuario (0 quando vazio ou nao encontrado)
int PermissaoWindow::resolverUsuario()
{
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty())
        return 0;
    QSqlQuery q;
    q.prepare("SELECT id_usuario FROM tb_usuarios WHERE nome LIKE ? "
              "ORDER BY nome LIMIT 1");
    q.addBindValue("%" + nome + "%");
    if (q.exec() && q.next())
        return q.value(0).toInt();
    return 0;
}

//limpa a tela
void PermissaoWindow::limparTudo()
{
    ui->lineEdit->clear();
    ui->comboBox->setCurrentIndex(0);
    for (QCheckBox *cb : findChildren<QCheckBox *>())
        cb->setChecked(false);
}

//carrega o nivel (cargo) e as permissoes do funcionario digitado
void PermissaoWindow::carregarPermissoes()
{
    int idUsuario = resolverUsuario();
    if (idUsuario == 0) {
        if (!ui->lineEdit->text().trimmed().isEmpty())
            QMessageBox::information(this, "Permissões",
                                     "Usuário não encontrado:\n" +
                                     ui->lineEdit->text().trimmed());
        return;
    }

    //cargo/nivel do usuario
    QSqlQuery u;
    u.prepare("SELECT nome, nivel FROM tb_usuarios WHERE id_usuario=?");
    u.addBindValue(idUsuario);
    if (u.exec() && u.next()) {
        ui->lineEdit->setText(u.value(0).toString());   //mostra o nome completo
        int nivel = u.value(1).toInt();                 //1=Admin 2=Gerente 3=Operador
        ui->comboBox->setCurrentIndex(nivel >= 1 && nivel <= 3 ? nivel - 1 : 2);
    }

    //permissoes marcadas
    for (QCheckBox *cb : findChildren<QCheckBox *>())
        cb->setChecked(false);

    QSqlQuery p;
    p.prepare("SELECT pe.nome FROM tb_usuario_permissao up "
              "JOIN tb_permissoes pe ON pe.id_permissao = up.id_permissao "
              "WHERE up.id_usuario=?");
    p.addBindValue(idUsuario);
    if (p.exec()) {
        while (p.next()) {
            QString nomePermissao = p.value(0).toString();
            for (QCheckBox *cb : findChildren<QCheckBox *>())
                if (cb->text() == nomePermissao)
                    cb->setChecked(true);
        }
    }
}

//salva o cargo e as permissoes do funcionario
void PermissaoWindow::on_pushButton_clicked()
{
    int idUsuario = resolverUsuario();
    if (idUsuario == 0) {
        QMessageBox::warning(this, "Permissões",
                             "Digite o nome de um usuário cadastrado.\n"
                             "(Cadastre-o em Configurações > Permissões antes)");
        return;
    }

    int nivel = ui->comboBox->currentIndex() + 1;   //1=Admin 2=Gerente 3=Operador

    //grava o cargo
    QSqlQuery nivelQ;
    nivelQ.prepare("UPDATE tb_usuarios SET nivel=? WHERE id_usuario=?");
    nivelQ.addBindValue(nivel);
    nivelQ.addBindValue(idUsuario);
    if (!nivelQ.exec()) {
        QMessageBox::warning(this, "Permissões",
                             "Erro ao salvar o cargo:\n" + nivelQ.lastError().text());
        return;
    }

    //refaz as permissoes do usuario
    QSqlQuery del;
    del.prepare("DELETE FROM tb_usuario_permissao WHERE id_usuario=?");
    del.addBindValue(idUsuario);
    if (!del.exec()) {
        QMessageBox::warning(this, "Permissões",
                             "Erro ao limpar permissões:\n" + del.lastError().text());
        return;
    }

    int total = 0;
    for (QCheckBox *cb : findChildren<QCheckBox *>()) {
        if (!cb->isChecked())
            continue;
        QSqlQuery pe;
        pe.prepare("SELECT id_permissao FROM tb_permissoes WHERE nome=?");
        pe.addBindValue(cb->text());
        if (!pe.exec() || !pe.next())
            continue;
        QSqlQuery ins;
        ins.prepare("INSERT INTO tb_usuario_permissao (id_usuario, id_permissao) "
                    "VALUES (?,?)");
        ins.addBindValue(idUsuario);
        ins.addBindValue(pe.value(0).toInt());
        if (ins.exec())
            ++total;
    }

    registrarLog("Permissões", "Configurações",
                 "usuario id " + QString::number(idUsuario) +
                 " nivel " + QString::number(nivel) +
                 " permissoes: " + QString::number(total));
    QMessageBox::information(this, "Permissões",
                             "Permissões salvas com sucesso!\n"
                             "Cargo: " + ui->comboBox->currentText() +
                             "\nMódulos liberados: " + QString::number(total));
}

//cancelar: volta ao estado vazio
void PermissaoWindow::on_pushButton_2_clicked()
{
    limparTudo();
}
