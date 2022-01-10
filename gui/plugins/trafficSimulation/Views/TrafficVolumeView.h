/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#ifndef TRAFFICVOLUMEVIEW_H
#define TRAFFICVOLUMEVIEW_H

#include <QWidget>

namespace Ui {
class TrafficVolumeView;
}

class TrafficVolumeView : public QWidget
{
    Q_OBJECT

public:
    explicit TrafficVolumeView(unsigned int ID,
                               int index,
                               QWidget *parent = 0);
    ~TrafficVolumeView();

Q_SIGNALS:
    void valueChanged(unsigned int ID, int value);
    void probabilityChanged(unsigned int ID, double value);
    void remove(unsigned int ID);

private Q_SLOTS:
    void on_remove_clicked();
    void on_probabilityValue_editingFinished();
    void on_VolumeValue_editingFinished();

public:
    unsigned int getID() const;
    int getIndex() const;

public:
    void setValue(int value);
    void setProbability(double value);
    void setIndex(int newIndex);

private:
    int index;
    unsigned int ID;
    Ui::TrafficVolumeView *ui;
};

#endif // TRAFFICVOLUMEVIEW_H
