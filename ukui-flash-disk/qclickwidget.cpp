/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */
#include "qclickwidget.h"

void frobnitz_force_result_func(GDrive *source_object,GAsyncResult *res,QClickWidget *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_eject_with_operation_finish (source_object, res, &err);
    if (!err)
    {
      findGDriveList()->removeOne(source_object);
      p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(source_object),NORMALDEVICE);
      p_this->m_eject->show();
    }
}

void frobnitz_result_func(GDrive *source_object,GAsyncResult *res,QClickWidget *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_eject_with_operation_finish (source_object, res, &err);

//    qDebug()<<"oh no"<<err->message<<err->code;

    if (!err)
    {
      findGDriveList()->removeOne(source_object);
      p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(source_object),NORMALDEVICE);
      p_this->m_eject->show();
    }

    else /*if(g_drive_can_stop(source_object) == true)*/
    {
        g_drive_eject_with_operation(source_object,
                                     G_MOUNT_UNMOUNT_FORCE,
                                     NULL,
                                     NULL,
                                     GAsyncReadyCallback(frobnitz_force_result_func),
                                     p_this
                                     );
    }
}



QClickWidget::QClickWidget(QWidget *parent,
                           int num,
                           GDrive *Drive,
                           QString driveName,
                           QString nameDis1,
                           QString nameDis2,
                           QString nameDis3,
                           QString nameDis4,
                           qlonglong capacityDis1,
                           qlonglong capacityDis2,
                           qlonglong capacityDis3,
                           qlonglong capacityDis4,
                           QString pathDis1,
                           QString pathDis2,
                           QString pathDis3,
                           QString pathDis4)
    : QWidget(parent),
      m_Num(num),
      m_Drive(Drive),
      m_driveName(driveName),
      m_nameDis1(nameDis1),
      m_nameDis2(nameDis2),
      m_nameDis3(nameDis3),
      m_nameDis4(nameDis4),
      m_capacityDis1(capacityDis1),
      m_capacityDis2(capacityDis2),
      m_capacityDis3(capacityDis3),
      m_capacityDis4(capacityDis4),
      m_pathDis1(pathDis1),
      m_pathDis2(pathDis2),
      m_pathDis3(pathDis3),
      m_pathDis4(pathDis4)

{
//union layout
/*
 * it's the to set the title interface,we get the drive name and add picture of a u disk
*/
    const QByteArray id(THEME_QT_SCHEMA);

    if(QGSettings::isSchemaInstalled(id))
    {
        fontSettings = new QGSettings(id);
    }

    const QByteArray idd(THEME_QT_SCHEMA);

    if(QGSettings::isSchemaInstalled(idd))
    {
        qtSettings = new QGSettings(idd);
    }

    initFontSize();
    initThemeMode();

    QHBoxLayout *drivename_H_BoxLayout = new QHBoxLayout();
    drivename_H_BoxLayout = new QHBoxLayout();
    image_show_label = new QLabel();
    image_show_label->setFocusPolicy(Qt::NoFocus);
    image_show_label->installEventFilter(this);
    //to get theme picture for label
    imgIcon = QIcon::fromTheme("drive-removable-media-usb");
    QPixmap pixmap = imgIcon.pixmap(QSize(25, 25));
    image_show_label->setPixmap(pixmap);
    image_show_label->setFixedSize(40,40);
    m_driveName_label = new QLabel();
    m_driveName_label->setFont(QFont("Noto Sans CJK SC",fontSize));
    QString DriveName = getElidedText(m_driveName_label->font(), m_driveName, 180);
    m_driveName_label->setText(DriveName);
    m_driveName_label->setFixedSize(180,40);
    m_driveName_label->setObjectName("driveNameLabel");

    m_eject_button = new QPushButton(this);
    m_eject_button->setFlat(true);   //this property set that when the mouse is hovering in the icon the icon will move up a litte
    m_eject_button->move(m_eject_button->x()+234,m_eject_button->y()+2);
    //->setObjectName("Button");
    m_eject_button->installEventFilter(this);
    m_eject_button->setIcon(drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("media-eject-symbolic").pixmap(24,24).toImage())));
//    QIcon eject_icon(":picture/media-eject-symbolic.svg");
//    m_eject_button->setIcon(eject_icon);
    m_eject_button->setFixedSize(40,40);
    m_eject_button->setToolTip(tr("弹出"));

    drivename_H_BoxLayout->addSpacing(8);
    drivename_H_BoxLayout->addWidget(image_show_label);
    drivename_H_BoxLayout->addWidget(m_driveName_label);
    drivename_H_BoxLayout->addStretch();

    QVBoxLayout *main_V_BoxLayout = new QVBoxLayout(this);
    main_V_BoxLayout->setContentsMargins(0,0,0,0);

    connect(m_eject_button,SIGNAL(clicked()),SLOT(switchWidgetClicked()));  // this signal-slot function is to emit a signal which
                                                                            //is to trigger a slot in mainwindow
    connect(m_eject_button, &QPushButton::clicked,this,[=]()
    {
        qDebug()<<g_drive_get_name(Drive)<<"1----------------2";
        g_drive_eject_with_operation(Drive,
                     G_MOUNT_UNMOUNT_NONE,
                     NULL,
                     NULL,
                     GAsyncReadyCallback(frobnitz_result_func),
                     this);

    });

