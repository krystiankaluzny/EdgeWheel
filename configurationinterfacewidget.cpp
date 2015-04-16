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

#include <QFileDialog>

#include "configurationinterfacewidget.h"

ConfigurationInterfaceWidget::ConfigurationInterfaceWidget(QVector< QVector<LaunchIcon*>* > rings, QWidget* parent) :
	QWidget(parent)
{
	this->rings = rings;
	
	v_layout = new QVBoxLayout;
	{
		save_load_layout = new QHBoxLayout;
		{
			l_load_config = new QLabel("Wszystkie niezapisane zmiany\nzostana utracone");
			save_load_layout->addWidget(l_load_config);
			
			pb_reload = new QPushButton("Wczytaj z edgewheel.xml");
			connect(pb_reload, SIGNAL(clicked(bool)), this, SLOT(onPushButtonReload()));
			save_load_layout->addWidget(pb_reload);
		}
		v_layout->addLayout(save_load_layout);
		
		group_box = new QGroupBox;
		v_grup_box_layout = new QVBoxLayout;
		{
			choose_icon_layout = new QHBoxLayout;
			{
				l_ring = new QLabel("Pierscien:");
				choose_icon_layout->addWidget(l_ring);
				
				sb_ring = new QSpinBox;
				sb_ring->setMinimum(1);
				sb_ring->setMaximum(rings.size());
				connect(sb_ring, SIGNAL(valueChanged(int)), this, SLOT(setIconsCount(int)));
				choose_icon_layout->addWidget(sb_ring);
				
				l_icon = new QLabel("Ikona:");
				choose_icon_layout->addWidget(l_icon);
				
				sb_icon = new QSpinBox;
				connect(sb_icon, SIGNAL(valueChanged(int)), this, SLOT(iconNumberChanged(int)));
				choose_icon_layout->addWidget(sb_icon);
				
				splitter2 = new QSplitter;
				splitter2->setOrientation(Qt::Horizontal);
				choose_icon_layout->addWidget(splitter2);
			}
			v_grup_box_layout->addLayout(choose_icon_layout);
			
			path_layout = new QHBoxLayout;
			{
				l_path = new QLabel("Polecenie:");
				path_layout->addWidget(l_path);
				
				le_path = new QLineEdit;
				path_layout->addWidget(le_path);
				
				pb_path = new QPushButton("...");
				connect(pb_path, SIGNAL(clicked()), this, SLOT(onPathPushButtonClicked()));
				path_layout->addWidget(pb_path);
			}
			v_grup_box_layout->addLayout(path_layout);
			
			icon_layout = new QHBoxLayout;
			{
				l_icon2 = new QLabel("Sciezka ikony:");
				icon_layout->addWidget(l_icon2);
				
				le_icon = new QLineEdit;
				icon_layout->addWidget(le_icon);
				
				pb_icon = new QPushButton("...");
				connect(pb_icon, SIGNAL(clicked()), this, SLOT(onIconPushButtonClicked()));
				icon_layout->addWidget(pb_icon);
			}
			v_grup_box_layout->addLayout(icon_layout);
			
			description_layout = new QHBoxLayout;
			{
				l_description = new QLabel("Opis:");
				description_layout->addWidget(l_description);
				
				le_description = new QLineEdit;
				description_layout->addWidget(le_description);
			}
			v_grup_box_layout->addLayout(description_layout);
			
			save_launcher_layout = new QHBoxLayout;
			{
				l_save_launcher = new QLabel("Zapisac zmiany w aktywatorze?");
				save_launcher_layout->addWidget(l_save_launcher);
				
				pb_save_launcher = new QPushButton("Zapisz");
				connect(pb_save_launcher, SIGNAL(clicked()), this, SLOT(onSaveLaunchIcon()));
				save_launcher_layout->addWidget(pb_save_launcher);
			}
			v_grup_box_layout->addLayout(save_launcher_layout);
		}
		group_box->setLayout(v_grup_box_layout);
		v_layout->addWidget(group_box);
		
		splitter = new QSplitter;
		splitter->setOrientation(Qt::Vertical);
		v_layout->addWidget(splitter);
	}
	setLayout(v_layout);
	
	setIconsCount(1);
	iconNumberChanged(1);
}

ConfigurationInterfaceWidget::~ConfigurationInterfaceWidget()
{
	//usuwaj w kolejności od najgłębszych dzieci
	delete pb_save_launcher;
	delete pb_reload;
	delete splitter2;
	delete l_save_launcher;
	delete l_ring;
	delete l_icon;
	delete l_path;
	delete l_icon2;
	delete l_description;
	delete l_load_config;
	delete sb_ring;
	delete sb_icon;
	delete le_path;
	delete le_icon;
	delete le_description;
	delete pb_path;
	delete pb_icon;
	
	delete save_launcher_layout;
	delete description_layout;
	delete icon_layout;
	delete path_layout;
	delete choose_icon_layout;
	delete splitter;
	delete v_grup_box_layout;
	delete group_box;
	delete save_load_layout;
	delete v_layout;
}

void ConfigurationInterfaceWidget::onPushButtonReload()
{
	emit loadConfigFile();
	iconNumberChanged(sb_icon->value());
}

void ConfigurationInterfaceWidget::setIconsCount(int s)
{
	sb_icon->setMinimum(1);
	sb_icon->setMaximum(rings.size() >= s ? rings.at(s-1)->size() : 1);
	sb_icon->setValue(1);
	iconNumberChanged(1);
}

void ConfigurationInterfaceWidget::iconNumberChanged(int s)
{
	if(!rings.size()) return;
	LaunchIcon * i = rings.at(sb_ring->value()-1)->at(s-1);
	if(i == nullptr) return;
	le_path->setText(i->path);
	le_icon->setText(i->icon_path);
	le_description->setText(i->description);
}

void ConfigurationInterfaceWidget::onPathPushButtonClicked()
{
	QString file_name = QFileDialog::getOpenFileName(this, "Plik do uruchomienia");
	if(!file_name.isEmpty())
		le_path->setText(file_name);
}

void ConfigurationInterfaceWidget::onIconPushButtonClicked()
{
	QString file_name = QFileDialog::getOpenFileName(this, "Ikona aktywatora", "", tr("Obrazy (*.png *.jpg *.jpeg *.bmp *.ico)"));
	if(!file_name.isEmpty())
		le_icon->setText(file_name);
}

void ConfigurationInterfaceWidget::onSaveLaunchIcon()
{
	if(rings.size() == 0) return;
	LaunchIcon * i = rings.at(sb_ring->value()-1)->at(sb_icon->value()-1);
	if(i == nullptr) return;
	
	i->path = le_path->text();
	i->icon_path = le_icon->text();
	KIcon icon;
	icon.addFile(i->icon_path);
	i->icon.swap(icon);
	i->description = le_description->text();
	
	emit updateEdge();
	emit saveConfigFile();
}