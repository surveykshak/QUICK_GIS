$src = "C:\Users\chowd\Documents\GitHub\surveykshak\src\app\qgsstatusbarcoordinateswidget.cpp"
$dst = "C:\Users\chowd\Documents\GitHub\QuickGIS\src\app\qgsstatusbarcoordinateswidget.cpp"
$srcText = [System.IO.File]::ReadAllText($src)
$dstText = [System.IO.File]::ReadAllText($dst)

$startMarker = "//Nihcas below Function for Mil Grid"
$endMarker = "//Nihcas above"
$sIdx = $srcText.IndexOf($startMarker)
$eIdx = $srcText.IndexOf($endMarker, $sIdx) + $endMarker.Length
$mathFunctions = $srcText.Substring($sIdx, $eIdx - $sIdx)

$startU = $srcText.IndexOf("//Overload updatecordinate")
$endU = $srcText.IndexOf("void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()")
$updateExtra = $srcText.Substring($startU, $endU - $startU)
$updateExtra = $updateExtra.Replace('QgsCoordinateReferenceSystem(4326)', 'QgsCoordinateReferenceSystem("EPSG:4326")')

$replaceLabels = @"
  mLabel->setToolTip( tr( `"Current map coordinate`" ) );

  mLabelgeocord = new QLabel(QString(), this);
  mLabelgeocord->setObjectName(u`"mCoordsGeocord`"_s);
  mLabelgeocord->setMinimumWidth(10);
  mLabelgeocord->setMargin(3);
  mLabelgeocord->setAlignment(Qt::AlignCenter);
  mLabelgeocord->setFrameStyle(QFrame::NoFrame);
  mLabelgeocord->setText(tr(`"Geographic`"));
  mLabelgeocord->setToolTip(tr(`"Show Geographic Coordinate`"));

  mLabeldgr = new QLabel(QString(), this);
  mLabeldgr->setObjectName(u`"mCoordsLabeldgr`"_s);
  mLabeldgr->setMinimumWidth(10);
  mLabeldgr->setMargin(3);
  mLabeldgr->setAlignment(Qt::AlignCenter);
  mLabeldgr->setFrameStyle(QFrame::NoFrame);
  mLabeldgr->setText(tr(`"DSM GR`"));
  mLabeldgr->setToolTip(tr(`"Show DSM GR`"));

  mLabeldsheet = new QLabel(QString(), this);
  mLabeldsheet->setObjectName(u`"mCoordsLabeldsheet`"_s);
  mLabeldsheet->setMinimumWidth(10);
  mLabeldsheet->setMargin(3);
  mLabeldsheet->setAlignment(Qt::AlignCenter);
  mLabeldsheet->setFrameStyle(QFrame::NoFrame);
  mLabeldsheet->setText(tr(`"DSM No.`"));
  mLabeldsheet->setToolTip(tr(`"Show DSM Sheet No`"));

  mLabelegr = new QLabel(QString(), this);
  mLabelegr->setObjectName(u`"mCoordsLabelegr`"_s);
  mLabelegr->setMinimumWidth(10);
  mLabelegr->setMargin(3);
  mLabelegr->setAlignment(Qt::AlignCenter);
  mLabelegr->setFrameStyle(QFrame::NoFrame);
  mLabelegr->setText(tr(`"ESM GR`"));
  mLabelegr->setToolTip(tr(`"Show ESM GR`"));

  mLabelesheet = new QLabel(QString(), this);
  mLabelesheet->setObjectName(u`"mCoordsLabelesheet`"_s);
  mLabelesheet->setMinimumWidth(10);
  mLabelesheet->setMargin(3);
  mLabelesheet->setAlignment(Qt::AlignCenter);
  mLabelesheet->setFrameStyle(QFrame::NoFrame);
  mLabelesheet->setText(tr(`"ESM No.`"));
  mLabelesheet->setToolTip(tr(`"Show ESM Sheet No`"));
"@

$dstText = $dstText.Replace('  mLabel->setToolTip( tr( "Current map coordinate" ) );', $replaceLabels)


$replaceLineEdits = @"
  mLineEdit->setToolTip( tr( `"Current map coordinate (longitude latitude or east north)`" ) );

  mCoordsGeocord = new QLineEdit(this);
  mCoordsGeocord->setObjectName("mCoordsGeocord");
  mCoordsGeocord->setMinimumWidth(10);
  mCoordsGeocord->setContentsMargins(0, 0, 0, 0);
  mCoordsGeocord->setAlignment(Qt::AlignLeft);
  mCoordsGeocord->setWhatsThis(tr(`"Shows Geographic coordinates at the current cursor position.`"));
  mCoordsGeocord->setToolTip(tr(`"Shows the Geographic Coordinates`"));
  connect(mCoordsGeocord, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);

  mCoordsEditMgrid = new QLineEdit(this);
  mCoordsEditMgrid->setObjectName("mCoordsEditMgrid");
  mCoordsEditMgrid->setMinimumWidth(110);
  mCoordsEditMgrid->setMaximumWidth(150);
  mCoordsEditMgrid->setContentsMargins(0, 0, 0, 0);
  mCoordsEditMgrid->setAlignment(Qt::AlignLeft);
  mCoordsEditMgrid->setWhatsThis(tr(`"Shows DSM GR at the current cursor position.`"));
  mCoordsEditMgrid->setToolTip(tr(`"Shows the Mil grid of DSM series maps`"));
  connect(mCoordsEditMgrid, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);

  mCoordsEditMsheet = new QLineEdit(this);
  mCoordsEditMsheet->setObjectName("mCoordsEditMsheet");
  mCoordsEditMsheet->setMinimumWidth(10);
  mCoordsEditMsheet->setMaximumWidth(50);
  mCoordsEditMsheet->setContentsMargins(0, 0, 0, 0);
  mCoordsEditMsheet->setAlignment(Qt::AlignLeft);
  mCoordsEditMsheet->setWhatsThis(tr(`"Shows the DSM Sheet No.`"));
  mCoordsEditMsheet->setToolTip(tr(`"Shows the DSM series Map Sheet No.`"));
  connect(mCoordsEditMsheet, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);

  mCoordsEditMgrideve = new QLineEdit(this);
  mCoordsEditMgrideve->setObjectName("mCoordsEditMgrideve");
  mCoordsEditMgrideve->setMinimumWidth(150);
  mCoordsEditMgrideve->setMaximumWidth(150);
  mCoordsEditMgrideve->setContentsMargins(0, 0, 0, 0);
  mCoordsEditMgrideve->setAlignment(Qt::AlignLeft);
  mCoordsEditMgrideve->setWhatsThis(tr(`"Shows ESM GR at the current cursor position`"));
  mCoordsEditMgrideve->setToolTip(tr(`"Show the ESM GR of Everest series maps)`"));
  connect(mCoordsEditMgrideve, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);

  mCoordsEditMsheeteve = new QLineEdit(this);
  mCoordsEditMsheeteve->setObjectName("mCoordsEditMsheeteve");
  mCoordsEditMsheeteve->setMinimumWidth(60);
  mCoordsEditMsheeteve->setMaximumWidth(60);
  mCoordsEditMsheeteve->setContentsMargins(0, 0, 0, 0);
  mCoordsEditMsheeteve->setAlignment(Qt::AlignLeft);
  mCoordsEditMsheeteve->setWhatsThis(tr(`"Shows ESM Sheet No.`"));
  mCoordsEditMsheeteve->setToolTip(tr(`"Shows the ESM series Map Sheet No.`"));
  connect(mCoordsEditMsheeteve, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates);
"@
$dstText = $dstText.Replace('  mLineEdit->setToolTip( tr( "Current map coordinate (longitude latitude or east north)" ) );', $replaceLineEdits)


$replaceLayout = @"
  layout->addWidget( mLabel );
  layout->addWidget( mLineEdit );
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
  layout->addWidget( mToggleExtentsViewButton );
"@

$targetLayout = @"
  layout->addWidget( mLabel );
  layout->addWidget( mLineEdit );
  layout->addWidget( mToggleExtentsViewButton );
"@

$dstText = $dstText.Replace($targetLayout, $replaceLayout)


$replaceShowmouse = @"
void QgsStatusBarCoordinatesWidget::showMouseCoordinates( const QgsPointXY &mapPoint )
{
  mLastCoordinate = mapPoint;
  mLastCoordinateCrs = mMapCanvas->mapSettings().destinationCrs();

  QgsPointXY p;
  QgsPointXY p1 = QgsPointXY( mapPoint);
  QgsCoordinateReferenceSystem crsSrc, crsWgs;
  crsSrc = mMapCanvas->mapSettings().destinationCrs();
  if (crsSrc.authid() != `"EPSG:4326`")
  {
    crsWgs = QgsCoordinateReferenceSystem(`"EPSG:4326`");
    QgsCoordinateTransform xform = QgsCoordinateTransform(crsSrc, crsWgs, QgsProject::instance());
    p = xform.transform(p1);
    mCoordsGeocord->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), p, crsSrc, 4));
    ensureCoordinatesVisible();
  }
  else
  {
    p = mapPoint;
    mLineEdit->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), p, mMapCanvas->mapSettings().destinationCrs(), mMousePrecisionDecimalPlaces));
    crsWgs = QgsCoordinateReferenceSystem(`"EPSG:4326`");
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
    str = `"OUT OF BOUND AREA`";
  }

  if (((p.x() > -180 && p.x() < 180)) && (p.y() > -90 && p.y() < 90))
  {
    str1 = LatLongTopoSheetConversion(p);
  }
  else
  {
    str1 = `"OUT OF BOUND AREA`";
  }

  if ((p.x() > 57 && p.x() < 110) && (p.y() > 8 && p.y() < 44))
  {
    str2 = eveLatLongToMilgridConversion(p);
  }
  else
  {
    str2 = `"OUT OF BOUND AREA`";
  }

  if (((p.x() > 44 && p.x() < 104)) && (p.y() > 4 && p.y() < 40))
  {
    str3 = eveLatLongTopoSheetConversion(p);
  }
  else
  {
    str3 = `"NOT AVAILABLE`";
  }

  mCoordsEditMgrid->setText(str);
  mCoordsEditMsheet->setText(str1);
  mCoordsEditMgrideve->setText(str2);
  mCoordsEditMsheeteve->setText(str3);

  updateCoordinateDisplay();
}
"@

$targetShowmouse = @"
void QgsStatusBarCoordinatesWidget::showMouseCoordinates( const QgsPointXY &mapPoint )
{
  mLastCoordinate = mapPoint;
  mLastCoordinateCrs = mMapCanvas->mapSettings().destinationCrs();
  updateCoordinateDisplay();
}
"@

$dstText = $dstText.Replace($targetShowmouse, $replaceShowmouse)

$insertionPoint = "void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()"
$parts = $dstText.Split([string[]]@($insertionPoint), [System.StringSplitOptions]::None)
$newText = $parts[0] + $mathFunctions + "`r`n`r`n" + $updateExtra + "`r`n`r`n" + $insertionPoint + $parts[1]

[System.IO.File]::WriteAllText($dst, $newText)
Write-Output "Done replacing."
