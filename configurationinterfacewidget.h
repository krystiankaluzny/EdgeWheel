/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2015  <copyright holder> <email>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef CONFIGURATIONINTERFACEWIDGET_H
#define CONFIGURATIONINTERFACEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QVector>


#include "animengine.h"

struct LaunchIcon;

class ConfigurationInterfaceWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ConfigurationInterfaceWidget(QVector< QVector<LaunchIcon*>* > rings,  QWidget* parent = 0);
	~ConfigurationInterfaceWidget();
	
signals:
	void loadConfigFile();
	void saveConfigFile();
	void updateEdge();
	
private slots:
	void onPushButtonReload();
	void setIconsCount(int s);
	void iconNumberChanged(int s);
	void onPathPushButtonClicked();
	void onIconPushButtonClicked();
	void onSaveLaunchIcon();
	
private:
	QVector< QVector<LaunchIcon*>* > rings;
	
	QGroupBox* group_box;
	QPushButton* pb_reload;
	QPushButton* pb_save_launcher;
	QHBoxLayout* save_load_layout;
	QHBoxLayout* choose_icon_layout;
	QHBoxLayout* path_layout;	
	QHBoxLayout* icon_layout;	
	QHBoxLayout* description_layout;
	QHBoxLayout* save_launcher_layout;
	QVBoxLayout* v_layout;
	QVBoxLayout* v_grup_box_layout;
	QSplitter* splitter;
	QSplitter* splitter2;
	QLabel* l_ring;
	QLabel* l_icon;
	QLabel* l_path;
	QLabel* l_icon2;
	QLabel* l_description;
	QLabel* l_save_launcher;
	QLabel* l_load_config;
	QSpinBox* sb_ring;
	QSpinBox* sb_icon;
	QLineEdit* le_path;
	QLineEdit* le_icon;
	QLineEdit* le_description;
	QPushButton* pb_path;
	QPushButton* pb_icon;
};

#endif // CONFIGURATIONINTERFACEWIDGET_H
