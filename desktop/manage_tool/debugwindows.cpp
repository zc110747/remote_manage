#include "debugwindows.h"
#include "ui_debugwindows.h"

debugWindows::debugWindows(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::debugWindows)
{
    ui->setupUi(this);
}

debugWindows::~debugWindows()
{
    delete ui;
}
