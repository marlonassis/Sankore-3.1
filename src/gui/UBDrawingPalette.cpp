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



#include "UBDrawingPalette.h"

#include <QtGui>

#include "UBDrawingPolygonPalette.h"
#include "UBMainWindow.h"

#include "core/UBApplication.h"
#include "core/UBSettings.h"
#include "core/UBApplicationController.h"

#include "board/UBDrawingController.h"

#include "frameworks/UBPlatformUtils.h"

#include "board/UBBoardController.h"
#include "board/UBBoardPaletteManager.h"

#include <QActionGroup>
#include "domain/UBGraphicsRegularPathItem.h"
#include "UBDrawingStrokePropertiesPalette.h"
#include "UBDrawingFillPropertiesPalette.h"

UBDrawingPalette::UBDrawingPalette(QWidget *parent, Qt::Orientation orient)
    : UBActionPalette(Qt::TopLeftCorner, parent, orient)
{
    UBActionPaletteButton * btnSubPaletteShape = addButtonSubPalette(new UBShapesPalette(Qt::Horizontal, parentWidget()));
    UBActionPaletteButton * btnSubPaletteRegularShape = addButtonSubPalette(new UBRegularShapesPalette(Qt::Horizontal, parentWidget()));
    UBActionPaletteButton * btnSubPalettePolygon = addButtonSubPalette(new UBDrawingPolygonPalette(Qt::Horizontal, parentWidget()));
    addButtonSubPalette(new UBDrawingStrokePropertiesPalette(Qt::Horizontal, parentWidget()), UBApplication::mainWindow->actionStrokeProperties);
    addButtonSubPalette(new UBDrawingFillPropertiesPalette(Qt::Horizontal, parentWidget()), UBApplication::mainWindow->actionFillProperties);
    UBActionPaletteButton * btnPaintBucket = addActionButton(UBApplication::mainWindow->actionChangeFillingColor);

    // Certains de ces boutons sont groupés
    mButtonGroup = new QButtonGroup(this);
    mButtonGroup->addButton(btnSubPaletteShape);
    mButtonGroup->addButton(btnSubPaletteRegularShape);
    mButtonGroup->addButton(btnSubPalettePolygon);
    mButtonGroup->addButton(btnPaintBucket);    

    initSubPalettesPosition();

    adjustSizeAndPosition();
}



UBActionPaletteButton * UBDrawingPalette::addButtonSubPalette(UBAbstractSubPalette * subPalette, QAction* action)
{
    UBActionPaletteButton * button = 0;
    if (action == 0 && subPalette->actions().size()>0)
    {
        action = subPalette->actions()[0];
    }

    if (action != 0)
    {
        button = new UBActionPaletteButton(action, this);
        button->setToolButtonStyle(mToolButtonStyle);
        subPalette->setActionPaletteButtonParent(button);
        mSubPalettes[button] = subPalette;
        layout()->addWidget(button);
        layout()->setAlignment(button,Qt::AlignHCenter | Qt::AlignVCenter);
        connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    }
    return button;
}

UBActionPaletteButton * UBDrawingPalette::addActionButton(QAction * action)
{
    UBActionPaletteButton * actionButton = new UBActionPaletteButton(action);

    layout()->addWidget(actionButton);
    layout()->setAlignment(actionButton, Qt::AlignHCenter | Qt::AlignVCenter);

    connect(action, SIGNAL(triggered()), this, SLOT(buttonClicked()));

    return actionButton;
}

UBDrawingPalette::~UBDrawingPalette()
{

}

void UBDrawingPalette::buttonClicked()
{
    UBActionPaletteButton * button = dynamic_cast<UBActionPaletteButton *>( sender() );
    if (button && mSubPalettes.contains(button))
    {
        initSubPalettesPosition();
        mSubPalettes.value(button)->togglePalette();
    }

    // Terminer tout dessin en cours de tracé (polygone)
    //UBApplication::boardController->shapeFactory().desactivate();

}


void UBDrawingPalette::initPosition()
{
    // Rem : positions would be very different if drawingPalette were horizontal...

    mCustomPosition = true;

    int x = 0;
    int y = 0;

    // By default, place the drawingPalette next to the Library (RightPalette)
    if (UBApplication::boardController
            && UBApplication::boardController->paletteManager()
            && UBApplication::boardController->paletteManager()->rightPalette())
    {
        x = UBApplication::boardController->paletteManager()->rightPalette()->pos().x();
        x -= this->width();
    }

    // By default, center vertically the drawingPalette in her parent
    y = ( parentWidget()->height() - this->height() ) / 2;

    moveInsideParent(QPoint(x, y));

    initSubPalettesPosition(); // place the subPalettes next to the palette.
}

void UBDrawingPalette::initSubPalettesPosition()
{
    foreach(UBActionPaletteButton* button, mSubPalettes.keys())
    {
        UBAbstractSubPalette* subPalette = mSubPalettes.value(button);

        // Depending on position of palette,
        int x = this->x() + this->width(); // place subPalette on the right of the palette ...
        if (this->x() > parentWidget()->width()/2)
        {
            x = this->x() - subPalette->width(); // ... or on the left on the palette.
        }
        // Align vertically the center of subPalette to center of button :
        int y = this->y() + button->y() + button->height()/2 - subPalette->height()/2;

        subPalette->move(x, y);
    }
}

void UBDrawingPalette::setVisible(bool checked)
{
    UBActionPalette::setVisible(checked);

    if ( ! checked)
    {
        foreach(UBAbstractSubPalette* subPalette, mSubPalettes.values())
        {
            subPalette->hide();
        }
    }
}

void UBDrawingPalette::mouseMoveEvent(QMouseEvent *event)
{
    QPoint oldPos = this->pos();

    UBActionPalette::mouseMoveEvent(event);

    QPoint delta = this->pos() - oldPos;
    updateSubPalettesPosition(delta);
}

void UBDrawingPalette::updateSubPalettesPosition(const QPoint& delta)
{
    foreach (UBAbstractSubPalette* subPalette, mSubPalettes.values()) {
        QPoint newPos = subPalette->pos() + delta;
        subPalette->move(newPos);
    }
}

void UBDrawingPalette::stackUnder(QWidget * w)
{
    UBActionPalette::stackUnder(w);

    // For all subpalettes :
    foreach (UBAbstractSubPalette* subPalette, mSubPalettes.values()) {
        subPalette->stackUnder(w);
    }
}

void UBDrawingPalette::raise()
{
    UBActionPalette::raise();

    // For all subpalettes :
    foreach (UBAbstractSubPalette* subPalette, mSubPalettes.values()) {
        subPalette->raise();
    }
}

