//-----------------------------------------------------------------------------
//! @file  TrafficParametersPresenter.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the presenter object which handles events coming from
//!        the view (TrafficParametersView) and edits the model accordingly (TrafficInterface),
//!        i.e. more specifically the parameters therein.
//!
//! @note  This presenter class emits signals after editing the model which are noticed by the
//!        TrafficDisplayPresenter class. Normally, these signals should be emitted by the model
//!        itself but since it invokes template class instances, for which Qt does not support the signal-slot
//!        mechanism, signals are exceptionally handled by this presenter class.
//-----------------------------------------------------------------------------
#ifndef TRAFFICPARAMETERSPRESENTER_H
#define TRAFFICPARAMETERSPRESENTER_H

#include "openPASS-TrafficSimulation/TrafficInterface.h"
#include "Views/TrafficParametersView.h"


#include <QObject>

//-----------------------------------------------------------------------------
//! @brief This class defines the presenter object which handles events coming from
//!        the view (TrafficParametersView) and edits the model accordingly (TrafficInterface),
//!        i.e. more specifically the parameters therein.
//!
//! @note  This presenter class emits signals after editing the model which are noticed by the
//!        TrafficDisplayPresenter class. Normally, these signals should be emitted by the model
//!        itself but since it invokes template class instances, for which Qt does not support the signal-slot
//!        mechanism, signals are exceptionally handled by this presenter class.
//-----------------------------------------------------------------------------
class TrafficParametersPresenter : public QObject
{
    Q_OBJECT
public:
    explicit TrafficParametersPresenter(TrafficInterface * traffic,
                                        TrafficParametersView * trafficParametersView,
                                        QObject *parent = 0);
    virtual ~TrafficParametersPresenter() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when a traffic volume has been added to the model.
    //-----------------------------------------------------------------------------
    void volume_added();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when a traffic volume has been removed from the model.
    //-----------------------------------------------------------------------------
    void volume_removed();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the probability of a traffic volume has been changed
    //-----------------------------------------------------------------------------
    void volume_probabilityChanged();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when a platoon rate has been added to the model.
    //-----------------------------------------------------------------------------
    void platoonRate_added();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when a platoon rate has been removed from the model.
    //-----------------------------------------------------------------------------
    void platoonRate_removed();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the probability of a platoon rate has been changed
    //-----------------------------------------------------------------------------
    void platoonRate_probabilityChanged();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when a velocity has been added to the model.
    //-----------------------------------------------------------------------------
    void velocity_added();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when a velocity has been removed from the model.
    //-----------------------------------------------------------------------------
    void velocity_removed();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the probability of a velocity item has been changed
    //-----------------------------------------------------------------------------
    void velocity_probabilityChanged();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when a homogeneity has been added to the model.
    //-----------------------------------------------------------------------------
    void homogeneity_added();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when a homogeneity has been removed from the model.
    //-----------------------------------------------------------------------------
    void homogeneity_removed();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the probability of a homogeneity item has been changed
    //-----------------------------------------------------------------------------
    void homogeneity_probabilityChanged();
    //-----------------------------------------------------------------------------
    //! Signal, emitted when a the entire configuration has been cleared in the model.
    //-----------------------------------------------------------------------------
    void cleared();

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the add-button has been clicked in the volumes
    //! section of TrafficParametersView. Adds a traffic volume with default value
    //! 500 and probability 0.5 to the model, creates the corresponding view and inserts
    //! it into the volumes section of TrafficParametersView.
    //-----------------------------------------------------------------------------
    void addVolume();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the add-button has been clicked in the platoon rates
    //! section of TrafficParametersView. Adds a platoon rate with default value
    //! 0.5 and probability 0.5 to the model, creates the corresponding view and inserts
    //! it into the platoon rates section of TrafficParametersView.
    //-----------------------------------------------------------------------------
    void addPlatoonRate();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the add-button has been clicked in the velocities
    //! section of TrafficParametersView. Adds a velocity with default value
    //! 50.0 and probability 0.5 to the model, creates the corresponding view and inserts
    //! it into the velocities section of TrafficParametersView.
    //-----------------------------------------------------------------------------
    void addVelocity();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the add-button has been clicked in the homogeneities
    //! section of TrafficParametersView. Adds a homogeneity with default value
    //! 0.5 and probability 0.5 to the model, creates the corresponding view and inserts
    //! it into the homogeneities section of TrafficParametersView.
    //-----------------------------------------------------------------------------
    void addHomogeneity();

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the remove-button of some traffic item has been clicked.
    //! Removes the traffic item of the transmitted ID and type from the model and destroys
    //! the corresponding view.
    //!
    //! @param[in]      id      the ID of the traffic item to be removed
    //! @param[in]      type    the type of the traffic item to be removed
    //-----------------------------------------------------------------------------
    void removeTrafficItem(unsigned int id, TrafficItemView::Type type);

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the displayed probability value of a traffic item
    //! has been modified by the user. Will adopt the transmitted probability for the model
    //! item's probability specified by the transmitted ID and type.
    //!
    //! @param[in]      id      the ID of the traffic item
    //! @param[in]      value   the probability value to be adopted
    //! @param[in]      type    the type of the traffic item
    //-----------------------------------------------------------------------------
    void setProbability(unsigned int id,  double value, TrafficItemView::Type type);

    //-----------------------------------------------------------------------------
    //! Slot which is triggered when the displayed value of a traffic item
    //! has been modified by the user. Will adopt the transmitted value for the model
    //! item's value specified by the transmitted ID and type.
    //!
    //! @param[in]      id      the ID of the traffic item
    //! @param[in]      value   the value to be adopted for the model item
    //! @param[in]      type    the type of the traffic item
    //-----------------------------------------------------------------------------
    void setValue(unsigned int id, QString value, TrafficItemView::Type type);

    //-----------------------------------------------------------------------------
    //! Slot which deletes all traffic item views.
    //-----------------------------------------------------------------------------
    void clearTrafficParameters();

public:
    //-----------------------------------------------------------------------------
    //! Method which updates the whole TrafficParametersView widget to display the
    //! entire model configuration. Will be called after loading a combination xml
    //! file.
    //-----------------------------------------------------------------------------
    void updateView();

private:
    TrafficInterface *traffic;                      //!< a pointer to the traffic model to be edited by this presenter
    TrafficParametersView * trafficParametersView;  //!< a pointer to the editor widget whose user events are managed by this presenter


private:
    int const default_volume = 600;
    double const default_platoonRate = 0.5;
    double const default_velocity = 90.0;
    double const default_homogeneity = 0.8;
    double const default_probability = 0.5;
};

#endif // TRAFFICPARAMETERSPRESENTER_H
