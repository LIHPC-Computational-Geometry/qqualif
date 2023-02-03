#ifndef Q_CAL_QUAL_THREAD_H
#define Q_CAL_QUAL_THREAD_H

#include "GQualif/AbstractQualifSerie.h"

#include <TkUtil/Threads.h>

#include <vector>

namespace GQualif
{

class QualifSerieData;

/**
 * Classe de thread analysant une série de mailles pour un critère de qualité
 * donné.
 */
class QCalQualThread : public TkUtil::ReportedJoinableThread < QualifSerieData >
{
	public :

	/**
	 * Constructeur. RAS. Ne modifie pas les données prises en charge.
	 * @param	Données prises en charge pour l'analyse.
	 */
	QCalQualThread (QualifSerieData& data);

	/**
	 * Destructeur. Détruit l'instance de QualifSerieData associée.
	 */
	virtual ~QCalQualThread ( );


	protected :

	/**
	 * Effectue l'analyse de qualité.
	 */
	virtual void execute ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QCalQualThread (const QCalQualThread&);
	QCalQualThread& operator = (const QCalQualThread&);
};	// class QCalQualThread


/**
 * Classe véhiculant les informations utiles à une instance de QCalQualThread
 * pour effectuer une analyse.
 */
class QualifSerieData : public TkUtil::ThreadData
{
	public :

	/**
	 * Constructeurs. RAS. Ne modifient pas les données prises en charge.
	 * @param	Série analysée
	 * @param	Critère de qualité appliqué
	 * @param	Nombre de classes formées
	 * @param	Types de mailles prises en compte
	 * @param	Domaine (min/max) d'observation du critère
	 * @param	Utilisation stricte (<I>true</I> ou non (<I>false</I>) du
	 * 			domaine d'observation du critère.
	 */
	QualifSerieData (const AbstractQualifSerie&, Qualif::Critere criterion,
		size_t classNum, size_t cellTypes, double min, double max, bool strict);
	QualifSerieData (const QualifSerieData&);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QualifSerieData ( );

	/**
	 * Modification du résultat de l'analyse.
	 */
	virtual void increment (unsigned char cl, size_t c);

	/**
	 * Modification du domaine.
	 */
	virtual void setRange (double min, double max);

	/**
	 * Accesseurs.
	 */
	inline const AbstractQualifSerie& serie ( ) const
	{ return _serie; }
	inline Qualif::Critere criterion ( ) const
	{ return _criterion; }
	inline size_t classNum ( ) const
	{ return _classNum; }
	inline size_t cellTypes ( ) const
	{ return _cellTypes; }
	inline double min ( ) const
	{ return _min; }
	inline double max ( ) const
	{ return _max; }
	inline bool strictMode ( ) const
	{ return _strictMode; }
	/** Les ids des mailles par classe. Cette méthode retourne un vecteur
	 * de classNum ( ) vecteurs d'ids. */
	inline const std::vector< std::vector<size_t> >& classment ( ) const
	{ return _classment; }
	size_t cellNum (unsigned char cl) const;


	private :

	/**
	 * Opérateur = : interdit (données membres constantes).
	 */
	QualifSerieData& operator = (const QualifSerieData&);

	const AbstractQualifSerie&			_serie;
	const Qualif::Critere				_criterion;
	const size_t						_classNum, _cellTypes;
	double								_min, _max;
	const bool							_strictMode;
	std::vector< std::vector<size_t> >	_classment;
};	// class QualifSerieData


/**
 * Classe de thread recherchant le domaine d'un critère de qualité donné pour
 * série.
 */
class QSerieRangeThread : public TkUtil::ReportedJoinableThread < QualifSerieData >
{
	public :

	/**
	 * Constructeur. RAS. Ne modifie pas les données prises en charge.
	 * @param	Données prises en charge pour l'analyse.
	 */
	QSerieRangeThread (QualifSerieData& data);

	/**
	 * Destructeur. Détruit l'instance de QualifSerieData associée.
	 */
	virtual ~QSerieRangeThread ( );


	protected :

	/**
	 * Effectue l'analyse de qualité.
	 */
	virtual void execute ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QSerieRangeThread (const QSerieRangeThread&);
	QSerieRangeThread& operator = (const QSerieRangeThread&);
};	// class QSerieRangeThread

}	// namespace GQualif


#endif	// Q_CAL_QUAL_THREAD_H
