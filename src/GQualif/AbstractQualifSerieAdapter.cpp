#include "GQualif/AbstractQualifSerieAdapter.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <iostream>
#include <assert.h>


USING_UTIL
USING_STD
using namespace Qualif;


namespace GQualif
{


// =============================================================================
//                     LA CLASSE AbstractQualifSerieAdapter
// =============================================================================

AbstractQualifSerieAdapter::AbstractQualifSerieAdapter (
			const string& fileName, const string& name, unsigned char dimension)
	: AbstractQualifSerie (fileName, name, dimension),
	  _noeuds ( ),
	  _triangle (2, _noeuds), _quadrangle (2, _noeuds), _tetraedron (_noeuds),
	  _pyramid (_noeuds), _prism (_noeuds), _hexaedron (_noeuds)
{
}	// AbstractQualifSerieAdapter::AbstractQualifSerieAdapter


AbstractQualifSerieAdapter::AbstractQualifSerieAdapter (
											const AbstractQualifSerieAdapter&)
	: AbstractQualifSerie ("Invalid file", "Invalid name", (unsigned char)-1),
	  _noeuds ( ),
	  _triangle (2, _noeuds), _quadrangle (2, _noeuds), _tetraedron (_noeuds),
	  _pyramid (_noeuds), _prism (_noeuds), _hexaedron (_noeuds)
{
	assert (0 && "AbstractQualifSerieAdapter copy constructor is not allowed.");
}	// AbstractQualifSerieAdapter::AbstractQualifSerieAdapter


AbstractQualifSerieAdapter& AbstractQualifSerieAdapter::operator = (
											const AbstractQualifSerieAdapter&)
{
	assert (0 && "AbstractQualifSerieAdapter assignment operator is not allowed.");
	return *this;
}	// AbstractQualifSerieAdapter::operator =


AbstractQualifSerieAdapter::~AbstractQualifSerieAdapter ( )
{
}	// AbstractQualifSerieAdapter::~AbstractQualifSerieAdapter


// =============================================================================
//                              FONCTIONS STATIQUES
// =============================================================================


}	// namespace GQualif

