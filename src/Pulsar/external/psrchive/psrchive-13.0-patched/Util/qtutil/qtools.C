/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>
using namespace std;

#include "qtools.h"

bool qevent_trace::eventFilter (QObject *k, QEvent *e)
{
  std::cerr << "QEventTrace: " << k->name() << " receives ";

  switch (e->type()) {
  case QEvent::None:
    std::cerr << "NONE" << std::endl;
    break;
  case QEvent::Timer:
    std::cerr << "TIMER" << std::endl;
    break;
  case QEvent::MouseButtonPress:
    std::cerr << "MOUSE BUTTON PRESS" << std::endl;
    break;
  case QEvent::MouseButtonRelease:
    std::cerr << "MOUSE BUTTON RELEASE" << std::endl;
    break;
  case QEvent::MouseButtonDblClick:
    std::cerr << "DOUBLE CLICK" << std::endl;
    break;
  case QEvent::MouseMove:
    std::cerr << "MOUSE MOVE" << std::endl;
    break;
  case QEvent::KeyPress:
    std::cerr << "KEY PRESS" << std::endl;
    break;
  case QEvent::KeyRelease:
    std::cerr << "KEY RELEASE" << std::endl;
    break;
  case QEvent::FocusIn:
    std::cerr << "FOCUS IN" << std::endl;
    break;
  case QEvent::FocusOut:
    std::cerr << "FOCUS OUT" << std::endl;
    break;  
  case QEvent::Enter:
    std::cerr << "ENTER" << std::endl;
    break;
  case QEvent::Leave:
    std::cerr << "LEAVE" << std::endl;
    break;
  case QEvent::Paint:
    std::cerr << "PAINT" << std::endl;
    break;
  case QEvent::Move:
    std::cerr << "MOVE" << std::endl;
    break;
  case QEvent::Resize:
    std::cerr << "RESIZE" << std::endl;
    break;
  case QEvent::Create:
    std::cerr << "CREATE" << std::endl;
    break;
  case QEvent::Destroy:
    std::cerr << "DESTROY" << std::endl;
    break;
  case QEvent::Show:
    std::cerr << "SHOW" << std::endl;
    break;
  case QEvent::Hide:
    std::cerr << "HIDE" << std::endl;
    break;
  case QEvent::Close:
    std::cerr << "CLOSE" << std::endl;
    break;
  case QEvent::Quit:
    std::cerr << "QUIT" << std::endl;
    break;
  case QEvent::Accel:
    std::cerr << "ACCEL" << std::endl;
    break;
  case QEvent::Wheel:
    std::cerr << "WHEEL" << std::endl;
    break;
  case QEvent::AccelAvailable:
    std::cerr << "AccelAvailable" << std::endl;
    break;
  case QEvent::Clipboard:
    std::cerr << "Clipboard" << std::endl;
    break;
  case QEvent::SockAct:
    std::cerr << "SockAct" << std::endl;
    break;
  case QEvent::DragEnter:
    std::cerr << "DragEnter" << std::endl;
    break;
  case QEvent::DragMove:
    std::cerr << "DragMove" << std::endl;
    break;
  case QEvent::DragLeave:
    std::cerr << "DragLeave" << std::endl;
    break;
  case QEvent::Drop:
    std::cerr << "Drop" << std::endl;
    break;
  case QEvent::DragResponse:
    std::cerr << "DragResponse" << std::endl;
    break;
  case QEvent::ChildInserted:
    std::cerr << "CHILD INSERTED" << std::endl;
    break;
  case QEvent::ChildRemoved:
    std::cerr << "CHILD REMOVED" << std::endl;
    break;
  case QEvent::LayoutHint:
    std::cerr << "LAYOUT HINT" << std::endl;
    break;
  case QEvent::ActivateControl:
    std::cerr << "ACTIVATE CONTROL" << std::endl;
    break;
  case QEvent::DeactivateControl:
    std::cerr << "DE-ACTIVATE CONTROL" << std::endl;
    break;
  case QEvent::User:
    std::cerr << "USER" << std::endl;
    break;
  default:
    std::cerr << "INVALID!" << std::endl;
    break;
  }
  return false;  // standard event processing
}

std::ostream & operator<< (std::ostream & s, const QSize & sz) 
{
  if (sz.isValid ())
    return s << "(" << sz.width() << "," << sz.height() << ")";
  else
    return s << "(invalid)";
}
