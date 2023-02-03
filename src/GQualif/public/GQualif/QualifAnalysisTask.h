#ifndef QUALIF_ANALYSIS_TASK_H
#define QUALIF_ANALYSIS_TASK_H

#include "GQualif/AbstractQualifTask.h"

#include <vector>


namespace GQualif
{

/**
 * <P>Classe de base représentant une tâche <I>Qualif</I> consistant en une
 * analyse qualitative de séries de mailles.
 * </P>
 *
 * <P>Cette analyse est effectuée avec concurrence (multithreading) entre les
 * séries si les conditions le permettent.
 * </P>
 */
class QualifAnalysisTask: public AbstractQualifTask
{
	public :

	/**
	 * Constructeur.
	 * @param		Les types de mailles au sens <I>QualifHelper</I>, définis
	 * 				par un ou exclusif sur les types élémentaires
	 * 				(TRIANGLE, ...).
	 * @param		Le critère <I>Qualif</I> appliqué à la tâche.
	 * @param		Le nombre de classes pour la répartition des mailles selon
	 *				le domaine d'échantillonnage (!= 0).
	 * @param		La valeur minimale du domaine d'échantillonnage.
	 * @param		La valeur maximale du domaine d'échantillonnage.
	 * @param		<I>true</I> s'il ne faut pas prendre en compte les mailles
	 * 				dont le critère est hors-domaine, <I>false</I> dans le cas
	 * 				contraire.
	 * @param		Séries soumises à la tâche.
	 * @warning		Les séries transmises en argument doivent être utilisables
	 * 				pendant toute la durée de vie de cette instance.
	 */
	QualifAnalysisTask (
			size_t types, Qualif::Critere criterion, size_t classNum,
			double min, double max, bool strict, 
			const std::vector<AbstractQualifSerie*>& series);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QualifAnalysisTask ( );

	/**
	 * @return		Le nombre de classes pour la répartition des mailles
	 * 				dans le domaine d'échantillonnage .
	 */
	virtual size_t getClassesNum ( ) const;

	/**
	 * @return		La valeur minimale du domaine d'échantillonnage.
	 */
	virtual double getMin ( ) const;

	/**
	 * @return		La valeur maximale du domaine d'échantillonnage.
	 */
	virtual double getMax ( ) const;
	/**
	 * @param		<I>true</I> s'il ne faut pas prendre en compte les mailles
	 * 				dont le critère est hors-domaine, <I>false</I> dans le cas
	 * 				contraire.
	 */
	virtual bool useStrictDomain ( ) const;

	/**
	 * Exécute la tâche.
	 */
	virtual void execute ( );


	protected :

	/**
	 * <P>Cette méthode incrémentee la <I>c-ième</I> classe de la
	 * <I>s-ième</I> série et stocke l'index de la maille dans sa classe.
	 * </P>
	 * @param		Indice de la classe incrémentée
	 * 				(<I>0 <= c < getClassesNum ( )</I>).
	 * @param		Indice de la série incrémentée
	 * 				(<I>0 <= s < getSeriesNum ( )</I>).
	 * @param		Indice de la maille dans <I>s</I>
	 * 				(<I>0 <= i < serie.getCellCount ( )</I>).
	 * @warning		<I>initializeSeriesValues</I> doit avoir été invoqué pour
	 * 				pouvoir utiliser ce service.
	 * @see			initializeSeriesValues
	 */
	virtual void increment (size_t cl, size_t s, size_t c);

	/**
	 * Initialisation des services de gestion du nombre de mailles par classe.
	 * @see		increment
	 */
	virtual void initializeSeriesValues (size_t classesNum, size_t seriesNum);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QualifAnalysisTask (const QualifAnalysisTask&);
	QualifAnalysisTask& operator = (const QualifAnalysisTask&);

	/** Le nombre de classes. */
	size_t		_classesNum;

	/** Valeurs extrémales du domaines du critère de qualité pris en compte. */
	double		_min, _max;

	/** <I>true</I> s'il ne faut pas prendre en compte les mailles dont le
	 * critère est hors-domaine, <I>false</I> dans le cas contraire. */
	bool		_strictDomain;
};	// class QualifAnalysisTask

}	// namespace GQualif

#endif	// QUALIF_ANALYSIS_TASK_H
