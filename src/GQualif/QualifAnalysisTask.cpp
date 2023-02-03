#include "GQualif/QualifAnalysisTask.h"
#include "GQualif/QCalQualThread.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/ThreadManager.h>

#include <iostream>
#include <assert.h>

#include <cmath>


USING_UTIL
USING_STD
using namespace Qualif;

static const	Charset	charset ("àéèùô");


namespace GQualif
{

QualifAnalysisTask::QualifAnalysisTask (
			size_t types, Critere criterion, size_t classesNum,
			double min, double max, bool strict,
			const vector<AbstractQualifSerie*>& series)
	: AbstractQualifTask (types, criterion, series),
	  _classesNum (classesNum), _min (min), _max (max), _strictDomain (strict)
{
	if (0 == classesNum)
		throw Exception (UTF8String ("QualifAnalysisTask::QualifAnalysisTask  : nombre de classes nul.", charset));
}	// QualifAnalysisTask::QualifAnalysisTask


QualifAnalysisTask::QualifAnalysisTask (const QualifAnalysisTask&)
	: AbstractQualifTask (
			(size_t)-1, (Critere)-1, vector<AbstractQualifSerie*> ( )),
	  _classesNum ((size_t)-1),
	  _min (NumericServices::doubleMachMax ( )),
	  _max (-NumericServices::doubleMachMax ( )),
	  _strictDomain (false)
{
	assert (0 && "QualifAnalysisTask copy constructor is not allowed.");
}	// QualifAnalysisTask::QualifAnalysisTask


QualifAnalysisTask& QualifAnalysisTask::operator = (const QualifAnalysisTask&)
{
	assert (0 && "QualifAnalysisTask assignment operator is not allowed.");
	return *this;
}	// QualifAnalysisTask::operator =


QualifAnalysisTask::~QualifAnalysisTask ( )
{
}	// QualifAnalysisTask::~QualifAnalysisTask


size_t QualifAnalysisTask::getClassesNum ( ) const
{
	return _classesNum;
}	// QualifAnalysisTask::getClassesNum


double QualifAnalysisTask::getMin ( ) const
{
	return _min;
}	// QualifAnalysisTask::getMin


double QualifAnalysisTask::getMax ( ) const
{
	return _max;
}	// QualifAnalysisTask::getMax


bool QualifAnalysisTask::useStrictDomain ( ) const
{
	return _strictDomain;
}	// QualifAnalysisTask::useStrictDomain


void QualifAnalysisTask::execute ( )
{
	// Version 0.20.0 : exécution dans un ou plusieurs thread ?
	bool			multiThreaded	= true;
	size_t			i				= 0;
	const	size_t	seriesNum		= getSeriesNum ( );
	const	size_t	classNum		= getClassesNum ( );
	const	Critere	criterion		= getCriterion ( );
	const	size_t	cellTypes		= getDataTypes ( );
	const	double	min				= getMin ( );
	const	double	max				= getMax ( );
	const	bool	strict			= useStrictDomain ( );
	if (0 == classNum)
		throw Exception (UTF8String ("QualifAnalysisTask::execute : nombre de classes nul.", charset));
	initializeSeriesValues (classNum, seriesNum);
	vector<AbstractQualifSerie*>&	series	= getSeries ( );
	for (i = 0; i < seriesNum; i++)
	{
		AbstractQualifSerie*	serie	= series [i];
		CHECK_NULL_PTR_ERROR (serie)
		if (false == serie->isThreadable ( ))
		{
			multiThreaded	= false;
			break;
		}	// if (false == serie->isThreadable ( ))
	}	// for (i = 0; i < seriesNum; i++)

	vector<QualifSerieData*>	threadsData;
	if (true == multiThreaded)
	{
//cout << "LANCEMENT ANALYSE QUALIF DANS " << (unsigned long)seriesNum << " THREADS" << endl;
		for (i = 0; i < seriesNum; i++)
		{
			AbstractQualifSerie*	serie	= series [i];
			CHECK_NULL_PTR_ERROR (serie)
			QualifSerieData*	threadData	= new QualifSerieData (
				*serie, criterion, classNum, cellTypes, min, max, strict);
			threadsData.push_back (threadData);
			QCalQualThread*	thread	= new QCalQualThread (*threadData);
			ThreadManager::instance ( ).addTask (thread);
		}	// for (i = 0; i < seriesNum; i++)
		ThreadManager::instance ( ).join ( );
		i	= 0;
		for (vector<QualifSerieData*>::iterator ittd = threadsData.begin ( );
		     threadsData.end ( ) != ittd; ittd++, i++)
		{
			CHECK_NULL_PTR_ERROR (*ittd)
			const vector< vector <size_t> >& classment	= (*ittd)->classment( );
			for (unsigned char cl = 0; cl < classNum; cl++)
			{
				const vector<size_t>&	ids	= classment [cl];
				for (vector<size_t>::const_iterator iti = ids.begin ( );
				     ids.end ( ) != iti; iti++)
					increment (cl, i, *iti);
			}	// for (unsigned char cl = 0; cl < classNum; cl++)
			delete *ittd;
		}	// for (vector<QualifSerieData*>::iterator ittd = ...
		threadsData.clear ( );
	}	// if (true == multiThreaded)
	else
	{
//cout << "LANCEMENT ANALYSE QUALIF EN MODE MONOTHREAD" << endl;
		const double	cnRatio	= max / classNum - min / classNum;
		for (i = 0; i < seriesNum; i++)
		{
			AbstractQualifSerie*	serie	= series [i];
			CHECK_NULL_PTR_ERROR (serie)
			const size_t			count	= serie->getCellCount ( );
			for (size_t c = 0; c < count; c++)
			{
				try
				{

					if (0 == (serie->getCellType (c) & cellTypes))
						continue;

					double value;
					if(serie->isCriteriaStored (criterion))
					{
						value = serie->getStoredCriteria(criterion,c);
					}
					else
					{
						Maille&	cell	= serie->getCell (c);
						value	= cell.AppliqueCritere (criterion);
					}
					if ((true == strict) && ((value < min) || (value > max)))
						continue;
					size_t	cl	= (value - min) / cnRatio;
					if (cl >= classNum)
						cl	= classNum - 1;
					increment (cl, i, c);

				}
				catch (...)
				{	// Certains types ne sont pas supportés (pentagone, ...).
					// On ne s'arrête pas à ça.
				}

			}	// for (size_t c = 0; c < count; c++)
		}	// for (i = 0; i < seriesNum; i++)

	}	// else if (true == multiThreaded)

}	// QualifAnalysisTask::execute


void QualifAnalysisTask::increment (size_t cl, size_t s, size_t c)
{
	AbstractQualifTask::increment (cl, s, c);
	getSerie (s).storeIndex (cl, c);
}	// QualifAnalysisTask::increment


void QualifAnalysisTask::initializeSeriesValues (
										size_t classesNum, size_t seriesNum)
{
	AbstractQualifTask::initializeSeriesValues (classesNum, seriesNum);
	vector<AbstractQualifSerie*>&	series	= getSeries ( );
	for (vector<AbstractQualifSerie*>::iterator its = series.begin ( );
	     series.end ( ) != its; its++)
		(*its)->setClassesNum (classesNum);
}	// QualifAnalysisTask::initializeSeriesValues


}	// namespace GQualif

