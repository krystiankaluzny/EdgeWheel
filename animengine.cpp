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

#include <QFile>
#include <QTextStream>

// QFile dupa("/home/ogcc/.config/Obywatel GCC/w_log.txt");
// QTextStream ddupa(&dupa);

#include "animengine.h"
#include <cmath>
#include <QTime>

//========================================AnimEngine========================================

AnimEngine::AnimEngine(QObject *parent) 
	: QObject(parent)
{
	m_timer = new QTimer(this);
    m_timer->setInterval(65);
	number_of_animated_objects = 0;
    speed = 1.7;  //speed < 5
	is_working = false;
    go_forward = true; //następny kierunek animacji
	
	connect(m_timer, SIGNAL(timeout()), this, SLOT(runAnimation()));
// 	dupa.open(QIODevice::WriteOnly | QIODevice::Text);
}

AnimEngine::~AnimEngine()
{
// 	dupa.close();
	m_timer->stop();
	delete m_timer;
    for(AnimeItem*& ad : m_anim_obects)
		delete ad;
}

void AnimEngine::append(QVector<LaunchIcon*>* ring, AnimType type)
{
    AnimIcons* ad = new AnimIcons(ring, this);
    ad->setType(type);
    if(ad->type() == IconsType1) ad->setAnimPos(QPointF(-35, 0));
	else ad->setAnimPos(QPointF(-35, -35));
	m_anim_obects.append(ad);
}

void AnimEngine::append(double* angle, AnimType type)
{
    AnimRotor* ar =  new AnimRotor(angle, this);
    ar->setType(type);
    m_anim_obects.append(ar);
}

void AnimEngine::append(QPainterPath* path, double r1, double r2, AnimType type)
{
    AnimPath* ap = new AnimPath(path, r1, r2, this);
    ap->setType(type);
    m_anim_obects.append(ap);
}

void AnimEngine::append(QPointF* top_left, double* gate_angle, AnimType type)
{
    AnimGate* ag = new AnimGate(top_left, gate_angle, this);
    ag->setType(type);
    m_anim_obects.append(ag);
}

void AnimEngine::checkIconsPosition()
{
	if(is_working) return;
	if(go_forward)
	{
        for(AnimeItem*& ad : m_anim_obects)
            if(ad->type() == AnimType::IconsType1)
                dynamic_cast<AnimIcons*>(ad)->setAnimPos(QPointF(-35, 0));
            else if(ad->type() == AnimType::IconsType2)
                dynamic_cast<AnimIcons*>(ad)->setAnimPos(QPointF(-35, -35));
	}
	else
	{
        for(AnimeItem*& ad : m_anim_obects)
            if(ad->type() == AnimType::IconsType1 || ad->type() == AnimType::IconsType1)
                dynamic_cast<AnimIcons*>(ad)->setAnimPos(QPointF(0, 0), true, false);
	}
}

void AnimEngine::startAnimation()
{
	if(!is_working)
	{
        QTime t = QTime::currentTime();
//         ddupa << "anim end " << go_forward << " " << t.minute() * 60 * 1000 + t.second() * 1000 + t.msec() << "\n";
		is_working = true;
        for(AnimeItem*& ad : m_anim_obects)
            ad->first_stage_done = false;

		number_of_animated_objects = 0;
		m_timer->start();
	}
}

void AnimEngine::runAnimation()
{
	int number_of_objects = 0;
	number_of_animated_objects = 0;
    for(AnimeItem*& ad : m_anim_obects)
	{
		ad->anim();
		number_of_objects += ad->animItemsCount();
	}
	if(number_of_animated_objects >= number_of_objects)
	{
		m_timer->stop();
        is_working = false;
        go_forward = !go_forward;
        QTime t = QTime::currentTime();
//         ddupa << "anim end " << go_forward << " " << t.minute() * 60 * 1000 + t.second() * 1000 + t.msec() << "\n\n";
	}

	emit animed();
}
bool AnimEngine::isShow()
{
	return !go_forward && !is_working;//ma w następnym kroku ma iść do tyłu i nie pracuje
}

//========================================AnimeItem========================================

AnimeItem::AnimeItem(AnimEngine *parent)
{
    m_parent = nullptr;
    m_type = UnknownType;
    first_stage_done = false;
}

