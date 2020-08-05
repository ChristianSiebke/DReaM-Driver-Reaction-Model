#include "DelegateDoubleView.h"

#include <QDoubleSpinBox>

DelegateDoubleView::DelegateDoubleView(double min, double max, double increment, int decimals, QObject *parent)
    : QStyledItemDelegate(parent)
    , min(min)
    , max(max)
    , increment(increment)
    , decimals(decimals)
{

}

QWidget * DelegateDoubleView::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QDoubleSpinBox * editor = new QDoubleSpinBox(parent);
    editor->setMinimum(min);
    editor->setMaximum(max);
    editor->setDecimals(decimals);
    editor->setSingleStep(increment);
    editor->setFrame(false);
    editor->setAlignment(Qt::AlignCenter);

    editor->setKeyboardTracking(true);

    return editor;
}

void DelegateDoubleView::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::DisplayRole).toDouble();
    QDoubleSpinBox *doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
    doubleSpinBox->setValue(value);
}

void DelegateDoubleView::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
    model->setData(index,  QString::number(doubleSpinBox->value(),'f',decimals), Qt::EditRole);
}

void DelegateDoubleView::updateEditorGeometry(QWidget *editor,
                                              const QStyleOptionViewItem &option,
                                              const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