//when the drive only has one vlolume
//we set the information and set all details of the U disk in main interface
    if(m_Num == 1)
    {
        disWidgetNumOne = new QWidget(this);
        QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis1_label = new ClickLabel(disWidgetNumOne);
        m_nameDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeName = getElidedText(m_nameDis1_label->font(), m_nameDis1, 120);
        m_nameDis1_label->adjustSize();
        qDebug()<<m_nameDis1_label->width()<<"-----------------------"<<m_nameDis1_label->height();
        m_nameDis1_label->setText("- "+VolumeName+":");
        m_capacityDis1_label = new QLabel(disWidgetNumOne);
        QString str_capacityDis1 = size_human(m_capacityDis1);
        m_capacityDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis1_label->setText("("+str_capacityDis1+")");
        m_capacityDis1_label->setObjectName("capacityLabel");
        qDebug()<<m_capacityDis1_label->width()<<"++++++++++++++++++++++++"<<m_capacityDis1_label->height();
        onevolume_h_BoxLayout->setSpacing(0);
        onevolume_h_BoxLayout->addSpacing(50);
        onevolume_h_BoxLayout->setMargin(0);   //使得widgetg上的label得以居中显示
        onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
        onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
        onevolume_h_BoxLayout->addSpacing(10);
        onevolume_h_BoxLayout->addStretch();

        disWidgetNumOne->setObjectName("OriginObjectOnly");
//        disWidgetNumOne->setContentsMargins(0,0,0,0);
        disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
        disWidgetNumOne->installEventFilter(this);
        disWidgetNumOne->setFixedHeight(30);
//        main_V_BoxLayout->setContentsMargins(0,0,0,0);
        main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
        main_V_BoxLayout->addWidget(disWidgetNumOne);
//        main_V_BoxLayout->setAlignment(Qt::AlignVCenter);
//        setContentsMargins(0,0,0,0);
        this->setLayout(main_V_BoxLayout);
        this->setFixedSize(276,68);
//            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
    }
//when the drive has two volumes
    if(m_Num == 2)
    {
        QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis1_label = new ClickLabel();
        m_nameDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis1 = getElidedText(m_nameDis1_label->font(), m_nameDis1, 120);
        m_nameDis1_label->adjustSize();
        m_nameDis1_label->setText("- "+VolumeNameDis1+":");
//            m_nameDis1_label->setStyleSheet(
//                                    //正常状态样式
//                                    "QLabel{"
//                                    "font-size:14px;"
//                                    "font-family:Microsoft YaHei;"
//                                    "font-weight:400;"
//                                    "color:rgba(255,255,255,0.35);"
//                                    "line-height:28px;"
//                                    "opacity:0.35;"
//                                    "}"
//                                    );

        //m_nameDis1_label->setAlignment(Qt::AlignRight);
        m_nameDis1_label->installEventFilter(this);
        //m_nameDis1_label->setText("- "+m_nameDis1+":");
        m_capacityDis1_label = new QLabel();
        QString str_capacityDis1 = size_human(m_capacityDis1);
        m_capacityDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis1_label->setText("("+str_capacityDis1+")");
        m_capacityDis1_label->setObjectName("capacityLabel");
        onevolume_h_BoxLayout->addSpacing(50);
        onevolume_h_BoxLayout->setSpacing(0);
        onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
        onevolume_h_BoxLayout->setMargin(0);
//            QVBoxLayout *capacityDis1_V_BoxLayout = new QVBoxLayout;
//            capacityDis1_V_BoxLayout->addSpacing(2);
//            capacityDis1_V_BoxLayout->addWidget(m_capacityDis1_label);
//            onevolume_h_BoxLayout->addLayout(capacityDis1_V_BoxLayout);
        onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
        onevolume_h_BoxLayout->addStretch();
        disWidgetNumOne = new QWidget;
        disWidgetNumOne->setFixedHeight(30);
        disWidgetNumOne->setObjectName("OriginObjectOnly");
        disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
        disWidgetNumOne->installEventFilter(this);

        QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis2_label = new ClickLabel(this);
        m_nameDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis2 = getElidedText(m_nameDis2_label->font(), m_nameDis2, 120);
        m_nameDis2_label->setText("- "+VolumeNameDis2+":");
//            m_nameDis2_label->setStyleSheet(
//                                    //正常状态样式
//                                    "QLabel{"
//                                    "font-size:14px;"
//                                    "font-family:Microsoft YaHei;"
//                                    "font-weight:400;"
//                                    "color:rgba(255,255,255,0.35);"
//                                    "line-height:28px;"
//                                    "opacity:0.35;"
//                                    "}"
//                                    );
        m_nameDis2_label->adjustSize();
        m_nameDis2_label->installEventFilter(this);
        m_capacityDis2_label = new QLabel();
        QString str_capacityDis2 = size_human(m_capacityDis2);
        m_capacityDis2_label->setText("("+str_capacityDis2+")");
        m_capacityDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis2_label->setObjectName("capacityLabel");
        twovolume_h_BoxLayout->addSpacing(50);
        twovolume_h_BoxLayout->setSpacing(0);
        twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
        twovolume_h_BoxLayout->setMargin(0);
//            QVBoxLayout *capacityDis2_V_BoxLayout = new QVBoxLayout;
//            capacityDis2_V_BoxLayout->addWidget(m_capacityDis2_label);
//            twovolume_h_BoxLayout->addLayout(capacityDis2_V_BoxLayout);
        twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);
        twovolume_h_BoxLayout->addStretch();
        disWidgetNumTwo = new QWidget;
        disWidgetNumTwo->setFixedHeight(30);
        disWidgetNumTwo->setObjectName("OriginObjectOnly");
        disWidgetNumTwo->setLayout(twovolume_h_BoxLayout);
        disWidgetNumTwo->installEventFilter(this);

        main_V_BoxLayout->setContentsMargins(0,0,0,0);
        main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
        if(m_pathDis1 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumOne);
        }

        if(m_pathDis2 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumTwo);
        }

        main_V_BoxLayout->addStretch();
        this->setLayout(main_V_BoxLayout);
        this->setFixedSize(276,97);
