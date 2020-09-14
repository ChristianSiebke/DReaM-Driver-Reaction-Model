#include "SelectedSystemItemView.h"

#include "ui_SelectedSystemItemView.h"

SelectedSystemItemView::SelectedSystemItemView(double probability, QWidget *parent) :
    WidgetView(parent),
    ui(new Ui::SelectedSystemItemView)
{
    ui->setupUi(this);
    ui->probability->setValue(probability);
}

SelectedSystemItemView::~SelectedSystemItemView()
{
    delete ui;
}

void SelectedSystemItemView::on_remove_clicked()
{
    Q_EMIT removeSystem(ui->System->text());
}

void SelectedSystemItemView::setSystem(const QString &name)
{
    ui->System->setText(name);
}

QString SelectedSystemItemView::getSystem() const
{
    return ui->System->text();
}

void SelectedSystemItemView::setIndex(unsigned int _index)
{
    index = _index;
    ui->selection_label->setText(QString("System %1:").arg(index));
}

unsigned int SelectedSystemItemView::getIndex() const
{
    return index;
}

void SelectedSystemItemView::on_probability_editingFinished()
{
    Q_EMIT modifiedProbability(ui->System->text(), ui->probability->value());
}
