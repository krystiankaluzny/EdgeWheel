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


#ifndef ANIMENGINE_H
#define ANIMENGINE_H

#include <KIcon>
#include <QTimer>
#include <QPainterPath>

struct LaunchIcon
{
	LaunchIcon(){ hover = false; }
	KIcon icon;
	QPointF pos;
	QPointF anim_pos;
	QSize size;
	QString description;
	QString path;
	QString icon_path;
	bool hover;
};

enum AnimType
{
	IconsType1,
	IconsType2,
    RotorType1,
    GateType1,
    PathType1,
    PathType2,
    UnknownType
};

class AnimEngine;

class AnimeItem
{
public:
	AnimeItem(AnimEngine *parent = nullptr);
	virtual void anim() = 0;
    virtual int animItemsCount() const = 0;
	
	AnimEngine* parent() const;
    void setParent(AnimEngine* p);
    AnimType type() const;
    void setType(AnimType t);
	
    bool first_stage_done;
private:
    AnimEngine* m_parent;
    AnimType m_type;
};

//jakiś tekst
class AnimIcons : public AnimeItem
{
public:
    explicit AnimIcons(AnimEngine *parent = nullptr);
    AnimIcons(QVector<LaunchIcon*>* ar, AnimEngine* parent = nullptr);
	void moveAnimPos(QPointF d_pos);
	void setAnimPos(QPointF pos, bool destination = false, bool set_radius = false);
	void anim();
	virtual int animItemsCount() const;

private:
	static constexpr double sin_45 = 0.707107;
	QVector<LaunchIcon*>* anim_ring;
	double radius;
};

class AnimRotor : public AnimeItem
{
public:
    explicit AnimRotor(AnimEngine *parent = nullptr);
    AnimRotor(double* angle, AnimEngine *parent = nullptr);
    void anim();
    int animItemsCount() const;
	
private:
	double* rotor_angle;
};

class AnimPath : public AnimeItem
{
public:
    explicit AnimPath(AnimEngine* parent = nullptr);
    AnimPath(QPainterPath* path, double r1, double r2, AnimEngine* parent = nullptr);
    void anim();
    int animItemsCount() const;

private:
	void anim1();
	void anim2();

    double angle;
    double r;
    QPainterPath* path;
	const double R1;
	const double R2;
};

class AnimGate : public AnimeItem
{
public:
    explicit AnimGate(AnimEngine *parent = nullptr);
    AnimGate(QPointF* top_left, double* gate_angle, AnimEngine *parent = nullptr);
    void anim();
    int animItemsCount() const;

private:
    QPointF* top_left;
    double* gate_angle;
};

class AnimEngine : public QObject
{
	Q_OBJECT
public:	
	AnimEngine(QObject *parent = nullptr);
	~AnimEngine();
	void append(QVector<LaunchIcon*>* ring, AnimType type);
	void append(double* angle, AnimType type);
	void append(QPainterPath* path, double r1, double r2, AnimType type);
    void append(QPointF* top_left, double* gate_angle, AnimType type);
	void checkIconsPosition();
	void startAnimation();
	bool isShow();
	
signals:
	void animed();
	
private slots:
	void runAnimation();
	
private:
    QVector<AnimeItem*> m_anim_obects;
	QTimer* m_timer;
	bool is_working;
	bool go_forward;
	
	int number_of_animated_objects;
	double speed;
	
	friend AnimIcons;
	friend AnimRotor;
	friend AnimPath;
    friend AnimGate;
};

#endif // ANIMENGINE_H