//            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
//            connect(m_nameDis2_label,SIGNAL(clicked()),this,SLOT(on_volume2_clicked()));
    }
//when the drive has three volumes
    if(m_Num == 3)
    {
        QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis1_label = new ClickLabel(this);
        m_nameDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis1 = getElidedText(m_nameDis1_label->font(), m_nameDis2, 120);
        m_nameDis1_label->setText("- "+VolumeNameDis1+":");
//            m_nameDis1_label->setStyleSheet(
//                                    //正常状态样式
//                                    "QLabel{"
//                                    "width:111px;"
//                                    "height:14px;"
//                                    "font-size:14px;"
//                                    "font-family:Microsoft YaHei;"
//                                    "font-weight:400;"
//                                    "color:rgba(255,255,255,0.35);"
//                                    "line-height:28px;"
//                                    "opacity:0.35;"
//                                    "}"
//                                    );
        m_nameDis1_label->adjustSize();
        m_nameDis1_label->installEventFilter(this);
        //m_nameDis1_label->setText("- "+m_nameDis1+":");
        m_capacityDis1_label = new QLabel();
        QString str_capacityDis1 = size_human(m_capacityDis1);
        m_capacityDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis1_label->setText("("+str_capacityDis1+")");
        m_capacityDis1_label->setObjectName("capacityLabel");
        onevolume_h_BoxLayout->addSpacing(50);
        onevolume_h_BoxLayout->setSpacing(0);
        onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
        onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
        onevolume_h_BoxLayout->addStretch();
        onevolume_h_BoxLayout->setMargin(0);

        disWidgetNumOne = new QWidget;
        disWidgetNumOne->setFixedHeight(30);
        disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
        disWidgetNumOne->setObjectName("OriginObjectOnly");
        disWidgetNumOne->installEventFilter(this);

        QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis2_label = new ClickLabel(this);
        m_nameDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis2 = getElidedText(m_nameDis2_label->font(), m_nameDis2, 120);
        m_nameDis2_label->setText("- "+VolumeNameDis2+":");
//            m_nameDis2_label->setStyleSheet(
//                                    //正常状态样式
//                                    "QLabel{"
//                                    "width:111px;"
//                                    "height:14px;"
//                                    "font-size:14px;"
//                                    "font-family:Microsoft YaHei;"
//                                    "font-weight:400;"
//                                    "color:rgba(255,255,255,0.35);"
//                                    "line-height:28px;"
//                                    "opacity:0.35;"
//                                    "}"
//                                    );
        m_nameDis2_label->adjustSize();
        m_nameDis2_label->installEventFilter(this);
        //m_nameDis2_label->setText("- "+m_nameDis2+":");
        m_capacityDis2_label = new QLabel();
        QString str_capacityDis2 = size_human(m_capacityDis2);
        m_capacityDis2_label->setText("("+str_capacityDis2+")");
        m_capacityDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis2_label->setObjectName("capacityLabel");
//            m_capacityDis2_label->setAlignment(Qt::AlignLeft);
        twovolume_h_BoxLayout->addSpacing(50);
        twovolume_h_BoxLayout->setSpacing(0);
        twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
        twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);
        twovolume_h_BoxLayout->addStretch();
        twovolume_h_BoxLayout->setMargin(0);

        disWidgetNumTwo = new QWidget;
        disWidgetNumTwo->setFixedHeight(30);
        disWidgetNumTwo->setObjectName("OriginObjectOnly");
        disWidgetNumTwo->setLayout(twovolume_h_BoxLayout);
        disWidgetNumTwo->installEventFilter(this);

        QHBoxLayout *threevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis3_label = new ClickLabel(this);
        m_nameDis3_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis3 = getElidedText(m_nameDis3_label->font(), m_nameDis3, 120);
        m_nameDis3_label->setText("- "+VolumeNameDis3+":");
        m_nameDis3_label->adjustSize();
        m_nameDis3_label->installEventFilter(this);
        //m_nameDis3_label->setText("- "+m_nameDis3+":");
        m_capacityDis3_label = new QLabel();
        QString str_capacityDis3 = size_human(m_capacityDis3);
        m_capacityDis3_label->setText("("+str_capacityDis3+")");
        m_capacityDis3_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis3_label->setObjectName("capacityLabel");
        threevolume_h_BoxLayout->addSpacing(50);
        threevolume_h_BoxLayout->setSpacing(0);
        threevolume_h_BoxLayout->addWidget(m_nameDis3_label);
        threevolume_h_BoxLayout->addWidget(m_capacityDis3_label);
        threevolume_h_BoxLayout->addStretch();
        threevolume_h_BoxLayout->setMargin(0);

        disWidgetNumThree = new QWidget;
        disWidgetNumThree->setFixedHeight(30);
        disWidgetNumThree->setObjectName("OriginObjectOnly");
        disWidgetNumThree->setLayout(threevolume_h_BoxLayout);
        disWidgetNumThree->installEventFilter(this);


        main_V_BoxLayout->setContentsMargins(0,0,0,0);
        main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
        if(m_pathDis1 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumOne);
        }

        if(m_pathDis2 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumTwo);
        }

        if(m_pathDis3 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumThree);
        }

        this->setLayout(main_V_BoxLayout);
        this->setFixedSize(276,136);
