#ifndef QUALIF_HELPER_H
#define QUALIF_HELPER_H

#include <TkUtil/UTF8String.h>

#include <unistd.h>
#include <cmath>
#include <string>

#include <ConstQualif.h>
#include <Maille.h>
#include <TkUtil/Version.h>


namespace GQualif
{

/** Classe fournissant des services relatifs à la bibliothèque de qualité de maillage Qualif.
 * A partir de la version 3.0.0 les exceptions levées sont de type TkUtil::Exception.
 */
class QualifHelper
{
	public :

	/**
	 * @return		La version de la bibliothèque.
	 */
	static IN_UTIL Version getVersion ( );

	/**
	 * Les services <I>Qualif</I>.
	 */
	//@{

	/** Les types de mailles supportés par Qualif. */
	static const size_t		TRIANGLE, QUADRANGLE, TETRAEDRON, PYRAMID, HEXAEDRON, TRIANGULAR_PRISM;

	/**
	 * Retourne le domaine théorique de valeurs de retourne d'un appel Qualif
	 * ( AnalyseSurface, AnalyseVolume, ...). Ce domaine théorique prend en
	 * compte le critère appliqué et les types de mailles soumis.
	 * @param		Critère de qualité à appliquer
	 * @param		Types de mailles à prendre en compte, obtenu par ou 
	 *				exclusif (ex : TETRAEDRON | PYRAMID | HEXAEDRON).
	 * @param		Valeur minimale théorique
	 * @param		Valeur maximale théorique
	 * @exception	Une exception est levée si cellTypes est nul ou ne fait
	 *				référence qu'à des types inconnus.
	 */
	static void getDomain (Qualif::Critere criteria, size_t cellTypes, double& min, double& max);

	/**
	 * @return		true si le type transmis en argument est supporté
	 *				par Qualif, ou false dans le cas contraire.
	 */
	static bool isTypeSupported (size_t type);

	/**
	 * @return		true si le type transmis en argument est supporté
	 *				par Qualif pour le critère donné, ou false dans le 
	 *				cas contraire.
	 */
	static bool isTypeSupported (size_t type, Qualif::Critere criteria);

	/**
	 * @return		La valeur minimale pour un triangle et pour le critère
	 *				transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux triangles.
	 */
	static double getTriangleMinValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur maximale pour un triangle et pour le critère
	 *				transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux triangles.
	 */
	static double getTriangleMaxValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur minimale pour un quadrangle et pour le critère
	 *				transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux quadrangles.
	 */
	static double getQuadrangleMinValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur maximale pour un quadrangle et pour le critère
	 *				transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux quadrangles.
	 */
	static double getQuadrangleMaxValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur minimale pour un hexaèdre et pour le 
	 *				critère transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux hexaèdres.
	 */
	static double getHexaedronMinValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur maximale pour un hexaèdre et pour le 
	 *				critère transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux hexaèdres.
	 */
	static double getHexaedronMaxValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur minimale pour un tétraèdre et pour le 
	 *				critère transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux tétraèdres.
	 */
	static double getTetraedronMinValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur maximale pour un tétraèdre et pour le 
	 *				critère transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux tétraèdres.
	 */
	static double getTetraedronMaxValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur minimale pour une pyramide et pour le critère
	 *				transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux pyramides.
	 */
	static double getPyramidMinValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur maximale pour une pyramide et pour le critère
	 *				transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux pyramides.
	 */
	static double getPyramidMaxValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur minimale pour un prisme à base triangulaire et
	 *				pour le critère transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux prismes à base triangulaire.
	 */
	static double getTriangularPrismMinValue (Qualif::Critere criteria);

	/**
	 * @return		La valeur maximale pour un prisme à base triangulaire et
	 *				pour le critère transmis en argument.
	 * @exception	Une exception est levée si le critère ne s'applique
	 *				pas aux prismes à base triangulaire.
	 */
	static double getTriangularPrismMaxValue (Qualif::Critere criteria);

	/*
	 * @return		true si la valeur est infinie, sinon false
	 */
//	static bool isInfinite (double value);

	/**
	 * @return		Le nom du type de donnée transmis en argument.
	 */
	static IN_UTIL UTF8String dataTypeToName (size_t type);

	//@}	// Les services <I>Qualif</I>.

	/**
	 * Autres services que <I>Qualif</I>.
	 */
	//@{

	/**
	 * Vérifie que les coordonnées de la maille transmise en argument sont
	 * valides (au sens machine, c.a.d. pas <I>NaN</I> par exemple).
	 * \param		Maille dont on évalue les noeuds
	 * \param		<I>true</I> s'il faut lever une exception en cas de
	 * 				coordonnée invalide, <I>false</I> s'il faut retourner
	 * 				<I>false</I> en cas de coordonnée invalide.
	 * \return		<I>true</I> si les coordonnées de la maille sont valides,
	 *				<I>false</I> dans le contraire.
	 */
	static bool hasValideCoordinates (const Qualif::Maille& cell, bool exc);

	//@}	// Autres services que <I>Qualif</I>.


	private :

	QualifHelper ( );

	QualifHelper (const QualifHelper&);

	QualifHelper& operator = (const QualifHelper&);

	~QualifHelper ( );
};	// class QualifHelper

}	// namespace GQualif


#endif	// QUALIF_HELPER_H
