/**
 * @file /src/touchegg/actions/implementation/MaximizeRestoreWindow.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Touchégg, usted puede redistribuirlo y/o
 * modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Touchégg project, you can redistribute it and/or
 * modify it under the terms of the GNU GPL v3.
 *
 * @class  MaximizeRestoreWindow
 * @author Copyright (C) 2011 José Expósito <jose.exposito89@gmail.com>
 */
#include "MaximizeRestoreWindow.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

MaximizeRestoreWindow::MaximizeRestoreWindow(const QString& settings, Window window)
        : Action(settings, window) {}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

void MaximizeRestoreWindow::executeStart(const QHash<QString, QVariant>&) {}

void MaximizeRestoreWindow::executeUpdate(const QHash<QString, QVariant>&) {}

void MaximizeRestoreWindow::executeFinish(const QHash<QString, QVariant>&) {
    Atom atomMaxVert = XInternAtom(QX11Info::display(),
        "_NET_WM_STATE_MAXIMIZED_VERT", false);
    Atom atomMaxHorz = XInternAtom(QX11Info::display(),
        "_NET_WM_STATE_MAXIMIZED_HORZ", false);

    // Vemos si la ventanan está maximizada
    Atom atomRet;
    int size;
    unsigned long numItems, bytesAfterReturn;
    unsigned char* propRet;

    XGetWindowProperty(QX11Info::display(), this->window,
            XInternAtom(QX11Info::display(), "_NET_WM_STATE", false),
            0, 100, false, XA_ATOM, &atomRet, &size, &numItems,
            &bytesAfterReturn, &propRet);
    Atom* states = (Atom*)propRet;

    bool maxHor  = false;
    bool maxVert = false;
    for(unsigned int n=0; n<numItems; n++) {
        if(states[n] == atomMaxVert)
            maxVert = true;
        if(states[n] == atomMaxHorz)
            maxHor = true;
    }
    bool maximized = maxHor && maxVert;
    XFree(propRet);

    // Si la ventana está maximizada la restauramos
    XClientMessageEvent event;
    event.window =this-> window;
    event.type = ClientMessage;
    event.message_type = XInternAtom(QX11Info::display(), "_NET_WM_STATE",
                                     false);
    event.format = 32;
    event.data.l[0] = maximized ? 0 : 1;
    event.data.l[1] = atomMaxVert;
    event.data.l[2] = atomMaxHorz;

    XSendEvent(QX11Info::display(),
            QX11Info::appRootWindow(QX11Info::appScreen()), false,
            (SubstructureNotifyMask | SubstructureRedirectMask),
            (XEvent*)&event);

    XFlush(QX11Info::display());
}