//            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
//            connect(m_nameDis2_label,SIGNAL(clicked()),this,SLOT(on_volume2_clicked()));
//            connect(m_nameDis3_label,SIGNAL(clicked()),this,SLOT(on_volume3_clicked()));
    }
//when the drive has four volumes
    if(m_Num == 4)
    {
        QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis1_label = new ClickLabel(this);
        m_nameDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis1 = getElidedText(m_nameDis1_label->font(), m_nameDis1, 120);
        m_nameDis1_label->setText("- "+VolumeNameDis1+":");
//            m_nameDis1_label->setStyleSheet(
//                                    //正常状态样式
//                                    "QLabel{"
//                                    "width:111px;"
//                                    "height:14px;"
//                                    "font-size:14px;"
//                                    "font-family:Microsoft YaHei;"
//                                    "font-weight:400;"
//                                    "color:rgba(255,255,255,0.35);"
//                                    "line-height:28px;"
//                                    "opacity:0.35;"
//                                    "}"
//                                    );
        m_nameDis1_label->adjustSize();
        m_nameDis1_label->installEventFilter(this);
        //m_nameDis1_label->setText("- "+m_nameDis1+":");
        m_capacityDis1_label = new QLabel();
        QString str_capacityDis1 = size_human(m_capacityDis1);
        m_capacityDis1_label->setText("("+str_capacityDis1+")");
        m_capacityDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis1_label->setObjectName("capacityLabel");
        onevolume_h_BoxLayout->addSpacing(50);
        onevolume_h_BoxLayout->setSpacing(0);
        onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
        onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
        onevolume_h_BoxLayout->addStretch();
        onevolume_h_BoxLayout->setMargin(0);

        disWidgetNumOne = new QWidget;
        disWidgetNumOne->setFixedHeight(30);
        disWidgetNumOne->setObjectName("OriginObjectOnly");
        disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
        disWidgetNumOne->installEventFilter(this);

        QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis2_label = new ClickLabel(this);
        m_nameDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis2 = getElidedText(m_nameDis2_label->font(), m_nameDis2, 120);
        m_nameDis2_label->setText("- "+VolumeNameDis2+":");
        m_nameDis2_label->installEventFilter(this);
//            m_nameDis2_label->setStyleSheet(
//                                    //正常状态样式
//                                    "QLabel{"
//                                    "width:111px;"
//                                    "height:14px;"
//                                    "font-size:14px;"
//                                    "font-family:Micrifosoft YaHei;"
//                                    "font-weight:400;"
//                                    "color:rgba(255,255,255,0.35);"
//                                    "line-height:28px;"
//                                    "opacity:0.35;"
//                                    "}"
//                                    );
        m_nameDis2_label->adjustSize();
        m_nameDis2_label->installEventFilter(this);
        //m_nameDis2_label->setText("- "+m_nameDis2+":");
        m_capacityDis2_label = new QLabel();
        QString str_capacityDis2 = size_human(m_capacityDis2);
        m_capacityDis2_label->setText("("+str_capacityDis2+")");
        m_capacityDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis2_label->setObjectName("capacityLabel");
        twovolume_h_BoxLayout->addSpacing(50);
        twovolume_h_BoxLayout->setSpacing(0);
        twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
        twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);
        twovolume_h_BoxLayout->addStretch();
        twovolume_h_BoxLayout->setMargin(0);

        disWidgetNumTwo = new QWidget;
        disWidgetNumTwo->setFixedHeight(30);
        disWidgetNumTwo->setObjectName("OriginObjectOnly");
        disWidgetNumTwo->setLayout(twovolume_h_BoxLayout);
        disWidgetNumTwo->installEventFilter(this);

        QHBoxLayout *threevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis3_label = new ClickLabel(this);
        m_nameDis3_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis3 = getElidedText(m_nameDis3_label->font(), m_nameDis3, 120);
        m_nameDis3_label->setText("- "+VolumeNameDis3+":");
