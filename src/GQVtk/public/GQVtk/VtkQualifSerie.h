#ifndef VTK_QUALIF_SERIE_H
#define VTK_QUALIF_SERIE_H

#include "GQualif/AbstractQualifSerieAdapter.h"

#include <vtkUnstructuredGrid.h>
#include <vtkGenericCell.h>

#include <memory>

namespace GQualif
{

/**
 * @mainpage	Page principale de la bibliothèque GQVtk
 *
 * @section		presentation	Présentation
 *
 * <P>Cette bibliothèque propose un adapteur de maillage <I>VTK</I> au
 * format <I>Qualif</I>.
 * </P>
 *
 * <P>Classe représentant une série de mailles issues d'un maillage <I>VTK</I>
 * à soumettre à <I>Qualif</I> en vue d'être analysée (ex : surface composée de
 * polygones, ...).</P>
 *
 * @warning		Seuls les maillages <I>VTK</I> non structurés sont actuellement
 * 				supportés.
 */
class VtkQualifSerie : public AbstractQualifSerieAdapter
{
	public :

	/**
	 * Constructeur 1. Charge les données depuis le fichier <I>fileName</I>.
	 * Les données du maillage seront détruites lors de la destruction de cette
	 * série.
	 * @param		Nom du fichier d'où est chargée la série.
	 * @param		Eventuel nom de la série (au sens de l'application). En son
	 * 				absence c'est le nom du fichier qui sera utilisé.
	 */
	VtkQualifSerie (const IN_STD string& fileName, const IN_STD string& name);

	/**
	 * Constructeur 2. Les données reçues en argument ne seront pas détruites
	 * lors de la destruction de cette série.
	 * @param		Maillage représenté.
	 * @param		Eventuel nom de la série (au sens de l'application). En son
	 * 				absence c'est le nom du fichier qui sera utilisé.
	 * @param		Nom du fichier d'où est éventuellement issue la série.
	 */
	VtkQualifSerie (
				vtkUnstructuredGrid& mesh, const IN_STD string& name,
				const IN_STD string& fileName);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~VtkQualifSerie ( );

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
	 * 				multithread).
	 */
	virtual bool isThreadable ( ) const;


	protected :


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	VtkQualifSerie (const VtkQualifSerie&);
	VtkQualifSerie& operator = (const VtkQualifSerie&);

	/** Le maillage représenté. */
	// mutable car VTK ne sait pas trop ce que c'est que "const".
	mutable vtkUnstructuredGrid*				_mesh;

	/** Pour une bonne gestion des accès concurrents. */
	vtkGenericCell*								_cell;
};	// class VtkQualifSerie

}	// namespace GQualif

#endif	// VTK_QUALIF_SERIE_H
