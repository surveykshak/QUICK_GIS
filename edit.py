import os
import re

src_path = r"C:\Users\chowd\Documents\GitHub\surveykshak\src\app\qgsstatusbarcoordinateswidget.cpp"
dst_path = r"C:\Users\chowd\Documents\GitHub\QuickGIS\src\app\qgsstatusbarcoordinateswidget.cpp"

with open(src_path, 'r', encoding='utf-8') as f:
    src_text = f.read()

# get the methods code starting at line 644 (the comment "//Nihcas below Function for Mil Grid")
# down to the end of checkarray function
start_marker = "//Nihcas below Function for Mil Grid"
end_marker = "//Nihcas above"

s_idx = src_text.find(start_marker)
e_idx = src_text.find(end_marker, s_idx) + len(end_marker)

math_functions = src_text[s_idx:e_idx]

# extract updateCoordinateDisplayUpdated
start_u = src_text.find("//Overload updatecordinate")
end_u = src_text.find("void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()")
update_extra = src_text[start_u:end_u]

# Fix the missing QgsCoordinateReferenceSystem constructor argument
update_extra = update_extra.replace('QgsCoordinateReferenceSystem(4326)', 'QgsCoordinateReferenceSystem("EPSG:4326")')

# Now read dst_path
with open(dst_path, 'r', encoding='utf-8') as f:
    dst_text = f.read()

replace_labels = """  mLabel->setToolTip( tr( "Current map coordinate" ) );

  // add a label to show DSM GR
  mLabelgeocord = new QLabel(QString(), this);
  mLabelgeocord->setObjectName(u"mCoordsGeocord"_s);
  mLabelgeocord->setMinimumWidth(10);
  mLabelgeocord->setMargin(3);
  mLabelgeocord->setAlignment(Qt::AlignCenter);
  mLabelgeocord->setFrameStyle(QFrame::NoFrame);
  mLabelgeocord->setText(tr("Geographic"));
  mLabelgeocord->setToolTip(tr("Show Geographic Coordinate"));

  // add a label to show DSM GR
  mLabeldgr = new QLabel(QString(), this);
  mLabeldgr->setObjectName(u"mCoordsLabeldgr"_s);
  mLabeldgr->setMinimumWidth(10);
  mLabeldgr->setMargin(3);
  mLabeldgr->setAlignment(Qt::AlignCenter);
  mLabeldgr->setFrameStyle(QFrame::NoFrame);
  mLabeldgr->setText(tr("DSM GR"));
  mLabeldgr->setToolTip(tr("Show DSM GR"));

  // add a label to show DSM Sheet No
  mLabeldsheet = new QLabel(QString(), this);
  mLabeldsheet->setObjectName(u"mCoordsLabeldsheet"_s);
  mLabeldsheet->setMinimumWidth(10);
  mLabeldsheet->setMargin(3);
  mLabeldsheet->setAlignment(Qt::AlignCenter);
  mLabeldsheet->setFrameStyle(QFrame::NoFrame);
  mLabeldsheet->setText(tr("DSM No."));
  mLabeldsheet->setToolTip(tr("Show DSM Sheet No"));

  // add a label to show ESM GR
  mLabelegr = new QLabel(QString(), this);
  mLabelegr->setObjectName(u"mCoordsLabelegr"_s);
  mLabelegr->setMinimumWidth(10);
  mLabelegr->setMargin(3);
  mLabelegr->setAlignment(Qt::AlignCenter);
  mLabelegr->setFrameStyle(QFrame::NoFrame);
  mLabelegr->setText(tr("ESM GR"));
  mLabelegr->setToolTip(tr("Show ESM GR"));

  // add a label to show ESM Sheet No
  mLabelesheet = new QLabel(QString(), this);
  mLabelesheet->setObjectName(u"mCoordsLabelesheet"_s);
  mLabelesheet->setMinimumWidth(10);
  mLabelesheet->setMargin(3);
  mLabelesheet->setAlignment(Qt::AlignCenter);
  mLabelesheet->setFrameStyle(QFrame::NoFrame);
  mLabelesheet->setText(tr("ESM No."));
  mLabelesheet->setToolTip(tr("Show ESM Sheet No"));
"""

