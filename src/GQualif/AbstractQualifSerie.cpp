#include "GQualif/AbstractQualifSerie.h"
#include "GQualif/QualifHelper.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>

#include <iostream>
#include <assert.h>

#include <cmath>


USING_UTIL
USING_STD
using namespace Qualif;

static const Charset	charset ("àéèùô");

namespace GQualif
{

AbstractQualifSerie::AbstractQualifSerie (
			const string& fileName, const string& name, unsigned char dimension)
	: _fileName (fileName), _name (name),
	  _dimension (dimension), _dataTypes (0),
	  _coordinatesValidityEvaluated (false), _coordinatesValidity (false),
	  _coordinatesValidityErrorMsg (charset),
	  _storedValues ( ), _minValues ( ), _maxValues ( ),
	  _storedValuesDefined ( ), _classesCellsIndexes ( )
{
}	// AbstractQualifSerie::AbstractQualifSerie


AbstractQualifSerie::AbstractQualifSerie (const AbstractQualifSerie&)
	: _fileName ( ), _name ( ),
	  _dimension ((unsigned char)-1), _dataTypes (0),
	  _coordinatesValidityEvaluated (false), _coordinatesValidity (false),
	  _coordinatesValidityErrorMsg (charset),
	  _storedValues ( ), _minValues ( ), _maxValues ( ),
	  _storedValuesDefined ( ), _classesCellsIndexes ( )
{
	assert (0 && "AbstractQualifSerie copy constructor is not allowed.");
}	// AbstractQualifSerie::AbstractQualifSerie


AbstractQualifSerie& AbstractQualifSerie::operator = (
												const AbstractQualifSerie&)
{
	assert (0 && "AbstractQualifSerie assignment operator is not allowed.");
	return *this;
}	// AbstractQualifSerie::operator =


AbstractQualifSerie::~AbstractQualifSerie ( )
{
}	// AbstractQualifSerie::~AbstractQualifSerie


string AbstractQualifSerie::getName ( ) const
{
	return _name;
}	// AbstractQualifSerie::getName


string AbstractQualifSerie::getFileName ( ) const
{
	return _fileName;
}	// AbstractQualifSerie::getFileName


unsigned char AbstractQualifSerie::getDimension ( ) const
{
	return _dimension;
}	// AbstractQualifSerie::getDimension


void AbstractQualifSerie::getDataRange (
					Qualif::Critere criterion, double& min, double& max) const
{
	min	= DBL_MAX;	max	= -DBL_MAX;
	const size_t	count	= getCellCount ( );

	// Ce calcul a t-il déjà été fait ?
	if (false == isCriteriaStored (criterion))
	{	// Non => allocation et calcul pour toutes les mailles. On en profite
		// pour récupérer min/max :
		_storedValues.insert(std::pair<Qualif::Critere,std::vector<double> > (criterion, std::vector<double> (count)));
		_storedValuesDefined.insert(std::pair<Qualif::Critere,std::vector<bool> > (criterion, std::vector<bool> (count,true)));
		_minValues.insert(pair <Qualif::Critere, double> (criterion, -DBL_MAX));
		_maxValues.insert(pair <Qualif::Critere, double> (criterion, DBL_MAX));

		for (size_t c = 0; c < count; c++)
		{

			try
			{
				Maille&	cell	= getCell (c);
				double	value	= cell.AppliqueCritere (criterion);
				min	= value < min ? value : min;
				max	= value > max ? value : max;

				_storedValues [criterion][c] = value;
			}
			catch (...)
			{	// Certains types ne sont pas supportés (pentagone, ...).
				// On ne s'arrête pas à ça.
				_storedValuesDefined[criterion][c] = false;
			}

		}	// for (size_t c = 0; c < count; c++)

		_minValues [criterion]	= min;
		_maxValues [criterion]	= max;

	}	// if (false == isCriteriaStored (criterion))
	else
	{	// Calcul déjà fait => on récupère min/max :
		min	= _minValues [criterion];
		max	= _maxValues [criterion];
	}	// else if (false == isCriteriaStored (criterion))
}	// AbstractQualifSerie::getDataRange


size_t AbstractQualifSerie::getDataTypes ( ) const
{
	if (0 == _dataTypes)
	{
		const size_t	count	= getCellCount ( );
		const bool		volumic	= isVolumic ( );
		for (size_t c = 0; c < count; c++)
		{
			try
			{
				_dataTypes	|= getCellType (c);
			}
			catch (...)
			{	// Certains types ne sont pas supportés (pentagone, ...).
				// On ne s'arrête pas à ça.
				continue;
			}

		}	// for (size_t c = 0; c < count; c++)
	}	// if (0 == _dataTypes)

	return _dataTypes;
}	// AbstractQualifSerie::getDataTypes


void AbstractQualifSerie::validateCoordinates ( )
{
	if (true == _coordinatesValidityEvaluated)
	{
		if (false == _coordinatesValidity)
			throw Exception (_coordinatesValidityErrorMsg);

		return;
	}	// if (true == _coordinatesValidityEvaluated)

	const size_t	count	= getCellCount ( );
	for (size_t c = 0; c < count; c++)
	{
		try
		{

			QualifHelper::hasValideCoordinates (getCell (c), true);
		}
		catch (const Exception& exc)
		{
			UTF8String	msg (charset);
			msg << "Les coordonnées de la " << (unsigned long)c
			    << (0 == c ? "-ère" : "-ème") << " maille sont invalides : "
			    << exc.getFullMessage ( );
			setCoordinatesValidity (true, false, msg);
			throw Exception (msg);
		}	// if (false == QualifHelper::hasValideCoordinates (...
	}	// for (size_t c = 0; c < count; c++)

	setCoordinatesValidity (true, true, "");
}	// AbstractQualifSerie::validateCoordinates


void AbstractQualifSerie::setCoordinatesValidity (bool evaluated, bool valid, const UTF8String& msg)
{
	_coordinatesValidityEvaluated	= evaluated;
	_coordinatesValidity			= valid;
	_coordinatesValidityErrorMsg	= msg;
}	// AbstractQualifSerie::setCoordinatesValidity


void AbstractQualifSerie::setName (const string& name)
{
	_name	= name;
}	// AbstractQualifSerie::setName


bool AbstractQualifSerie::isCriteriaStored (Qualif::Critere criterion) const
{
	return _storedValues.find(criterion) != _storedValues.end();
}	// AbstractQualifSerie::isCriteriaStored


double AbstractQualifSerie::getStoredCriteria (Qualif::Critere criterion, size_t i) const
{
	if(_storedValuesDefined[criterion][i]) {
		return _storedValues[criterion][i];
	} else {
		UTF8String	msg (charset);
		msg << "Ce critère n'a pas été stocké pour cette maille.";
		throw Exception (msg);
	}
}	// AbstractQualifSerie::getStoredCriteria


const std::vector < std::vector <size_t> >&
						AbstractQualifSerie::getClassesCellsIndexes ( ) const
{
	return _classesCellsIndexes;
}	// AbstractQualifSerie::getClassesCellsIndexes


std::vector < std::vector <size_t> >&
						AbstractQualifSerie::getClassesCellsIndexes ( )
{
	return _classesCellsIndexes;
}	// AbstractQualifSerie::getClassesCellsIndexes


void AbstractQualifSerie::getCellsIndexes (
								vector<size_t>& indexes, size_t clas) const
{
	if (clas >= _classesCellsIndexes.size ( ))
		throw Exception (UTF8String ("AbstractQualifSerie::getCellsIndexes : index de classe invalide.", charset));

	indexes	= _classesCellsIndexes [clas];
}	// AbstractQualifSerie::getCellsIndexes


void AbstractQualifSerie::storeIndex (size_t cl, size_t c)
{
	_classesCellsIndexes [cl].push_back (c);
}	// AbstractQualifSerie::storeIndex


void AbstractQualifSerie::setClassesNum (size_t nb)
{
	_classesCellsIndexes.clear ( );
	_classesCellsIndexes.resize (nb);
}	// AbstractQualifSerie::setClassesNum


void AbstractQualifSerie::releaseStoredData ( )
{
	_storedValues.clear ( );
	_minValues.clear ( );
	_maxValues.clear ( );
	_storedValuesDefined.clear ( );
	_classesCellsIndexes.clear ( );
}	// AbstractQualifSerie::releaseStoredData


bool AbstractQualifSerie::isThreadable ( ) const
{
	return false;
}	// AbstractQualifSerie::isThreadable

}	// namespace GQualif

