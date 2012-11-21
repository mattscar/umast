#ifndef PROPERTYBROWSER_H
#define PROPERTYBROWSER_H

#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "../graphic_editor/spline_data.h"

#include <QDebug>

class PropertyBrowser : public QtTreePropertyBrowser {

  Q_OBJECT

public:
  PropertyBrowser();

  void setSplineData(struct SplineProperties*);

  QtIntPropertyManager *lodManager;
  QtIntPropertyManager *ctrlManager;
  QtStringPropertyManager *coordsManager;
  QtDoublePropertyManager *weightManager;

private:
  // Display LOD of spline
  QtProperty *lod;

  // Display ID of selected object
  QtProperty *ctrl_id;

  // Display coords of selected object
  QtProperty *coords;

  // Display weight of selected object
  QtProperty *weight;
};

#endif