dst_text = dst_text.replace('  mLabel->setToolTip( tr( "Current map coordinate" ) );', replace_labels)


replace_lineedits = """  mLineEdit->setToolTip( tr( "Current map coordinate (longitude latitude or east north)" ) );

  //Coordinate status bar widget GeoCoord
  mCoordsGeocord = new QLineEdit(this);
  mCoordsGeocord->setObjectName("mCoordsGeocord");
  mCoordsGeocord->setMinimumWidth(10);
  mCoordsGeocord->setContentsMargins(0, 0, 0, 0);
  mCoordsGeocord->setAlignment(Qt::AlignLeft);
  mCoordsGeocord->setWhatsThis(tr("Shows Geographic coordinates at the current cursor position."));
  mCoordsGeocord->setToolTip(tr("Shows the Geographic Coordinates"));
  connect(mCoordsGeocord, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);

  //Coordinate status bar widget DSM
  mCoordsEditMgrid = new QLineEdit(this);
  mCoordsEditMgrid->setObjectName("mCoordsEditMgrid");
  mCoordsEditMgrid->setMinimumWidth(110);
  mCoordsEditMgrid->setMaximumWidth(150);
  mCoordsEditMgrid->setContentsMargins(0, 0, 0, 0);
  mCoordsEditMgrid->setAlignment(Qt::AlignLeft);
  mCoordsEditMgrid->setWhatsThis(tr("Shows DSM GR at the current cursor position."));
  mCoordsEditMgrid->setToolTip(tr("Shows the Mil grid of DSM series maps"));
  connect(mCoordsEditMgrid, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);

  //DSM Sheet Number widget 
  mCoordsEditMsheet = new QLineEdit(this);
  mCoordsEditMsheet->setObjectName("mCoordsEditMsheet");
  mCoordsEditMsheet->setMinimumWidth(10);
  mCoordsEditMsheet->setMaximumWidth(50);
  mCoordsEditMsheet->setContentsMargins(0, 0, 0, 0);
  mCoordsEditMsheet->setAlignment(Qt::AlignLeft);
  mCoordsEditMsheet->setWhatsThis(tr("Shows the DSM Sheet No."));
  mCoordsEditMsheet->setToolTip(tr("Shows the DSM series Map Sheet No."));
  connect(mCoordsEditMsheet, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);

  //Cordinate status bar widget Everest
  mCoordsEditMgrideve = new QLineEdit(this);
  mCoordsEditMgrideve->setObjectName("mCoordsEditMgrideve");
  mCoordsEditMgrideve->setMinimumWidth(150);
  mCoordsEditMgrideve->setMaximumWidth(150);
  mCoordsEditMgrideve->setContentsMargins(0, 0, 0, 0);
  mCoordsEditMgrideve->setAlignment(Qt::AlignLeft);
  mCoordsEditMgrideve->setWhatsThis(tr("Shows ESM GR at the current cursor position"));
  mCoordsEditMgrideve->setToolTip(tr("Show the ESM GR of Everest series maps)"));
  connect(mCoordsEditMgrideve, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);

  //Everest Series Map Sheet Number 
  mCoordsEditMsheeteve = new QLineEdit(this);
  mCoordsEditMsheeteve->setObjectName("mCoordsEditMsheeteve");
  mCoordsEditMsheeteve->setMinimumWidth(60);
  mCoordsEditMsheeteve->setMaximumWidth(60);
  mCoordsEditMsheeteve->setContentsMargins(0, 0, 0, 0);
  mCoordsEditMsheeteve->setAlignment(Qt::AlignLeft);
  mCoordsEditMsheeteve->setWhatsThis(tr("Shows ESM Sheet No."));
  mCoordsEditMsheeteve->setToolTip(tr("Shows the ESM series Map Sheet No."));
  connect(mCoordsEditMsheeteve, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);
"""
dst_text = dst_text.replace('  mLineEdit->setToolTip( tr( "Current map coordinate (longitude latitude or east north)" ) );', replace_lineedits)

