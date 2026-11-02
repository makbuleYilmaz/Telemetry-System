#include "logindialog.h"
#include "ui_logindialog.h"
#include "databasemanager.h"

#include <QMessageBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRegularExpression>
#include <QMouseEvent>
#include <QLabel>
#include <QPixmap>
#include <QLineEdit>
#include <QFrame>


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);


    // ==========================================================
    // 1. PENCERE AYARLARI
    // ==========================================================

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    // Pencere boyutu
    resize(700, 600);

    setWindowTitle("Sistem Kimlik Doğrulaması");


    // ==========================================================
    // 2. MODERN / SİBER TEMA
    // ==========================================================

    QString modernTasarim = R"(

        /* =====================================================
           ANA PENCERE
           ===================================================== */

        QDialog {
            background-color: #0b1118;
        }


        /* =====================================================
           GİRİŞ KARTI
           ===================================================== */

        QFrame#frame {
            background-color: #161b22;
            border: 1px solid #30363d;
            border-radius: 12px;
        }


        /* =====================================================
           NORMAL YAZILAR
           ===================================================== */

        QLabel {
            color: #c9d1d9;
            font-size: 13px;
            font-weight: bold;
        }


        /* =====================================================
           HATA MESAJI
           ===================================================== */

        QLabel#lblStatus {
            color: #ff5252;
            font-size: 13px;
            font-weight: bold;
            background-color: transparent;
            border: none;
        }


        /* =====================================================
           TEXTBOX
           ===================================================== */

        QLineEdit {
            background-color: #0d1117;
            color: #58a6ff;
            border: 1px solid #30363d;
            border-radius: 6px;
            padding: 6px 12px;
            font-size: 13px;
            font-weight: bold;
        }

        QLineEdit:focus {
            border: 1px solid #58a6ff;
        }


        /* =====================================================
           NORMAL BUTONLAR
           ===================================================== */