AnimEngine *AnimeItem::parent() const
{
    return m_parent;
}

void AnimeItem::setParent(AnimEngine *p)
{
    m_parent = p;
}

AnimType AnimeItem::type() const
{
    return m_type;
}
void AnimeItem::setType(AnimType t)
{
    m_type = t;
}

//========================================AnimIcons========================================

AnimIcons::AnimIcons(AnimEngine* parent)
{
    setParent(parent);
    setType(IconsType1);
	radius = 0;
	first_stage_done = false;
}

AnimIcons::AnimIcons(QVector<LaunchIcon*>* ar, AnimEngine* parent)
{
    setParent(parent);
	anim_ring = ar;
    setType(IconsType1);
	first_stage_done = false;
    radius = ar->size() > 0 ? sqrt(pow(ar->at(0)->pos.x(), 2) + pow(ar->at(0)->pos.y(), 2)) :  0;
}

void AnimIcons::moveAnimPos(QPointF d_pos)
{
	for(int i = 0; i < anim_ring->size(); i++)
		anim_ring->at(i)->anim_pos += d_pos;
}

void AnimIcons::setAnimPos(QPointF pos, bool destination, bool set_radius)
{
	if(set_radius)
	{
		first_stage_done = true;
        if(type() == IconsType1)
		{
			for(int i = 0; i < anim_ring->size(); i++)
				anim_ring->at(i)->anim_pos = QPointF(sqrt(pow(anim_ring->at(i)->pos.x(), 2) + pow(anim_ring->at(i)->pos.y(), 2)), 0);
		}
		else
		{
			double r;
			for(int i = 0; i < anim_ring->size(); i++)
			{
				r = sqrt(pow(anim_ring->at(i)->pos.x(), 2) + pow(anim_ring->at(i)->pos.y(), 2));
				anim_ring->at(i)->anim_pos = QPointF(r * sin_45, r * sin_45);
			}
		}	
	}
	else if(destination)
		for(int i = 0; i < anim_ring->size(); i++)
			anim_ring->at(i)->anim_pos = anim_ring->at(i)->pos;
	else
		for(int i = 0; i < anim_ring->size(); i++)
			anim_ring->at(i)->anim_pos = pos;
}

