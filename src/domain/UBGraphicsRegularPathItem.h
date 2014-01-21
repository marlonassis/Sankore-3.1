/*
 * Copyright (C) 2010-2013 Groupement d'Intérêt Public pour l'Education Numérique en Afrique (GIP ENA)
 *
 * This file is part of Open-Sankoré.
 *
 * Open-Sankoré is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * Open-Sankoré is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Open-Sankoré.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UBGRAPHICSREGULARSHAPES_H
#define UBGRAPHICSREGULARSHAPES_H

#include <QGraphicsPathItem>
#include "UBShape.h"

class UBGraphicsRegularPathItem : public QGraphicsPathItem, public UBShape
{
    public:

        enum NVertices
        {
            Triangle = 3,
            Carre,
            Pentagone,
            Hexagone,
            Heptagone,
            Octogone
        };

        UBGraphicsRegularPathItem(int nVertices = 3, QPointF startPoint = QPointF(0,0), QGraphicsItem* parent = 0);

        ~UBGraphicsRegularPathItem();

        void createGraphicsRegularPathItem();

        void addPoint(const QPointF &point);
        inline bool isClosed() const {return mClosed;}
        inline void setClosed(bool closed);

        void updatePath(QPointF newPos);
        void setStartPoint(QPointF pos);


        // UBItem interface
        UBItem *deepCopy() const;
        void copyItemParameters(UBItem *copy) const;

        // QGraphicsItem interface
        enum { Type = UBGraphicsItemType::GraphicsRegularPathItemType };
        virtual int type() const { return Type; }        
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    protected:
        // QGraphicsItem interface
        virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    private:
        void initialize();
        bool mClosed;
        int mNVertices;
        QList<QPair<double, double>> mVertices;
        QPointF mStartPoint;

};

#endif // UBGRAPHICSREGULARSHAPES_H
