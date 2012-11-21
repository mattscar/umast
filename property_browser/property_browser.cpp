#include "property_browser.h"
#include "qteditorfactory.h"

PropertyBrowser::PropertyBrowser() : QtTreePropertyBrowser() {

  QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory();
  QtLineEditFactory *lineEditFactory = new QtLineEditFactory();
  QtDoubleSpinBoxFactory *doubleSpinBoxFactory = new QtDoubleSpinBoxFactory();

  // Display level of detail
  lodManager = new QtIntPropertyManager;
  lod = lodManager->addProperty("Level of Detail:");
  lod->setToolTip("Number of spline points per control point");
  lodManager->setValue(lod, 20);
  setFactoryForManager(lodManager, spinBoxFactory);
  addProperty(lod);

  // Display ID of control point
  ctrlManager = new QtIntPropertyManager;
  ctrl_id = ctrlManager->addProperty("Control Point:");
  ctrl_id->setToolTip("ID of control point");
  ctrlManager->setValue(ctrl_id, -1);
  setFactoryForManager(ctrlManager, spinBoxFactory);
  addProperty(ctrl_id);

  // Display coordinates of control point
  coordsManager = new QtStringPropertyManager();
  coords = coordsManager->addProperty("Coordinates:");
  coords->setToolTip("Coordinates of control point");
  coordsManager->setValue(coords, QString(""));
  setFactoryForManager(coordsManager, lineEditFactory);
  addProperty(coords);

  // Display weight of selected object;
  weightManager = new QtDoublePropertyManager();
  weight = weightManager->addProperty("Weight:");
  weight->setToolTip("Weight of control point");
  weightManager->setValue(weight, 1.0);
  setFactoryForManager(weightManager, doubleSpinBoxFactory);
  addProperty(weight);
}

void PropertyBrowser::setSplineData(struct SplineProperties *props) {

  lodManager->setValue(lod, props->lod);
  ctrlManager->setValue(ctrl_id, props->ctrl);
  coordsManager->setValue(coords, QString("(%1, %2, %3)").arg(props->coords.x, 0, 'g', 4).arg(props->coords.y, 0, 'g', 4).arg(props->coords.z, 0, 'g', 4));
  weightManager->setValue(weight, props->coords.w);
}
