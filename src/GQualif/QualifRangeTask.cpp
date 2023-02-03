#include "GQualif/QualifRangeTask.h"
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

namespace GQualif
{

QualifRangeTask::QualifRangeTask (
	size_t types, Critere criterion, const vector<AbstractQualifSerie*>& series)
	: AbstractQualifTask (types, criterion, series),
	  _min (-NumericServices::doubleMachMax ( )),
	  _max (NumericServices::doubleMachMax ( ))
{
}	// QualifRangeTask::QualifRangeTask


QualifRangeTask::QualifRangeTask (const QualifRangeTask&)
	: AbstractQualifTask (
			(size_t)-1, (Critere)-1, vector<AbstractQualifSerie*> ( )),
	  _min (-NumericServices::doubleMachMax ( )),
	  _max (NumericServices::doubleMachMax ( ))
{
	assert (0 && "QualifRangeTask copy constructor is not allowed.");
}	// QualifRangeTask::QualifRangeTask


QualifRangeTask& QualifRangeTask::operator = (const QualifRangeTask&)
{
	assert (0 && "QualifRangeTask assignment operator is not allowed.");
	return *this;
}	// QualifRangeTask::operator =


QualifRangeTask::~QualifRangeTask ( )
{
}	// QualifRangeTask::~QualifRangeTask


void QualifRangeTask::execute ( )
{
	// Version 0.20.0 : exécution dans un ou plusieurs thread ?
	bool			multiThreaded	= true;
	size_t			i				= 0;
	const	size_t	seriesNum		= getSeriesNum ( );
	const	Critere	criterion		= getCriterion ( );
	const	size_t	cellTypes		= getDataTypes ( );
	bool	minOK	= false,	maxOK	= false;
	_min	= NumericServices::doubleMachMax ( );
	_max	= -NumericServices::doubleMachMax ( );
	vector<AbstractQualifSerie*>&	series	= getSeries ( );
	for (i = 0; i < seriesNum; i++)
	{
		AbstractQualifSerie*	serie	= series [i];
		CHECK_NULL_PTR_ERROR (serie)
		if (false == serie->isThreadable ( ))
		{
//cout << "SERIE NON MT DETECTEE." << endl;
			multiThreaded	= false;
			break;
		}	// if (false == serie->isThreadable ( ))
	}	// for (i = 0; i < seriesNum; i++)

	vector<QualifSerieData*>	threadsData;
	if (true == multiThreaded)
	{
//cout << "LANCEMENT CALCUL DOMAINE QUALIF DANS " << (unsigned long)seriesNum << " THREADS" << endl;
		for (i = 0; i < seriesNum; i++)
		{
			AbstractQualifSerie*	serie	= series [i];
			CHECK_NULL_PTR_ERROR (serie)
			QualifSerieData*	threadData	= new QualifSerieData (
				*serie, criterion, 1, cellTypes, _min, _max, false);
			threadsData.push_back (threadData);
			QSerieRangeThread*	thread	= new QSerieRangeThread (*threadData);
			ThreadManager::instance ( ).addTask (thread);
		}	// for (i = 0; i < seriesNum; i++)
		ThreadManager::instance ( ).join ( );
		i	= 0;
		for (vector<QualifSerieData*>::iterator ittd = threadsData.begin ( );
		     threadsData.end ( ) != ittd; ittd++, i++)
		{
			CHECK_NULL_PTR_ERROR (*ittd)
			if (true == NumericServices::isValidAndNotMax ((*ittd)->min ( )))
			{
				minOK	= true;
				_min	= (*ittd)->min ( ) < _min ? (*ittd)->min ( ) : _min;
			}	// if (true == NumericServices::isValidAndNotMax (...
			if (true == NumericServices::isValidAndNotMax ((*ittd)->max ( )))
			{
				maxOK	= true;
				_max	= (*ittd)->max ( ) > _max ? (*ittd)->max ( ) : _max;
			}	// if (true == NumericServices::isValidAndNotMax (...
			delete *ittd;
		}	// for (vector<QualifSerieData*>::iterator ittd = ...
		threadsData.clear ( );
	}	// if (true == multiThreaded)
	else
	{
//cout << "LANCEMENT CALCUL DOMAINE QUALIF EN MODE MONOTHREAD" << endl;

		for (i = 0; i < seriesNum; i++)
		{
			AbstractQualifSerie*	serie	= series [i];
			CHECK_NULL_PTR_ERROR (serie)
			double	mn	= NumericServices::doubleMachMax ( );
			double	mx	= -NumericServices::doubleMachMax ( );
            serie->getDataRange (criterion, mn, mx);
			if (true == NumericServices::isValidAndNotMax (mn))
			{
				minOK	= true;
				_min	= mn < _min ? mn : _min;
			}	// if (true == NumericServices::isValidAndNotMax (...
			if (true == NumericServices::isValidAndNotMax (mx))
			{
				maxOK	= true;
				_max	= mx > _max ? mx : _max;
			}	// if (true == NumericServices::isValidAndNotMax (...
		}	// for (i = 0; i < seriesNum; i++)

	}	// else if (true == multiThreaded)

	// Si une valeur est infinie il faut trouver quelque chose car Qwt risque
	// de ne pas apprécier et de provoquer un bad_alloc dans les calculs de
	// graduations
	if (true == minOK)
	{
		if (false == maxOK)
			_max	= _min + fabs (_min);
	}
	else
	{
		if (true == maxOK)
			_min	= _max - fabs (_max);
		else
		{
			_min	= -1000.;
			_max	= 1000.;
		}
	}
}	// QualifRangeTask::execute


void QualifRangeTask::getRange (double& min, double& max) const
{
	min	= _min;
	max	= _max;
}	// QualifRangeTask::getRange


double QualifRangeTask::getMin ( ) const
{
	return _min;
}	// QualifRangeTask::getMin


double QualifRangeTask::getMax ( ) const
{
	return _max;
}	// QualifRangeTask::getMax


}	// namespace GQualif

