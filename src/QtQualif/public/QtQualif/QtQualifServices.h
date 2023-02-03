#ifndef QT_QUALIF_SERVICES_H
#define QT_QUALIF_SERVICES_H

#include <QwtCharts/QwtHistogramChartPanel.h>

#include <TkUtil/Version.h>


namespace GQualif
{

/**
 * \mainpage	Page principale de la bibliothèque <I>GQualif</I> et de ses
 * 				bibliothèques complémentaires.
 *
 * \section	presentation	Présentation
 * <P>La bibliothèque <I>GQualif</I> est une surcouche de la bibliothèque de
 * qualité de maillage <I>Qualif</I>, et plus précisemment de son noyau interne
 * <I>qualifinternal</I>. Elle offre un service générique, à savoir indépendant
 * de l'<I>API</I> du maillage, qui repose sur la notion d'adapteur.
 * </P>
 *
 * <P>La classe <I>AbstractQualifSerie</I> défini un ensemble de mailles à
 * soumettre à <I>Qualif</I> en vue d'être analysée (ex : surface composée de
 * polygones, ...). La classe <I>AbstractQualifSerieAdapter</I>, qui en dérive,
 * propose des services aux classes implémentants l'interface de base
 * <I>AbstractQualifSerie</I> : support d'un nom de fichier,
 * mailles et noeuds <I>Qualif</I> pour les algorithmes.
 * </P>
 *
 * <P>La classe <I>QualifHelper</I> offre des services additionnels à la
 * bibliothèque <I>Qualif</I> : domaine des critères de qualité selon les types
 * de mailles, disponibilité des algorithmes selon les types de mailles, ...
 * </P>
 *
 * <P>Les bibliothèques GQLima, GQGMDS et GQVtk
 * sont des adaptateurs respectivement pour les bibliothèques de maillage
 * <I>Lima</I>, <I>GMDS</I> et <I>VTK</I>.
 * <P>
 *
 * <P>La bibliothèque <I>QtQualif</I> est une surcouche graphique en
 * environnement <I>Qt</I> et <I>Qwt</I>. Elle s'appuie à cette fin sur la
 * bibliothèque QwtCharts.
 * La classe <I>QtQualifWidget</I> est un panneau <I>Qt</I> prenant en charge
 * des séries de mailles et proposant des analyses qualitives adaptées. Les
 * résultats sont présentés sous forme d'histogrammes. Il est possible, par
 * dérivation de la classe et surcharge de sa méthode <I>increment</I>, de
 * récupérer les mailles par classes d'histogrammes en vue de les afficher ou
 * de les soumettre à d'autres traitements.
 * </P>
 *
 * <P>Le programme <I>QCalQual</I> est un petit programme graphique permettant
 * une analyse simple de maillages à différents formats.
 * </P>
 */
class QtQualifServices
{
	public :

	/**
	 * @return		La version de la bibliothèque.
	 */
	static IN_UTIL Version getVersion ( );

	/**
	 * Initialise les services de cette <I>API</I>.4i et de
	 * l'<I>API QwtCharts</I> sous-jacente.
	 * @see		finalize
	 */
	static void initialize ( );

	/**
	 * Finalise les services de cette <I>API</I>.4i et de
	 * l'<I>API QwtCharts</I> sous-jacente.
	 * @see		initialize
	 */
	static void finalize ( );


	private :

	/**
	 * Constructeurs et opérateur = : interdits.
	 */
	QtQualifServices ( );
	QtQualifServices (const QtQualifServices&);
	QtQualifServices& operator = (const QtQualifServices&);

	/**
	 * Destructeur. Interdit.
	 */
	~QtQualifServices ( );
};	// class QtQualifServices

}	// namespace GQualif

#endif	// QT_QUALIF_SERVICES_H
