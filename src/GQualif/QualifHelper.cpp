#include "GQualif/QualifHelper.h"

#include <TkUtil/Exception.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>

#include <assert.h>
//#include <float.h>		// DBL_*
//#include <values.h>		// DBL_* sous Linux
#include <math.h>		// isinf (norme C 99)
#include <stdexcept>
#include <string>

/*
#ifndef isinf

#if defined sun || defined __sgi || defined __osf__
#include <nan.h>	// IsNANorINF, IsINF
static bool isinf (double value)
{
    if (true == IsNANorINF (value))
        return IsINF(value);
    else return false;
}
#endif	// sun || sgi || __osf__

#endif	// isinf
*/


namespace Qualif
{
	extern const double Not_A_Number, Infini;
}


using namespace Qualif;
using namespace TkUtil;
using namespace std;

static const Charset	charset ("àéèùô");


namespace GQualif
{

const size_t	QualifHelper::TRIANGLE			= 1;
const size_t	QualifHelper::QUADRANGLE		= 1 << 1;
const size_t	QualifHelper::TETRAEDRON		= 1 << 2;
const size_t	QualifHelper::PYRAMID			= 1 << 3;
const size_t	QualifHelper::HEXAEDRON			= 1 << 4;
const size_t	QualifHelper::TRIANGULAR_PRISM	= 1 << 5;


QualifHelper::QualifHelper ( )
{
	assert (0 && "QualifHelper::QualifHelper is not allowed.");
}	// QualifHelper::QualifHelper


QualifHelper::QualifHelper (const QualifHelper&)
{
	assert (0 && "QualifHelper::QualifHelper (const QualifHelper&) is not allowed.");
}	// QualifHelper::QualifHelper (const QualifHelper&)


QualifHelper& QualifHelper::operator = (const QualifHelper&)
{
	assert (0 && "QualifHelper::operator = is not allowed.");
	return *this;
}	// QualifHelper::operator =


QualifHelper::~QualifHelper ( )
{
}	// QualifHelper::~QualifHelper


Version QualifHelper::getVersion ( )
{
	static const Version	version (QQUALIF_VERSION);
	return version;
}	// QualifHelper::QualifHelper


void QualifHelper::getDomain (Critere criteria, size_t cellTypes,
                              double& min, double& max)
{
	bool	set	= false;
	min			= NumericServices::doubleMachMax ( );
	max			= -NumericServices::doubleMachMax ( );

	if (0 != (TRIANGLE & cellTypes))
	{
		try
		{
			double	tmin	= getTriangleMinValue (criteria);
			double	tmax	= getTriangleMaxValue (criteria);
			min	= min < tmin ? min : tmin;
			max	= max > tmax ? max : tmax;
			set	= true;
		}
		catch (...)
		{
		}
	}	// if (0 != (TRIANGLE & cellTypes))

	if (0 != (QUADRANGLE & cellTypes))
	{
		try
		{
			double	qmin	= getQuadrangleMinValue (criteria);
			double	qmax	= getQuadrangleMaxValue (criteria);
			min	= min < qmin ? min : qmin;
			max	= max > qmax ? max : qmax;
			set	= true;
		}
		catch (...)
		{
		}
	}	// if (0 != (QUADRANGLE & cellTypes))

	if (0 != (TETRAEDRON & cellTypes))
	{
		try
		{
			double	tmin	= getTetraedronMinValue (criteria);
			double	tmax	= getTetraedronMaxValue (criteria);
			min	= min < tmin ? min : tmin;
			max	= max > tmax ? max : tmax;
			set	= true;
		}
		catch (...)
		{
		}
	}	// if (0 != (TETRAEDRON & cellTypes))

	if (0 != (PYRAMID & cellTypes))
	{
		try
		{
			double	pmin	= getPyramidMinValue (criteria);
			double	pmax	= getPyramidMaxValue (criteria);
			min	= min < pmin ? min : pmin;
			max	= max > pmax ? max : pmax;
			set	= true;
		}
		catch (...)
		{
		}
	}	// if (0 != (PYRAMID & cellTypes))

	if (0 != (HEXAEDRON & cellTypes))
	{
		try
		{
			double	hmin	= getHexaedronMinValue (criteria);
			double	hmax	= getHexaedronMaxValue (criteria);
			min	= min < hmin ? min : hmin;
			max	= max > hmax ? max : hmax;
			set	= true;
		}
		catch (...)
		{
		}
	}	// if (0 != (HEXAEDRON & cellTypes))

	if (0 != (TRIANGULAR_PRISM & cellTypes))
	{
		try
		{
			double	tmin	= getTriangularPrismMinValue (criteria);
			double	tmax	= getTriangularPrismMaxValue (criteria);
			min	= min < tmin ? min : tmin;
			max	= max > tmax ? max : tmax;
			set	= true;
		}
		catch (...)
		{
		}
	}	// if (0 != (TRIANGULAR_PRISM & cellTypes))


	if (false == set)
		throw Exception (UTF8String ("Type de mailles non défini ou types inconnus.", charset));
}	// QualifHelper::getDomain

bool QualifHelper::isTypeSupported (size_t type)
{
	if (TRIANGLE == type)
		return true;
	if (QUADRANGLE == type)
		return true;
	if (TETRAEDRON == type)
		return true;
	if (PYRAMID == type)
		return true;
	if (HEXAEDRON == type)
		return true;
	if (TRIANGULAR_PRISM == type)
		return true;

	return false;
}	// QualifHelper::isTypeSupported (size_t type)


bool QualifHelper::isTypeSupported (size_t type, Critere criteria)
{
	if (false == isTypeSupported (type))
		return false;

	switch (criteria)
	{
		case VOLUMESURFACE		: return true;
		case PAOLETTI			: return true;
		case ODDY				: return true;
		case CONDITIONNEMENT	: return true;
		case SCALEDJACOBIAN		: return PYRAMID == type ? false : true;
		case KNUPP_SKEW			: return true;
		case KNUPP_SHAPE		: return true;
		case KNUPP_VOLUME		: return true;
		case KNUPP_VOLUMESHAPE	: return true;
		case ASPECTRATIO_GAMMA	:
			if ((TRIANGLE == type) || (TETRAEDRON == type))
				return true;
			return false;
		case ASPECTRATIO_BETA	: return TETRAEDRON == type ? true : false;
		case ASPECTRATIO_Q		:
			if ((TRIANGLE == type) || (TETRAEDRON == type))
				return true;
			return false;
		case ANGLEMIN			:
		case ANGLEMAX			:
			if ((TRIANGLE == type) || (QUADRANGLE == type))
				return true;
			return false;
		case JACOBIENMIN		: return true;
		case JACOBIENCENTRE		:
			if ((QUADRANGLE == type) || (HEXAEDRON == type))
				return true;
			return false;
		case ASPECTRATIOCENTER	:
			if ((QUADRANGLE == type) || (HEXAEDRON == type))
				return true;
			return false;
		case SKEW				:
			if ((QUADRANGLE == type) || (HEXAEDRON == type))
				return true;
			return false;
		case TAPER				:
			if ((QUADRANGLE == type) || (HEXAEDRON == type))
				return true;
			return false;
		case ETIREMENT			:
			if ((QUADRANGLE == type) || (HEXAEDRON == type))
				return true;
			return false;
		case WARP				: return QUADRANGLE == type ? true : false;
		case WARPBASE			: return PYRAMID == type ? true : false;
		case DIAGONALRATIO		: return HEXAEDRON == type ? true : false;
		case VALIDITY			: return HEXAEDRON == type ? true : false;
	}	// switch (criteria)

	return false;
}	// QualifHelper::isTypeSupported (size_t type, Critere criteria)


double QualifHelper::getTriangleMinValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return -NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 0.;
		case ODDY				: return 0.;
		case CONDITIONNEMENT	: return 1.;
		case SCALEDJACOBIAN		: return -1.;
		case KNUPP_SKEW			: return 0.;
		case KNUPP_SHAPE		: return 0.;
		case KNUPP_VOLUME		: return -1.;
		case KNUPP_VOLUMESHAPE	: return -1.;
		case ASPECTRATIO_GAMMA	: return -NumericServices::doubleMachMax ( );
		case ASPECTRATIO_Q		: return -1.;
		case ANGLEMIN			: return -180.;
		case ANGLEMAX			: return -60.;
		case JACOBIENMIN		: return -NumericServices::doubleMachMax ( );
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux triangles.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getTriangleMinValue


double QualifHelper::getTriangleMaxValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 1.;
		case ODDY				: return NumericServices::doubleMachMax ( );
		case CONDITIONNEMENT	: return NumericServices::doubleMachMax ( );
		case SCALEDJACOBIAN		: return 1.;
		case KNUPP_SKEW			: return 1.;
		case KNUPP_SHAPE		: return 1.;
		case KNUPP_VOLUME		: return 1.;
		case KNUPP_VOLUMESHAPE	: return 1.;
		case ASPECTRATIO_GAMMA	: return NumericServices::doubleMachMax ( );
		case ASPECTRATIO_Q		: return 1.;
		case ANGLEMIN			: return 60.;
		case ANGLEMAX			: return -180.;
		case JACOBIENMIN		: return NumericServices::doubleMachMax ( );
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux triangles.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getTriangleMaxValue


double QualifHelper::getQuadrangleMinValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return -NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 0.;
		case ODDY				: return 0.;
		case CONDITIONNEMENT	: return 1.;
		case SCALEDJACOBIAN		: return -1.;
		case KNUPP_SKEW			: return 0.;
		case KNUPP_SHAPE		: return 0.;
		case KNUPP_VOLUME		: return -1.;
		case KNUPP_VOLUMESHAPE	: return -1.;
		case ANGLEMIN			: return -180.;
		case ANGLEMAX			: return -90.;
		case JACOBIENMIN		: return -NumericServices::doubleMachMax ( );
		case JACOBIENCENTRE		: return -NumericServices::doubleMachMax ( );
		case ASPECTRATIOCENTER	: return 1.;
		case SKEW				: return 0.;
		case TAPER				: return 0.;
		case ETIREMENT			: return 0.;
		case WARP				: return 0.;
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux quadrangles.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getQuadrangleMinValue


double QualifHelper::getQuadrangleMaxValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 1.;
		case ODDY				: return NumericServices::doubleMachMax ( );
		case CONDITIONNEMENT	: return NumericServices::doubleMachMax ( );
		case SCALEDJACOBIAN		: return 1.;
		case KNUPP_SKEW			: return 1.;
		case KNUPP_SHAPE		: return 1.;
		case KNUPP_VOLUME		: return 1.;
		case KNUPP_VOLUMESHAPE	: return 1.;
		case ANGLEMIN			: return 90.;
		case ANGLEMAX			: return 180.;
		case JACOBIENMIN		: return NumericServices::doubleMachMax ( );
		case JACOBIENCENTRE		: return NumericServices::doubleMachMax ( );
		case ASPECTRATIOCENTER	: return NumericServices::doubleMachMax ( );
		case SKEW				: return 1.;
		case TAPER				: return NumericServices::doubleMachMax ( );
		case ETIREMENT			: return 1.;
		case WARP				: return 90.;
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux quadrangles.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getQuadrangleMaxValue


double QualifHelper::getHexaedronMinValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return -NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 0.;
		case ODDY				: return 0.;
		case CONDITIONNEMENT	: return 1.;
		case SCALEDJACOBIAN		: return -1.;
		case KNUPP_SKEW			: return 0.;
		case KNUPP_SHAPE		: return 0.;
		case KNUPP_VOLUME		: return -1.;
		case KNUPP_VOLUMESHAPE	: return -1.;
		case JACOBIENMIN		: return -NumericServices::doubleMachMax ( );
		case JACOBIENCENTRE 	: return -NumericServices::doubleMachMax ( );
		case ASPECTRATIOCENTER	: return 1.;
		case SKEW				: return 0.;
		case TAPER				: return 0.;
		case ETIREMENT			: return 0.;
		case DIAGONALRATIO		: return 0.;
		case VALIDITY			: return 0.;
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux hexaèdres.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getHexaedronMinValue


double QualifHelper::getHexaedronMaxValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 1.;
		case ODDY				: return NumericServices::doubleMachMax ( );
		case CONDITIONNEMENT	: return NumericServices::doubleMachMax ( );
		case SCALEDJACOBIAN		: return 1.;
		case KNUPP_SKEW			: return 1.;
		case KNUPP_SHAPE		: return 1.;
		case KNUPP_VOLUME		: return 1.;
		case KNUPP_VOLUMESHAPE	: return 1.;
		case JACOBIENMIN		: return NumericServices::doubleMachMax ( );
		case JACOBIENCENTRE 	: return NumericServices::doubleMachMax ( );
		case ASPECTRATIOCENTER	: return NumericServices::doubleMachMax ( );
		case SKEW				: return 1.;
		case TAPER				: return NumericServices::doubleMachMax ( );
		case ETIREMENT			: return 1.;
		case DIAGONALRATIO		: return 1.;
		case VALIDITY			: return 1.;
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux hexaèdres.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getHexaedronMaxValue


double QualifHelper::getTetraedronMinValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return -NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 0.;
		case ODDY				: return 0.;
		case CONDITIONNEMENT	: return 1.;
		case SCALEDJACOBIAN		: return -1.;
		case KNUPP_SKEW			: return 0.;
		case KNUPP_SHAPE		: return 0.;
		case KNUPP_VOLUME		: return -1.;
		case KNUPP_VOLUMESHAPE	: return -1.;
		case ASPECTRATIO_GAMMA	: return -NumericServices::doubleMachMax ( );
		case ASPECTRATIO_BETA	: return 1.;
		case ASPECTRATIO_Q		: return -1.;
		case JACOBIENMIN		: return -NumericServices::doubleMachMax ( );
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux tétraèdres.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getTetraedronMinValue


double QualifHelper::getTetraedronMaxValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 1.;
		case ODDY				: return 0.;
		case CONDITIONNEMENT	: return NumericServices::doubleMachMax ( );
		case SCALEDJACOBIAN		: return 1.;
		case KNUPP_SKEW			: return 1.;
		case KNUPP_SHAPE		: return 1.;
		case KNUPP_VOLUME		: return 1.;
		case KNUPP_VOLUMESHAPE	: return 1.;
		case ASPECTRATIO_GAMMA	: return NumericServices::doubleMachMax ( );
		case ASPECTRATIO_BETA	: return NumericServices::doubleMachMax ( );
		case ASPECTRATIO_Q		: return 1.;
		case JACOBIENMIN		: return NumericServices::doubleMachMax ( );
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux tétraèdres.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getTetraedronMaxValue


double QualifHelper::getPyramidMinValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return -NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 0.;
		case ODDY				: return 0.;
		case CONDITIONNEMENT	: return 1.;
		case KNUPP_SKEW			: return 0.;
		case KNUPP_SHAPE		: return 0.;
		case KNUPP_VOLUME		: return -1.;
		case KNUPP_VOLUMESHAPE	: return -1.;
		case JACOBIENMIN		: return -NumericServices::doubleMachMax ( );
		case WARPBASE			: return 0.;
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux pyramides.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getPyramidMinValue


double QualifHelper::getPyramidMaxValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 1.;
		case ODDY				: return NumericServices::doubleMachMax ( );
		case CONDITIONNEMENT	: return NumericServices::doubleMachMax ( );
		case KNUPP_SKEW			: return 1.;
		case KNUPP_SHAPE		: return 1.;
		case KNUPP_VOLUME		: return 1.;
		case KNUPP_VOLUMESHAPE	: return 1.;
		case JACOBIENMIN		: return NumericServices::doubleMachMax ( );
		case WARPBASE			: return 90.;
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux pyramides.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getPyramidMaxValue


double QualifHelper::getTriangularPrismMinValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return -NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 0.;
		case ODDY				: return 0.;
		case CONDITIONNEMENT	: return 1.;
		case SCALEDJACOBIAN		: return -1.;
		case KNUPP_SKEW			: return 0.;
		case KNUPP_SHAPE		: return 0.;
		case KNUPP_VOLUME		: return -1.;
		case KNUPP_VOLUMESHAPE	: return -1.;
		case JACOBIENMIN		: return -NumericServices::doubleMachMax ( );
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux prismes à base triangulaire.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getTriangularPrismMinValue


double QualifHelper::getTriangularPrismMaxValue (Critere criteria)
{
	switch (criteria)
	{
		case VOLUMESURFACE		: return NumericServices::doubleMachMax ( );
		case PAOLETTI			: return 1.;
		case ODDY				: return NumericServices::doubleMachMax ( );
		case CONDITIONNEMENT	: return NumericServices::doubleMachMax ( );
		case SCALEDJACOBIAN		: return 1.;
		case KNUPP_SKEW			: return 1.;
		case KNUPP_SHAPE		: return 1.;
		case KNUPP_VOLUME		: return 1.;
		case KNUPP_VOLUMESHAPE	: return 1.;
		case JACOBIENMIN		: return NumericServices::doubleMachMax ( );
	}	// switch (criteria)

	UTF8String	errorMsg ("Critère ", charset);
	errorMsg << CRITERESTR [criteria] << " non applicable aux prismes à base triangulaire.";
	throw Exception (UTF8String (errorMsg));
}	// QualifHelper::getTriangularPrismMaxValue


/*	v 0.4.0
bool QualifHelper::isInfinite (double value)
{
	// L'auteur a tendance à initialiser des domaines numérique à déterminer
	// par des domaines [DBL_MAX, -DBL_MAX]. Or -DBL_MAX n'est pas l'infini ...
//	return Infini == value ? true : false;
	if ((-DBL_MAX == value) || (DBL_MAX == value))
		return true;

	return isinf (value);
}	// QualifHelper::isInfinite
*/


UTF8String QualifHelper::dataTypeToName (size_t type)
{
	switch (type)
	{
		case QualifHelper::TRIANGLE			: return UTF8String ("Triangle", charset);
		case QualifHelper::QUADRANGLE		: return UTF8String ("Quadrangle", charset);
		case QualifHelper::TETRAEDRON		: return UTF8String ("Tetraèdre", charset);
		case QualifHelper::PYRAMID			: return UTF8String ("Pyramide", charset);
		case QualifHelper::HEXAEDRON		: return UTF8String ("Hexaèdre", charset);
		case QualifHelper::TRIANGULAR_PRISM	: return UTF8String ("Prisme à base triangulaire", charset);
	}	// switch (type)

	UTF8String	error (charset);
	error << "Type de maille indéfini (" << type << ").";
	return error;
}	// QualifHelper::dataTypeToName


bool QualifHelper::hasValideCoordinates (const Maille& cell, bool exc)
{
	const int	nbNodes	= cell.NombreSommets ( );
	const int	dim		= cell.Dimension ( );

	for (int n = 0; n < nbNodes; n++)
	{
		Vecteur	nodes (cell.Sommet (n));
		for (int d =  0; d < dim; d++)
			if (false == NumericServices::isValid (nodes.GetCoor (d)))
			{
				if (false == exc)
					return false;

				UTF8String	msg (charset);
				msg << "Les coordonnées du " << (unsigned long)n
				    << (0 == n ? "-er" : "-ème") <<" noeud (";
				for (int e = 0; e < dim; e++)
				{
					if (0 != e)
						msg << ", ";
					msg << nodes.GetCoor (e);
				}	// for (int e = 0; e < dim; e++)
				msg << ") sont invalides.";
				throw Exception (msg);
			}	// if (false == NumericServices:::isValid (nodes.GetCoor (d)))
	}	// for (int n = 0; n < nbNodes; n++)

	return true;
}	// QualifHelper::hasValideCoordinates



}	// namespace GQualif