//            m_nameDis3_label->setStyleSheet(
//                                    //正常状态样式
//                                    "QLabel{"
//                                    "width:111px;"
//                                    "height:14px;"
//                                    "font-size:14px;"
//                                    "font-family:Microsoft YaHei;"
//                                    "font-weight:400;"
//                                    "color:rgba(255,255,255,0.35);"
//                                    "line-height:28px;"
//                                    "opacity:0.35;"
//                                    "}"
//                                    );
        m_nameDis3_label->installEventFilter(this);
        //m_nameDis3_label->setText("- "+m_nameDis3+":");
        m_capacityDis3_label = new QLabel();
        QString str_capacityDis3 = size_human(m_capacityDis3);
        m_capacityDis3_label->setText("("+str_capacityDis3+")");
        m_capacityDis3_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis3_label->setObjectName("capacityLabel");
        threevolume_h_BoxLayout->addSpacing(50);
        threevolume_h_BoxLayout->setSpacing(0);
        threevolume_h_BoxLayout->addWidget(m_nameDis3_label);
        threevolume_h_BoxLayout->addWidget(m_capacityDis3_label);
        threevolume_h_BoxLayout->addStretch(0);
        threevolume_h_BoxLayout->setMargin(0);

        disWidgetNumThree = new QWidget;
        disWidgetNumThree->setFixedHeight(30);
//        disWidgetNumOne->setMaximumHeight(64);
        disWidgetNumThree->setObjectName("OriginObjectOnly");
        disWidgetNumThree->setLayout(threevolume_h_BoxLayout);
        disWidgetNumThree->installEventFilter(this);

        QHBoxLayout *fourvolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis4_label = new ClickLabel(this);
        m_nameDis4_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis4 = getElidedText(m_nameDis4_label->font(), m_nameDis4, 120);
        m_nameDis4_label->setText("- "+VolumeNameDis4+":");
//            m_nameDis4_label->setStyleSheet(
//                                    //正常状态样式
//                                    "QLabel{"
//                                    "width:111px;"
//                                    "height:14px;"
//                                    "font-size:14px;"
//                                    "font-family:Microsoft YaHei;"
//                                    "font-weight:400;"
//                                    "color:rgba(255,255,255,0.35);"
//                                    "line-height:28px;"
//                                    "opacity:0.35;"
//                                    "}"
//                                    );
        m_nameDis4_label->adjustSize();
        m_nameDis4_label->installEventFilter(this);
        //m_nameDis4_label->setText("- "+m_nameDis4+":");
        m_capacityDis4_label = new QLabel();
        QString str_capacityDis4 = size_human(m_capacityDis4);
        m_capacityDis4_label->setText("("+str_capacityDis4+")");
        m_capacityDis4_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis4_label->setObjectName("capacityLabel");
        fourvolume_h_BoxLayout->addSpacing(50);
        fourvolume_h_BoxLayout->setSpacing(0);
        fourvolume_h_BoxLayout->addWidget(m_nameDis4_label);
        fourvolume_h_BoxLayout->addWidget(m_capacityDis4_label);
        fourvolume_h_BoxLayout->addStretch();
        fourvolume_h_BoxLayout->setMargin(0);

        disWidgetNumFour = new QWidget;
        disWidgetNumFour->setFixedHeight(30);
//        disWidgetNumOne->setMaximumHeight(64);
        disWidgetNumFour->setObjectName("OriginObjectOnly");
        disWidgetNumFour->setLayout(fourvolume_h_BoxLayout);
        disWidgetNumFour->installEventFilter(this);

        main_V_BoxLayout->setContentsMargins(0,0,0,0);
        main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
        if(m_pathDis1 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumOne);
        }

        if(m_pathDis2 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumTwo);
        }

        if(m_pathDis3 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumThree);
        }

        if(m_pathDis4 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumFour);
        }

        this->setLayout(main_V_BoxLayout);
        this->setFixedSize(276,165);
