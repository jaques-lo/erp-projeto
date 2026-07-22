QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    caixawindow.cpp \
    categoriawindow.cpp \
    clientewindow.cpp \
    comprawindow.cpp \
    configwindow.cpp \
    contaspagarwindow.cpp \
    contasreceberwindow.cpp \
    estoquewindow.cpp \
    financeirowindow.cpp \
    fornecedorwindow.cpp \
    funcionariowindow.cpp \
    loginwindow.cpp \
    logwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    movimentacaoestoquewindow.cpp \
    permissaowindow.cpp \
    produtowindow.cpp \
    relatoriowindow.cpp \
    sobrewindow.cpp \
    vendawindow.cpp

HEADERS += \
    caixawindow.h \
    categoriawindow.h \
    clientewindow.h \
    comprawindow.h \
    configwindow.h \
    contaspagarwindow.h \
    contasreceberwindow.h \
    estoquewindow.h \
    financeirowindow.h \
    fornecedorwindow.h \
    funcionariowindow.h \
    loginwindow.h \
    logwindow.h \
    mainwindow.h \
    movimentacaoestoquewindow.h \
    permissaowindow.h \
    produtowindow.h \
    relatoriowindow.h \
    sobrewindow.h \
    vendawindow.h

FORMS += \
    caixawindow.ui \
    categoriawindow.ui \
    clientewindow.ui \
    comprawindow.ui \
    configwindow.ui \
    contaspagarwindow.ui \
    contasreceberwindow.ui \
    estoquewindow.ui \
    financeirowindow.ui \
    fornecedorwindow.ui \
    funcionariowindow.ui \
    loginwindow.ui \
    logwindow.ui \
    mainwindow.ui \
    movimentacaoestoquewindow.ui \
    permissaowindow.ui \
    produtowindow.ui \
    relatoriowindow.ui \
    sobrewindow.ui \
    vendawindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ArquivosDeRecursos.qrc
