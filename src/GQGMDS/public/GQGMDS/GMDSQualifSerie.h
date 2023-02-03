#ifndef GMDS_QUALIF_SERIE_H
#define GMDS_QUALIF_SERIE_H

#include "GQualif/AbstractQualifSerieAdapter.h"

#include <GMDS/IG/IGMesh.h>

#include <vector>


namespace GQualif
{

/**
 * @mainpage	Page principale de la bibliothèque GQGMDS
 *
 * @section		presentation	Présentation
 *
 * <P>Cette bibliothèque propose un adapteur de maillage <I>GMDS</I> au
 * format <I>Qualif</I>.
 * </P>
 *
 * <P>Classe représentant une série de mailles issues d'un maillage <I>GMDS</I>
 * à soumettre à <I>Qualif</I> en vue d'être analysée (ex : surface composée de
 * polygones, ...).</P>
 */
class GMDSQualifSerie : public AbstractQualifSerieAdapter
{
public :

	/**
	 * Constructeur 1.
	 * @param		Maillage chargé.
	 * @param		<I>true</I> si le maillage est adopté (et donc détruit par
	 * 				cet adapteur), <I>false</I> dans le cas contraire.
	 * @param		Dimension des données.
	 * @param		Eventuel nom de la série du maillage à analyser. Si nul,
	 *				utilise <I>mesh.getLFace</I> ou <I>mesh.getLRegion</I>,
	 *				selon <I>dimension</I>, pour accéder aux mailles.
	 * @param		Eventuel nom du fichier d'où est chargée la série.
	 */
	GMDSQualifSerie (
			gmds::IGMesh& mesh, bool destroy,
			unsigned char dimension,
			const std::string& name, const std::string& fileName);

	/**
	 * Constructeur 2. Les données reçues en argument ne seront pas détruites
	 * lors de la destruction de cette série.
	 * @param		Surface <I>GMDS</I> représentée.
	 * @param		Eventuel nom de la surface (au sens de l'application). En 
	 * 				son absence c'est le nom du fichier qui sera utilisé.
	 * @param		Nom du fichier d'où est éventuellement issue la surface.
	 */
	GMDSQualifSerie (
			gmds::IGMesh::surface&, const std::string& name,
			const std::string& fileName);

	/**
	 * Constructeur 3. Les données reçues en argument ne seront pas détruites
	 * lors de la destruction de cette série.
	 * @param		Volume <I>GMDS</I> représenté.
	 * @param		Eventuel nom du volume (au sens de l'application). En 
	 * 				son absence c'est le nom du fichier qui sera utilisé.
	 * @param		Nom du fichier d'où est éventuellement issue le volume.
	 */
	GMDSQualifSerie (
			gmds::IGMesh::volume&, const std::string& name,
			const std::string& fileName);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~GMDSQualifSerie ( );

	/**
	 * @return		Le nombre de mailles de la série.
	 */
	virtual size_t getCellCount ( ) const;

	/**
	 * @return		La i-ème maille.
	 * @warning		<B>A utiliser tout de suite, le contenu de la maille
	 * 				étant susceptible d'évoluer lors d'autres appels à cette
	 * 				<I>API</I></B>.
	 * @see			getCellType
	 * @see			getGMDSFace
	 * @see			getGMDSRegion
	 */
	virtual Qualif::Maille& getCell (size_t i) const;

	/**
	 * @return		La i-ème maille en tant que polygone <I>GMDS</I>.
	 * @warning		Lève une exception si <I>i >= getCellCount ( )</I> ou si
	 *				la i-ème maille n'est pas de type <I>Face de GMDS</I>.
	 * @see			getCell
	 * @see			getGMDSRegion
	 */
	virtual gmds::Face getGMDSFace (size_t i) const;

	/**
	 * @return		La i-ème maille en tant que polyèdre <I>GMDS</I>.
	 * @warning		Lève une exception si <I>i >= getCellCount ( )</I> ou si
	 *				la i-ème maille n'est pas de type <I>Region de GMDS</I>.
	 * @see			getCell
	 * @see			getGMDSFace
	 */
	virtual gmds::Region getGMDSRegion (size_t i) const;

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
	 * 				multithread).
	 */
	virtual bool isThreadable ( ) const;


protected :


private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	GMDSQualifSerie (const GMDSQualifSerie&);
	GMDSQualifSerie& operator = (const GMDSQualifSerie&);

	/** L'éventuel maillage associé. */
	gmds::IGMesh*					_mesh;

	/** Faut-il détruire ce maillageà la fin ? */
	bool										_destroy;

	/** L'éventuelle surface représentée. */
	gmds::IGMesh::surface*		_surface;

	/** L'éventuel volume représenté. */
	gmds::IGMesh::volume*			_volume;

	/** Pour les maillages sans groupes de mailles (surfaces ou volumes) :
	 * avoir la liste (non trouée) des mailles analysées (polygones/polyèdres).
	 */
	std::vector<gmds::Face>					_faces;
	std::vector<gmds::Region>					_regions;
};	// class GMDSQualifSerie

}	// namespace GQualif

#endif	// GMDS_QUALIF_SERIE_H
