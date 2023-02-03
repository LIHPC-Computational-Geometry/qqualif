#ifndef QUALIF_RANGE_TASK_H
#define QUALIF_RANGE_TASK_H

#include "GQualif/AbstractQualifTask.h"

#include <vector>


namespace GQualif
{

/**
 * <P>Classe de base représentant une tâche <I>Qualif</I> consistant en une
 * recherche des valeurs extrêmales rencontrées par les mailles pour un
 * critère de qualité donné.
 * </P>
 *
 * <P>Cette analyse est effectuée avec concurrence (multithreading) entre les
 * séries si les conditions le permettent.
 * </P>
 */
class QualifRangeTask: public AbstractQualifTask
{
	public :

	/**
	 * Constructeur.
	 * @param		Les types de mailles au sens <I>QualifHelper</I>, définis
	 * 				par un ou exclusif sur les types élémentaires
	 * 				(TRIANGLE, ...).
	 * @param		Le critère <I>Qualif</I> appliqué à la tâche.
	 * @param		Séries soumises à la tâche.
	 * @warning		Les séries transmises en argument doivent être utilisables
	 * 				pendant toute la durée de vie de cette instance.
	 */
	QualifRangeTask (
			size_t types, Qualif::Critere criterion, 
			const std::vector<AbstractQualifSerie*>& series);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QualifRangeTask ( );

	/**
	 * Exécute la tâche.
	 */
	virtual void execute ( );

	/**
	 * @return	Les extrema calculés par <I>execute</I>.
	 */
	virtual void getRange (double& min, double& max) const;
	virtual double getMin ( ) const;
	virtual double getMax ( ) const;


	protected :


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QualifRangeTask (const QualifRangeTask&);
	QualifRangeTask& operator = (const QualifRangeTask&);

	/* Les valeurs extrêmales rencontrées. */
	double			_min, _max;
};	// class QualifRangeTask

}	// namespace GQualif

#endif	// QUALIF_RANGE_TASK_H
