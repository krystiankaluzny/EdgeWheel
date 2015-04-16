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

#include "edgewheel.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include <QDomDocument>
#include <QDir>
#include <QCursor>
#include <QPushButton>
#include <KConfigDialog>
#include <cmath>

EdgeWheel::EdgeWheel(QObject *parent, const QVariantList &args) :
	Plasma::Applet(parent, args)
{
	setBackgroundHints(NoBackground);
    setHasConfigurationInterface(true);//true - dodatkowa opcja do konfiguracji w contexte menu
    setAcceptHoverEvents(true);
    resize(380, 380);
	anim_engine = nullptr;
	animation_type = 1;
	
	file_path  = QString(getenv("HOME"));
	file_path += "/.config/Obywatel GCC/EdgeWheel/";
    rotor_angle = 0.0;
    gate_angle = 0.0;
    gate_top_left = new QPointF(0, 0);

    mouse_hover_timer = new QTimer(this);
    mouse_hover_timer->setInterval(100);
	mouse_hover_timer_cycle_counter = 0;
    connect(mouse_hover_timer, SIGNAL(timeout()), this, SLOT(onHoverMouseTimer()));
	
	loadItems();
	
    in.load(file_path + "in.png");
    in_ring.load(file_path + "in_ring.png");
    out_ring.load(file_path + "out_ring.png");
    gate.load(file_path + "gate.png");

	anim_engine = new AnimEngine;
	anim_engine->append(&ring1, animation_type  == 1 ? AnimType::IconsType1 : AnimType::IconsType2);
	anim_engine->append(&ring2, animation_type  == 1 ? AnimType::IconsType1 : AnimType::IconsType2);
	anim_engine->append(&ring3, animation_type  == 1 ? AnimType::IconsType1 : AnimType::IconsType2);
	anim_engine->append(&ring4, animation_type  == 1 ? AnimType::IconsType1 : AnimType::IconsType2);
	anim_engine->append(&ring5, animation_type  == 1 ? AnimType::IconsType1 : AnimType::IconsType2);
	anim_engine->append(&ring6, animation_type  == 1 ? AnimType::IconsType1 : AnimType::IconsType2);
	anim_engine->append(&rotor_angle, AnimType::RotorType1);
    anim_engine->append(&path, 106, 380, animation_type  == 1 ? AnimType::PathType1 : AnimType::PathType2);
    anim_engine->append(gate_top_left, &gate_angle, AnimType::GateType1);

	connect(anim_engine, SIGNAL(animed()), this, SLOT(animUpdate()));
}

EdgeWheel::~EdgeWheel()
{
	delete mouse_hover_timer;
	delete anim_engine;
    delete gate_top_left;

// 	saveItems();
	auto clear = [](QVector<LaunchIcon*>& ring)
	{
		while(ring.size())
		{
			delete ring.at(0);
			ring.remove(0);
		}
	};
	clear(ring1);
	clear(ring2);
	clear(ring3);
	clear(ring4);
	clear(ring5);
	clear(ring6);
}

void EdgeWheel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Applet::mousePressEvent(event);
	
	QPoint e_pos(event->pos().x(), event->pos().y());
	LaunchIcon* clicked = nullptr;
	if(event->button() == Qt::LeftButton)
	{
		if(anim_engine->isShow())
		{
			QVector < QVector< LaunchIcon*> > rings = {ring1, ring2, ring3, ring4, ring5, ring6};
			for(QVector< LaunchIcon*>& r : rings)
			{
				clicked = checkContains(r, e_pos);
				if(clicked != nullptr) 
				{
					system(QString((clicked->path + " &")).toUtf8().constData());
					break;
				}
			}
		}
		QPainterPath button_path;
		button_path.addEllipse(-110, -110, 220, 220);
		if(button_path.contains(e_pos))
		{
			anim_engine->startAnimation();
			mouse_hover_timer_cycle_counter = 0;
			mouse_hover_timer->start();
		}
	}
	
	update();
}

void EdgeWheel::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	if(anim_engine->isShow())
	{
		QPoint e_pos(event->pos().x(), event->pos().y());

		checkContains(ring1, e_pos);
		checkContains(ring2, e_pos);
		checkContains(ring3, e_pos);
		checkContains(ring4, e_pos);
		checkContains(ring5, e_pos);
		checkContains(ring6, e_pos);
		update();

		mouse_hover_timer_cycle_counter = 0;
		mouse_hover_timer->start();
	}
}

void EdgeWheel::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	if(anim_engine->isShow())
	{
		QPoint e_pos(event->pos().x(), event->pos().y());

		checkContains(ring1, e_pos);
		checkContains(ring2, e_pos);
		checkContains(ring3, e_pos);
		checkContains(ring4, e_pos);
		checkContains(ring5, e_pos);
		checkContains(ring6, e_pos);
		update();
	}
    mouse_hover_timer->stop();
}

