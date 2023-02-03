#ifndef ABSTRACT_QUALIF_SERIE_ADAPTER_H
#define ABSTRACT_QUALIF_SERIE_ADAPTER_H

#include "GQualif/AbstractQualifSerie.h"

#include <TkUtil/util_config.h>

#include <Triangle.h>
#include <Quadrangle.h>
#include <Prisme.h>
#include <Tetraedre.h>
#include <Pyramide.h>
#include <Hexaedre.h>


namespace GQualif
{

/**
 * <P>Classe abstraite représentant une série de mailles à soumettre à
 * <I>Qualif</I> en vue d'être analysée (ex : surface composée de polygones,
 * ...). Cette classe propose des services pour les classes implémentants
 * l'interface de base <I>AbstractQualifSerie</I>.</P>
 *
 * <P>Cette classe est à spécialiser selon le type de maillage pris en entrée
 * (ex : <I>Lima</I>, <I>VTK</I>, ...).
 * </P>
 *
 * @see		AbstractQualifSerie
 */
class AbstractQualifSerieAdapter : public AbstractQualifSerie
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~AbstractQualifSerieAdapter ( );


	protected :

	/**
	 * Constructeur.
	 * @param		Nom du fichier d'où est éventuellement issue la série.
	 * @param		Nom de la série.
	 * @param		Dimension des données.
	 */
	AbstractQualifSerieAdapter (
				const IN_STD string& fileName, const IN_STD string& name,
				unsigned char dimension);

	/**
	 * @warning	Pour le multithreading les noeuds sont partagés par les
	 * 			mailles d'une d'une même série => seul un thread doit accéder
	 * 			à cette série.
	 */
	Qualif::Vecteur				_noeuds [8];	// A AUGMENTER SI NECESSAIRE
	mutable Qualif::Triangle	_triangle;
	mutable Qualif::Quadrangle	_quadrangle;
	mutable Qualif::Tetraedre	_tetraedron;
	mutable Qualif::Pyramide	_pyramid;
	mutable Qualif::Prisme		_prism;
	mutable Qualif::Hexaedre	_hexaedron;

	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	AbstractQualifSerieAdapter (const AbstractQualifSerieAdapter&);
	AbstractQualifSerieAdapter& operator = (const AbstractQualifSerieAdapter&);

	/** Le nom du fichier d'où est éventuellement issue la série. */
	IN_STD string					_fileName;

	/** Le nom de la série. */
	IN_STD string					_name;
};	// class AbstractQualifSerieAdapter

}	// namespace GQualif

#endif	// ABSTRACT_QUALIF_SERIE_ADAPTER_H