void AnimIcons::anim()
{
	if(anim_ring->size()==0) return;
    if(radius <= 0 ) return;

	double x, y, r, fi, fi2, fi3, dr;

    if(parent()->go_forward)
    {
        if(!first_stage_done)
        {
            x = anim_ring->at(0)->anim_pos.x();
			y = anim_ring->at(0)->anim_pos.y();
            dr = radius - sqrt(x*x + y*y);

            if(dr < 1) 	setAnimPos(QPointF(0, 0), false, true);
            else if(type() == IconsType1) moveAnimPos(QPointF(dr * 0.5 * (parent()->speed > 2 ? 1.999 : parent()->speed), 0));
            else moveAnimPos(QPointF(dr * 0.35 * (parent()->speed > 2 ? 1.999 : parent()->speed), dr * 0.35 * (parent()->speed > 2 ? 1.999 : parent()->speed)));
        }
        else
        {
			for(int i = 0; i < anim_ring->size(); i++)
            {
                x = anim_ring->at(i)->anim_pos.x();
                y = anim_ring->at(i)->anim_pos.y();
                r = sqrt(pow(x, 2) + pow(y, 2));
				fi2 = atan2(anim_ring->at(i)->pos.x(), anim_ring->at(i)->pos.y());
                fi3 = atan2(x, y);
                fi = (fi3 - fi2) * 0.48 *  parent()->speed;

                if(fabs(fi3 - fi2) < 0.01)
                {
                    anim_ring->at(i)->anim_pos = anim_ring->at(i)->pos;
                    parent()->number_of_animated_objects++;
                }
                else
                {
                    anim_ring->at(i)->anim_pos.setX(cos(fi) * x - sin(fi) * y);
                    anim_ring->at(i)->anim_pos.setY(sin(fi) * x + cos(fi) * y);
                }
            }
        }
    }
    else //go backword
    {
        if(!first_stage_done)
        {
            for(int i = 0; i < anim_ring->size(); i++)
            {
                x = anim_ring->at(i)->anim_pos.x();
                y = anim_ring->at(i)->anim_pos.y();
                r = sqrt(x*x + y*y);
				fi2 = atan2(anim_ring->at(i)->pos.x(), anim_ring->at(i)->pos.y());
				fi3 = atan2(x, y) + 0.03;
                fi = (type() == IconsType1 ? -log(fi3 - fi2 + 1.2) : (fi3 - M_PI_4)) * 0.4 *  parent()->speed;
				
                if(fi3 >= M_PI_2 - 0.03 && type() == IconsType1)
                {
					anim_ring->at(i)->anim_pos = QPointF(r, 0);
                    first_stage_done += false;
                }
                else if(fabs(fi3 - M_PI_4) < 0.05 && type() == IconsType2)
				{
					anim_ring->at(i)->anim_pos = QPointF(r * sin_45, r * sin_45);
                    first_stage_done += false;
				}
                else
                {
                    anim_ring->at(i)->anim_pos.setX(cos(fi) * x - sin(fi) * y);
                    anim_ring->at(i)->anim_pos.setY(sin(fi) * x + cos(fi) * y);
                    first_stage_done += true;
                }
            }
            first_stage_done = !first_stage_done;
        }
        else
        {
            x = anim_ring->at(0)->anim_pos.x()-2;
			y = anim_ring->at(0)->anim_pos.y()-2;
            dr = radius - sqrt(x*x + y*y);

            if(type() == IconsType1)
			{
				if(x < -35)
				{
					setAnimPos(QPointF(-35, 0));
                    parent()->number_of_animated_objects += animItemsCount();
				}
                else moveAnimPos(QPointF(-dr * 2 * (parent()->speed > 2 ? 1.999 : parent()->speed), 0));
			}
			else
			{
				if(x < -35)
				{
					setAnimPos(QPointF(-35, -35));
                    parent()->number_of_animated_objects += animItemsCount();
				}
                else moveAnimPos(QPointF(-dr * 1 * (parent()->speed > 2 ? 1.999 : parent()->speed), -dr * 1 * (parent()->speed > 2 ? 1.999 : parent()->speed)));
			}
			
        }
    }
}

int AnimIcons::animItemsCount() const
{
	return anim_ring->size();
}


//========================================AnimRotor========================================
AnimRotor::AnimRotor(AnimEngine *parent) :
    rotor_angle(nullptr)
{
    setParent(parent);
    setType(RotorType1);
}

AnimRotor::AnimRotor(double* angle, AnimEngine *parent) :
    rotor_angle(angle)
{
    setParent(parent);
    setType(RotorType1);
}

void AnimRotor::anim()
{
    (*rotor_angle) += 1.5 * parent()->speed * (parent()->go_forward ? 1 : -1);
}

int AnimRotor::animItemsCount() const
{
	return 0;
}

//========================================AnimPath========================================
AnimPath::AnimPath(AnimEngine *parent) :
    path(nullptr),
    R1(0),
    R2(0)
{
	angle = 0.0;
    r = 0.0;
    setParent(parent);
    setType(PathType1);
}

AnimPath::AnimPath(QPainterPath* path, double r1, double r2, AnimEngine *parent) :
    R1(r1),
    R2(r2)
{
	angle = 0.0;
    r = r1;
    setParent(parent);
    setType(PathType1);
	this->path = path;
}

void AnimPath::anim()
{
    if(type() == PathType1) anim1();
	else anim2();
}