//            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
//            connect(m_nameDis2_label,SIGNAL(clicked()),this,SLOT(on_volume2_clicked()));
//            connect(m_nameDis3_label,SIGNAL(clicked()),this,SLOT(on_volume3_clicked()));
//            connect(m_nameDis4_label,SIGNAL(clicked()),this,SLOT(on_volume4_clicked()));
    }
    //connect(this, SIGNAL(clicked()), this, SLOT(mouseClicked()));
}

void QClickWidget::initFontSize()
{
    if (!fontSettings)
    {
       fontSize = 11;
       return;
    }

    QStringList keys = fontSettings->keys();
    if (keys.contains("systemFont") || keys.contains("systemFontSize"))
    {
        fontSize = fontSettings->get("system-font").toInt();
    }
}

void QClickWidget::initThemeMode()
{
    if(!qtSettings)
    {
        currentThemeMode = "ukui-white";
    }
    QStringList keys = qtSettings->keys();
    if(keys.contains("styleName"))
    {
        currentThemeMode = qtSettings->get("style-name").toString();
    }
}

QClickWidget::~QClickWidget()
{
//    if(image_show_label)
//    delete image_show_label;
}

//to show the text,when the contents is too much,we use the "..."to replace it
//QString QClickWidget::getElidedText(QFont font, QString str, int MaxWidth)
//{
//    if (str.isEmpty())
//    {
//        return "";
//    }

//    QFontMetrics fontWidth(font);

//    //计算字符串宽度
//    //calculat the width of the string
//    int width = fontWidth.width(str);

//    //当字符串宽度大于最大宽度时进行转换
//    //Convert when string width is greater than maximum width
//    if (width >= MaxWidth)
//    {
//        //右部显示省略号
//        //show by ellipsis in right
//        str = fontWidth.elidedText(str, Qt::ElideRight, MaxWidth);
//    }
//    //返回处理后的字符串
//    //return the string that is been handled
//    return str;
//}

void QClickWidget::mouseClicked()
{
    //处理代码
    //Processing code
//        std::string str = m_path.toStdString();
//        const char* ch = str.c_str();
//    QString aaa = "caja "+m_pathDis1;
//    system(aaa.toUtf8().data());
    QProcess::startDetached("peony "+m_pathDis1);
    this->topLevelWidget()->hide();
}


void QClickWidget::mousePressEvent(QMouseEvent *ev)
{
    mousePos = QPoint(ev->x(), ev->y());
}

void QClickWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mousePos == QPoint(ev->x(), ev->y())) Q_EMIT clicked();
}

//void QClickWidget::paintEvent(QPaintEvent *)
// {
//     //解决QClickWidget类设置样式不生效的问题
//     QStyleOption opt;
//     opt.init(this);
//     QPainter p(this);
//     p.setBrush(QColor(0x00,0x00,0x00));
//     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
// }

//click the first area to show the interface
void QClickWidget::on_volume1_clicked()
{
//    QProcess::startDetached("caja "+m_pathDis1);
    QString aaa = "peony "+m_pathDis1;
    QProcess::startDetached(aaa.toUtf8().data());
    qDebug()<<"------1-------QString"<<aaa;
    this->topLevelWidget()->hide();
}

//click the second area to show the interface
void QClickWidget::on_volume2_clicked()
{
    //QProcess::startDetached("caja "+m_pathDis2);
    QString aaa = "peony "+m_pathDis2;
    QProcess::startDetached(aaa.toUtf8().data());
    qDebug()<<"-------2------QString"<<aaa;
    this->topLevelWidget()->hide();
}

//click the third area to show the interface
void QClickWidget::on_volume3_clicked()
{
    QProcess::startDetached("peony "+m_pathDis3);
    this->topLevelWidget()->hide();
}

//click the forth area to show the interface
void QClickWidget::on_volume4_clicked()
{
    QProcess::startDetached("peony "+m_pathDis4);
    this->topLevelWidget()->hide();
}

void QClickWidget::switchWidgetClicked()
{
    qDebug()<<"出发信号";
    Q_EMIT clickedConvert();

}

