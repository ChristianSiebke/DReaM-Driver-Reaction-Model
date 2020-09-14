#ifndef DELEGATEDOUBLEVIEW_H
#define DELEGATEDOUBLEVIEW_H

#include <QStyledItemDelegate>

class DelegateDoubleView : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DelegateDoubleView(double min, double max, double increment, int decimals, QObject *parent = 0);
    virtual ~DelegateDoubleView() = default;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of createEditor. Creates a frameless QDoubleSpinBox object, sets its minimum
    //! and maximum values and returns it as an editor.
    //!
    //! @param[in]      parent      The parent widget in which the spinbox should appear
    //! @param[in]      option      an option item
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //!
    //! @return                     The pointer to the spinbox created as an editor.
    //-----------------------------------------------------------------------------
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const override;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of setEditorData. Sets the number being shown by the spinbox
    //! to the current value in the model.
    //!
    //! @param[in]      editor      a widget which should be casted on a spinbox
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //-----------------------------------------------------------------------------
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    //-----------------------------------------------------------------------------
    //! Reimplementation of setModelData. Sets the model value to the current
    //! value shown in the spinbox.
    //!
    //! @param[in]      editor      a widget which should be casted on a spinbox
    //! @param[in]      model       The pointer to the model whose value should be edited
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //-----------------------------------------------------------------------------
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

public:
    //-----------------------------------------------------------------------------
    //! Reimplementation of updateEditorGeometry. Sets the geometry of the editor
    //! according to the passed option.
    //!
    //! @param[in]      editor      a widget referring to the editor
    //! @param[in]      option      an option item
    //! @param[in]      index       The model index of the data item which should be
    //!                             addressed by this delegate
    //-----------------------------------------------------------------------------
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

private:
    double const min;
    double const max;
    double const increment;
    int const decimals;
};

#endif // DELEGATEDOUBLEVIEW_H