void AnimPath::anim1()
{
    if(parent()->go_forward)
	{
		if(r < R1) r = R1;
		if(!first_stage_done)
		{
			r += (R2 - R1) / 2;
			if(r >= R2)
			{
				r = R2;
				first_stage_done = true;
			}
			QPainterPath path1, path2;
			path1.arcTo(QRectF(-r, -r, 2*r, 2*r), 0, -15);
			path2.addEllipse(-R1, -R1, 2*R1, 2*R1);
			(*path) = path1 - path2;
		}
		else
		{
			angle += 25;
			if(angle >= 90)
			{
				angle = 90;
                parent()->number_of_animated_objects++;
			}
			QPainterPath path1, path2;
			path1.arcTo(QRectF(-R2, -R2, 2*R2, 2*R2), 0, -angle);
			path2.addEllipse(-R1, -R1, 2*R1, 2*R1);
			(*path) = path1 - path2;
		}
	}
	else
	{
		if(r > R2) r = R2;
		if(!first_stage_done)
		{
			angle -= 9;
			if(angle <= 9)
			{
				angle = 9;
				first_stage_done = true;
			}
			QPainterPath path1, path2;
			path1.arcTo(QRectF(-R2, -R2, 2*R2, 2*R2), 0, -angle);
			path2.addEllipse(-R1, -R1, 2*R1, 2*R1);
			(*path) = path1 - path2;
		}
		else
		{
			r -= (R2 - R1) / 3;
			if(r <= R1)
			{
				r = R1;
                parent()->number_of_animated_objects++;
			}
			QPainterPath path1, path2;
			path1.arcTo(QRectF(-r, -r, 2*r, 2*r), 0, -9);
			path2.addEllipse(-R1, -R1, 2*R1, 2*R1);
			(*path) = path1 - path2;
		}
	}
}

void AnimPath::anim2()
{
    if(parent()->go_forward)
	{
		if(r < R1) r = R1;
		if(!first_stage_done)
		{
			r += (R2 - R1) / 2;
			if(r >= R2)
			{
				r = R2;
				first_stage_done = true;
			}
			QPainterPath path1, path2;
			
			path1.arcTo(QRectF(-r, -r, 2*r, 2*r), -40, -10);
			path2.addEllipse(-R1, -R1, 2*R1, 2*R1);
			(*path) = path1 - path2;
		}
		else
		{
			angle += 10;
			if(angle >= 45)
			{
				angle = 45;
                parent()->number_of_animated_objects++;
			}
			QPainterPath path1, path2;
			path1.arcTo(QRectF(-R2, -R2, 2*R2, 2*R2), -45+angle, -2*angle);
			path2.addEllipse(-R1, -R1, 2*R1, 2*R1);
			(*path) = path1 - path2;
		}
	}
	else
	{
		if(r > R2) r = R2;
		if(!first_stage_done)
		{
			angle -= 9;
			if(angle <= 9)
			{
				angle = 9;
				first_stage_done = true;
			}
			QPainterPath path1, path2;
			path1.arcTo(QRectF(-R2, -R2, 2*R2, 2*R2), -45+angle, -2*angle);
			path2.addEllipse(-R1, -R1, 2*R1, 2*R1);
			(*path) = path1 - path2;
		}
		else
		{
			r -= (R2 - R1) / 3;
			if(r <= R1)
			{
				r = R1;
                parent()->number_of_animated_objects++;
			}
			QPainterPath path1, path2;
			path1.arcTo(QRectF(-r, -r, 2*r, 2*r), -40, -10);
			path2.addEllipse(-R1, -R1, 2*R1, 2*R1);
			(*path) = path1 - path2;
		}
	}
}
	
int AnimPath::animItemsCount() const
{
	return 1;
}



//========================================AnimPath========================================

AnimGate::AnimGate(AnimEngine *parent)
{
    setParent(parent);
    setType(GateType1);
}

AnimGate::AnimGate(QPointF *top_left, double *gate_angle, AnimEngine *parent)
{
    this->top_left = top_left;
    this->gate_angle = gate_angle;
    setParent(parent);
    setType(GateType1);
}

void AnimGate::anim()
{
    double r;
    static double dy = 6;
    if(parent()->go_forward)
    {
        r = sqrt(pow(top_left->x() + 45, 2) + pow(top_left->y() + 13, 2));
        if(r < 82)
        {
            top_left->operator +=(QPointF(0, 6));
            (*gate_angle) += 5;
        }
        else
            parent()->number_of_animated_objects++;
    }
    else
    {
        if(top_left->y() > 6)
        {
            (*gate_angle) -= 5;
            top_left->operator -=(QPointF(0, 6));
        }
        else
        {
            top_left->setX(0);
            top_left->setY(0);
            parent()->number_of_animated_objects++;
        }
    }
}

int AnimGate::animItemsCount() const
{
    return 1;
}