QPushButton {
    background-color: #21262d;
    color: #c9d1d9;
    border: 1px solid #30363d;
    border-radius: 6px;
    padding: 2px 4px;
    font-size: 12px;
    font-weight: bold;
}

        QPushButton:hover {
            background-color: #30363d;
            border-color: #8b949e;
        }

        QPushButton:pressed {
            background-color: #1f6feb;
            color: white;
        }


        /* =====================================================
           GİRİŞ YAP BUTONU
           ===================================================== */

        QPushButton#btnLogin {
            background-color: #1f6feb;
            color: #ffffff;
            border: 1px solid #388bfd;
        }

        QPushButton#btnLogin:hover {
            background-color: #388bfd;
        }

    )";

    setStyleSheet(modernTasarim);


    // ==========================================================
    // 3. ŞİFRE KUTUSU
    // ==========================================================

    ui->txtPassword->setEchoMode(QLineEdit::Password);
    ui->txtPassword->setMaxLength(10);


    // Textbox boyutları
    ui->txtUsername->setFixedSize(220, 26);
    ui->txtPassword->setFixedSize(220, 26);
    ui->btnChangePassword->setFixedHeight(35);
    ui->btnLogin->setFixedHeight(35);
    ui->btnRegister->setFixedHeight(35);

    // ==========================================================
    // 4. GİRİŞ KARTI
    // ==========================================================

    ui->frame->setFixedSize(420, 250);


    // ==========================================================
    // 5. HATA MESAJI
    // ==========================================================

    ui->lblStatus->setAlignment(Qt::AlignCenter);

    ui->lblStatus->setFixedHeight(28);

    ui->lblStatus->setText("");

    ui->lblStatus->setStyleSheet(
        "QLabel#lblStatus {"
        "color: #ff5252;"
        "font-size: 13px;"
        "font-weight: bold;"
        "background-color: transparent;"
        "border: none;"
        "}"
        );


    // ==========================================================
    // 6. MAC TARZI SAĞ ÜST BUTONLAR
    // ==========================================================

    QPushButton *btnKucult = new QPushButton(this);
    QPushButton *btnBuyut = new QPushButton(this);
    QPushButton *btnKapat = new QPushButton(this);


    // Buton boyutları
    btnKucult->setFixedSize(14, 14);
    btnBuyut->setFixedSize(14, 14);
    btnKapat->setFixedSize(14, 14);


    // ----------------------------------------------------------
    // KIRMIZI - KAPAT
    // ----------------------------------------------------------

    btnKapat->setStyleSheet(
        "QPushButton {"
        "background-color: #FF5F56;"
        "border-radius: 7px;"
        "border: none;"
        "padding: 0px;"
        "}"
        "QPushButton:hover {"
        "background-color: #E0443E;"
        "}"
        );


    // ----------------------------------------------------------
    // SARI - KÜÇÜLT
    // ----------------------------------------------------------

    btnKucult->setStyleSheet(
        "QPushButton {"
        "background-color: #FFBD2E;"
        "border-radius: 7px;"
        "border: none;"
        "padding: 0px;"
        "}"
        "QPushButton:hover {"
        "background-color: #DDA127;"
        "}"
        );


    // ----------------------------------------------------------
    // YEŞİL - BÜYÜT
    // ----------------------------------------------------------

    btnBuyut->setStyleSheet(
        "QPushButton {"
        "background-color: #27C93F;"
        "border-radius: 7px;"
        "border: none;"
        "padding: 0px;"
        "}"
        "QPushButton:hover {"
        "background-color: #1EA532;"
        "}"
        );


    // ==========================================================
    // 7. MAC BUTONLARININ İŞLEVLERİ
    // ==========================================================

    connect(
        btnKapat,
        &QPushButton::clicked,
        this,
        &LoginDialog::close
        );


    connect(
        btnKucult,
        &QPushButton::clicked,
        this,
        &LoginDialog::showMinimized
        );


    connect(
        btnBuyut,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (isMaximized()) {
                showNormal();
            }
            else {
                showMaximized();
            }
        }
        );


    // ==========================================================
    // 8. ÜST BAR
    // ==========================================================

    QHBoxLayout *ustBar = new QHBoxLayout();

    ustBar->addStretch();

    ustBar->addWidget(btnKucult);
    ustBar->addWidget(btnBuyut);
    ustBar->addWidget(btnKapat);

    ustBar->setSpacing(6);

    ustBar->setContentsMargins(
        0,
        10,
        15,
        0
        );


    // ==========================================================
    // 9. LOGO
    // ==========================================================

    QLabel *lblLogo = new QLabel(this);

    QPixmap pixLogo("turkport-logo.png");


    if (!pixLogo.isNull())
    {
        lblLogo->setPixmap(
            pixLogo.scaled(
                180,
                180,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );
    }


    lblLogo->setAlignment(Qt::AlignCenter);

    lblLogo->setStyleSheet(
        "background: transparent;"
        "border: none;"
        );


    // ==========================================================
    // 10. ESKİ DESIGNER LAYOUT'UNU KALDIR
    // ==========================================================
    //
    // Designer'daki layout'u C++ tarafında yeniden düzenliyoruz.
    //
    // ÖNEMLİ:
    // Widget'lar silinmez.
    // Sadece eski layout kaldırılır.
    //

    QLayout *eskiLayout = layout();

    if (eskiLayout)
    {
        delete eskiLayout;
    }


    // ==========================================================
    // 11. ANA LAYOUT
    // ==========================================================

    QVBoxLayout *anaDuzen = new QVBoxLayout();

    anaDuzen->setContentsMargins(
        0,
        0,
        0,
        20
        );

    anaDuzen->setSpacing(0);


    // ==========================================================
    // 12. ÜST BAR
    // ==========================================================

    anaDuzen->addLayout(ustBar);


    // ==========================================================
    // 13. ÜST BOŞLUK
    // ==========================================================

    anaDuzen->addStretch(1);


    // ==========================================================
    // 14. LOGO
    // ==========================================================

    anaDuzen->addWidget(
        lblLogo,
        0,
        Qt::AlignCenter
        );


    // Logo ile hata mesajı arasındaki boşluk
    anaDuzen->addSpacing(8);


    // ==========================================================
    // 15. HATA MESAJI
    // ==========================================================

    anaDuzen->addWidget(
        ui->lblStatus,
        0,
        Qt::AlignCenter
        );


    // Hata mesajı ile kart arasındaki boşluk
    anaDuzen->addSpacing(6);


    // ==========================================================
    // 16. GİRİŞ KARTI
    // ==========================================================

    anaDuzen->addWidget(
        ui->frame,
        0,
        Qt::AlignCenter
        );


    // ==========================================================
    // 17. ALT BOŞLUK
    // ==========================================================

    anaDuzen->addStretch(1);


    // ==========================================================
    // 18. LAYOUT'U PENCEREYE ATA
    // ==========================================================

    setLayout(anaDuzen);


    // ==========================================================
    // 19. GİRİŞ YAP
    // ==========================================================

    connect(
        ui->btnLogin,
        &QPushButton::clicked,
        this,
        [this]()
        {
            QString user = ui->txtUsername->text();
            QString pass = ui->txtPassword->text();


            DatabaseManager db;

            db.initializeDatabase();


            if (db.verifyLogin(user, pass))
            {
                // Giriş başarılı
                accept();
            }
            else
            {
                // Hata mesajını KARTIN ÜSTÜNDE göster
                ui->lblStatus->setText(
                    "Hatalı kullanıcı adı veya şifre!"
                    );
            }
        }
        );


    // ==========================================================
    // 20. KAYIT OL
    // ==========================================================

    connect(
        ui->btnRegister,
        &QPushButton::clicked,
        this,
        [this]()
        {
            QString user = ui->txtUsername->text();
            QString pass = ui->txtPassword->text();


            // Boş kontrolü
            if (user.isEmpty() || pass.isEmpty())
            {
                ui->lblStatus->setText(
                    "Kullanıcı adı ve şifre boş bırakılamaz!"
                    );

                return;
            }


            // Şifre kuralları
            QRegularExpression regex(
                "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{4,10}$"
                );


            if (!regex.match(pass).hasMatch())
            {
                QMessageBox::warning(
                    this,
                    "Zayıf Şifre",
                    "Şifreniz 4 ile 10 karakter arasında olmalı.\n"
                    "En az 1 büyük harf, 1 küçük harf ve 1 rakam içermelidir!"
                    );

                return;
            }


            DatabaseManager db;

            db.initializeDatabase();


            if (db.addUser(user, pass))
            {
                QMessageBox::information(
                    this,
                    "Sistem Mesajı",
                    "Yeni kullanıcı sisteme başarıyla kaydedildi!"
                    );

                ui->lblStatus->clear();
            }
            else
            {
                ui->lblStatus->setText(
                    "Bu kullanıcı adı zaten kullanılıyor!"
                    );
            }
        }
        );


    // ==========================================================
    // 21. ŞİFRE DEĞİŞTİR
    // ==========================================================

    connect(
        ui->btnChangePassword,
        &QPushButton::clicked,
        this,
        [this]()
        {
            QString user = ui->txtUsername->text();
            QString newPass = ui->txtPassword->text();


            // Boş kontrolü
            if (user.isEmpty() || newPass.isEmpty())
            {
                ui->lblStatus->setText(
                    "Kullanıcı adını ve YENİ şifreyi kutulara girin!"
                    );

                return;
            }


            // Şifre kuralları
            QRegularExpression regex(
                "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{4,10}$"
                );


            if (!regex.match(newPass).hasMatch())
            {
                QMessageBox::warning(
                    this,
                    "Zayıf Şifre",
                    "Yeni şifreniz 4 ile 10 karakter arasında olmalı.\n"
                    "En az 1 büyük harf, 1 küçük harf ve 1 rakam içermelidir!"
                    );

                return;
            }


            DatabaseManager db;

            db.initializeDatabase();


            if (db.changePassword(user, newPass))
            {
                QMessageBox::information(
                    this,
                    "Sistem Mesajı",
                    "Kullanıcı şifresi başarıyla güncellendi!"
                    );

                ui->lblStatus->clear();
            }
            else
            {
                ui->lblStatus->setText(
                    "Kayıtlı böyle bir kullanıcı bulunamadı!"
                    );
            }
        }
        );
}


// ==========================================================
// DESTRUCTOR
// ==========================================================

LoginDialog::~LoginDialog()
{
    delete ui;
}


// ==========================================================
// PENCEREYİ FAREYLE SÜRÜKLEME
// ==========================================================

void LoginDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition =
            event->globalPosition().toPoint()
            - frameGeometry().topLeft();

        event->accept();
    }
}


// ==========================================================

void LoginDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(
            event->globalPosition().toPoint()
            - dragPosition
            );

        event->accept();
    }
}
