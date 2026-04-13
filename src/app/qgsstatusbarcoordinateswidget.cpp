/***************************************************************************
   qgsstatusbarcoordinateswidget.cpp
    --------------------------------------
   Date                 : 05.08.2015
   Copyright            : (C) 2015 Denis Rouzaud
   Email                : denis.rouzaud@gmail.com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "qgsstatusbarcoordinateswidget.h"

#include "qgsapplication.h"
#include "qgscoordinatereferencesystemutils.h"
#include "qgscoordinateutils.h"
#include "qgsmapcanvas.h"
#include "qgsproject.h"
#include "qgsvectorlayer.h"
#include "qgsvectorlayerjoininfo.h"

#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSpacerItem>
#include <QString>
#include <QTimer>
#include <QToolButton>

#include "moc_qgsstatusbarcoordinateswidget.cpp"

using namespace Qt::StringLiterals;

QgsStatusBarCoordinatesWidget::QgsStatusBarCoordinatesWidget( QWidget *parent )
  : QWidget( parent )
{
  // calculate the size of two chars
  mTwoCharSize = fontMetrics().boundingRect( 'O' ).width();
  mMinimumWidth = mTwoCharSize * 4;

  // add a label to show current position
  mLabel = new QLabel( QString(), this );
  mLabel->setObjectName( u"mCoordsLabel"_s );
  mLabel->setMinimumWidth( 10 );
  //mCoordsLabel->setMaximumHeight( 20 );
  mLabel->setMargin( 3 );
  mLabel->setAlignment( Qt::AlignCenter );
  mLabel->setFrameStyle( QFrame::NoFrame );
  mLabel->setText( tr( "Coordinate" ) );
  mLabel->setToolTip( tr( "Current map coordinate" ) );

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


  mLineEdit = new QLineEdit( this );
  mLineEdit->setMinimumWidth( 10 );
  //mLineEdit->setMaximumHeight( 20 );
  mLineEdit->setContentsMargins( 0, 0, 0, 0 );
  mLineEdit->setAlignment( Qt::AlignCenter );
  connect( mLineEdit, &QLineEdit::returnPressed, this, &QgsStatusBarCoordinatesWidget::validateCoordinates );

  mLineEdit->setToolTip( tr( "Current map coordinate (longitude latitude or east north)" ) );

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


  //toggle to switch between mouse pos and extents display in status bar widget
  mToggleExtentsViewButton = new QToolButton( this );
  mToggleExtentsViewButton->setIcon( QgsApplication::getThemeIcon( u"tracking.svg"_s ) );
  mToggleExtentsViewButton->setToolTip( tr( "Toggle extents and mouse position display" ) );
  mToggleExtentsViewButton->setCheckable( true );
  mToggleExtentsViewButton->setAutoRaise( true );
  connect( mToggleExtentsViewButton, &QAbstractButton::toggled, this, &QgsStatusBarCoordinatesWidget::extentsViewToggled );

  QHBoxLayout *layout = new QHBoxLayout( this );
  setLayout( layout );
  layout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding ) );
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
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setAlignment( Qt::AlignRight );
  layout->setSpacing( 0 );

  // When you feel dizzy
  mDizzyTimer = new QTimer( this );
  connect( mDizzyTimer, &QTimer::timeout, this, &QgsStatusBarCoordinatesWidget::dizzy );

  connect( QgsProject::instance()->displaySettings(), &QgsProjectDisplaySettings::coordinateCrsChanged, this, &QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged );
  connect( QgsProject::instance()->displaySettings(), &QgsProjectDisplaySettings::geographicCoordinateFormatChanged, this, &QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged );
  connect( QgsProject::instance()->displaySettings(), &QgsProjectDisplaySettings::coordinateTypeChanged, this, &QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged );

  coordinateDisplaySettingsChanged();
}

void QgsStatusBarCoordinatesWidget::setMapCanvas( QgsMapCanvas *mapCanvas )
{
  if ( mMapCanvas )
  {
    disconnect( mMapCanvas, &QgsMapCanvas::xyCoordinates, this, &QgsStatusBarCoordinatesWidget::showMouseCoordinates );
    disconnect( mMapCanvas, &QgsMapCanvas::extentsChanged, this, &QgsStatusBarCoordinatesWidget::showExtent );
  }

  mMapCanvas = mapCanvas;
  connect( mMapCanvas, &QgsMapCanvas::xyCoordinates, this, &QgsStatusBarCoordinatesWidget::showMouseCoordinates );
  connect( mMapCanvas, &QgsMapCanvas::extentsChanged, this, &QgsStatusBarCoordinatesWidget::showExtent );
}

void QgsStatusBarCoordinatesWidget::setFont( const QFont &myFont )
{
  mLineEdit->setFont( myFont );
  mLabel->setFont( myFont );
}

void QgsStatusBarCoordinatesWidget::setMouseCoordinatesPrecision( unsigned int precision )
{
  mMousePrecisionDecimalPlaces = precision;
}

void QgsStatusBarCoordinatesWidget::validateCoordinates()
{
  if ( !mMapCanvas )
  {
    return;
  }
  else if ( mLineEdit->text() == "world"_L1 )
  {
    world();
  }
  if ( mLineEdit->text() == "contributors"_L1 )
  {
    contributors();
  }
  else if ( mLineEdit->text() == "hackfests"_L1 )
  {
    hackfests();
  }
  else if ( mLineEdit->text() == "user groups"_L1 )
  {
    userGroups();
  }
  else if ( mLineEdit->text() == "dizzy"_L1 )
  {
    // sometimes you may feel a bit dizzy...
    if ( mDizzyTimer->isActive() )
    {
      mDizzyTimer->stop();
      mMapCanvas->setSceneRect( mMapCanvas->viewport()->rect() );
      mMapCanvas->setTransform( QTransform() );
    }
    else
    {
      mDizzyTimer->start( 100 );
    }
    return;
  }
  else if ( mLineEdit->text() == "retro"_L1 )
  {
    mMapCanvas->setProperty( "retro", !mMapCanvas->property( "retro" ).toBool() );
    refreshMapCanvas();
    return;
  }
  else if ( mLineEdit->text() == "bored"_L1 )
  {
    // it's friday afternoon and too late to start another piece of work...
    emit weAreBored();
  }

  bool xOk = false;
  bool yOk = false;
  double first = 0;
  double second = 0;
  QString coordText = mLineEdit->text();
  const thread_local QRegularExpression sMultipleWhitespaceRx( u" {2,}"_s );
  coordText.replace( sMultipleWhitespaceRx, u" "_s );
  coordText.remove( u"°"_s );

  QStringList parts = coordText.split( ',' );
  if ( parts.size() == 2 )
  {
    first = parts.at( 0 ).toDouble( &xOk );
    second = parts.at( 1 ).toDouble( &yOk );
  }

  if ( !xOk || !yOk )
  {
    parts = coordText.split( ' ' );
    if ( parts.size() == 2 )
    {
      first = parts.at( 0 ).toDouble( &xOk );
      second = parts.at( 1 ).toDouble( &yOk );
    }
  }

  // Use locale
  if ( !xOk || !yOk )
  {
    parts = coordText.split( ' ' );
    if ( parts.size() == 2 )
    {
      first = QLocale().toDouble( parts.at( 0 ), &xOk );
      second = QLocale().toDouble( parts.at( 1 ), &yOk );
    }
  }

  if ( !xOk || !yOk )
    return;

  const Qgis::CoordinateOrder projectAxisOrder = QgsProject::instance()->displaySettings()->coordinateAxisOrder();

  const Qgis::CoordinateOrder coordinateOrder = projectAxisOrder == Qgis::CoordinateOrder::Default
                                                  ? QgsCoordinateReferenceSystemUtils::defaultCoordinateOrderForCrs( mMapCanvas->mapSettings().destinationCrs() )
                                                  : projectAxisOrder;
  // we may need to flip coordinates depending on crs axis ordering
  switch ( coordinateOrder )
  {
    case Qgis::CoordinateOrder::Default:
    case Qgis::CoordinateOrder::XY:
      break;
    case Qgis::CoordinateOrder::YX:
      std::swap( first, second );
      break;
  }

  QgsPointXY centerPoint { first, second };

  const QgsCoordinateReferenceSystem displayCrs = QgsProject::instance()->displaySettings()->coordinateCrs();
  const QgsCoordinateReferenceSystem canvasCrs = mMapCanvas->mapSettings().destinationCrs();
  if ( displayCrs.isValid() && canvasCrs.isValid() && displayCrs != canvasCrs )
  {
    const QgsCoordinateTransform ct { displayCrs, canvasCrs, QgsProject::instance()->transformContext() };
    try
    {
      centerPoint = ct.transform( centerPoint );
    }
    catch ( const QgsCsException & )
    {
      return;
    }
  }

  mMapCanvas->setCenter( centerPoint );

  mMapCanvas->refresh();
}


void QgsStatusBarCoordinatesWidget::dizzy()
{
  if ( !mMapCanvas )
  {
    return;
  }
  // constants should go to options so that people can customize them to their taste
  const int d = 10; // max. translational dizziness offset
  const int r = 4;  // max. rotational dizzines angle
  QRectF rect = mMapCanvas->sceneRect();
  if ( rect.x() < -d || rect.x() > d || rect.y() < -d || rect.y() > d )
    return; // do not affect panning

  rect.moveTo( static_cast<int>( QRandomGenerator::global()->generate() % ( 2 * d ) ) - d, static_cast<int>( QRandomGenerator::global()->generate() % ( 2 * d ) ) - d );
  mMapCanvas->setSceneRect( rect );
  QTransform matrix;
  matrix.rotate( static_cast<int>( QRandomGenerator::global()->generate() % ( 2 * r ) ) - r );
  mMapCanvas->setTransform( matrix );
}

void QgsStatusBarCoordinatesWidget::contributors()
{
  if ( !mMapCanvas )
  {
    return;
  }
  const QString fileName = QgsApplication::pkgDataPath() + u"/resources/data/contributors.json"_s;
  const QFileInfo fileInfo = QFileInfo( fileName );
  const QgsVectorLayer::LayerOptions options { QgsProject::instance()->transformContext() };
  QgsVectorLayer *layer = new QgsVectorLayer( fileInfo.absoluteFilePath(), tr( "QGIS Contributors" ), u"ogr"_s, options );
  // Register this layer with the layers registry
  QgsProject::instance()->addMapLayer( layer );
  layer->setAutoRefreshInterval( 500 );
  layer->setAutoRefreshMode( Qgis::AutoRefreshMode::RedrawOnly );
}

void QgsStatusBarCoordinatesWidget::world()
{
  if ( !mMapCanvas )
  {
    return;
  }
  const QString fileName = QgsApplication::pkgDataPath() + u"/resources/data/world_map.gpkg|layername=countries"_s;
  const QFileInfo fileInfo = QFileInfo( fileName );
  QgsVectorLayer::LayerOptions options { QgsProject::instance()->transformContext() };
  options.forceReadOnly = true;
  QgsVectorLayer *layer = new QgsVectorLayer( fileInfo.absoluteFilePath(), tr( "World Map" ), u"ogr"_s, options );
  // Register this layer with the layers registry
  QgsProject::instance()->addMapLayer( layer );
}

void QgsStatusBarCoordinatesWidget::hackfests()
{
  if ( !mMapCanvas )
  {
    return;
  }
  const QString fileName = QgsApplication::pkgDataPath() + u"/resources/data/qgis-hackfests.json"_s;
  const QFileInfo fileInfo = QFileInfo( fileName );
  const QgsVectorLayer::LayerOptions options { QgsProject::instance()->transformContext() };
  QgsVectorLayer *layer = new QgsVectorLayer( fileInfo.absoluteFilePath(), tr( "QGIS Hackfests" ), u"ogr"_s, options );
  // Register this layer with the layers registry
  QgsProject::instance()->addMapLayer( layer );
}

void QgsStatusBarCoordinatesWidget::userGroups()
{
  if ( !mMapCanvas )
  {
    return;
  }
  const QString fileName = QgsApplication::pkgDataPath() + u"/resources/data/world_map.gpkg|layername=countries"_s;
  const QFileInfo fileInfo = QFileInfo( fileName );
  const QgsVectorLayer::LayerOptions options { QgsProject::instance()->transformContext() };
  QgsVectorLayer *layer = new QgsVectorLayer( fileInfo.absoluteFilePath(), tr( "User Groups" ), u"ogr"_s, options );

  const QString fileNameData = QgsApplication::pkgDataPath() + u"/resources/data/user_groups_data.json"_s;
  const QFileInfo fileInfoData = QFileInfo( fileNameData );
  QgsVectorLayer *layerData = new QgsVectorLayer( fileInfoData.absoluteFilePath(), tr( "user_groups_data" ), u"ogr"_s, options );

  // Register layers with the layers registry
  QgsProject::instance()->addMapLayers( QList<QgsMapLayer *>() << layer << layerData );

  // Create join
  QgsVectorLayerJoinInfo joinInfo;
  joinInfo.setTargetFieldName( u"iso_a2"_s );
  joinInfo.setJoinLayer( layerData );
  joinInfo.setJoinFieldName( u"country"_s );
  joinInfo.setUsingMemoryCache( true );
  joinInfo.setPrefix( u"ug_"_s );
  joinInfo.setJoinFieldNamesSubset( nullptr ); // Use all join fields
  layer->addJoin( joinInfo );

  // Load QML for polygon symbology and maptips
  const QString fileNameStyle = QgsApplication::pkgDataPath() + u"/resources/data/user_groups.qml"_s;
  bool styleFlag = false;
  layer->loadNamedStyle( fileNameStyle, styleFlag, true );
}

void QgsStatusBarCoordinatesWidget::extentsViewToggled( bool flag )
{
  if ( flag )
  {
    //extents view mode!
    mToggleExtentsViewButton->setIcon( QgsApplication::getThemeIcon( u"extents.svg"_s ) );
    mLineEdit->setToolTip( tr( "Map coordinates for the current view extents" ) );
    mLineEdit->setReadOnly( true );
    showExtent();
  }
  else
  {
    //mouse cursor pos view mode!
    mToggleExtentsViewButton->setIcon( QgsApplication::getThemeIcon( u"tracking.svg"_s ) );
    mLineEdit->setToolTip( tr( "Map coordinates at mouse cursor position" ) );
    mLineEdit->setReadOnly( false );
    mLabel->setText( tr( "Coordinate" ) );
  }
}

void QgsStatusBarCoordinatesWidget::refreshMapCanvas()
{
  if ( !mMapCanvas )
    return;
  //stop any current rendering
  mMapCanvas->stopRendering();
  mMapCanvas->redrawAllLayers();
}

void QgsStatusBarCoordinatesWidget::showMouseCoordinates( const QgsPointXY &mapPoint)
{
  mLastCoordinate = mapPoint;
  //updateCoordinateDisplay();
  //updateCoordinateDisplayUpdated(p);

 //Nihcas added below

  QgsPointXY p;
  QgsPointXY p1 = QgsPointXY( mapPoint);
  QgsCoordinateReferenceSystem crsSrc, crsWgs;
  crsSrc = mMapCanvas->mapSettings().destinationCrs();
  if (crsSrc.authid() != "EPSG:4326")
  {
    crsWgs = QgsCoordinateReferenceSystem(4326);
    QgsCoordinateTransform xform = QgsCoordinateTransform(crsSrc, crsWgs, QgsProject::instance());
    p = xform.transform(p1);
    //QString qs = p.toString();

    mCoordsGeocord->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), p, crsSrc, 4));  //precision for latitude and longitude to show on screen
    ensureCoordinatesVisible();
  }
  else
  {
    p = mapPoint;
    mLineEdit->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), p, mMapCanvas->mapSettings().destinationCrs(),
      mMousePrecisionDecimalPlaces));
    crsWgs = QgsCoordinateReferenceSystem(4326);
    QgsCoordinateTransform xform = QgsCoordinateTransform(crsSrc, crsWgs, QgsProject::instance());
    p = xform.transform(p1);
    QString qs = p.toString();
    mCoordsGeocord->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), p, crsSrc, 4));  //precision for latitude and longitude to show on screen
    ensureCoordinatesVisible();
  }
  //Nihcas added below
  //Mil Grid Display for dsm series map

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
  if ((p.x() >= 60 && p.x() <= 78 && p.y() >= 36 && p.y() <= 44) ||
    (p.x() >= 60 && p.x() <= 79 && p.y() >= 28 && p.y() <= 36) ||
    (p.x() >= 79 && p.x() <= 81 && p.y() >= 29 && p.y() <= 36) ||
    (p.x() >= 78 && p.x() <= 81 && p.y() >= 36 && p.y() <= 37) ||
    (p.x() >= 81 && p.x() <= 102 && p.y() >= 30 && p.y() <= 37) ||
    (p.x() >= 57 && p.x() <= 60 && p.y() >= 25 && p.y() <= 28) ||
    (p.x() >= 60 && p.x() <= 72 && p.y() >= 20 && p.y() <= 28) ||
    (p.x() >= 72 && p.x() <= 79 && p.y() >= 22 && p.y() <= 28) ||
    (p.x() >= 79 && p.x() <= 82 && p.y() >= 22 && p.y() <= 29) ||
    (p.x() >= 82 && p.x() <= 83 && p.y() >= 26 && p.y() <= 28) ||
    (p.x() >= 81 && p.x() <= 82 && p.y() >= 29 && p.y() <= 30) ||
    (p.x() >= 82 && p.x() <= 83 && p.y() >= 28 && p.y() <= 30) ||
    (p.x() >= 82 && p.x() <= 83 && p.y() >= 22 && p.y() <= 26) ||
    (p.x() >= 83 && p.x() <= 85 && p.y() >= 22 && p.y() <= 30) ||
    (p.x() >= 85 && p.x() <= 91 && p.y() >= 21 && p.y() <= 30) ||
    (p.x() >= 91 && p.x() <= 93 && p.y() >= 22 && p.y() <= 30) ||
    (p.x() >= 93 && p.x() <= 110 && p.y() >= 22.5 && p.y() <= 30) ||
    (p.x() >= 60 && p.x() <= 72 && p.y() >= 15 && p.y() <= 20) ||
    (p.x() >= 72 && p.x() <= 85 && p.y() >= 15 && p.y() <= 22) ||
    (p.x() >= 85 && p.x() <= 90 && p.y() >= 15 && p.y() <= 21) ||
    (p.x() >= 90 && p.x() <= 91 && p.y() >= 15.5 && p.y() <= 21) ||
    (p.x() >= 91 && p.x() <= 93 && p.y() >= 15.5 && p.y() <= 22) ||
    (p.x() >= 93 && p.x() <= 112 && p.y() >= 15.5 && p.y() <= 22.5) ||
    (p.x() >= 112 && p.x() <= 116 && p.y() >= 15.5 && p.y() <= 22.5) ||
    (p.x() >= 60 && p.x() <= 76 && p.y() >= 8 && p.y() <= 15) ||
    (p.x() >= 76 && p.x() <= 79.5 && p.y() >= 5.5 && p.y() <= 15) ||
    (p.x() >= 79.5 && p.x() <= 82 && p.y() >= 10 && p.y() <= 15) ||
    (p.x() >= 82 && p.x() <= 90 && p.y() >= 8 && p.y() <= 15) ||
    (p.x() >= 90 && p.x() <= 92 && p.y() >= 8 && p.y() <= 15.5) ||
    (p.x() >= 92 && p.x() <= 94 && p.y() >= 6 && p.y() <= 15.5) ||
    (p.x() >= 94 && p.x() <= 98.67 && p.y() >= 7 && p.y() <= 15.5) ||
    (p.x() >= 98.67 && p.x() <= 105 && p.y() >= 8 && p.y() <= 15.5) ||
    (p.x() >= 105 && p.x() <= 116 && p.y() >= 7 && p.y() <= 15.5))
  {
    str2 = eveLatLongToMilgridConversion(p);
  }
  else
  {
    str2 = "OUT OF BOUND AREA";
  }

  if (((p.x() >= 60 && p.x() <= 78 && p.y() >= 36 && p.y() <= 44) ||
    (p.x() >= 60 && p.x() <= 79 && p.y() >= 28 && p.y() <= 36) ||
    (p.x() >= 79 && p.x() <= 81 && p.y() >= 29 && p.y() <= 36) ||
    (p.x() >= 78 && p.x() <= 81 && p.y() >= 36 && p.y() <= 37) ||
    (p.x() >= 81 && p.x() <= 102 && p.y() >= 30 && p.y() <= 37) ||
    (p.x() >= 57 && p.x() <= 60 && p.y() >= 25 && p.y() <= 28) ||
    (p.x() >= 60 && p.x() <= 72 && p.y() >= 20 && p.y() <= 28) ||
    (p.x() >= 72 && p.x() <= 79 && p.y() >= 22 && p.y() <= 28) ||
    (p.x() >= 79 && p.x() <= 82 && p.y() >= 22 && p.y() <= 29) ||
    (p.x() >= 82 && p.x() <= 83 && p.y() >= 26 && p.y() <= 28) ||
    (p.x() >= 81 && p.x() <= 82 && p.y() >= 29 && p.y() <= 30) ||
    (p.x() >= 82 && p.x() <= 83 && p.y() >= 28 && p.y() <= 30) ||
    (p.x() >= 82 && p.x() <= 83 && p.y() >= 22 && p.y() <= 26) ||
    (p.x() >= 83 && p.x() <= 85 && p.y() >= 22 && p.y() <= 30) ||
    (p.x() >= 85 && p.x() <= 91 && p.y() >= 21 && p.y() <= 30) ||
    (p.x() >= 91 && p.x() <= 93 && p.y() >= 22 && p.y() <= 30) ||
    (p.x() >= 93 && p.x() <= 110 && p.y() >= 22.5 && p.y() <= 30) ||
    (p.x() >= 60 && p.x() <= 72 && p.y() >= 15 && p.y() <= 20) ||
    (p.x() >= 72 && p.x() <= 85 && p.y() >= 15 && p.y() <= 22) ||
    (p.x() >= 85 && p.x() <= 90 && p.y() >= 15 && p.y() <= 21) ||
    (p.x() >= 90 && p.x() <= 91 && p.y() >= 15.5 && p.y() <= 21) ||
    (p.x() >= 91 && p.x() <= 93 && p.y() >= 15.5 && p.y() <= 22) ||
    (p.x() >= 93 && p.x() <= 112 && p.y() >= 15.5 && p.y() <= 22.5) ||
    (p.x() >= 112 && p.x() <= 116 && p.y() >= 15.5 && p.y() <= 22.5) ||
    (p.x() >= 60 && p.x() <= 76 && p.y() >= 8 && p.y() <= 15) ||
    (p.x() >= 76 && p.x() <= 79.5 && p.y() >= 5.5 && p.y() <= 15) ||
    (p.x() >= 79.5 && p.x() <= 82 && p.y() >= 10 && p.y() <= 15) ||
    (p.x() >= 82 && p.x() <= 90 && p.y() >= 8 && p.y() <= 15) ||
    (p.x() >= 90 && p.x() <= 92 && p.y() >= 8 && p.y() <= 15.5) ||
    (p.x() >= 92 && p.x() <= 94 && p.y() >= 6 && p.y() <= 15.5) ||
    (p.x() >= 94 && p.x() <= 98.67 && p.y() >= 7 && p.y() <= 15.5) ||
    (p.x() >= 98.67 && p.x() <= 105 && p.y() >= 8 && p.y() <= 15.5) ||
    (p.x() >= 105 && p.x() <= 116 && p.y() >= 7 && p.y() <= 15.5)))
  {
    str3 = eveLatLongTopoSheetConversion(p);
  }
  else
  {
    str3 = "NOT AVAILABLE";
  }

  //To display values

  mCoordsEditMgrid->setText(str); //Display DSM GR
  mCoordsEditMsheet->setText(str1); //Display DSM Sheet No
  mCoordsEditMgrideve->setText(str2); //Display ESM GR
  mCoordsEditMsheeteve->setText(str3); //Display ESM Sheet No
  //mCoordsGeocord->setText("T");
  updateCoordinateDisplay();

  //Nihcas above
}

void QgsStatusBarCoordinatesWidget::showExtent()
{
  if ( !mToggleExtentsViewButton->isChecked() )
  {
    return;
  }

  mLabel->setText( tr( "Extents" ) );
  mLineEdit->setText( QgsCoordinateUtils::formatExtentForProject( QgsProject::instance(), mMapCanvas->extent(), mMapCanvas->mapSettings().destinationCrs(), mMousePrecisionDecimalPlaces ) );

  ensureCoordinatesVisible();
}

void QgsStatusBarCoordinatesWidget::ensureCoordinatesVisible()
{
  //ensure the label is big (and small) enough
  const int width = std::max(mLineEdit->fontMetrics().boundingRect(mLineEdit->text()).width() + 16, mMinimumWidth);

  bool allowResize = false;
  if (mIsFirstSizeChange)
  {
    allowResize = true;
  }
  else if (mLineEdit->minimumWidth() < width)
  {
    // always immediately grow to fit
    allowResize = true;
  }
  else if ((mLineEdit->minimumWidth() - width) > mTwoCharSize)
  {
    // only allow shrinking when a sufficient time has expired since we last resized.
    // this avoids extraneous shrinking/growing resulting in distracting UI changes
    allowResize = mLastSizeChangeTimer.hasExpired(2000);
  }

  if (allowResize)
  {
    mLineEdit->setMinimumWidth(width);
    mLineEdit->setMaximumWidth(width);
    mLastSizeChangeTimer.restart();
    mIsFirstSizeChange = false;
  }
}

void QgsStatusBarCoordinatesWidget::updateCoordinateDisplay()
{
  if (mToggleExtentsViewButton->isChecked())
  {
    return;
  }

  if (mLastCoordinate.isEmpty())
    mLineEdit->clear();
  else
    mLineEdit->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), mLastCoordinate, mMapCanvas->mapSettings().destinationCrs(),
      static_cast<int>(mMousePrecisionDecimalPlaces)));

  ensureCoordinatesVisible();
}

//Overload updatecordinate
void QgsStatusBarCoordinatesWidget::updateCoordinateDisplayUpdated(const QgsPointXY& Qp)
{
  if (mToggleExtentsViewButton->isChecked())
  {
    return;
  }

  if (mLastCoordinate.isEmpty())
    mCoordsGeocord->clear();
  else {
    QgsPointXY p;
    QgsPointXY p1 = QgsPointXY(Qp);
    QgsCoordinateReferenceSystem crsSrc, crsWgs;
    crsSrc = mMapCanvas->mapSettings().destinationCrs();
    if (crsSrc.authid() != "EPSG:4326")
    {
      crsWgs = QgsCoordinateReferenceSystem(4326);
      QgsCoordinateTransform xform = QgsCoordinateTransform(crsSrc, crsWgs, QgsProject::instance());
      p = xform.transform(p1);
      mCoordsGeocord->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), p, crsWgs, 2));  //precision for latitude and longitude to show on screen
      ensureCoordinatesVisible();
    }
    else
    {
      p = Qp;
      mCoordsGeocord->setText(QgsCoordinateUtils::formatCoordinateForProject(QgsProject::instance(), Qp, mMapCanvas->mapSettings().destinationCrs(),
        mMousePrecisionDecimalPlaces));

      ensureCoordinatesVisible();
    }

  }

  ensureCoordinatesVisible();
}

//Nihcas below Function for Mil Grid
QString QgsStatusBarCoordinatesWidget::eveLatLongTopoSheetConversion(const QgsPointXY& mp)
{
  double LAT = mp.y();
  double LONG = mp.x();
  double LATdiff, LONGdiff;
  int x, y;
  QChar arr[10];
  QString finalarr;
  char str_temp1;
  char str_temp2;
  char str_temp3;
  char str_temp4;

  int lat_esm = ceil(LAT);
  double lat_esm1 = LAT;
  int long_esm = LONG;
  double long_esm1 = LONG - 44;
  int lat1;


  int matrix1[9][15] = {0};
  QChar matrix2[4][4] = { { 'D','H','L','P' },{ 'C','G','K','O' },{ 'B','F','J','N' },{ 'A','E','I','M' } };
  int matrix3[4][4] = { { 4, 8, 12, 16 },{ 3, 7, 11, 15 },{ 2, 6, 10, 14 },{ 1, 5, 9, 13 } };

  lat_esm = lat_esm - 4;
  long_esm = long_esm - 44;

  int i = 0;



  for (int col = 0; col < 15; col++) {
    for (int row = 8; row >= 0; row--) {
      if ((col == 0 && (row == 1 || row == 0)) || (col == 1 && (row == 1 || row == 0)) || (col == 2 && (row == 1 || row == 0)) || (col == 3 && (row == 2 || row == 1 || row == 0)) || (col == 4 && (row == 4 || row == 3 || row == 2 || row == 1 || row == 0)) || (col == 5 && (row == 3 || row == 2 || row == 1 || row == 0)) || (col == 6 && (row == 3 || row == 2 || row == 1 || row == 0)) || (col == 10 && (row == 2 || row == 1 || row == 0)) || (col == 11 && (row == 3 || row == 2 || row == 1 || row == 0))) {
      }
      else {
        i++;
        matrix1[row][col] = i;
      }
    }
  }

  if ((lat_esm % 4) == 0) {
    lat1 = int(lat_esm / 4);
    lat1 = lat1 - 1;
    if (lat1 < 0) {
      lat1 = 0;
    }
  }
  else {
    lat1 = int(lat_esm / 4);
  }


  int long1 = int(long_esm / 4);
  int first = matrix1[lat1][long1];

  int lat2 = int(lat_esm - (lat1) * 4);
  lat2 = lat2 - 1;
  if (lat2 < 0) {
    lat2 = 0;
  }

  int long2 = int(long_esm - (long1) * 4);
  QChar second = matrix2[lat2][long2];


  LATdiff = LAT - floor(LAT);
  if ((0.0 <= (float)(4 * LATdiff)) && ((float)(4 * LATdiff) <= 1.0))   y = 0;
  if ((1.0 <= (float)(4 * LATdiff)) && ((float)(4 * LATdiff) <= 2.0))      y = 1;
  if ((2.0 <= (float)(4 * LATdiff)) && ((float)(4 * LATdiff) <= 3.0))      y = 2;
  if ((3.0 <= (float)(4 * LATdiff)) && ((float)(4 * LATdiff) <= 4.0))      y = 3;

  LONGdiff = LONG - floor(LONG);
  if ((0.0 <= (float)(4 * LONGdiff)) && ((float)(4 * LONGdiff) <= 1.0))    x = 0;
  if ((1.0 < (float)(4 * LONGdiff)) && ((float)(4 * LONGdiff) <= 2.0))     x = 1;
  if ((2.0 < (float)(4 * LONGdiff)) && ((float)(4 * LONGdiff) <= 3.0))   x = 2;
  if ((3.0 < (float)(4 * LONGdiff)) && ((float)(4 * LONGdiff) <= 4.0))	 x = 3;

  int third = matrix3[y][x];

  str_temp1 = (char)(((int)'0') + ((first / 10) % 10));
  str_temp2 = (char)(((int)'0') + (first % 10));
  str_temp3 = (char)(((int)'0') + ((third / 10) % 10));
  str_temp4 = (char)(((int)'0') + ((third) % 10));


  arr[0] = str_temp1;
  arr[1] = str_temp2;
  arr[2] = second;
  arr[3] = str_temp3;
  arr[4] = str_temp4;

  finalarr.resize(5);
  for (int i = 0; i < 5; i++)
  {

    finalarr[i] = arr[i];

  }


  return finalarr;
  //ensureCoordinatesVisible();
}

QString QgsStatusBarCoordinatesWidget::LatLongTopoSheetConversion(const QgsPointXY& mp)
{
  double LAT = mp.y();
  double LONG = mp.x();
  double LATdiff, LONGdiff;
  int x, y, longdex;

  int lat_ndsm = ceil(mp.y());
  float lat_ndsm1 = mp.y();
  int long_ndsm = mp.x();
  float long_ndsm1 = (mp.x() - 0);
  int lat1 = 0, long1 = 0;
  QChar temp[2];
  QChar arr[10];
  QString arr_ndsm;
  QChar matrix2[4][6] = { { 'S','T','U','V','W','X' },{ 'M','N','O','P','Q','R' },{ 'G','H','I','J','K','L' },{ 'A','B','C','D','E','F' } };
  int matrix3[4][4] = { { 4, 8, 12, 16 },{ 3, 7, 11, 15 },{ 2, 6, 10, 14 },{ 1, 5, 9, 13 } };
  QChar matrix1[46] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W' };
  int matrix1_part1[61] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61 };
  char str_temp1;
  char str_temp2;
  char str_temp3;
  char str_temp4;
  char sheetno;
  lat_ndsm = lat_ndsm - 0;
  lat_ndsm1 = lat_ndsm1 - 0;
  long_ndsm = long_ndsm - 0;


  LATdiff = abs(LAT) - floor(abs(LAT));
  if ((0.0 <= (float)(4 * LATdiff)) && ((float)(4 * LATdiff) <= 1.0)) 	y = 0;
  if ((1.0 <= (float)(4 * LATdiff)) && ((float)(4 * LATdiff) <= 2.0))	    y = 1;
  if ((2.0 <= (float)(4 * LATdiff)) && ((float)(4 * LATdiff) <= 3.0))	    y = 2;
  if ((3.0 <= (float)(4 * LATdiff)) && ((float)(4 * LATdiff) <= 4.0))	    y = 3;

  LONGdiff = abs(LONG) - floor(abs(LONG));
  if ((0.0 <= (float)(4 * LONGdiff)) && ((float)(4 * LONGdiff) <= 1.0)) 	 x = 0;
  if ((1.0 < (float)(4 * LONGdiff)) && ((float)(4 * LONGdiff) <= 2.0))     x = 1;
  if ((2.0 < (float)(4 * LONGdiff)) && ((float)(4 * LONGdiff) <= 3.0))	 x = 2;
  if ((3.0 < (float)(4 * LONGdiff)) && ((float)(4 * LONGdiff) <= 4.0))	 x = 3;

  int third = matrix3[y][x];



  if ((lat_ndsm % 4) == 0)
  {
    lat1 = abs(int(lat_ndsm)) / 4;
    lat1 = lat1 - 1;
    //	grid
    if (lat1 < 0)
    {
      lat1 = 0;
    }
  }
  else
  {
    lat1 = abs(int(lat_ndsm / 4));
  }

  long1 = int(long_ndsm / 6);


  temp[0] = matrix1[lat1];
  if ((long_ndsm >= 0) && (long_ndsm < 180))
  {
    longdex = long1 + 30;
  }

  else
  {
    longdex = long1 + 29;
  }


  int first_part1 = matrix1_part1[longdex];

  int lat2 = int(abs(lat_ndsm) - abs(lat1) * 4);
  lat2 = lat2 - 1;
  if (lat2 < 0) {
    lat2 = 0;
  }

  int long2 = int(abs(long_ndsm) - abs(long1) * 6);


  temp[1] = matrix2[lat2][long2];


  int lat3 = int((lat_ndsm1 - int(lat_ndsm1)) * 4);
  lat3 = lat3 - 1;
  if (lat3 < 0 || lat3 == 0) {
    lat3 = 3;
  }

  int long3 = int((long_ndsm1 - int(long_ndsm1)) * 4);



  str_temp1 = (char)(((int)'0') + ((first_part1 / 10) % 10));
  str_temp2 = (char)(((int)'0') + (first_part1 % 10));
  str_temp3 = (char)(((int)'0') + ((third / 10) % 10));
  str_temp4 = (char)(((int)'0') + ((third) % 10));



  arr[0] = temp[0];
  arr[1] = str_temp1;
  arr[2] = str_temp2;
  arr[3] = temp[1];
  arr[4] = str_temp3;
  arr[5] = str_temp4;

  /*arr[0]=  0;
  arr[1] = 0;
  arr[2] = 0;
  arr[3] = 0;
  arr[4] = 0;
  arr[5] = 0;*/



  arr_ndsm.resize(6);
  for (int i = 0; i < 6; i++)
  {
    arr_ndsm[i] = arr[i];
  }

  return arr_ndsm;
  //ensureCoordinatesVisible();
}

