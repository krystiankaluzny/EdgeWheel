/***************************************************************************
 *   Copyright (C) %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

// Here we avoid loading the header multiple times
#ifndef EDGEWHEEL_HEADER
#define EDGEWHEEL_HEADER

#include <Plasma/Applet>
#include <Plasma/Svg>
#include <QList>
#include <QVector>
#include "animengine.h"
#include "configurationinterfacewidget.h"

class EdgeWheel : public Plasma::Applet
{
    Q_OBJECT
public:
	EdgeWheel(QObject *parent, const QVariantList &args);
    ~EdgeWheel();
	
	void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect& contentsRect);
	void createConfigurationInterface(KConfigDialog *parent);

private slots:
	void animUpdate();
    void onHoverMouseTimer();
	void saveItems();
	void loadItems();
	
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	
private:
	bool createIconRing(QVector< LaunchIcon* > & ring, int nr, double r, int w, int icon_count, double density);
	LaunchIcon* checkContains(QVector< LaunchIcon* >& ring, QPoint& e_pos);
	
    QPainterPath path;
	QVector<LaunchIcon*> ring1;
	QVector<LaunchIcon*> ring2;
	QVector<LaunchIcon*> ring3;
	QVector<LaunchIcon*> ring4;
	QVector<LaunchIcon*> ring5;
	QVector<LaunchIcon*> ring6;
	QPixmap in;
	QPixmap in_ring;
	QPixmap out_ring;
    QPixmap gate;

	double rotor_angle;
    double gate_angle;
    QPointF* gate_top_left;

	QString file_path;
	AnimEngine* anim_engine;
    QTimer* mouse_hover_timer;
	long long int mouse_hover_timer_cycle_counter;
	int animation_type;
	
	ConfigurationInterfaceWidget *interface_widget;
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(edgewheel, EdgeWheel)
#endif
