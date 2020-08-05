#include "EnvironmentItemView.h"
#include "ui_EnvironmentItemView.h"

#include <QTimeEdit>
#include <QDoubleSpinBox>
#include <QComboBox>

EnvironmentItemView::EnvironmentItemView(Type type,
                                         unsigned int ID,
                                         int index,
                                         QWidget *parent) :
    WidgetView(parent)
    , type(type)
    , ID(ID)
    , index(index)
    , ui(new Ui::EnvironmentItemView)
{
    ui->setupUi(this);

    switch(type)
    {

    case Type::TimeOfDay:
    {
        QTimeEdit * valueView = new QTimeEdit(this);
        valueView->setObjectName("valueView");

        ui->Index->setText(QString("%1. ").arg(index));
        ui->Unit->setText("");
        ui->valueLayout->insertWidget(2, valueView);
        connect(valueView, static_cast<void (QTimeEdit::*)(QTime const &)>(&QTimeEdit::timeChanged),
                this, valueView_timeChanged);
        break;
    }
    case Type::VisibilityDistance:
    {
        QDoubleSpinBox * valueView = new QDoubleSpinBox(this);
        valueView->setObjectName("valueView");
        valueView->setMinimumWidth(60);
        valueView->setMaximumWidth(60);

        valueView->setMinimum(0.0);
        valueView->setMaximum(5000.0);
        valueView->setDecimals(1);
        valueView->setSingleStep(10);
        valueView->setLocale(QLocale::C);
        ui->Index->setText( QString("%1. ").arg(index) );
        ui->Unit->setText("[m]");
        ui->valueLayout->insertWidget(2,valueView);
        connect(valueView, static_cast<void (QDoubleSpinBox::*)(QString const &)>(&QDoubleSpinBox::valueChanged),
                this, valueView_valueChanged);
        break;
    }
    case Type::Friction:
    {
        QDoubleSpinBox * valueView = new QDoubleSpinBox(this);
        valueView->setObjectName("valueView");
        valueView->setMinimumWidth(60);
        valueView->setMaximumWidth(60);

        valueView->setMinimum(0.0);
        valueView->setMaximum(10.0);
        valueView->setDecimals(2);
        valueView->setSingleStep(0.1);
        valueView->setLocale(QLocale::C);
        ui->Index->setText( QString("%1. ").arg(index) );
        ui->Unit->setText("");
        ui->valueLayout->insertWidget(2,valueView);
        connect(valueView, static_cast<void (QDoubleSpinBox::*)(QString const &)>(&QDoubleSpinBox::valueChanged),
                this, valueView_valueChanged);
        break;
    }
    case Type::Weather:
    {
        QComboBox * valueView = new QComboBox (this);
        valueView->setMinimumWidth(60);
        valueView->setMaximumWidth(60);

        valueView->setObjectName("valueView");
        valueView->insertItems(0, QStringList({"rainy", "sunny", "cloudy", "foggy", "snowy", "..."}));

        ui->Index->setText(QString("%1. ").arg(index));
        ui->Unit->setText("");
        ui->valueLayout->insertWidget(2,valueView);

        connect(valueView, static_cast<void (QComboBox::*)(QString const &)>(&QComboBox::currentTextChanged),
                this, valueView_valueChanged);
        break;

    }
    default:
        break;

    }
}

EnvironmentItemView::~EnvironmentItemView()
{
    delete ui;
}

void EnvironmentItemView::on_remove_clicked()
{
    Q_EMIT remove(ID, type);
}

void EnvironmentItemView::valueView_valueChanged(const QString &value)
{
    Q_EMIT valueChanged(ID,value,type);
}

void EnvironmentItemView::valueView_timeChanged(const QTime &time)
{
    Q_EMIT valueChanged(ID, time.toString(), type);
}

void EnvironmentItemView::on_probabilityValue_editingFinished()
{
    Q_EMIT probabilityChanged(ID, ui->probabilityValue->value(), type);
}

void EnvironmentItemView::setValue(QString value)
{    
    switch(type)
    {

    case Type::TimeOfDay:
    {        
        findChild<QTimeEdit*>("valueView")->setTime(QTime::fromString(value));
        break;
    }
    case Type::VisibilityDistance:
    {
        findChild<QDoubleSpinBox*>("valueView")->setValue(value.toDouble());
        break;
    }
    case Type::Friction:
    {
        findChild<QDoubleSpinBox*>("valueView")->setValue(value.toDouble());
        break;
    }
    case Type::Weather:
    {
        findChild<QComboBox*>("valueView")->setCurrentText(value);
        break;
    }
    default:
        break;
    }
}


void EnvironmentItemView::setProbability(double value)
{
    ui->probabilityValue->setValue(value);
}

double EnvironmentItemView::getProbability() const
{
    return ui->probabilityValue->value();
}

void EnvironmentItemView::setIndex(int newIndex)
{
    index = newIndex;
    ui->Index->setText(QString("%1. ").arg(index));
}

unsigned int EnvironmentItemView::getID() const
{
    return ID;
}

int EnvironmentItemView::getIndex() const
{
    return index;
}

EnvironmentItemView::Type EnvironmentItemView::getType() const
{
    return type;
}

QString EnvironmentItemView::getValue() const
{
    switch(type)
    {

    case Type::TimeOfDay:
        return findChild<QTimeEdit*>("valueView")->time().toString();

    case Type::VisibilityDistance:
        return QString("%1").arg(findChild<QDoubleSpinBox*>("valueView")->value());

    case Type::Friction:
        return QString("%1").arg(findChild<QDoubleSpinBox*>("valueView")->value());

    case Type::Weather:
        return findChild<QComboBox*>("valueView")->currentText();

    default:
        return QString("");
    }
}