replace_layout = """  layout->addWidget( mLabel );
  layout->addWidget( mLineEdit );
  //Nihcas add below
  layout->addWidget(mLabelgeocord);
  layout->addWidget(mCoordsGeocord);
  layout->addWidget(mLabeldgr);
  layout->addWidget(mCoordsEditMgrid);
  layout->addWidget(mLabeldsheet);
  layout->addWidget(mCoordsEditMsheet);
  layout->addWidget(mLabelegr);
  layout->addWidget(mCoordsEditMgrideve);
  layout->addWidget(mLabelesheet);
  layout->addWidget(mCoordsEditMsheeteve);
  //Nihcas add above
  layout->addWidget( mToggleExtentsViewButton );"""

dst_text = dst_text.replace("""  layout->addWidget( mLabel );
  layout->addWidget( mLineEdit );
  layout->addWidget( mToggleExtentsViewButton );""", replace_layout)

replace_showmouse = """void QgsStatusBarCoordinatesWidget::showMouseCoordinates( const QgsPointXY &mapPoint )
{
  mLastCoordinate = mapPoint;
  mLastCoordinateCrs = mMapCanvas->mapSettings().destinationCrs();
  //updateCoordinateDisplay();

  QgsPointXY p;
  QgsPointXY p1 = QgsPointXY( mapPoint);
  QgsCoordinateReferenceSystem crsSrc, crsWgs;
  crsSrc = mMapCanvas->mapSettings().destinationCrs();
  if (crsSrc.authid() != "EPSG:4326")
  {
    crsWgs = QgsCoordinateReferenceSystem("EPSG:4326");
    QgsCoordinateTransform xform = QgsCoordinateTransform(crsSrc, crsWgs, QgsProject::instance());
    p = xform.transform(p1);
    mCoordsGeocord->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), p, crsSrc, 4));
    ensureCoordinatesVisible();
  }
  else
  {
    p = mapPoint;
    mLineEdit->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), p, mMapCanvas->mapSettings().destinationCrs(), mMousePrecisionDecimalPlaces));
    crsWgs = QgsCoordinateReferenceSystem("EPSG:4326");
    QgsCoordinateTransform xform = QgsCoordinateTransform(crsSrc, crsWgs, QgsProject::instance());
    p = xform.transform(p1);
    mCoordsGeocord->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), p, crsSrc, 4));
    ensureCoordinatesVisible();
  }

  QString str, str1, str2, str3;
  if ((p.x() > -180 && p.x() < 180) && (p.y() > -90 && p.y() < 90))
  {
    str = LatLongToMilgridConversion(p);
  }
  else
  {
    str = "OUT OF BOUND AREA";
  }

  if (((p.x() > -180 && p.x() < 180)) && (p.y() > -90 && p.y() < 90))
  {
    str1 = LatLongTopoSheetConversion(p);
  }
  else
  {
    str1 = "OUT OF BOUND AREA";
  }

  //for everest series map
  if ((p.x() > 57 && p.x() < 110) && (p.y() > 8 && p.y() < 44))
  {
    str2 = eveLatLongToMilgridConversion(p);
  }
  else
  {
    str2 = "OUT OF BOUND AREA";
  }

  if (((p.x() > 44 && p.x() < 104)) && (p.y() > 4 && p.y() < 40))
  {
    str3 = eveLatLongTopoSheetConversion(p);
  }
  else
  {
    str3 = "NOT AVAILABLE";
  }

  mCoordsEditMgrid->setText(str); //Display DSM GR
  mCoordsEditMsheet->setText(str1); //Display DSM Sheet No
  mCoordsEditMgrideve->setText(str2); //Display ESM GR
  mCoordsEditMsheeteve->setText(str3); //Display ESM Sheet No

  updateCoordinateDisplay();
}"""

dst_text = re.sub(
    r'void QgsStatusBarCoordinatesWidget::showMouseCoordinates\( const QgsPointXY &mapPoint \)\n\{\n  mLastCoordinate = mapPoint;\n  mLastCoordinateCrs = mMapCanvas->mapSettings\(\)\.destinationCrs\(\);\n  updateCoordinateDisplay\(\);\n\}',
    replace_showmouse,
    dst_text)

insertion_point = "void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()"
parts = dst_text.split(insertion_point)
new_text = parts[0] + math_functions + "\n\n" + update_extra + "\n\n" + insertion_point + parts[1]

with open(dst_path, 'w', encoding='utf-8') as f:
    f.write(new_text)

print("Done replacing.")
