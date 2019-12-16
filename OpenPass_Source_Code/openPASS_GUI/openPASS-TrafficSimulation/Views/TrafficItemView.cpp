#include "TrafficItemView.h"
#include "ui_TrafficItemView.h"

#include <QString>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFont>



TrafficItemView::TrafficItemView(Type type,
                                 unsigned int ID,
                                 int index,
                                 QWidget *parent) :
     WidgetView(parent)
    , ID(ID)
    , index(index)
    , type(type)
    , ui(new Ui::TrafficItemView)
{
    ui->setupUi(this);

    switch(type)
    {
        case Type::Volume:
        {
            QSpinBox * valueView = new QSpinBox(this);
            valueView->setObjectName("valueView");
            valueView->setMinimumWidth(50);
            valueView->setMaximumWidth(50);

            valueView->setMinimum(0);
            valueView->setMaximum(3000);
            valueView->setSingleStep(100);
            ui->Title->setText( QString("%1. ").arg(index) );
            ui->Unit->setText(" [1/h]");
            setToolTip("Specifies the target rate of vehicles per hour and per lane.");
            ui->valueLayout->insertWidget(1, valueView);
            connect(valueView, static_cast<void (QSpinBox::*)(QString const &)>(&QSpinBox::valueChanged),
                    this, valueView_valueChanged);
            break;
        }
        case Type::PlatoonRate:
        {
            QDoubleSpinBox * valueView = new QDoubleSpinBox(this);
            valueView->setObjectName("valueView");
            valueView->setMinimumWidth(50);
            valueView->setMaximumWidth(50);

            valueView->setMinimum(0.0);
            valueView->setMaximum(1.0);
            valueView->setDecimals(2);
            valueView->setSingleStep(0.1);
            valueView->setLocale(QLocale::C);
            ui->Title->setText( QString("%1. ").arg(index) );
            ui->Unit->setText("");
            setToolTip("Specifies how strong agents are grouped together. A value of 0 corresponds to equally distributed traffic along the road.");
            ui->valueLayout->insertWidget(1,valueView);
            connect(valueView, static_cast<void (QDoubleSpinBox::*)(QString const &)>(&QDoubleSpinBox::valueChanged),
                    this, valueView_valueChanged);
            break;
        }
        case Type::Velocity:
        {
            QDoubleSpinBox * valueView = new QDoubleSpinBox(this);
            valueView->setObjectName("valueView");
            valueView->setMinimumWidth(50);
            valueView->setMaximumWidth(50);

            valueView->setMinimum(0.0);
            valueView->setMaximum(250.0);
            valueView->setDecimals(0);
            valueView->setSingleStep(10.0);
            valueView->setLocale(QLocale::C);
            ui->Title->setText( QString("%1. ").arg(index) );
            ui->Unit->setText("[km/h]");
            setToolTip("Specifies the average velocity on the rightmost lane.");
            ui->valueLayout->insertWidget(1,valueView);
            connect(valueView, static_cast<void (QDoubleSpinBox::*)(QString const &)>(&QDoubleSpinBox::valueChanged),
                    this, valueView_valueChanged);
            break;
        }
        case Type::Homogeneity:
        {
            QDoubleSpinBox * valueView = new QDoubleSpinBox(this);
            valueView->setObjectName("valueView");
            valueView->setMinimumWidth(50);
            valueView->setMaximumWidth(50);

            valueView->setMinimum(0.0);
            valueView->setMaximum(1.0);
            valueView->setDecimals(2);
            valueView->setSingleStep(0.1);
            valueView->setLocale(QLocale::C);
            ui->Title->setText( QString("%1. ").arg(index) );
            ui->Unit->setText("");
            setToolTip("Specifies how much the average velocity increases per lane with respect to its individual right lane. "
                       "A homogeneity of 1 results in equal velocities across all lanes. \n"
                       "A value of 0 results in doubled velocity on the left lane, compared to its individual right lane.");
            ui->valueLayout->insertWidget(1,valueView);
            connect(valueView, static_cast<void (QDoubleSpinBox::*)(QString const &)>(&QDoubleSpinBox::valueChanged),
                    this, valueView_valueChanged);
            break;
        }
        case Type::Agent:
        {
            QLabel * valueView = new QLabel(this);
            QFont font;

            font.setPointSize(10);
            valueView->setObjectName("valueView");
            valueView->setFont(font);

            ui->Title->setText(QString("%1. ").arg(index));
            ui->Unit->setText("");
            ui->value->setText("");
            ui->valueLayout->insertWidget(1,valueView);
            break;
        }
        default:
            break;
    }
}

TrafficItemView::~TrafficItemView()
{
    delete ui;
}

void TrafficItemView::on_remove_clicked()
{
    Q_EMIT remove(ID, type);
}

void TrafficItemView::valueView_valueChanged(const QString &value)
{
    Q_EMIT valueChanged(ID,value,type);
}

void TrafficItemView::on_probabilityValue_editingFinished()
{
    Q_EMIT probabilityChanged(ID, ui->probabilityValue->value(), type);
}

unsigned int TrafficItemView::getID() const
{
    return ID;
}

int TrafficItemView::getIndex() const
{
    return index;
}



void TrafficItemView::setValue(QString value)
{
    switch(type)
    {

    case Type::Volume:
    {
        ui->ContentWidget->findChild<QSpinBox*>("valueView")->setValue(value.toInt());
        break;
    }
    case Type::PlatoonRate:
    {
        ui->ContentWidget->findChild<QDoubleSpinBox*>("valueView")->setValue(value.toDouble());
        break;
    }
    case Type::Velocity:
    {
        ui->ContentWidget->findChild<QDoubleSpinBox*>("valueView")->setValue(value.toDouble());
        break;
    }
    case Type::Homogeneity:
    {
        ui->ContentWidget->findChild<QDoubleSpinBox*>("valueView")->setValue(value.toDouble());
        break;
    }
    case Type::Agent:
    {
        ui->ContentWidget->findChild<QLabel*>("valueView")->setText(value);
        break;
    }
    }
}


void TrafficItemView::setProbability(double value)
{
    ui->probabilityValue->setValue(value);
}

double TrafficItemView::getProbability() const
{
    return ui->probabilityValue->value();
}

void TrafficItemView::setIndex(int newIndex)
{
    index = newIndex;
    ui->Title->setText(QString("%1. ").arg(index));
}


TrafficItemView::Type TrafficItemView::getType() const
{
    return type;
}

QString TrafficItemView::getValue() const
{
    switch(type)
    {

    case Type::Volume:
        return QString("%1").arg(findChild<QSpinBox*>("valueView")->value());

    case Type::PlatoonRate:
        return QString("%1").arg(findChild<QDoubleSpinBox*>("valueView")->value());

    case Type::Velocity:
        return QString("%1").arg(findChild<QDoubleSpinBox*>("valueView")->value());

    case Type::Homogeneity:
        return QString("%1").arg(findChild<QDoubleSpinBox*>("valueView")->value());

    case Type::Agent:
        return findChild<QLabel*>("valueView")->text();

    default:
        return QString("");
    }
}
