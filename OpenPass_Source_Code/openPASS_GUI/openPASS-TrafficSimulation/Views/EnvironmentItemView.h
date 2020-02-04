#ifndef ENVIRONMENTITEMVIEW_H
#define ENVIRONMENTITEMVIEW_H

#include "WidgetView.h"

namespace Ui {
class EnvironmentItemView;
}

class EnvironmentItemView : public WidgetView
{
    Q_OBJECT


public:
    enum class Type {            //!< An enum class defining the type of the environment item
        TimeOfDay = 1,
        VisibilityDistance = 2,
        Friction = 3,
        Weather = 4,
    };

public:
    explicit EnvironmentItemView(Type type,
                                 unsigned int ID,
                                 int index,
                                 QWidget *parent = 0);
    ~EnvironmentItemView();


Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the value of the environment item has been changed. Transmits
    //! the ID, value and type of the item.
    //-----------------------------------------------------------------------------
    void valueChanged(unsigned int ID, QString value, Type type);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when remove button has been clicked. Transmits the ID and type
    //! of the environment item.
    //-----------------------------------------------------------------------------
    void remove(unsigned int ID, Type type);

    //-----------------------------------------------------------------------------
    //! Signal, emitted when probability of the environment item has been modified by
    //! the user. Transmits the ID, probability value and type of the item.
    //-----------------------------------------------------------------------------
    void probabilityChanged(unsigned int ID, double probability, Type type);

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the remove button has been clicked.
    //! Emits the #remove signal.
    //-----------------------------------------------------------------------------
    void on_remove_clicked();

    //-----------------------------------------------------------------------------
    //! Qt Desinger-created slot activated when the probability value of the double
    //! spinbox has been edited by the user. Emits the #probabilityChanged signal.
    //-----------------------------------------------------------------------------
    void on_probabilityValue_editingFinished();

    //-----------------------------------------------------------------------------
    //! A slot which is activated when the value of the environment item
    //! has been edited by the user. Emits the #valueChanged signal.
    //!
    //! @param[in]      value       the new value of the traffic item
    //-----------------------------------------------------------------------------
    void valueView_valueChanged(QString const & value);

    //-----------------------------------------------------------------------------
    //! A slot which is activated when the value of time of day edit element has
    //! been modified by the user. Emits the #timeChanged signal.
    //!
    //! @param[in]      value       the new value of the traffic item
    //-----------------------------------------------------------------------------
    void valueView_timeChanged(QTime const & time);

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the ID of the environment item.
    //!
    //! @return             the ID of the environment item
    //-----------------------------------------------------------------------------
    unsigned int getID() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the index of the environment item. Items will be numerated
    //! in the order they appear in the EnvironmentView.
    //!
    //! @return             the index of the environment item
    //-----------------------------------------------------------------------------
    int getIndex() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the type of the environment item. The types are defined by
    //! the enum class #Type.
    //!
    //! @return             the type of the environment item
    //-----------------------------------------------------------------------------
    Type getType() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the value of the environment item.
    //!
    //! @return             the value of the environment item
    //-----------------------------------------------------------------------------
    QString getValue() const;

    //-----------------------------------------------------------------------------
    //! Method which returns the probability of the environment item.
    //!
    //! @return             the probability of the traffic item
    //-----------------------------------------------------------------------------
    double getProbability() const;

public:
    //-----------------------------------------------------------------------------
    //! Method which sets the displayed value of the environment item to the one passed as
    //! an argument.
    //!
    //! @param[in]      the new value to be displayed
    //-----------------------------------------------------------------------------
    void setValue(QString value);

    //-----------------------------------------------------------------------------
    //! Method which sets the displayed probability of the environment item to the one passed as
    //! an argument.
    //!
    //! @param[in]      the new probability to be displayed
    //-----------------------------------------------------------------------------
    void setProbability(double value);

    //-----------------------------------------------------------------------------
    //! Method which sets the displayed index of the environment item to the one passed as
    //! an argument.
    //!
    //! @param[in]      the new index to be displayed
    //-----------------------------------------------------------------------------
    void setIndex(int newIndex);

private:
    Type type;                      //!< the type of the environment item
    QString value;
    unsigned int ID;                //!< the ID of the environment item
    int index;                      //!< the numeration index of the environment item (changes, e.g., if traffic items in the list are removed)
    Ui::EnvironmentItemView *ui;
};

#endif // ENVIRONMENTITEMVIEW_H