QString QgsStatusBarCoordinatesWidget::LatLongToMilgridConversion(const QgsPointXY& mp)
{

  int false_easting = 500000, false_northing = 500000;

  double ecc = 298.2572, semi_minor_axis = 6356752.3142, e1 = 0.081819190842622, e = 2.718281828, semi_major_axis = 6378137.0;
  double diff = 6.0 / 7.0, pi = 180.0, correct;
  //double round(double, int);
  double clat, clong;
  clat = 0;
  clong = 0;
  correct = 3.14159265358979 / 180;
  QString zone[] = { "19A6","19A5","19A4","19A3","19A2","19A1","19A","19B","19C","19D","19E","19F","19G","19H","19J","19K","19L","19M","19N","19O","19P","19Q","19R","19S","19T","19U","19V","19W","19X","19Y","20A6","20A5","20A4","20A3","20A2","20A1","20A","20B","20C","20D","20E","20F","20G","20H","20J","20K","20L","20M","20N","20O","20P","20Q","20R","20S","20T","20U","20V","20W","20X","20Y","21A6","21A5","21A4","21A3","21A2","21A1","21A","21B","21C","21D","21E","21F","21G","21H","21J","21K","21L","21M","21N","21O","21P","21Q","21R","21S","21T","21U","21V","21W","21X","21Y","22A6","22A5","22A4","22A3","22A2","22A1","22A","22B","22C","22D","22E","22F","22G","22H","22J","22K","22L","22M","22N","22O","22P","22Q","22R","22S","22T","22U","22V","22W","22X","22Y","23A6","23A5","23A4","23A3","23A2","23A1","23A","23B","23C","23D","23E","23F","23G","23H","23J","23K","23L","23M","23N","23O","23P","23Q","23R","23S","23T","23U","23V","23W","23X","23Y","24A6","24A5","24A4","24A3","24A2","24A1","24A","24B","24C","24D","24E","24F","24G","24H","24J","24K","24L","24M","24N","24O","24P","24Q","24R","24S","24T","24U","24V","24W","24X","24Y","25A6","25A5","25A4","25A3","25A2","25A1","25A","25B","25C","25D","25E","25F","25G","25H","25J","25K","25L","25M","25N","25O","25P","25Q","25R","25S","25T","25U","25V","25W","25X","25Y","26A6","26A5","26A4","26A3","26A2","26A1","26A","26B","26C","26D","26E","26F","26G","26H","26J","26K","26L","26M","26N","26O","26P","26Q","26R","26S","26T","26U","26V","26W","26X","26Y","27A6","27A5","27A4","27A3","27A2","27A1","27A","27B","27C","27D","27E","27F","27G","27H","27J","27K","27L","27M","27N","27O","27P","27Q","27R","27S","27T","27U","27V","27W","27X","27Y","28A6","28A5","28A4","28A3","28A2","28A1","28A","28B","28C","28D","28E","28F","28G","28H","28J","28K","28L","28M","28N","28O","28P","28Q","28R","28S","28T","28U","28V","28W","28X","28Y","29A6","29A5","29A4","29A3","29A2","29A1","29A","29B","29C","29D","29E","29F","29G","29H","29J","29K","29L","29M","29N","29O","29P","29Q","29R","29S","29T","29U","29V","29W","29X","29Y","30A6","30A5","30A4","30A3","30A2","30A1","30A","30B","30C","30D","30E","30F","30G","30H","30J","30K","30L","30M","30N","30O","30P","30Q","30R","30S","30T","30U","30V","30W","30X","30Y","31A6","31A5","31A4","31A3","31A2","31A1","31A","31B","31C","31D","31E","31F","31G","31H","31J","31K","31L","31M","31N","31O","31P","31Q","31R","31S","31T","31U","31V","31W","31X","31Y","32A6","32A5","32A4","32A3","32A2","32A1","32A","32B","32C","32D","32E","32F","32G","32H","32J","32K","32L","32M","32N","32O","32P","32Q","32R","32S","32T","32U","32V","32W","32X","32Y","33A6","33A5","33A4","33A3","33A2","33A1","33A","33B","33C","33D","33E","33F","33G","33H","33J","33K","33L","33M","33N","33O","33P","33Q","33R","33S","33T","33U","33V","33W","33X","33Y","34A6","34A5","34A4","34A3","34A2","34A1","34A","34B","34C","34D","34E","34F","34G","34H","34J","34K","34L","34M","34N","34O","34P","34Q","34R","34S","34T","34U","34V","34W","34X","34Y","35A6","35A5","35A4","35A3","35A2","35A1","35A","35B","35C","35D","35E","35F","35G","35H","35J","35K","35L","35M","35N","35O","35P","35Q","35R","35S","35T","35U","35V","35W","35X","35Y","36A6","36A5","36A4","36A3","36A2","36A1","36A","36B","36C","36D","36E","36F","36G","36H","36J","36K","36L","36M","36N","36O","36P","36Q","36R","36S","36T","36U","36V","36W","36X","36Y","37A6","37A5","37A4","37A3","37A2","37A1","37A","37B","37C","37D","37E","37F","37G","37H","37J","37K","37L","37M","37N","37O","37P","37Q","37R","37S","37T","37U","37V","37W","37X","37Y","38A6","38A5","38A4","38A3","38A2","38A1","38A","38B","38C","38D","38E","38F","38G","38H","38J","38K","38L","38M","38N","38O","38P","38Q","38R","38S","38T","38U","38V","38W","38X","38Y","39A6","39A5","39A4","39A3","39A2","39A1","39A","39B","39C","39D","39E","39F","39G","39H","39J","39K","39L","39M","39N","39O","39P","39Q","39R","39S","39T","39U","39V","39W","39X","39Y","40A6","40A5","40A4","40A3","40A2","40A1","40A","40B","40C","40D","40E","40F","40G","40H","40J","40K","40L","40M","40N","40O","40P","40Q","40R","40S","40T","40U","40V","40W","40X","40Y","41A6","41A5","41A4","41A3","41A2","41A1","41A","41B","41C","41D","41E","41F","41G","41H","41J","41K","41L","41M","41N","41O","41P","41Q","41R","41S","41T","41U","41V","41W","41X","41Y","42A6","42A5","42A4","42A3","42A2","42A1","42A","42B","42C","42D","42E","42F","42G","42H","42J","42K","42L","42M","42N","42O","42P","42Q","42R","42S","42T","42U","42V","42W","42X","42Y","43A6","43A5","43A4","43A3","43A2","43A1","43A","43B","43C","43D","43E","43F","43G","43H","43J","43K","43L","43M","43N","43O","43P","43Q","43R","43S","43T","43U","43V","43W","43X","43Y","44A6","44A5","44A4","44A3","44A2","44A1","44A","44B","44C","44D","44E","44F","44G","44H","44J","44K","44L","44M","44N","44O","44P","44Q","44R","44S","44T","44U","44V","44W","44X","44Y","45A6","45A5","45A4","45A3","45A2","45A1","45A","45B","45C","45D","45E","45F","45G","45H","45J","45K","45L","45M","45N","45O","45P","45Q","45R","45S","45T","45U","45V","45W","45X","45Y","1A6","1A5","1A4","1A3","1A2","1A1","1A","1B","1C","1D","1E","1F","1G","1H","1J","1K","1L","1M","1N","1O","1P","1Q","1R","1S","1T","1U","1V","1W","1X","1Y","2A6","2A5","2A4","2A3","2A2","2A1","2A","2B","2C","2D","2E","2F","2G","2H","2J","2K","2L","2M","2N","2O","2P","2Q","2R","2S","2T","2U","2V","2W","2X","2Y","3A6","3A5","3A4","3A3","3A2","3A1","3A","3B","3C","3D","3E","3F","3G","3H","3J","3K","3L","3M","3N","3O","3P","3Q","3R","3S","3T","3U","3V","3W","3X","3Y","4A6","4A5","4A4","4A3","4A2","4A1","4A","4B","4C","4D","4E","4F","4G","4H","4J","4K","4L","4M","4N","4O","4P","4Q","4R","4S","4T","4U","4V","4W","4X","4Y","5A6","5A5","5A4","5A3","5A2","5A1","5A","5B","5C","5D","5E","5F","5G","5H","5J","5K","5L","5M","5N","5O","5P","5Q","5R","5S","5T","5U","5V","5W","5X","5Y","6A6","6A5","6A4","6A3","6A2","6A1","6A","6B","6C","6D","6E","6F","6G","6H","6J","6K","6L","6M","6N","6O","6P","6Q","6R","6S","6T","6U","6V","6W","6X","6Y","7A6","7A5","7A4","7A3","7A2","7A1","7A","7B","7C","7D","7E","7F","7G","7H","7J","7K","7L","7M","7N","7O","7P","7Q","7R","7S","7T","7U","7V","7W","7X","7Y","8A6","8A5","8A4","8A3","8A2","8A1","8A","8B","8C","8D","8E","8F","8G","8H","8J","8K","8L","8M","8N","8O","8P","8Q","8R","8S","8T","8U","8V","8W","8X","8Y","9A6","9A5","9A4","9A3","9A2","9A1","9A","9B","9C","9D","9E","9F","9G","9H","9J","9K","9L","9M","9N","9O","9P","9Q","9R","9S","9T","9U","9V","9W","9X","9Y","10A6","10A5","10A4","10A3","10A2","10A1","10A","10B","10C","10D","10E","10F","10G","10H","10J","10K","10L","10M","10N","10O","10P","10Q","10R","10S","10T","10U","10V","10W","10X","10Y","11A6","11A5","11A4","11A3","11A2","11A1","11A","11B","11C","11D","11E","11F","11G","11H","11J","11K","11L","11M","11N","11O","11P","11Q","11R","11S","11T","11U","11V","11W","11X","11Y","12A6","12A5","12A4","12A3","12A2","12A1","12A","12B","12C","12D","12E","12F","12G","12H","12J","12K","12L","12M","12N","12O","12P","12Q","12R","12S","12T","12U","12V","12W","12X","12Y","13A6","13A5","13A4","13A3","13A2","13A1","13A","13B","13C","13D","13E","13F","13G","13H","13J","13K","13L","13M","13N","13O","13P","13Q","13R","13S","13T","13U","13V","13W","13X","13Y","14A6","14A5","14A4","14A3","14A2","14A1","14A","14B","14C","14D","14E","14F","14G","14H","14J","14K","14L","14M","14N","14O","14P","14Q","14R","14S","14T","14U","14V","14W","14X","14Y","15A6","15A5","15A4","15A3","15A2","15A1","15A","15B","15C","15D","15E","15F","15G","15H","15J","15K","15L","15M","15N","15O","15P","15Q","15R","15S","15T","15U","15V","15W","15X","15Y","16A6","16A5","16A4","16A3","16A2","16A1","16A","16B","16C","16D","16E","16F","16G","16H","16J","16K","16L","16M","16N","16O","16P","16Q","16R","16S","16T","16U","16V","16W","16X","16Y","17A6","17A5","17A4","17A3","17A2","17A1","17A","17B","17C","17D","17E","17F","17G","17H","17J","17K","17L","17M","17N","17O","17P","17Q","17R","17S","17T","17U","17V","17W","17X","17Y","18A6","18A5","18A4","18A3","18A2","18A1","18A","18B","18C","18D","18E","18F","18G","18H","18J","18K","18L","18M","18N","18O","18P","18Q","18R","18S","18T","18U","18V","18W","18X","18Y" };

  int limiting_meridian;
  int limiting_parallel;
  int i, j, k, l = 0, i1;

  double std_para1 = 0.0, std_para2 = 0.0, temp1 = 0.0, temp2 = 0.0;
  double central_meridian, central_parallel;
  double dist_east = 0.0, dist_north = 0.0, pe = 0.0, pn = 0.0;
  double ro = 0.0, r = 0.0, r0 = 0.0, n1 = 0.0, n2 = 0.0, n = 0.0, n0 = 0.0, q0 = 0.0, q = 0.0, q1 = 0.0, q2 = 0.0, l1 = 0.0, zone_constant = 0.0, scale_factor = 0.0, scale_factor_center = 0.0;

  char third, fourth;
  char str_temp[20];
  QChar milgrid[22];
  QString str;
  double LAT = mp.y();
  double LONG = mp.x();



  for (i = 0; i < 22; i++)
    milgrid[i] = ' ';




  k = -1;
  for (i = -180; i <= 180; i = i + 8)
  {
    for (j = 90; j > (-91); j = j - 6)
    {
      k = k + 1;
      if ((LONG >= i) && (LONG < i + 8) && (LAT < j) && (LAT >= j - 6))
      {
        l = k;

        limiting_meridian = i;
        limiting_parallel = j;
        break;
      }
    }
    k = k - 1;
  }


  milgrid[0] = zone[l][0];
  milgrid[1] = zone[l][1];
  milgrid[2] = zone[l][2];
  milgrid[3] = zone[l][3];
  milgrid[4] = ' ';
  std_para1 = limiting_parallel - 6;
  std_para1 = std_para1 + diff;
  std_para2 = limiting_parallel;
  std_para2 = std_para2 - diff;
  central_meridian = limiting_meridian + 4;


  temp1 = 0.0;
  temp2 = (1 - e1 * sin(std_para1 * correct)) / (1 + e1 * sin(std_para1 * correct));
  temp1 = pow(temp2, (e1 / 2));
  q1 = tan(((pi / 4) + (std_para1 / 2)) * correct);
  q1 = temp1 * q1;
  q1 = log(q1);
  temp1 = 0.0;
  temp2 = 0.0;

  double sinph2, sinq2;
  sinph2 = sin(std_para2 * correct);
  sinq2 = e1 * sinph2;
  temp2 = (1 - sinq2) / (1 + sinq2);
  temp2 = (1 - e1 * sin(std_para2 * correct)) / (1 + e1 * sin(std_para2 * correct));

  temp1 = pow(temp2, (e1 / 2));
  q2 = temp1 * (tan(((pi / 4) + (std_para2 / 2)) * correct));
  q2 = log(q2);
  temp1 = 0.0;
  temp2 = 0.0;


  temp2 = (1 - e1 * sin(LAT * correct)) / (1 + e1 * sin(LAT * correct));
  temp1 = pow(temp2, (e1 / 2));
  q = temp1 * (tan(((pi / 4) + (LAT / 2)) * correct));
  q = log(q);
  temp1 = 0.0;
  temp2 = 0.0;


  temp1 = pow(e1, 2.0) * pow(sin(std_para1 * correct), 2);
  n1 = semi_major_axis / sqrt(1 - temp1);
  temp1 = 0.0;

  temp1 = pow(e1, 2.0) * pow(sin(std_para2 * correct), 2);
  n2 = semi_major_axis / sqrt(1 - temp1);
  temp1 = 0.0;

  temp1 = pow(e1, 2.0) * pow(sin(LAT * correct), 2);
  n = semi_major_axis / sqrt(1 - temp1);
  temp1 = 0.0;

  temp1 = log(n1 * cos(std_para1 * correct));
  temp2 = log(n2 * cos(std_para2 * correct));
  temp1 = temp1 - temp2;
  l1 = temp1 / (q2 - q1);
  temp1 = 0.0;
  temp1 = asin(l1);
  central_parallel = temp1 / correct;
  temp1 = 0.0;

  temp2 = (1 - e1 * sin(central_parallel * correct)) / (1 + e1 * sin(central_parallel * correct));
  temp1 = pow(temp2, (e1 / 2));
  q0 = temp1 * (tan(((pi / 4) + (central_parallel / 2)) * correct));
  q0 = log(q0);
  temp1 = 0.0;
  temp2 = 0.0;

  temp1 = pow(e1, 2.0) * pow(sin(central_parallel * correct), 2);
  n0 = semi_major_axis / sqrt(1 - temp1);
  temp1 = 0.0;


  zone_constant = (n1 * cos(std_para1 * correct)) / (l1 * exp(-l1 * q1));
  zone_constant = (n2 * cos(std_para2 * correct)) / (l1 * exp(-l1 * q2));


  scale_factor = (zone_constant * l1 * exp(-l1 * q)) / (n * cos(LAT * correct));
  scale_factor_center = (zone_constant * l1 * exp(-l1 * q0)) / (n0 * cos(central_parallel * correct));

  r0 = scale_factor_center * n0 * (1 / (tan(central_parallel * correct)));
  r = zone_constant * exp(-l1 * q);

  dist_east = r * sin(l1 * (LONG - central_meridian) * correct);
  dist_north = r0 - r * cos(l1 * (LONG - central_meridian) * correct);

  pe = false_easting + dist_east;
  pn = false_northing + dist_north;

  i = round(pe);
  j = round(pn);

  if ((i >= 0) && (i < 100000))
    fourth = 'A';
  else if ((i >= 100000) && (i < 200000))
    fourth = 'B';
  else if ((i >= 200000) && (i < 300000))
    fourth = 'C';
  else if ((i >= 300000) && (i < 400000))
    fourth = 'D';
  else if ((i >= 400000) && (i < 500000))
    fourth = 'E';
  else if ((i >= 500000) && (i < 600000))
    fourth = 'F';
  else if ((i >= 600000) && (i < 700000))
    fourth = 'G';
  else if ((i >= 700000) && (i < 800000))
    fourth = 'H';
  else if ((i >= 800000) && (i < 900000))
    fourth = 'J';
  else if ((i >= 900000) && (i < 1000000))
    fourth = 'K';

  milgrid[5] = fourth;

  if ((j < 100000) && (j >= 900000))
    third = 'K';
  else if ((j < 900000) && (j >= 800000))
    third = 'J';
  else if ((j < 800000) && (j >= 700000))
    third = 'H';
  else if ((j < 700000) && (j >= 600000))
    third = 'G';
  else if ((j < 600000) && (j >= 500000))
    third = 'F';
  else if ((j < 500000) && (j >= 400000))
    third = 'E';
  else if ((j < 400000) && (j >= 300000))
    third = 'D';
  else if ((j < 300000) && (j >= 200000))
    third = 'C';
  else if ((j < 200000) && (j >= 100000))
    third = 'B';
  else if ((j < 100000) && (j >= 0))
    third = 'A';

  milgrid[6] = third;
  milgrid[7] = ' ';

  sprintf(str_temp, "%d", i);
  k = strlen(str_temp);
  if (k < 6)
  {
    for (l = 0; l < k; l++)
      milgrid[7 + l] = str_temp[l];
  }
  else
  {
    for (l = 1; l < k; l++)
      milgrid[7 + l] = str_temp[l];
  }
  //strcpy(str_temp,"");
  sprintf(str_temp, "%d", j);
  i1 = strlen(str_temp);
  for (l = 1; l < i1; l++)
    milgrid[7 + l + k] = str_temp[l];
  milgrid[7 + l + k + 1] = '\0';


  str.resize(20);
  for (i = 0; i < 20; i++)
  {
    str[i] = milgrid[i];
  }

  return str;
}