QPixmap QClickWidget::drawSymbolicColoredPixmap(const QPixmap &source)
{
//    qDebug()<<"wwj,wozhendeshishishishsishishsishsihsishsishsishsihs"<<currentThemeMode;
    if(currentThemeMode == "ukui-light" || currentThemeMode == "ukui-white")
    {
        QImage img = source.toImage();
        for (int x = 0; x < img.width(); x++)
        {
            for (int y = 0; y < img.height(); y++)
            {
                auto color = img.pixelColor(x, y);
                if (color.alpha() > 0)
                {
                        color.setRed(0);
                        color.setGreen(0);
                        color.setBlue(0);
                        img.setPixelColor(x, y, color);
                }
            }
        }
        return QPixmap::fromImage(img);
    }

    else if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default" )
    {
        QImage img = source.toImage();
        for (int x = 0; x < img.width(); x++)
        {
            for (int y = 0; y < img.height(); y++)
            {
                auto color = img.pixelColor(x, y);
                if (color.alpha() > 0)
                {
                        color.setRed(255);
                        color.setGreen(255);
                        color.setBlue(255);
                        img.setPixelColor(x, y, color);
                }
            }
        }
        return QPixmap::fromImage(img);
    }

    else
    {
        QImage img = source.toImage();
        for (int x = 0; x < img.width(); x++)
        {
            for (int y = 0; y < img.height(); y++)
            {
                auto color = img.pixelColor(x, y);
                if (color.alpha() > 0)
                {
                        color.setRed(0);
                        color.setGreen(0);
                        color.setBlue(0);
                        img.setPixelColor(x, y, color);
                }
            }
        }
        return QPixmap::fromImage(img);
    }
}

//to convert the capacity by another type
QString QClickWidget::size_human(qlonglong capacity)
{
    //    float capacity = this->size();
    if(capacity != NULL && capacity != 1)
    {
        QStringList list;
        list << "KB" << "MB" << "GB" << "TB";

        QStringListIterator i(list);
        QString unit("bytes");

        while(capacity >= 1024.0 && i.hasNext())
        {
            unit = i.next();
            capacity /= 1024.0;
        }
        QString str_capacity=QString(" %1%2").arg(capacity).arg(unit);
        return str_capacity;
     //   return QString().setNum(capacity,'f',2)+" "+unit;
    }
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    if(capacity == NULL)
    {
       QString str_capaticity = tr("the capacity is empty");
       return str_capaticity;
    }
#endif
    if(capacity == 1)
    {
        QString str_capacity = tr("blank CD");
        return str_capacity;
    }
}

//set the style of the eject button and label when the mouse doing some different operations
bool QClickWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_eject_button)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                m_eject_button->setIconSize(QSize(14,14));
                m_eject_button->setFixedSize(38,38);
                m_eject_button->setStyleSheet(
                        "background:rgba(255,255,255,0.08);"
                        "border-radius:4px;"
                        );
            }
            else
            {
                m_eject_button->setIconSize(QSize(14,14));
                m_eject_button->setFixedSize(38,38);
                m_eject_button->setStyleSheet(
                        "background:rgba(0,0,0,0.08);"
                        "border-radius:4px;"
                        );
            }

        }

        if(event->type() == QEvent::Enter)
        {
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                m_eject_button->setIconSize(QSize(16,16));
                m_eject_button->setFixedSize(40,40);
                m_eject_button->setStyleSheet(
                        "background-color:rgba(255,255,255,0.91);"
                        "background:rgba(255,255,255,0.12);"
                        "border-radius:4px;");
            }
            else
            {
                m_eject_button->setIconSize(QSize(16,16));
                m_eject_button->setFixedSize(40,40);
                m_eject_button->setStyleSheet(
                        "background-color:rgba(0,0,0,0.91);"
                        "background:rgba(0,0,0,0.12);"
                        "border-radius:4px;");
            }

        }

        if(event->type() == QEvent::Leave)
        {
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black")
            {
                m_eject_button->setIconSize(QSize(16,16));
                m_eject_button->setFixedSize(40,40);
                m_eject_button->setStyleSheet(
                        "background-color:rgba(255,255,255,0.75);"
                        "background-color:rgba(255,255,255,0.57);"
                        "background:rgba(255,255,255,0);"
                        "border-radius:4px;");
            }
            else
            {
                m_eject_button->setIconSize(QSize(16,16));
                m_eject_button->setFixedSize(40,40);
                m_eject_button->setStyleSheet(
                        "background-color:rgba(0,0,0,0.75);"
                        "background-color:rgba(0,0,0,0.57);"
                        "background:rgba(0,0,0,0);"
                        "border-radius:4px;");
            }
        }
    }

    if(obj == image_show_label)
    {
        if(event->type() == QEvent::Enter)
        {
//            image_show_label->setIconSize(QSize(25,25));
//            image_show_label->setFixedSize(40,40);
//            image_show_label->setFlat(true);
//            image_show_label->setStyleSheet(
//                    "background:transparent;"
//                    "border-radius:4px;");
        }
    }

    if(obj == disWidgetNumOne)
    {
       if(event->type() == QEvent::Enter)
       {
           if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
           {
               disWidgetNumOne->setStyleSheet(
                           "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
           }
           else
           {
               disWidgetNumOne->setStyleSheet(
                           "QWidget#OriginObjectOnly{background:rgba(0,0,0,0.12);}");
           }
           if(m_nameDis1_label)
           {
//           m_nameDis1_label->setStyleSheet(
//           //正常状态样式
//           //nomal state style
//           "QLabel{"
//           "font-size:14px;"
//           "font-family:Microsoft YaHei;"
//           "font-weight:400;"
//           "color:rgba(255,255,255,1);"
//           "line-height:28px;"
//           "}");
           }
//           m_capacityDis1_label->setStyleSheet(
//           //正常状态样式
//           "QLabel{"
//           "font-size:14px;"
//           "font-family:Microsoft YaHei;"
//           "font-weight:400;"
//           "color:rgba(255,255,255,1);"
//           "line-height:28px;"
//           "}");
       }

       if(event->type() == QEvent::Leave)
       {
           disWidgetNumOne->setStyleSheet(
                       "");
//           m_nameDis1_label->setStyleSheet(
//           //正常状态样式
//           "QLabel{"
//           "font-size:14px;"
//           "font-family:Microsoft YaHei;"
//           "font-weight:400;"
//           "color:rgba(255,255,255,0.35);"
//           "line-height:28px;"
//           "opacity:0.35;"
//           "}");
//           m_capacityDis1_label->setStyleSheet(
//           //正常状态样式
//           "QLabel{"
//           "font-size:14px;"
//           "font-family:Microsoft YaHei;"
//           "font-weight:400;"
//           "color:rgba(255,255,255,0.35);"
//           "line-height:28px;"
//           "opacity:0.35;"
//           "}");
       }

       if(event->type() == QEvent::MouseButtonPress)
       {
           on_volume1_clicked();
       }
    }

    if(obj == disWidgetNumTwo)
    {
        if(event->type() == QEvent::Enter )
        {
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                disWidgetNumTwo->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
            }
            else
            {
                disWidgetNumTwo->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(0,0,0,0.12);}");
            }
//            m_nameDis2_label->setStyleSheet(
//            //正常状态样式
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,1);"
//            "line-height:28px;"
//            "}");

