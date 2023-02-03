#ifndef ABSTRACT_QUALIF_SERIE_H
#define ABSTRACT_QUALIF_SERIE_H

#include <TkUtil/UTF8String.h>

#include <Maille.h>

#include <map>
#include <vector>

namespace GQualif
{

/**
 * <P>Classe abstraite représentant une série de mailles à soumettre à
 * <I>Qualif</I> en vue d'être analysée (ex : surface composée de polygones,
 * ...).</P>
 *
 * <P>Cette classe est à spécialiser selon le type de maillage pris en entrée
 * (ex : <I>Lima</I>, <I>VTK</I>, ...).
 * </P>
 *
 * <P>Les méthodes de cette classe sont susceptibles de lever des exceptions
 * de type <I>TkUtil::Exception</I>.
 * </P>
 *
 * <P>Cette classe offre un service de <B>validation des coordonnées</B>
 * (cf. méthode <I>validateCoordinates</I>), qui stocke le résultat en cache.
 * Il convient que les classes dérivées et/ou utilisateurs de ce service
 * vérifient que les noeuds et/ou mailles ne soient pas modifiées entre 2
 * utilisations du services, ou fassent appel si nécessaire à la méthode
 * <I>setCoordinatesValidity (false, false, "")</I>.
 * </P>
 *
 * <P>A propos du multithreading et de la méthode <I>isThreadable</I>. Les
 * classes dérivées peuvent être consultées en lecture de manière concurente.
 * Les classes supportant cet accès consultatif en parallèle doivent retourner
 * <I>true</I> à <I>isThreadable</I>. <U>Les accès concurrents ne sont pas faits
 * au sein d'une série donnée, mais entre séries différentes.</I>
 * </P>
 */
class AbstractQualifSerie
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~AbstractQualifSerie ( );

	/**
	 * @return		Le nom de la série.
	 */
	virtual IN_STD string getName ( ) const;

	/**
	 * @return		Le nom du fichier d'où est issue la série.
	 */
	virtual IN_STD string getFileName ( ) const;

	/**
	 * @return		Le nombre de mailles de la série.
	 */
	virtual size_t getCellCount ( ) const = 0;

	/**
	 * @return		La i-ème maille.
	 * @warning		<B>A utiliser tout de suite, le contenu de la maille
	 * 				étant susceptible d'évoluer lors d'autres appels à cette
	 * 				<I>API</I></B>.
	 * @see			getCellType
	 */
	virtual Qualif::Maille& getCell (size_t i) const = 0;

	/**
	 * @return		Le type, au sens <I>QualifHelper</I>, de la i-ème maille.
	 * @see			getCell
	 * @see			QualifHelper::TRIANGLE
	 * @see			QualifHelper::QUADRANGLE
	 * @see			QualifHelper::TETRAEDRON
	 * @see			QualifHelper::PYRAMID
	 * @see			QualifHelper::HEXAEDRON
	 * @see			QualifHelper::TRIANGULAR_PRISM
	 */
	virtual size_t getCellType (size_t i) const = 0;

	/**
	 * @return		La dimension de l'espace (a priori 2 ou 3).
	 * @see			isVolumic
	 */
	virtual unsigned char getDimension ( ) const;

	/**
	 * @return		<I>true</I> si les mailles de la série sont volumiques, à
	 *				savoir si ce sont des polyèdres. (REM : un polygone peut
	 *				être de dimension 3).
	 * @see			getDimension
	 */
	virtual bool isVolumic ( ) const = 0;

	/**
 	 * Rem : cette méthode met en cache les valeurs du critère de qualité donné
 	 * pour les mailles de la série, de même que les valeurs min et max
 	 * rencontrées.
	 * @return		Les extrema du domaine d'échantillonage pour le critère
	 *				transmis en argument.
	 * @see			getStoredCriteria
	 * @see			releaseStoredData
	 */
	virtual void getDataRange (
					Qualif::Critere criterion, double& min, double& max) const;
	
	/**
	 * @return		Les types de mailles au sens <I>QualifHelper</I>, définis
	 * 				par un ou exclusif sur les types élémentaires
	 * 				(TRIANGLE, ...).
	 * @warning		Par défaut, si la valeur en cache est nulle, parcourt alors
	 * 				toutes les mailles et conserve la valeur obtenue en cache.
	 */
	virtual size_t getDataTypes ( ) const;

	/**
	 * Evalue les coordonnées des noeuds de la série.
	 * @exception	Lève une exception au premier noeud rencontré de
	 * 				coordonnées invalides.
	 * @warning		<B>En cas de surcharge ou d'ajout/modification de noeuds à
	 * 				la série la méthode surchargée doit invoquer la méthode
	 * 				<I>setCoordinatesValidity</I>.</B>
	 * @see			setCoordinatesValidity
	 */
	virtual void validateCoordinates ( );

