#include "GQualif/AbstractQualifTask.h"
#include "GQualif/QualifHelper.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>

#include <iostream>
#include <assert.h>

#include <cmath>


USING_UTIL
USING_STD
using namespace Qualif;

namespace GQualif
{

AbstractQualifTask::AbstractQualifTask (
	size_t types, Critere criterion, const vector<AbstractQualifSerie*>& series)
	: _types (types), _criterion (criterion), _series (series), _seriesValues( )
{
}	// AbstractQualifTask::AbstractQualifTask


AbstractQualifTask::AbstractQualifTask (const AbstractQualifTask&)
	: _types ((size_t)-1), _criterion ((Critere)-1), _series ( ),
	  _seriesValues ( )
{
	assert (0 && "AbstractQualifTask copy constructor is not allowed.");
}	// AbstractQualifTask::AbstractQualifTask


AbstractQualifTask& AbstractQualifTask::operator = (const AbstractQualifTask&)
{
	assert (0 && "AbstractQualifTask assignment operator is not allowed.");
	return *this;
}	// AbstractQualifTask::operator =


AbstractQualifTask::~AbstractQualifTask ( )
{
}	// AbstractQualifTask::~AbstractQualifTask


size_t AbstractQualifTask::getSeriesNum ( ) const
{
	return _series.size ( );
}	// AbstractQualifTask::getSeriesNum


size_t AbstractQualifTask::getDataTypes ( ) const
{
	return _types;
}	// AbstractQualifTask::getDataTypes


Critere AbstractQualifTask::getCriterion ( ) const
{
	return _criterion;
}	// AbstractQualifTask::getCriterion


const vector<AbstractQualifSerie*>& AbstractQualifTask::getSeries ( ) const
{
	return _series;
}	// AbstractQualifTask::getSeries


vector<AbstractQualifSerie*>& AbstractQualifTask::getSeries ( )
{
	return _series;
}	// AbstractQualifTask::getSeries


void AbstractQualifTask::increment (size_t cl, size_t s, size_t c)
{
	_seriesValues [cl][s]	+= 1;
}	// AbstractQualifTask::increment


void AbstractQualifTask::initializeSeriesValues (
										size_t classNum, size_t seriesNum)
{
	_seriesValues.resize (classNum);
	for (size_t i = 0; i < classNum; i++)
	{
		_seriesValues [i].resize (seriesNum);
		for (size_t j = 0; j < seriesNum; j++)
			_seriesValues [i][j]	= 0;
	}
}	// AbstractQualifTask::initializeSeriesValues


bool AbstractQualifTask::isThreadable ( ) const
{
	for (vector<AbstractQualifSerie*>::const_iterator its = _series.begin ( );
	     _series.end ( ) != its; its++)
		if (false == (*its)->isThreadable ( ))
			return false;

	return true;
}	// AbstractQualifTask::getDataTypes


void AbstractQualifTask::execute ( )
{
	throw Exception ("AbstractQualifTask::execute : méthode à surcharger.");
}	// AbstractQualifTask::execute


const vector< vector <size_t> >&  AbstractQualifTask::getSeriesValues ( ) const
{
	return _seriesValues;
}	// AbstractQualifTask::getSeriesValues


vector< vector <size_t> >&  AbstractQualifTask::getSeriesValues ( )
{
	return _seriesValues;
}	// AbstractQualifTask::getSeriesValues




}	// namespace GQualif