QString QgsStatusBarCoordinatesWidget::eveLatLongToMilgridConversion(const QgsPointXY& mp)
{
  double std_para1 = 0.0, std_para2 = 0.0, temp1 = 0.0, temp2 = 0.0;
  double central_meridian, central_parallel;
  double dist_east = 0.0, dist_north = 0.0, pe = 0.0, pn = 0.0;
  double r = 0.0, r0 = 0.0, n1 = 0.0, n2 = 0.0, n = 0.0, n0 = 0.0, q0 = 0.0, q = 0.0, q1 = 0.0, q2 = 0.0, l1 = 0.0, zone_constant = 0.0, scale_factor = 0.0, scale_factor_center = 0.0;
  int false_easting, false_northing;
  double semi_minor_axis = 6356100.231, e = 2.718281828, semi_major_axis = 6377301.243, e1 = 0.0814729759340358;
  double pi = 180, correct = 3.14159265358979 / 180;
  int var = 0;

  QString finaloutput1, finaloutput2, grid;
  int grid_return[4];
  char str_temp1[1];
  char str_temp2[1];
  char str_temp3[9];
  char str_temp4[9];

  // Datum shift: WGS84 to Everest (Kalianpur 1975)
  double LAT_dsm = mp.y();
  double LONG_dsm = mp.x();
  double LAT, LONG, a, b, E, v, h, X, Y, Z, RAD;
  double dX = 295;
  double dY = 736;
  double dZ = 257;
  double a1, b1, E1, v1, Xb, Yb, Zb, TempLat, LAT_skc;
  int i;

  a = 6378137;
  b = 6356752.3142;
  h = 0.0;
  RAD = 3.14159265358979 / 180;
  TempLat = 0;
  E = sqrt((a * a - b * b) / (a * a));
  v = a / (sqrt(1 - (pow(E, 2)) * sin(RAD * LAT_dsm) * sin(RAD * LAT_dsm)));
  X = (v + h) * cos(RAD * LAT_dsm) * cos(RAD * LONG_dsm);
  Y = (v + h) * cos(RAD * LAT_dsm) * sin(RAD * LONG_dsm);
  Z = (((1 - pow(E, 2)) * v) + h) * sin(RAD * LAT_dsm);

  Xb = X - dX;
  Yb = Y - dY;
  Zb = Z - dZ;

  a1 = 6377301.243;
  b1 = 6356100.231;
  E1 = sqrt((a1 * a1 - b1 * b1) / (a1 * a1));
  v1 = a1 / sqrt(1 - pow(E1, 2) * sin(RAD * LAT_dsm) * sin(RAD * LAT_dsm));

  for (i = 0; i < 11; i++)
  {
    LAT_skc = TempLat;
    LAT_skc = (atan((Zb + (pow(E1, 2)) * v1 * sin(LAT_skc * RAD)) / (sqrt(Xb * Xb + Yb * Yb)))) / RAD;
    v1 = a1 / (sqrt(1 - pow(E1, 2) * sin(RAD * LAT_skc) * sin(RAD * LAT_skc)));
    TempLat = LAT_skc;
  }

  LAT = TempLat;
  LONG = atan(Yb / Xb) / RAD;
  if (LONG < 0) {
    LONG = 180 + LONG;
  }

  double CLong, CLat, sp1, sp2;
  double Lat_Grid_org, Long_Grid_Org, Central_Scale_Factor, False_Easting, False_Northing;

  CLong = LONG + 2.77777777777778E-10; // ADD THIS TO AVOID ERROR AT LIMITING ZONE
  CLat = LAT + 2.77777777777778E-10;
  Central_Scale_Factor = 823.0 / 824.0;
  False_Easting = 2743196.4;
  False_Northing = 914398.8;

  if (CLong >= 60 && CLong <= 78 && CLat >= 36 && CLat <= 44)
  {
    Lat_Grid_org = 39.5;
    Long_Grid_Org = 68;
    False_Easting = 2153866.4;
    False_Northing = 2368292.9;
    Central_Scale_Factor = 649.0 / 650.0;
    sp1 = 36.2916667;
    sp2 = 42.65625;
  }
  else if (CLong >= 60 && CLong <= 79 && CLat >= 28 && CLat <= 36) {
    grid = "I-A";
    Lat_Grid_org = 32.5;
    Long_Grid_Org = 68;
    sp1 = 29.6552711;
    sp2 = 35.3144722;
  }
  else if (CLong >= 79 && CLong <= 81 && CLat >= 29 && CLat <= 36) {
    grid = "I-A";
    Lat_Grid_org = 32.5;
    Long_Grid_Org = 68;
    sp1 = 29.6552711;
    sp2 = 35.3144722;
  }
  else if (CLong >= 78 && CLong <= 81 && CLat >= 36 && CLat <= 37) {
    grid = "I-B";
    Lat_Grid_org = 32.5;
    Long_Grid_Org = 90;
    sp1 = 29.6552711;
    sp2 = 35.3144722;
  }
  else if (CLong >= 81 && CLong <= 102 && CLat >= 30 && CLat <= 37) {
    grid = "I-B";
    Lat_Grid_org = 32.5;
    Long_Grid_Org = 90;
    sp1 = 29.6552711;
    sp2 = 35.3144722;
  }
  else if (CLong >= 57 && CLong <= 60 && CLat >= 25 && CLat <= 28) {
    grid = "II-A";
    Lat_Grid_org = 26;
    Long_Grid_Org = 74;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 60 && CLong <= 72 && CLat >= 20 && CLat <= 28) {
    grid = "II-A";
    Lat_Grid_org = 26;
    Long_Grid_Org = 74;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 72 && CLong <= 79 && CLat >= 22 && CLat <= 28) {
    grid = "II-A";
    Lat_Grid_org = 26;
    Long_Grid_Org = 74;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 79 && CLong <= 82 && CLat >= 22 && CLat <= 29) {
    grid = "II-A";
    Lat_Grid_org = 26;
    Long_Grid_Org = 74;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 82 && CLong <= 83 && CLat >= 26 && CLat <= 28) {
    grid = "II-A";
    Lat_Grid_org = 26;
    Long_Grid_Org = 74;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 81 && CLong <= 82 && CLat >= 29 && CLat <= 30) {
    grid = "II-B";
    Lat_Grid_org = 26;
    Long_Grid_Org = 90;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 82 && CLong <= 83 && CLat >= 28 && CLat <= 30) {
    grid = "II-B";
    Lat_Grid_org = 26;
    Long_Grid_Org = 90;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 82 && CLong <= 83 && CLat >= 22 && CLat <= 26) {
    grid = "II-B";
    Lat_Grid_org = 26;
    Long_Grid_Org = 90;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 83 && CLong <= 85 && CLat >= 22 && CLat <= 30) {
    grid = "II-B";
    Lat_Grid_org = 26;
    Long_Grid_Org = 90;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 85 && CLong <= 91 && CLat >= 21 && CLat <= 30) {
    grid = "II-B";
    Lat_Grid_org = 26;
    Long_Grid_Org = 90;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 91 && CLong <= 93 && CLat >= 22 && CLat <= 30) {
    grid = "II-B";
    Lat_Grid_org = 26;
    Long_Grid_Org = 90;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 93 && CLong <= 110 && CLat >= 22.5 && CLat <= 30) {
    grid = "II-B";
    Lat_Grid_org = 26;
    Long_Grid_Org = 90;
    sp1 = 23.157823056;
    sp2 = 28.81894083;
  }
  else if (CLong >= 60 && CLong <= 72 && CLat >= 15 && CLat <= 20) {
    grid = "III-A";
    Lat_Grid_org = 19;
    Long_Grid_Org = 80;
    sp1 = 16.1603875;
    sp2 = 21.8231625;
  }
  else if (CLong >= 72 && CLong <= 85 && CLat >= 15 && CLat <= 22) {
    grid = "III-A";
    Lat_Grid_org = 19;
    Long_Grid_Org = 80;
    sp1 = 16.1603875;
    sp2 = 21.8231625;
  }
  else if (CLong >= 85 && CLong <= 90 && CLat >= 15 && CLat <= 21) {
    grid = "III-A";
    Lat_Grid_org = 19;
    Long_Grid_Org = 80;
    sp1 = 16.1603875;
    sp2 = 21.8231625;
  }
  else if (CLong >= 90 && CLong <= 91 && CLat >= 15.5 && CLat <= 21) {
    grid = "III-B";
    Lat_Grid_org = 19;
    Long_Grid_Org = 100;
    sp1 = 16.1603875;
    sp2 = 21.8231625;
  }
  else if (CLong >= 91 && CLong <= 93 && CLat >= 15.5 && CLat <= 22) {
    grid = "III-B";
    Lat_Grid_org = 19;
    Long_Grid_Org = 100;
    sp1 = 16.1603875;
    sp2 = 21.8231625;
  }
  else if (CLong >= 93 && CLong <= 112 && CLat >= 15.5 && CLat <= 22.5) {
    grid = "III-B";
    Lat_Grid_org = 19;
    Long_Grid_Org = 100;
    sp1 = 16.1603875;
    sp2 = 21.8231625;
  }
  else if (CLong >= 112 && CLong <= 116 && CLat >= 15.5 && CLat <= 22.5) {
    grid = "III-B";
    Lat_Grid_org = 19;
    Long_Grid_Org = 100;
    sp1 = 16.1603875;
    sp2 = 21.8231625;
  }
  else if (CLong >= 60 && CLong <= 76 && CLat >= 8 && CLat <= 15) {
    grid = "IV-A";
    Lat_Grid_org = 12;
    Long_Grid_Org = 80;
    sp1 = 9.162838611;
    sp2 = 14.82698583;
  }
  else if (CLong >= 76 && CLong <= 79.5 && CLat >= 5.5 && CLat <= 15) {
    grid = "IV-A";
    Lat_Grid_org = 12;
    Long_Grid_Org = 80;
    sp1 = 9.162838611;
    sp2 = 14.82698583;
  }
  else if (CLong >= 79.5 && CLong <= 82 && CLat >= 10 && CLat <= 15) {
    grid = "IV-A";
    Lat_Grid_org = 12;
    Long_Grid_Org = 80;
    sp1 = 9.162838611;
    sp2 = 14.82698583;
  }
  else if (CLong >= 82 && CLong <= 90 && CLat >= 8 && CLat <= 15) {
    grid = "IV-A";
    Lat_Grid_org = 12;
    Long_Grid_Org = 80;
    sp1 = 9.162838611;
    sp2 = 14.82698583;
  }
  else if (CLong >= 90 && CLong <= 92 && CLat >= 8 && CLat <= 15.5) {
    grid = "IV-B";
    Lat_Grid_org = 12;
    Long_Grid_Org = 104;
    sp1 = 9.162838611;
    sp2 = 14.82698583;
  }
  else if (CLong >= 92 && CLong <= 94 && CLat >= 6 && CLat <= 15.5) {
    grid = "IV-B";
    Lat_Grid_org = 12;
    Long_Grid_Org = 104;
    sp1 = 9.162838611;
    sp2 = 14.82698583;
  }
  else if (CLong >= 94 && CLong <= 98.67 && CLat >= 7 && CLat <= 15.5) {
    grid = "IV-B";
    Lat_Grid_org = 12;
    Long_Grid_Org = 104;
    sp1 = 9.162838611;
    sp2 = 14.82698583;
  }
  else if (CLong >= 98.67 && CLong <= 105 && CLat >= 8 && CLat <= 15.5) {
    grid = "IV-B";
    Lat_Grid_org = 12;
    Long_Grid_Org = 104;
    sp1 = 9.162838611;
    sp2 = 14.82698583;
  }
  else if (CLong >= 105 && CLong <= 116 && CLat >= 7 && CLat <= 15.5) {
    grid = "IV-B";
    Lat_Grid_org = 12;
    Long_Grid_Org = 104;
    sp1 = 9.162838611;
    sp2 = 14.82698583;
  }

  central_meridian = Long_Grid_Org;
  central_parallel = Lat_Grid_org;
  std_para1 = sp1;
  std_para2 = sp2;
  false_easting = False_Easting;
  false_northing = False_Northing;

  // this is to find q1
  temp2 = (1 - e1 * sin(std_para1 * correct)) / (1 + e1 * sin(std_para1 * correct));
  temp1 = pow(temp2, (e1 / 2));
  q1 = tan(((pi / 4) + (std_para1 / 2)) * correct);
  q1 = temp1 * q1;
  q1 = log(q1);

  // this is to find q2
  temp1 = 0.0;
  temp2 = (1 - e1 * sin(std_para2 * correct)) / (1 + e1 * sin(std_para2 * correct));
  temp1 = pow(temp2, (e1 / 2));
  q2 = temp1 * (tan(((pi / 4) + (std_para2 / 2)) * correct));
  q2 = log(q2);

  // this is to find q
  temp1 = 0.0;
  temp2 = (1 - e1 * sin(LAT * correct)) / (1 + e1 * sin(LAT * correct));
  temp1 = pow(temp2, (e1 / 2));
  q = temp1 * (tan(((pi / 4) + (LAT / 2)) * correct));
  q = log(q);

  // this is to find n1
  temp1 = pow(e1, 2.0) * pow(sin(std_para1 * correct), 2);
  n1 = semi_major_axis / sqrt(1 - temp1);

  // this is to find n2
  temp1 = pow(e1, 2.0) * pow(sin(std_para2 * correct), 2);
  n2 = semi_major_axis / sqrt(1 - temp1);

  // this is to find n
  temp1 = pow(e1, 2.0) * pow(sin(LAT * correct), 2);
  n = semi_major_axis / sqrt(1 - temp1);

  // this is to find l1
  temp1 = log(n1 * cos(std_para1 * correct));
  temp2 = log(n2 * cos(std_para2 * correct));
  temp1 = temp1 - temp2;
  l1 = temp1 / (q2 - q1);

  temp1 = asin(l1);

  // this is to find q0
  temp1 = 0.0;
  temp2 = (1 - e1 * sin(central_parallel * correct)) / (1 + e1 * sin(central_parallel * correct));
  temp1 = pow(temp2, (e1 / 2));
  q0 = temp1 * (tan(((pi / 4) + (central_parallel / 2)) * correct));
  q0 = log(q0);

  // this is to find n0
  temp1 = pow(e1, 2.0) * pow(sin(central_parallel * correct), 2);
  n0 = semi_major_axis / sqrt(1 - temp1);

  // this is to find zone_constant(K)
  zone_constant = (n1 * cos(std_para1 * correct)) / (l1 * exp(-l1 * q1));

  // this is to find k
  scale_factor = (zone_constant * l1 * exp(-l1 * q)) / (n * cos(LAT * correct));

  // this is to find k0
  scale_factor_center = (zone_constant * l1 * exp(-l1 * q0)) / (n0 * cos(central_parallel * correct));

  // this is to find r0 and r
  r0 = zone_constant * exp(-l1 * q0);
  r = zone_constant * exp(-l1 * q);

  // this is to find dist_east and dist_north
  dist_east = r * sin(l1 * (LONG - central_meridian) * correct);
  dist_north = r0 - r * cos(l1 * (LONG - central_meridian) * correct);

  // this is to find pe and pn
  pe = false_easting + dist_east;
  pn = false_northing + dist_north;

  int Ep = round(pe);
  int Np = round(pn);

  int easting = abs(Ep);
  int northing = abs(Np);

  int EastM1 = easting / 500000;
  if (EastM1 >= 5) EastM1 = EastM1 % 5;

  int NorthM2 = northing / 500000;
  if (NorthM2 >= 5) NorthM2 = NorthM2 % 5;

  int rem1 = easting % 500000;
  int rem2 = northing % 500000;
  int eastn1 = rem1 / 100000;
  if (eastn1 >= 5) eastn1 = eastn1 % 5;

  int northn2 = rem2 / 100000;
  if (northn2 >= 5) northn2 = northn2 % 5;

  char p2arr_skc[18];
  char grid1[5][5] = {
    {'V', 'Q', 'L', 'F', 'A'},
    {'W', 'R', 'M', 'G', 'B'},
    {'X', 'S', 'N', 'H', 'C'},
    {'Y', 'T', 'O', 'J', 'D'},
    {'Z', 'U', 'P', 'K', 'E'}
  };

  p2arr_skc[0] = grid1[EastM1][NorthM2];
  p2arr_skc[1] = grid1[eastn1][northn2];
  p2arr_skc[2] = ' ';

  char str_temp3_skc[12];
  char str_temp4_skc[12];
  sprintf(str_temp3_skc, "%d", easting);
  sprintf(str_temp4_skc, "%d", northing);
  int k_len = strlen(str_temp3_skc);
  for (int i_val = 0; i_val < 5; i_val++)
  {
    p2arr_skc[3 + i_val] = str_temp3_skc[i_val + k_len - 5];
  }
  p2arr_skc[8] = ' ';
  int a_len = strlen(str_temp4_skc);
  for (int w_skc = 0; w_skc < 5; w_skc++)
  {
    p2arr_skc[9 + w_skc] = str_temp4_skc[w_skc + a_len - 5];
  }

  QString finaloutput1_skc;
  finaloutput1_skc.resize(14);
  for (int y_skc = 0; y_skc < 14; y_skc++)
  {
    finaloutput1_skc[y_skc] = p2arr_skc[y_skc];
  }

  return grid + " " + finaloutput1_skc;
}
const char* QgsStatusBarCoordinatesWidget::check_row_2_sides(int r)
{
  if (r == 0 || r == 1) return "GRID IB";
  if (r >= 2 && r <= 7) return "GRID IIB";
  if (r >= 8 && r <= 10) return "GRID IIIB";
  return nullptr;
}