void EdgeWheel::paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->setRenderHint(QPainter::Antialiasing);
	
	int icon_increase = 3;
    p->setBrush(QColor(5, 5, 5, 187));
    p->setPen(QColor(0, 0, 0, 0));
    
    p->drawPath(path);

	auto printRing = [&](QVector<LaunchIcon*>& ring)->void
	{
		for(int i = ring.size()-1; i >=0; i--)
		{
			if(ring.at(i)->hover)
			{
				p->setBrush(QColor(0, 255, 0, 50));
				p->drawPixmap(ring.at(i)->anim_pos.x()-icon_increase, ring.at(i)->anim_pos.y()-icon_increase, ring.at(i)->size.width()+2*icon_increase, ring.at(i)->size.height()+2*icon_increase, ring.at(i)->icon.pixmap(ring.at(i)->size + QSize(2*icon_increase, 2*icon_increase)));
				p->setPen(QColor(0, 0, 0, 0));
				
				if(anim_engine->isShow())
				{
					p->setPen(Qt::red);
					p->setFont(QFont("Times", 15));
					p->drawText(QPoint(ring.at(i)->anim_pos.x() + ring.at(i)->size.width(), ring.at(i)->anim_pos.y()+ring.at(i)->size.height()*3/4), ring.at(i)->description);
				}
			}
			else
			{
				p->drawPixmap(QRect(ring.at(i)->anim_pos.toPoint(), ring.at(i)->size), ring.at(i)->icon.pixmap(ring.at(i)->size));
			}
		}
	};
	
	
	printRing(ring6);
	printRing(ring5);
	printRing(ring4);
	printRing(ring3);
	printRing(ring2);
	printRing(ring1);
	
	p->drawPixmap(0, 0, 82, 82, in);
//    p->save();
//    QRegion region(-82, -82, 164, 164, QRegion::Ellipse);
//    p->setClipRegion(region);
//    p->rotate(gate_angle);
//    for(int i = 0; i < 12; i++)
//    {
//        p->drawPixmap(gate_top_left->x(), gate_top_left->y(), gate.width(), gate.width(), gate);
//        p->rotate(30);
//    }
    p->restore();
	p->save();
	p->rotate(-rotor_angle);
	p->drawPixmap(-108, -108, 216, 216, in_ring);
	p->restore();
	p->save();
	p->rotate(rotor_angle);
	p->drawPixmap(-out_ring.width()/2, -out_ring.width()/2, out_ring.width(), out_ring.width(), out_ring);
	p->restore();
}

void EdgeWheel::createConfigurationInterface(KConfigDialog *parent)
{
	QVector< QVector<LaunchIcon*>* > rings;
	rings << &ring1 << &ring2 << &ring3 << &ring4 << &ring5 << &ring6;
	
	interface_widget = new ConfigurationInterfaceWidget(rings, parent);
	parent->addPage(interface_widget, "Test", "Test1", "Test2");
	
	connect(interface_widget, SIGNAL(loadConfigFile()), this, SLOT(loadItems()));
	connect(interface_widget, SIGNAL(saveConfigFile()), this, SLOT(saveItems()));
	connect(interface_widget, SIGNAL(updateEdge()), this, SLOT(animUpdate()));
}


void EdgeWheel::animUpdate()
{
    update();
}

void EdgeWheel::onHoverMouseTimer()
{
	mouse_hover_timer_cycle_counter++;
    QPoint e_pos = QCursor::pos() - geometry().topLeft().toPoint();

    checkContains(ring1, e_pos);
    checkContains(ring2, e_pos);
    checkContains(ring3, e_pos);
    checkContains(ring4, e_pos);
    checkContains(ring5, e_pos);
    checkContains(ring6, e_pos);
    update();
	if(mouse_hover_timer_cycle_counter > 10 * 60 * 2) // l. cyk / sek * sek /min * min
	{
		mouse_hover_timer->stop();
		mouse_hover_timer_cycle_counter = 0;
	}
}

LaunchIcon* EdgeWheel::checkContains(QVector<LaunchIcon*>& ring, QPoint& e_pos)
{
	LaunchIcon* hovered = nullptr;
	for(int i = 0; i < ring.size(); i++)
		if(QRect(ring.at(i)->pos.toPoint(), ring.at(i)->size).contains(e_pos)) 
		{
			ring.at(i)->hover = true;
			hovered = ring.at(i);
		}
		else ring.at(i)->hover = false;
		
	return hovered;
}

