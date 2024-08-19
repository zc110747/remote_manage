#ifndef DEBUGWINDOWS_H
#define DEBUGWINDOWS_H

#include <QWidget>

namespace Ui {
class debugWindows;
}

class debugWindows : public QWidget
{
    Q_OBJECT

public:
    explicit debugWindows(QWidget *parent = nullptr);
    ~debugWindows();

private:
    Ui::debugWindows *ui;
};

#endif // DEBUGWINDOWS_H