int QgsStatusBarCoordinatesWidget::check_domain_2_sides(float lat1, float long1, float lat2, float long2, double latitude, double longitude)
{
  if (latitude > lat1 && latitude <= lat2) {
    if (longitude > long1 && longitude <= long2) {
      return 1;
    }
  }
  return 0;
}

const char* QgsStatusBarCoordinatesWidget::check_row_3_sides(int r)
{
  if (r == 0) return "GRID O";
  if (r == 1 || r == 2) return "GRID IA";
  if (r >= 3 && r <= 7) return "GRID IIA";
  if (r >= 8 && r <= 10) return "GRID IIIA";
  return nullptr;
}

int QgsStatusBarCoordinatesWidget::check_domain_3_sides(float lat1, float long1, float lat2, float long2, double latitude, double longitude)
{
  if (latitude > lat1 && latitude <= lat2) {
    if (longitude >= long1 && longitude <= long2) {
      return 1;
    }
  }
  return 0;
}

const char* QgsStatusBarCoordinatesWidget::check_row_4_sides(int r) {
  if (r >= 0 && r <= 3) return "GRID IVA";
  if (r >= 4 && r <= 7) return "GRID IVB";
  return nullptr;
}

int QgsStatusBarCoordinatesWidget::check_domain_4_sides(float lat1, float long1, float lat2, float long2, double latitude, double longitude)
{
  if (latitude >= lat1 && latitude <= lat2) {
    if (longitude >= long1 && longitude <= long2) {
      return 1;
    }
  }
  else {
    return 0;
  }
  return 0;
}