//            m_capacityDis2_label->setStyleSheet(
//            //正常状态样式
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,1);"
//            "line-height:28px;"
//            "}");
        }

        if(event->type() == QEvent::Leave)
        {
            disWidgetNumTwo->setStyleSheet(
                        "");
//            m_nameDis2_label->setStyleSheet(
//            //正常状态样式
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,0.35);"
//            "line-height:28px;"
//            "opacity:0.35;"
//            "}");

//            m_capacityDis2_label->setStyleSheet(
//            //正常状态样式
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,0.35);"
//            "line-height:28px;"
//            "opacity:0.35;"
//            "}");
        }

        if(event->type() == QEvent::MouseButtonPress)
        {
            on_volume2_clicked();
        }
    }

    if(obj == disWidgetNumThree)
    {
        if(event->type() == QEvent::Enter )
        {
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                disWidgetNumThree->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
            }
            else
            {
                disWidgetNumThree->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(0,0,0,0.12);}");
            }
//            m_nameDis3_label->setStyleSheet(
//            //正常状态样式
//            //normal state style
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,1);"
//            "line-height:28px;"
//            "}");

//            m_capacityDis3_label->setStyleSheet(
//            //正常状态样式
//            //normal state style
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,1);"
//            "line-height:28px;"
//            "}");
        }

        if(event->type() == QEvent::Leave)
        {
            disWidgetNumThree->setStyleSheet(
                        "");
//            m_nameDis3_label->setStyleSheet(
//            //正常状态样式
//            //normal state stlyle
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,0.35);"
//            "line-height:28px;"
//            "opacity:0.35;"
//            "}");

//            m_capacityDis3_label->setStyleSheet(
//            //正常状态样式
//            //normal state style
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,0.35);"
//            "line-height:28px;"
//            "opacity:0.35;"
//            "}");
        }

        if(event->type() == QEvent::MouseButtonPress)
        {
            on_volume3_clicked();
        }
    }

    if(obj == disWidgetNumFour)
    {
        if(event->type() == QEvent::Enter )
        {
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                disWidgetNumFour->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
            }
            else
            {
                disWidgetNumFour->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(0,0,0,0.12);}");
            }
//            m_nameDis4_label->setStyleSheet(
//            //正常状态样式
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,1);"
//            "line-height:28px;"
//            "}");

//            m_capacityDis4_label->setStyleSheet(
//            //正常状态样式
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,1);"
//            "line-height:28px;"
//            "}");
        }

        if(event->type() == QEvent::Leave)
        {
            disWidgetNumFour->setStyleSheet(
                        "");
//            m_nameDis4_label->setStyleSheet(
//            //正常状态样式
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,0.35);"
//            "line-height:28px;"
//            "opacity:0.35;"
//            "}");

//            m_capacityDis4_label->setStyleSheet(
//            //正常状态样式
//            "QLabel{"
//            "font-size:14px;"
//            "font-family:Microsoft YaHei;"
//            "font-weight:400;"
//            "color:rgba(255,255,255,0.35);"
//            "line-height:28px;"
//            "opacity:0.35;"
//            "}");
        }

        if(event->type() == QEvent::MouseButtonPress)
        {
            on_volume4_clicked();
        }
    }

    return false;
}

void QClickWidget::resizeEvent(QResizeEvent *event)
{
}
