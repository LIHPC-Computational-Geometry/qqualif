#ifndef LIMA_QUALIF_SERIE_H
#define LIMA_QUALIF_SERIE_H

#include "GQualif/AbstractQualifSerieAdapter.h"

#include <LimaP/maillage_it.h>
#include <LimaP/surface_it.h>
#include <LimaP/volume_it.h>

#include <memory>

namespace GQualif
{

/**
 * @mainpage	Page principale de la bibliothèque GQLima
 *
 * @section		presentation	Présentation
 *
 * <P>Cette bibliothèque propose un adapteur de maillage <I>Lima</I> au
 * format <I>Qualif</I>.
 * </P>
 *
 * <P>Classe représentant une série de mailles issues d'un maillage Lima
 * à soumettre à <I>Qualif</I> en vue d'être analysée (ex : surface composée de
 * polygones, ...).</P>
 */
class LimaQualifSerie : public AbstractQualifSerieAdapter
{
	public :

	/**
	 * Constructeur 1. Charge les données depuis le fichier <I>fileName</I>.
	 * Les données du maillage seront détruites lors de la destruction de cette
	 * série.
	 * @param		Nom du fichier d'où est chargée la série.
	 * @param		Nom de la série (groupe au sens <I>Lima</I>).
	 * @param		Dimension des données.
	 */
	LimaQualifSerie (const IN_STD string& fileName, const IN_STD string& name, unsigned char dimension);

	/**
	 * Constructeur 2. Maillage chargé.
	 * @param		<I>true</I> si le maillage est adopté (et donc détruit par
	 * 				cet adapteur), <I>false</I> dans le cas contraire.
	 * @param		Dimension des données.
	 * @param		Eventuel nom de la série (groupe au sens <I>Lima</I>) du
	 * 				maillage à analyser. Si nul, utilise
	 * 				<I>mesh.polygone</I> ou <I>mesh.polyedres</I>, selon
	 * 				<I>dimension</I>, pour accéder aux mailles.
	 * @param		Eventuel nom du fichier d'où est chargée la série.
	 */
	LimaQualifSerie (Lima::_MaillageInterne& mesh, bool destroy,
			unsigned char dimension, const IN_STD string& name, const IN_STD string& fileName);

	/**
	 * Constructeur 3. Les données reçues en argument ne seront pas détruites
	 * lors de la destruction de cette série.
	 * @param		Surface représentée.
	 */
	LimaQualifSerie (const Lima::_SurfaceInterne& surface, Lima::dim_t dimension,
			const IN_STD string& fileName);

	/**
	 * Constructeur 4. Les données reçues en argument ne seront pas détruites
	 * lors de la destruction de cette série.
	 * @param		Surface représentée.
	 */
	LimaQualifSerie (const Lima::_VolumeInterne& volume, const IN_STD string& fileName);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~LimaQualifSerie ( );

	/**
	 * @return		Le nombre de mailles de la série.
	 */
	virtual size_t getCellCount ( ) const;

	/**
	 * @return		La i-ème maille.
	 * @warning		<B>A utiliser tout de suite, le contenu de la maille
	 * 				étant susceptible d'évoluer lors d'autres appels à cette
	 * 				<I>API</I></B>.
	 */
	virtual Qualif::Maille& getCell (size_t i) const;

	/**
	 * @return		Le type, au sens <I>QualifHelper</I>, de la i-ème maille.
	 */
	virtual size_t getCellType (size_t i) const;

	/**
	 * @return		<I>true</I> si les mailles de la série sont volumiques,
	 *				savoir si ce sont des polyèdres.
	 */
	virtual bool isVolumic ( ) const;

	/**
	 * @return		<I>true</I> (la série est utilisable en contexte
	 * 				multithread)
	 */
	virtual bool isThreadable ( ) const;


	protected :

	/**
	 * Initialise certaines propriétés (dimension des mailles).
	 */
	virtual void initialize ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	LimaQualifSerie (const LimaQualifSerie&);
	LimaQualifSerie& operator = (const LimaQualifSerie&);

	/** Le maillage éventuellement chargé par cette instance. */
	IN_STD unique_ptr <Lima::_MaillageInterne>		_mesh;

	/** Faut-il détruire ce maillageà la fin ? */
	bool											_destroy;

	/** L'éventuelle surface représentée. */
	const Lima::_SurfaceInterne*				_surface;

	/** L'éventuel volume représenté. */
	const Lima::_VolumeInterne*				_volume;

	/** La dimension des mailles (polygones : 2, polyèdres : 3). Elle peut être
	 * différente de la dimension du maillage (ex : triangles dans l'espace), et
	 * Qualif fait bien cette distinction (ex : calcul du jacobien des
	 * triangles).
	 */
	mutable unsigned char					_cellsDimension;
};	// class LimaQualifSerie

}	// namespace GQualif

#endif	// LIMA_QUALIF_SERIE_H
