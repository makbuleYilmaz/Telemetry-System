#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QMouseEvent> // Sürükleme işlemi için kütüphane eklendi

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

protected:
    // Fare tıklama ve sürükleme olayları
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::LoginDialog *ui;
    QPoint dragPosition; // Pencerenin sürüklendiği konumu tutar
};

#endif // LOGINDIALOG_H