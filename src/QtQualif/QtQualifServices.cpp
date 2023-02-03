#include "QtQualif/QtQualifServices.h"
#include "GQualif/QualifHelper.h"

#include <QwtCharts/QwtChartsManager.h>

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <iostream>
#include <assert.h>


USING_UTIL
USING_STD

namespace GQualif
{

QtQualifServices::QtQualifServices ( )
{
	assert (0 && "QtQualifServices constructor is not allowed.");
}	// QtQualifServices::QtQualifServices


QtQualifServices::QtQualifServices (const QtQualifServices&)
{
	assert (0 && "QtQualifServices copy constructor is not allowed.");
}	// QtQualifServices::QtQualifServices


QtQualifServices& QtQualifServices::operator = (const QtQualifServices&)
{
	assert (0 && "QtQualifServices assignment operator is not allowed.");
	return *this;
}	// QtQualifServices::operator =


QtQualifServices::~QtQualifServices ( )
{
	assert (0 && "QtQualifServices destructor is not allowed.");
}	// QtQualifServices::~QtQualifServices


Version QtQualifServices::getVersion ( )
{
//	static const Version	version (Q_QUALIF_VERSION);
//	return version;
	return QualifHelper::getVersion ( );
}	// QtQualifServices::getVersion


void QtQualifServices::initialize ( )
{
	QwtChartsManager::initialize ( );
}	// QtQualifServices::initialize


void QtQualifServices::finalize ( )
{
	QwtChartsManager::finalize ( );
}	// QtQualifServices::finalize


}	// namespace GQualif

