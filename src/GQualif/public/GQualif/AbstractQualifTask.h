#ifndef ABSTRACT_QUALIF_TASK_H
#define ABSTRACT_QUALIF_TASK_H

#include <TkUtil/util_config.h>

#include "GQualif/AbstractQualifSerie.h"

#include <vector>


namespace GQualif
{

/**
 * <P>Classe abstraite représentant une tâche <I>Qualif</I> à effectuer sur
 * des séries de mailles.
 * </P>
 *
 * <P>Cette classe est à spécialiser selon le type de tâche à effectuer en
 * surchargeant, a minima, la méthode <I>execute</I>.
 * </P>
 *
 * <P>Les méthodes de cette classe sont susceptibles de lever des exceptions
 * de type <I>TkUtil::Exception</I>.
 * </P>
 */
class AbstractQualifTask
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~AbstractQualifTask ( );

	/**
	 * @return		Le nombre de séries traitées.
	 */
	virtual size_t getSeriesNum ( ) const;

	/**
	 * @return		Les types de mailles au sens <I>QualifHelper</I>, définis
	 * 				par un ou exclusif sur les types élémentaires
	 * 				(TRIANGLE, ...).
	 */
	virtual size_t getDataTypes ( ) const;

	/**
	 * @return		Le critère <I>Qualif</I> appliqué à la tâche.
	 */
	virtual Qualif::Critere getCriterion ( ) const;

	/**
	 * @return		<I>true</I> si la tâche est utilisable en contexte
	 * 				multithread, <I>false</I> dans le cas contraire.
	 */
	virtual bool isThreadable ( ) const;

	/**
	 * Exécute la tâche.
	 * <B>Par défaut lève une exception : cette méthode est à spécialiser.</B>
	 */
	virtual void execute ( );

	/**
	 * @return		La répartition en classes des différentes séries.
	 * Utilisation : getSeriesValues ( ) [cl][s], cl : classe, s : série.
	 */
	const std::vector< std::vector <size_t> >&	getSeriesValues ( ) const;
	std::vector< std::vector <size_t> >&	getSeriesValues ( );


	protected :

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
	AbstractQualifTask (
					size_t types, Qualif::Critere criterion,
					const std::vector<AbstractQualifSerie*>& series);

	/**
	 * @return		Les séries soumises à la tâche.
	 */
	const std::vector<AbstractQualifSerie*>& getSeries ( ) const;
	std::vector<AbstractQualifSerie*>& getSeries ( );

	/**
	 * @return	La i-ème série.
	 */
	inline AbstractQualifSerie& getSerie (size_t i)
	{ return *_series [i]; }

	/**
	 * <P><B>La méthode à surcharger</B> si l'on souhaite pouvoir associer à
	 * chaque classe de chaque série une information (identifiant ou pointeur
	 * sur une maille, ...).</P>
	 * <P>Cette méthode incrémentee la <I>c-ième</I> classe de la
	 * <I>s-ième</I> série.
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
	 * Initialisation du service de gestion du nombre de mailles par classe.
	 * @see			increment
	 */
	virtual void initializeSeriesValues (size_t classesNum, size_t seriesNum);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	AbstractQualifTask (const AbstractQualifTask&);
	AbstractQualifTask& operator = (const AbstractQualifTask&);

	/** Les types de mailles soumises à la tâche. */
	size_t								_types;

	/** Le critère de qualité appliqué. */
	Qualif::Critere						_criterion;

	/** Les séries soumises à l'analyse qualité. */
	std::vector<AbstractQualifSerie*>	_series;

	/** La répartition en classes des différentes séries.
	 * Utilisation : seriesValues [cl][s], cl : classe, s : série. */
	std::vector< std::vector <size_t> >	_seriesValues;
};	// class AbstractQualifTask

}	// namespace GQualif

#endif	// ABSTRACT_QUALIF_TASK_H
