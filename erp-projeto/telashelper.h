#ifndef TELASHELPER_H
#define TELASHELPER_H

#include <QApplication>
#include <QWidget>

//Abre a tela T do ERP como janela unica:
//se ela ja estiver aberta, apenas traz para frente em vez de abrir outra copia.
template <class T>
inline void abrirTela()
{
    const char *nomeClasse = T::staticMetaObject.className();
    for (QWidget *janela : QApplication::topLevelWidgets()) {
        if (janela->inherits(nomeClasse)) {
            janela->show();
            janela->raise();
            janela->activateWindow();
            return;
        }
    }
    T *tela = new T();
    tela->show();
}

//Dados do usuario logado (preenchido no login; usado nos logs e nas movimentacoes)
struct Sessao {
    int id = 0;
    QString nome;
    int nivel = 0;
};

inline Sessao &sessao()
{
    static Sessao s;
    return s;
}

#endif // TELASHELPER_H
