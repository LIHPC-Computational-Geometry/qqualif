#include "GQualif/QCalQualThread.h"
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>

#include <Maille.h>	// Qualif

#include <iostream>
#include <assert.h>

using namespace std;
using namespace Qualif;
using namespace TkUtil;


static const Charset	charset ("àéèùô");


namespace GQualif
{

// ============================================================================
//                         LA CLASSE QCalQualThread
// ============================================================================

QCalQualThread::QCalQualThread (QualifSerieData& data)
	: ReportedJoinableThread < QualifSerieData > (&data)
{
}	// QCalQualThread::QCalQualThread


QCalQualThread::QCalQualThread (const QCalQualThread&)
	: ReportedJoinableThread < QualifSerieData > (0)
{
	assert (0 && "QCalQualThread copy constructor is not allowed.");
}	// QCalQualThread::QCalQualThread


QCalQualThread& QCalQualThread::operator = (const QCalQualThread&)
{
	assert (0 && "QCalQualThread assignment operator is not allowed.");
	return *this;
}	// QCalQualThread::QCalQualThread


QCalQualThread::~QCalQualThread ( )
{
}	// QCalQualThread::~QCalQualThread


void QCalQualThread::execute ( )
{
	QualifSerieData*	data	= dynamic_cast<QualifSerieData*>(getData ( ));
	try
	{
		CHECK_NULL_PTR_ERROR (data)
		const size_t				classNum	= data->classNum ( );
		if (0 == classNum)
			throw Exception (UTF8String ("Nombre de classes nul.", charset));
		const AbstractQualifSerie&	serie		= data->serie ( );
		const size_t				count		= serie.getCellCount ( );
		const Qualif::Critere		criterion	= data->criterion ( );
		const size_t				cellTypes	= data->cellTypes ( );
		const double				min			= data->min ( );
		const double				max			= data->max ( );
		const bool					strictMode	= data->strictMode ( );
		const double				cnRatio		= max/classNum - min/classNum;
		for (size_t c = 0; c < count; c++)
		{
			try
			{
				if (0 == (serie.getCellType (c) & cellTypes))
					continue;

				double value;
				if(serie.isCriteriaStored (criterion))
					value = serie.getStoredCriteria (criterion,c);
				else
				{
					Maille&	cell	= serie.getCell (c);
					value	= cell.AppliqueCritere (criterion);
				}
				if ((true == strictMode) && ((value < min) || (value > max)))
					continue;
				size_t	cl	= (value - min) / cnRatio;
				if (cl >= classNum)
					cl	= classNum - 1;
				data->increment (cl, c);
			}
			catch (...)
			{	// Certains types ne sont pas supportés (pentagone, ...).
					// On ne s'arrête pas à ça.
			}
		}	// for (size_t c = 0; c < count; c++)
		data->setCompletionStatus (true);
	}
	catch (const Exception& e)
	{
		UTF8String	message (charset);
		message << "Erreur lors de l'analyse de qualité : "
		        << "\n" << e.getFullMessage ( );
		if (0 != data)
			data->setCompletionStatus (false, message);
	}
	catch (...)
	{
		if (0 != data)
			data->setCompletionStatus (false, "Erreur non documentée dans l'analyse de qualité.");
	}
}	// QCalQualThread::execute


// ============================================================================
//                         LA CLASSE QualifSerieData
// ============================================================================

QualifSerieData::QualifSerieData (
			const AbstractQualifSerie& serie, Qualif::Critere criterion,
			size_t classNum, size_t cellTypes, double min, double max,
			bool strictMode)
	: _serie (serie), _criterion (criterion),
	  _classNum (classNum), _cellTypes (cellTypes), _min (min), _max (max),
	  _strictMode (strictMode), _classment ( )
{
	if (0 == classNum)
		throw Exception (UTF8String ("Constructeur de QualifSerieData : nombre de classes nul.", charset));
	_classment.resize (classNum);
}	// QualifSerieData::QualifSerieData


QualifSerieData::QualifSerieData (const QualifSerieData& qsd)
	: _serie (qsd._serie), _criterion (qsd._criterion),
	  _classNum (qsd._classNum), _cellTypes (qsd._cellTypes),
	  _min (qsd._min), _max (qsd._max), _strictMode (qsd._strictMode),
	  _classment (qsd._classment)
{
}	// QualifSerieData::QualifSerieData


QualifSerieData& QualifSerieData::operator = (const QualifSerieData&)
{
	assert (0 && "QualifSerieData assignment operator is not allowed.");
	return *this;
}	// QualifSerieData::operator =


QualifSerieData::~QualifSerieData ( )
{
}	// QualifSerieData::~QualifSerieData


void QualifSerieData::increment (unsigned char cl, size_t cell)
{
	_classment [cl]	.push_back (cell);
}	// QualifSerieData::increment


void QualifSerieData::setRange (double min, double max)
{
	_min	= min;
	_max	= max;
}	// QualifSerieData::setRange


size_t QualifSerieData::cellNum (unsigned char cl) const
{
	return _classment [cl].size ( );
}	// QualifSerieData::cellNum


// ============================================================================
//                         LA CLASSE QSerieRangeThread
// ============================================================================

QSerieRangeThread::QSerieRangeThread (QualifSerieData& data)
	: ReportedJoinableThread < QualifSerieData > (&data)
{
}	// QSerieRangeThread::QSerieRangeThread


QSerieRangeThread::QSerieRangeThread (const QSerieRangeThread&)
	: ReportedJoinableThread < QualifSerieData > (0)
{
	assert (0 && "QSerieRangeThread copy constructor is not allowed.");
}	// QSerieRangeThread::QSerieRangeThread


QSerieRangeThread& QSerieRangeThread::operator = (const QSerieRangeThread&)
{
	assert (0 && "QSerieRangeThread assignment operator is not allowed.");
	return *this;
}	// QSerieRangeThread::QSerieRangeThread


QSerieRangeThread::~QSerieRangeThread ( )
{
}	// QSerieRangeThread::~QSerieRangeThread


void QSerieRangeThread::execute ( )
{
	QualifSerieData*	data	= dynamic_cast<QualifSerieData*>(getData ( ));
	try
	{
		CHECK_NULL_PTR_ERROR (data)
		double	min	= NumericServices::doubleMachMax ( );
		double	max	= -NumericServices::doubleMachMax ( );
		const AbstractQualifSerie&	serie	= data->serie ( );
		serie.getDataRange (data->criterion ( ), min, max);

		// Le domaine obtenu ne prend pas en compte les types de mailles :
		min	= NumericServices::doubleMachMax ( );
		max	= -NumericServices::doubleMachMax ( );
		const size_t			count		= serie.getCellCount ( );
		const size_t			cellTypes	= data->cellTypes ( );
		const Qualif::Critere	criterion	= data->criterion ( );
		for (size_t c = 0; c < count; c++)
		{
			if (0 == (serie.getCellType (c) & cellTypes))
				continue;

			try
			{
				if (0 == (serie.getCellType (c) & cellTypes))
					continue;

				double value	= serie.getStoredCriteria (criterion, c);
				min	= value < min ? value : min;
				max	= value > max ? value : max;
			}
			catch (...)
			{
			}
		}	// for (size_t c = 0; c < count; c++)

		data->setRange (min, max);
		data->setCompletionStatus (true);
	}
	catch (const Exception& e)
	{
		UTF8String	message (charset);
		message << "Erreur lors de l'analyse de qualité : "
		        << "\n" << e.getFullMessage ( );
		if (0 != data)
			data->setCompletionStatus (false, message);
	}
	catch (...)
	{
		if (0 != data)
			data->setCompletionStatus (false, "Erreur non documentée dans l'analyse de qualité.");
	}
}	// QSerieRangeThread::execute


}	// namespace GQualif