	/**
 	 * Méthode à appeler en cas de validation des noeuds.
 	 * @param		<I>true</I> si les noeuds viennent d'être soumis au service
 	 * 				de validation des noeuds (quelque soit le résultat de
 	 * 				l'évaluation), <I>false</I> dans le cas contraire (ou si
 	 * 				des noeuds sont ajoutés ou modifiés, donc une réévaluation
 	 * 				est nécessaire).
 	 * @param		<I>true</I> si les coordonnées des noeuds sont valides,
 	 *				<I>false</I> dans le cas contraire.
 	 * @param		Eventuel message d'erreur (si <I>valid</I> vaut
 	 * 				<I>false</I>).
 	 * @see			setCoordinatesValidity
	 */
	virtual void setCoordinatesValidity (bool evaluated, bool valid, const IN_UTIL UTF8String& msg);

	/**
	 * @param		Un critère de qualité Qualif.
	 * @return		<I>true</I> si la valeur pour chaque maille du critère
	 *				transmis en argument est stockée.
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool isCriteriaStored (Qualif::Critere criterion) const;

	/**
	 * @param		Un critère de qualité Qualif.
	 * @param		L'indice d'une maille dans cette série.
	 * @return		la valeur stockée du critère pour cette maille.
	 * @warning		La valeur pour ce critère doit avoir été précédemment stockée.
	 *
	 * @see			isCriteriaStored
	 * @see			releaseStoredData
	 */
	virtual double getStoredCriteria (Qualif::Critere criterion, size_t i) const;

	/**
	 * @return		Les index des mailles rangés par classe.
	 * Usage : getClassesCellsIndexes ( )[classe]
	 */
	virtual const std::vector < std::vector <size_t> >& getClassesCellsIndexes ( ) const;
	virtual std::vector < std::vector <size_t> >& getClassesCellsIndexes ( );

	/**
	 * @param		En retour les index des mailles dont l'index de la classe
	 * 				est transmis en second argument.
	 */
	virtual void getCellsIndexes (std::vector<size_t>& indexes, size_t clas) const;

	/**
	 * Ajoute l'index de maille transmis en second argument aux index de la
	 * classe transmise en premier argument.
	 * <I>setClassesNum</I> doit préalablement avoir été appelé.
	 * @see		setClassesNum
	 * @see		getClassesCellsIndexes
	 */
	virtual void storeIndex (size_t cl, size_t c);

	/**
	 * Alloue le vecteur pour stocker les indexe des mailles dans <I>nb</I>
	 * classes.
	 * @see		storeIndex
	 * @see		getClassesCellsIndexes
	 */
	virtual void setClassesNum (size_t nb);

	/**
	 * Libère la mémoire occupée par les données en cache.
	 */
	virtual void releaseStoredData ( );

	/**
	 * @return		<I>true</I> si la série est utilisable en contexte
	 * 				multithread, <I>false</I> dans le cas contraire.
	 * 				Retourne <I>false</I> par défaut, méthode à surcharger.
	 */
	virtual bool isThreadable ( ) const;


	protected :

	/**
	 * Constructeur.
	 * @param		Nom du fichier d'où est éventuellement issue la série.
	 * @param		Nom de la série.
	 * @param		Dimension des données.
	 */
	AbstractQualifSerie (
				const IN_STD string& fileName, const IN_STD string& name,
				unsigned char dimension);

	/**
	 * @param		Nom de la série représentée.
	 */
	virtual void setName (const IN_STD string& name);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	AbstractQualifSerie (const AbstractQualifSerie&);
	AbstractQualifSerie& operator = (const AbstractQualifSerie&);

	/** Le nom du fichier d'où est éventuellement issue la série. */
	IN_STD string					_fileName;

	/** Le nom de la série. */
	IN_STD string					_name;

	/** La dimension des données. */
	unsigned char					_dimension;

	/** Les types de mailles. */
	mutable size_t					_dataTypes;

	/** Les coordonnées ont elles été validées ? Sont elles valides ? */
	mutable bool					_coordinatesValidityEvaluated,
	                                _coordinatesValidity;

	/** Message d'erreur (en cache) concernant l'évaluation de la validité
	 * des coordonnées. */
	IN_UTIL UTF8String				_coordinatesValidityErrorMsg;

	/** Stockage de la valeur des critères précédemment calculés. */
	mutable std::map<Qualif::Critere,std::vector<double> > _storedValues;

	/** Stockage des extrema atteints aux mailles selon les critères
	 * appliqués. */
	mutable std::map<Qualif::Critere, double>			_minValues, _maxValues;

	/** Stockage du booléen décrivant si la valeur du critères a été stockée
	 * (utile pour les critères non définis sur certains type de mailles). */
	mutable std::map<Qualif::Critere,std::vector<bool> > _storedValuesDefined;

	/** Les index des mailles dans chaque classe. */
	mutable std::vector < std::vector <size_t> >		_classesCellsIndexes;
};	// class AbstractQualifSerie

}	// namespace GQualif

#endif	// ABSTRACT_QUALIF_SERIE_H
