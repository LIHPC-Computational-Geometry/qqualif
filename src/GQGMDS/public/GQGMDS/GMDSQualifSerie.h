#ifndef GMDS_QUALIF_SERIE_H
#define GMDS_QUALIF_SERIE_H

#include "GQualif/AbstractQualifSerieAdapter.h"

#include <gmds/ig/Mesh.h>

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
template<typename TCellType, unsigned char Dim>
class GMDSQualifSerie : public AbstractQualifSerieAdapter
{
public :

	/**
	 * Constructeur 1.
	 * @param		Maillage chargé.
	 * @param		<I>true</I> si le maillage est adopté (et donc détruit par
	 * 				cet adapteur), <I>false</I> dans le cas contraire.
	 * @param		Eventuel nom de la série du maillage à analyser. Si nul,
	 *				utilise <I>mesh.getLFace</I> ou <I>mesh.getLRegion</I>,
	 *				selon <I>dimension</I>, pour accéder aux mailles.
	 * @param		Eventuel nom du fichier d'où est chargée la série.
	 */
	GMDSQualifSerie (
			gmds::Mesh& mesh,
			const std::string& name, const std::string& fileName);

	/**
	 * Constructeur 2. Les données reçues en argument ne seront pas détruites
	 * lors de la destruction de cette série.
	 * @param		Vecteur de mailles<I>GMDS</I> représentées.
	 * @param		Eventuel nom du groupe de mailles (au sens de l'application). En 
	 * 				son absence c'est le nom du fichier qui sera utilisé.
	 * @param		Nom du fichier d'où est éventuellement issue la surface.
	 */
	GMDSQualifSerie (
			const std::vector<TCellType>& v, const std::string& name,
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
	 * @see			getCell
	 * @see			getGMDSCell
	 */
	virtual Qualif::Maille& getCell (size_t i) const;

	/**
	 * @return		La i-ème maille en tant que polygone <I>GMDS</I>.
	 * @see			getCell
	 */
	virtual TCellType getGMDSCell (size_t i) const;

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

	// gmdsCells est un vector de Face ou Region
	std::vector<TCellType> _gmdsCells;

	void fill(gmds::Mesh& mesh, const std::string& name);

};	// class GMDSQualifSerie

typedef GMDSQualifSerie<gmds::Face, 2> GMDSFaceQualifSerie;
typedef GMDSQualifSerie<gmds::Region, 3> GMDSRegionQualifSerie;

}	// namespace GQualif

#endif	// GMDS_QUALIF_SERIE_H