int* QgsStatusBarCoordinatesWidget::checkarray(int pe_local, int pn_local)
{
  static int temp[2];
  char ch = 'A';
  char arr[10][10];
  //this loop is for aray arr
  for (int i = 9; i >= 0; i--)
  {
    for (int j = 0; j < 10; j++)
    {
      if (i == 4 && j == 0)
      {
        ch = 'A';
      }
      else if (j == 5)
      {
        ch = arr[i][0];
      }
      arr[i][j] = ch;
      if (ch == 'H') {
        ch++;
      }
      ch++;
    }
  }
  int pe1_local = int(pe_local / 500);
  int pn1_local = int(pn_local / 500);
  int var = arr[pn1_local][pe1_local];
  int pe2_local = int((pe_local - (pe1_local * 500))) / 100;
  int pn2_local = int((pn_local - (pn1_local * 500))) / 100;
  int var1 = arr[pn2_local][pe2_local];
  int* pointer = temp;
  temp[0] = var;
  temp[1] = var1;
  return pointer;						//returns value of array

}
//Nihcas above

//Overload updatecordinate



void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()
{
  const QgsCoordinateReferenceSystem coordinateCrs = QgsProject::instance()->displaySettings()->coordinateCrs();

  const Qgis::CoordinateOrder projectOrder = QgsProject::instance()->displaySettings()->coordinateAxisOrder();
  const Qgis::CoordinateOrder order = projectOrder == Qgis::CoordinateOrder::Default ? QgsCoordinateReferenceSystemUtils::defaultCoordinateOrderForCrs( coordinateCrs ) : projectOrder;

  switch ( order )
  {
    case Qgis::CoordinateOrder::XY:
      if ( coordinateCrs.isGeographic() )
        mLineEdit->setToolTip( tr( "Current map coordinate (Longitude, Latitude)" ) );
      else
        mLineEdit->setToolTip( tr( "Current map coordinate (Easting, Northing)" ) );
      break;
    case Qgis::CoordinateOrder::YX:
      if ( coordinateCrs.isGeographic() )
        mLineEdit->setToolTip( tr( "Current map coordinate (Latitude, Longitude)" ) );
      else
        mLineEdit->setToolTip( tr( "Current map coordinate (Northing, Easting)" ) );
      break;
    case Qgis::CoordinateOrder::Default:
      break;
  }

  updateCoordinateDisplay();
}