bool EdgeWheel::createIconRing(QVector< LaunchIcon* > & ring, int nr, double r, int w, int icon_count, double density)
{
	while(ring.size())
	{
		delete ring.at(0);
		ring.remove(0);
	}
	QFile file(file_path + "edgewheel.xml");
	QDomDocument document;
	QDomNode root;
	QDomNodeList node_list;
	bool open_file = false;
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		if(document.setContent(&file))
		{
			file.close();
			root = document.firstChildElement("rings");
			animation_type = root.toElement().attribute("animation_type", "1").toInt();
			root = root.firstChildElement("ring"+QString::number(nr));
			node_list = root.childNodes();
			open_file = true;
		}
	}
		
	double angle = (M_PI_4 - (icon_count % 2 ? sqrt(2) / 2 * w / r : 0)) / (icon_count/2);
	double start_angle = angle / 2;
	double icon_angle = start_angle - angle;
	for(int i = 0; i < icon_count/2; i++)
	{
		LaunchIcon* li = new LaunchIcon;
		icon_angle += angle - (i > 0 ? (0.065 - 0.065/(icon_count/2) * (i+1)) * angle : 0);
		li->pos.setX(sin(icon_angle) * r - w/2);
		li->pos.setY(cos(icon_angle) * r - w/2);
		li->anim_pos = li->pos;
		li->size.setWidth(w);
		li->size.setHeight(w);
		if(open_file)
		{
			QDomNode node = node_list.at(i);
			if(!node.isNull())
			{
				QDomElement e = node.toElement();
				li->description = e.attribute("description");
				li->path = e.attribute("path");
				li->icon_path = e.attribute("icon");
				li->icon.addFile(li->icon_path);
			}
		}
		ring.append(li);
	}
	
	if(icon_count % 2) //jeśli icon_count jest nieparzysty
	{
		LaunchIcon* li = new LaunchIcon;
		li->pos.setX(sin(M_PI_4) * r - w/2);
		li->pos.setY(cos(M_PI_4) * r - w/2);
		li->anim_pos = li->pos;
		li->size.setWidth(w);
		li->size.setHeight(w);
		if(open_file)
		{
			QDomNode node = node_list.at(icon_count/2);
			if(!node.isNull())
			{
				QDomElement e = node.toElement();
				li->description = e.attribute("description");
				li->path = e.attribute("path");
				li->icon_path = e.attribute("icon");
				li->icon.addFile(li->icon_path);
			}
		}
		ring.append(li);
	}
	for(int i = icon_count/2 - 1; i >= 0; i--)
	{
		LaunchIcon* li = new LaunchIcon;
		li->pos.setX(ring.at(i)->pos.y());
		li->pos.setY(ring.at(i)->pos.x());
		li->anim_pos = li->pos;
		li->size.setWidth(w);
		li->size.setHeight(w);
		if(open_file)
		{
			QDomNode node = node_list.at(icon_count - i - 1);
			if(!node.isNull())
			{
				QDomElement e = node.toElement();
				li->description = e.attribute("description");
				li->path = e.attribute("path");
				li->icon_path = e.attribute("icon");
				li->icon.addFile(li->icon_path);
			}
		}
		ring.append(li);
	}
	return open_file;
}

void EdgeWheel::saveItems()
{
	QDir dir(file_path);
	if(!dir.exists()) dir.mkpath(file_path);
	
	QFile file(file_path + "edgewheel.xml");
	QDomDocument document;
	QDomElement root = document.createElement("rings");
	root.setAttribute("animation_type", QString::number(animation_type));
	document.appendChild(root);
	
	auto save = [&document, &root](QVector< LaunchIcon* > & ring, int nr)
	{
		QDomElement ring_element = document.createElement("ring"+QString::number(nr));
		ring_element.setAttribute("size", QString::number(ring.size()));
		root.appendChild(ring_element);
		for(int i = 0; i < ring.size(); i++)
		{
			QDomElement element = document.createElement("item");
			element.setAttribute("description", ring.at(i)->description);
			element.setAttribute("path", ring.at(i)->path);
			element.setAttribute("icon", ring.at(i)->icon_path);
			ring_element.appendChild(element);
		}
	};
	save(ring1, 1);
	save(ring2, 2);
	save(ring3, 3);
	save(ring4, 4);
	save(ring5, 5);
	save(ring6, 6);
	
	if(file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		stream << document.toString(4);
		file.close();
	}
}

void EdgeWheel::loadItems()
{
	bool file_no_exist = false;
	file_no_exist += !createIconRing(ring1, 1, 110 + 45.0 / 2 + 2, 32, 5, 1.1);
	file_no_exist += !createIconRing(ring2, 2, 110 + 45.0 / 2 * 3 - 1, 32, 6, 1.1);
	file_no_exist += !createIconRing(ring3, 3, 200 + 45.0 / 2, 32 + 2, 8, 1.1);
	file_no_exist += !createIconRing(ring4, 4, 200 + 45.0 / 2 * 3 - 1, 32, 9, 1.1);
	file_no_exist += !createIconRing(ring5, 5, 290 + 45.0 / 2 , 32 + 2, 11, 1.1);
	file_no_exist += !createIconRing(ring6, 6, 290 + 45.0 / 2 * 3 - 1, 32, 13, 1.1);
	
	if(file_no_exist) saveItems(); //tworzymy szablon pliku 
	if(anim_engine != nullptr)
		anim_engine->checkIconsPosition();
	
	update();
}

#include "edgewheel.moc"
