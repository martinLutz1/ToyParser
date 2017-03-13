#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QClipboard>

#define SPACE_BETWEEN_TEXTEDITS 5
#define BUTTON_SPACE 50
#define SPACE_BETWEEN_BUTTON_AND_TEXTEDIT 20

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->parseButton, SIGNAL(clicked()), this, SLOT(onParseButtonClick()));
    QObject::connect(ui->reprTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onReprComboBoxChanged(int)));
    QObject::connect(ui->copyToClipboardButton, SIGNAL(clicked()), this, SLOT(onCopyToClipboardButtonClick()));

    msgBox.setWindowTitle("Parserfehler");
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Ok);
    msgBox.setText("Beim Parsen ist ein Fehler aufgetreten");
    QFont* msgFont = new QFont(". SF NS Text", 13, 0, false);
    msgBox.setFont(*msgFont);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    int windowHeight = ui->centralWidget->height();
    int windowWidth = ui->centralWidget->width();

    int textEditWidth = (windowWidth - SPACE_BETWEEN_TEXTEDITS) / 2;
    int textEditHeight = windowHeight - BUTTON_SPACE;


    QSize* textEditSize = new QSize(textEditWidth, textEditHeight);
    QSize* groupBoxSize = new QSize(textEditWidth, windowHeight);
    QPoint* rightGroupBoxPos = new QPoint(textEditWidth + SPACE_BETWEEN_TEXTEDITS, ui->groupBox_2->pos().ry());
    QPoint* buttonParsePos = new QPoint(ui->parseButton->pos().rx(), textEditHeight + SPACE_BETWEEN_BUTTON_AND_TEXTEDIT);
    QPoint* comboBoxReprTypePos = new QPoint(ui->reprTypeComboBox->pos().rx(), textEditHeight + SPACE_BETWEEN_BUTTON_AND_TEXTEDIT + 2);
    QPoint* buttonCopyToClipboard = new QPoint(ui->copyToClipboardButton->pos().rx(), textEditHeight + SPACE_BETWEEN_BUTTON_AND_TEXTEDIT);

    ui->parseButton->move(*buttonParsePos);
    ui->reprTypeComboBox->move(*comboBoxReprTypePos);
    ui->copyToClipboardButton->move(*buttonCopyToClipboard);
    ui->groupBox_2->move(*rightGroupBoxPos);
    ui->groupBox->setFixedSize(*groupBoxSize);
    ui->groupBox_2->setFixedSize(*groupBoxSize);
    ui->outputText->setFixedSize(*textEditSize);
    ui->sourceCodeText->setFixedSize(*textEditSize);
}

void MainWindow::onParseButtonClick()
{
    QString toyCode = ui->sourceCodeText->toPlainText();

    Message msg;
    msg.type = msg.view_parse_text;
    msg.data[0].stringPointer = &toyCode;
    emit(sendMessage(msg));
}

void MainWindow::onReprComboBoxChanged(int index)
{
    Message msg;
    msg.type = Message::view_representation_type_changed;
    msg.data[0].i = index;
    emit(sendMessage(msg));
}

void MainWindow::onCopyToClipboardButtonClick()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString text = ui->outputText->toPlainText();
    clipboard->setText(text);
}

void MainWindow::notify(Message &msg)
{
    switch (msg.type)
    {
    case Message::parser_output:
    {
        QString* outputText = msg.data[0].stringPointer;
        ui->outputText->setText(*outputText);
        break;
    }
    case Message::parser_parser_error_message:
    {
        QString* errorMsg = msg.data[0].stringPointer;
        msgBox.setDetailedText(*errorMsg);
        msgBox.show();
    }
    default:
        break;
    }
}
