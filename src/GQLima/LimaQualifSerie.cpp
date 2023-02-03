#include "GQLima/LimaQualifSerie.h"
#include "GQualif/QualifHelper.h"

#include <TkUtil/Exception.h>
#include <TkUtil/File.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>

#include <iostream>
#include <assert.h>


USING_UTIL
USING_STD
using namespace Lima;
using namespace Qualif;

static const Charset	charset ("àéèùô");

namespace GQualif
{

LimaQualifSerie::LimaQualifSerie (
			const string& fileName, const string& name, unsigned char dimension)
	: AbstractQualifSerieAdapter (fileName, name, dimension),
	  _mesh ( ), _destroy (true), _surface (0), _volume (0), _cellsDimension (0)
{
	try
	{
		TkUtil::File	file (getFileName ( ));
		if (0 == name.length ( ))
			setName (file.getFileName ( ));
		
		// Petite vérification préalable :
		if (false == file.isReadable ( ))
		{
			UTF8String	message (charset);
			message << "Le groupe de mailles " << getName ( )
			        << " ne peut pas être chargé depuis le fichier "
			        << getFileName ( ) << " car celui-ci n'existe pas ou n'est "
			        << "pas accessible en lecture.";
			throw Exception (message);
		}	// if (false == file.isReadable ( ))

		// Chargement du maillage :
		_mesh.reset (new _MaillageInterne ( ));
		_mesh->lire (fileName);

		// On récupère le groupe demandé :
		switch (dimension)
		{
			case	2	: _surface	= _mesh->surface (name);	break;
			case	3	: _volume	= _mesh->volume (name);		break;
			default		:
			{
				UTF8String	error (charset);
				error << "Dimension de groupe de mailles invalide ("
				      << (unsigned long)dimension << " pour le groupe "
				      << name << " du maillage du fichier " << fileName << ".";
				throw Exception (error);
			}	// default
		}	// switch (dimension)

		initialize ( );
	}
	catch (const erreur& err)
	{
		UTF8String	error (charset);
		error << "Erreur Lima lors de la récupération du groupe " << name
		      << " du maillage du fichier " << fileName << " : "
		      << err.what ( );
		throw Exception (error);
	}
	catch (const Exception& e)
	{
		throw;
	}
	catch (const exception& exc)
	{
		UTF8String	error (charset);
		error << "Erreur lors de la récupération du groupe " << name
		      << " du maillage du fichier " << fileName << " : "
		      << exc.what ( );
		throw Exception (error);
	}
	catch (...)
	{
		UTF8String	error (charset);
		error << "Erreur non documentée lors de la récupération du groupe "
		      << name << " du maillage du fichier " << fileName << ".";
		throw Exception (error);
	}
}	// LimaQualifSerie::LimaQualifSerie


LimaQualifSerie::LimaQualifSerie (
			_MaillageInterne& mesh, bool destroy, unsigned char dimension,
			const string& name, const string& fileName)
	: AbstractQualifSerieAdapter (fileName, name, dimension),
	  _mesh (&mesh), _destroy (destroy), _surface (0), _volume (0)
{
	try
	{

		switch (dimension)
		{
			case	2	:
				if (0 != name.size ( ))
					_surface	= _mesh->surface (name);
				break;
			case	3	:
				if (0 != name.size ( ))
					_volume	= _mesh->volume (name);
				break;
			default		:
			{
				UTF8String	error (charset);
				 error << "Dimension non supportée (" <<(unsigned long)dimension
				       << ") pour l'adaptateur Lima++/Qualif du groupe "
				       << name << " du maillage Lima++ issu du fichier "
				       << fileName << ".";
				throw TkUtil::Exception (error);
			}
		}	// switch (dimension)

		initialize ( );

	}
	catch (const erreur& err)
	{
		UTF8String	error (charset);
		error << "Erreur Lima++ lors de la construction de l'adaptateur "
		      << "Lima++/Qualif du groupe " << name
		      << " du maillage Lima++ issu du fichier " << fileName << " :"
		      << "\n" << err.what ( );
		throw Exception (error);
	}
	catch (const TkUtil::Exception& exc)
	{
		throw;
	}
	catch (const std::exception& e)
	{
		UTF8String	error (charset);
		error << "Erreur lors de la construction de l'adaptateur "
		      << "Lima++/Qualif du groupe " << name
		      << " du maillage Lima++ issu du fichier " << fileName << " :"
		      << "\n" << e.what ( );
		throw Exception (error);
	}
	catch (...)
	{
		UTF8String	error (charset);
		error << "Erreur non documentée lors de la construction de "
		      << "l'adaptateur Lima++/Qualif du groupe " << name
		      << " du maillage Lima++ issu du fichier " << fileName << ".";
		throw Exception (error);
	}
}	// LimaQualifSerie::LimaQualifSerie


LimaQualifSerie::LimaQualifSerie (
					const _SurfaceInterne& surface, Lima::dim_t dimension,
					const string& fileName)
	: AbstractQualifSerieAdapter (
						fileName, surface.nom ( ), (unsigned char)dimension),
	  _mesh ( ), _destroy (false), _surface (&surface), _volume (0)
{
		initialize ( );
}	// LimaQualifSerie::LimaQualifSerie


LimaQualifSerie::LimaQualifSerie (
					const _VolumeInterne& volume, const string& fileName)
	: AbstractQualifSerieAdapter (fileName, volume.nom ( ), 3),
	  _mesh ( ), _destroy (false), _surface (0), _volume (&volume)
{
		initialize ( );
}	// LimaQualifSerie::LimaQualifSerie


LimaQualifSerie::LimaQualifSerie (const LimaQualifSerie&)
	: AbstractQualifSerieAdapter (
						"Invalid file name", "Invalid name", (unsigned char)-1),
	  _mesh ( ), _surface (0), _volume (0)
{
	assert (0 && "LimaQualifSerie copy constructor is not allowed.");
}	// LimaQualifSerie::LimaQualifSerie


LimaQualifSerie& LimaQualifSerie::operator = (const LimaQualifSerie&)
{
	assert (0 && "LimaQualifSerie assignment operator is not allowed.");
	return *this;
}	// LimaQualifSerie::operator =


LimaQualifSerie::~LimaQualifSerie ( )
{
	if (false == _destroy)
		_mesh.release ( );
}	// LimaQualifSerie::~LimaQualifSerie


size_t LimaQualifSerie::getCellCount ( ) const
{
	switch (_cellsDimension)
	{
		case	2	:
			if (0 == _surface)
			{
				if (0 != _mesh.get ( ))
					return _mesh->nb_polygones ( );
			}
			else
				return _surface->nb_elm ( );
			break;
		case	3	:
			if (0 == _volume)
			{
				if (0 != _mesh.get ( ))
					return _mesh->nb_polyedres ( );
			}
			else
				return _volume->nb_elm ( );
			break;
		case	0	:
			initialize ( );
			if (0 != _cellsDimension)
				return getCellCount ( );
	}	// switch (_cellsDimension)

	 INTERNAL_ERROR (exc, "Absence de groupe de mailles et de maillage.", "LimaQualifSerie::getCellCount")
	throw exc;
}	// LimaQualifSerie::getCellCount


Qualif::Maille& LimaQualifSerie::getCell (size_t i) const
{
	if (i >= getCellCount ( ))
	{
		UTF8String	error (charset);
		error << "Le groupe de données ne contient que " << getCellCount ( )
		      << " mailles. Accès à la " << i << "-ème maille impossible.";
		throw Exception (error);
	}	// if (i >= getCellCount ( ))

	try
	{

		switch (_cellsDimension)
		{
			case	0	:
				initialize ( );
				if (0 != _cellsDimension)
					return getCell (i);
				throw Exception (UTF8String ("Erreur lors de la détermination de la dimension des mailles.", charset));
				break;
			case	2	:
			{
				if ((0 == _surface) && (0 == _mesh.get ( )))
				{
					INTERNAL_ERROR (exc, "Surface nulle pour une dimension 2",
						"LimaQualifSerie::getCell")
					throw exc;
				}	// if ((0 == _surface) && (0 == _mesh.get ( )))
				_PolygoneInterne*	polygon	= 0 == _surface ?
						_mesh->polygone (i) : _surface->polygone (i);
				CHECK_NULL_PTR_ERROR (polygon)
				switch (polygon->nb_noeuds ( ))
				{
					case	3	:
						for (size_t s = 0; s < 3; s++)
						{
							_NoeudInterne*	node	= polygon->noeud (s);
							CHECK_NULL_PTR_ERROR (node)
							_triangle.Modifier_Sommet (
									s, node->x ( ), node->y ( ), node->z ( ));
						}	// for (size_t s = 0; s < 3; s++)
						return _triangle;
					case	4	:
						for (size_t s = 0; s < 4; s++)
						{
							_NoeudInterne*	node	= polygon->noeud (s);
							CHECK_NULL_PTR_ERROR (node)
							_quadrangle.Modifier_Sommet (
									s, node->x ( ), node->y ( ), node->z ( ));
						}	// for (size_t s = 0; s < 4; s++)
						return _quadrangle;
					default		:
					{
						UTF8String	error (charset);
						error << "Nombre de noeuds invalide ("
						      << polygon->nb_noeuds ( )
						      << ") pour la " << i
						      << "-ème maille de type polygone.";
						throw Exception (error);
					}	// default
				}	// switch (polygon->nb_noeuds ( ))
			}	// case 2
			case	3	:
			{
				if ((0 == _volume) && (0 == _mesh.get ( )))
				{
					INTERNAL_ERROR (exc, "Volume nul pour une dimension 3",
						"LimaQualifSerie::getCell")
					throw exc;
				}	// if ((0 == _volume) && (0 == _mesh.get ( )))
				_PolyedreInterne*	polyedron	= 0 == _volume ?
									_mesh->polyedre (i) : _volume->polyedre (i);
				CHECK_NULL_PTR_ERROR (polyedron)
				switch (polyedron->nb_noeuds ( ))
				{
					case	4	:
						for (size_t s = 0; s < 4; s++)
						{
							_NoeudInterne*	node	= polyedron->noeud (s);
							CHECK_NULL_PTR_ERROR (node)
							_tetraedron.Modifier_Sommet (
									s, node->x ( ), node->y ( ), node->z ( ));
						}	// for (size_t s = 0; s < 3; s++)
						return _tetraedron;
					case	5	:
						for (size_t s = 0; s < 5; s++)
						{
							_NoeudInterne*	node	= polyedron->noeud (s);
							CHECK_NULL_PTR_ERROR (node)
							_pyramid.Modifier_Sommet (
									s, node->x ( ), node->y ( ), node->z ( ));
						}	// for (size_t s = 0; s < 5; s++)
						return _pyramid;
					case	6	:
						for (size_t s = 0; s < 6; s++)
						{
							_NoeudInterne*	node	= polyedron->noeud (s);
							CHECK_NULL_PTR_ERROR (node)
							_prism.Modifier_Sommet (
									s, node->x ( ), node->y ( ), node->z ( ));
						}	// for (size_t s = 0; s < 6; s++)
						return _prism;
					case	8	:
						for (size_t s = 0; s < 8; s++)
						{
							_NoeudInterne*	node	= polyedron->noeud (s);
							CHECK_NULL_PTR_ERROR (node)
							_hexaedron.Modifier_Sommet (
									s, node->x ( ), node->y ( ), node->z ( ));
						}	// for (size_t s = 0; s < 8; s++)
						return _hexaedron;
					default		:
					{
						UTF8String	error (charset);
						error << "Nombre de noeuds invalide ("
						      << polyedron->nb_noeuds ( )
						      << ") pour la " << i
						      << "-ème maille de type polyèdre.";
						throw Exception (error);
					}	// default
				}	// switch (polyedron->nb_noeuds ( ))
			}	// case 2
			default		:
			{
				UTF8String	error (charset);
				error << "Dimension non supportée ("
				      << (unsigned long)getDimension ( ) << ").";
				INTERNAL_ERROR (exc, error, "LimaQualifSerie::getCell")
				throw exc;
			}
		}	// switch (_cellsDimension)
	}
	catch (const erreur& err)
	{
		UTF8String	error (charset);
		error << "Erreur Lima lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << err.what ( );
		throw Exception (error);
	}
	catch (const Exception& exc)
	{
		UTF8String	error (charset);
		error << "Erreur lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << exc.getFullMessage ( );
		throw Exception (error);
	}
	catch (const exception& e)
	{
		UTF8String	error (charset);
		error << "Erreur lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << e.what ( );
		throw Exception (error);
	}
	catch (...)
	{
		UTF8String	error (charset);
		error << "Erreur non documentée lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << ".";
		throw Exception (error);
	}
}	// LimaQualifSerie::getCell


size_t LimaQualifSerie::getCellType (size_t i) const
{
	if (i >= getCellCount ( ))
	{
		UTF8String	error (charset);
		error << "Le groupe de données ne contient que " << getCellCount ( )
		      << " mailles. Accès à la " << i << "-ème maille impossible.";
		throw Exception (error);
	}	// if (i >= getCellCount ( ))

	try
	{

		switch (_cellsDimension)
		{
			case	0	:
				initialize ( );
				if (0 != _cellsDimension)
					return getCellType (i);
				throw Exception (UTF8String ("Erreur lors de la détermination de la dimension des mailles.", charset));
				break;
			case	2	:
			{
				if ((0 == _surface) && (0 == _mesh.get ( )))
				{
					INTERNAL_ERROR (exc, "Surface nulle pour une dimension 2",
						"LimaQualifSerie::getCellType")
					throw exc;
				}	// if ((0 == _surface) && (0 == _mesh.get ( )))
				_PolygoneInterne*	polygon	= 0 == _surface ?
						_mesh->polygone (i) : _surface->polygone (i);
				CHECK_NULL_PTR_ERROR (polygon)
				switch (polygon->nb_noeuds ( ))
				{
					case	3	: return QualifHelper::TRIANGLE;
					case	4	: return QualifHelper::QUADRANGLE;
					default		:
					{
						UTF8String	error (charset);
						error << "Nombre de noeuds invalide ("
						      << polygon->nb_noeuds ( )
						      << ") pour la " << i
						      << "-ème maille de type polygone.";
						throw Exception (error);
					}	// default
				}	// switch (polygon->nb_noeuds ( ))
			}	// case 2
			case	3	:
			{
				if ((0 == _volume) && (0 == _mesh.get ( )))
				{
					INTERNAL_ERROR (exc, "Volume nul pour une dimension 3",
						"LimaQualifSerie::getCellType")
					throw exc;
				}	// if ((0 == _volume) && (0 == _mesh.get ( )))
				_PolyedreInterne*	polyedron	= 0 == _volume ?
									_mesh->polyedre (i) : _volume->polyedre (i);
				CHECK_NULL_PTR_ERROR (polyedron)
				switch (polyedron->nb_noeuds ( ))
				{
					case	4	: return QualifHelper::TETRAEDRON;
					case	5	: return QualifHelper::PYRAMID;
					case	6	: return QualifHelper::TRIANGULAR_PRISM;
					case	8	: return QualifHelper::HEXAEDRON;
					default		:
					{
						UTF8String	error (charset);
						error << "Nombre de noeuds invalide ("
						      << polyedron->nb_noeuds ( )
						      << ") pour la " << i
						      << "-ème maille de type polyèdre.";
						throw Exception (error);
					}	// default
				}	// switch (polyedron->nb_noeuds ( ))
			}	// case 2
			default		:
			{
				UTF8String	error (charset);
				error << "Dimension non supportée ("
				      << (unsigned long)getDimension ( ) << ").";
				INTERNAL_ERROR (exc, error, "LimaQualifSerie::getCellType")
				throw exc;
			}
		}	// switch (_cellsDimension)
	}
	catch (const erreur& err)
	{
		UTF8String	error (charset);
		error << "Erreur Lima lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << err.what ( );
		throw Exception (error);
	}
	catch (const Exception& exc)
	{
		UTF8String	error (charset);
		error << "Erreur lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << exc.getFullMessage ( );
		throw Exception (error);
	}
	catch (const exception& e)
	{
		UTF8String	error (charset);
		error << "Erreur lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << e.what ( );
		throw Exception (error);
	}
	catch (...)
	{
		UTF8String	error (charset);
		error << "Erreur non documentée lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << ".";
		throw Exception (error);
	}
}	// LimaQualifSerie::getCellType


bool LimaQualifSerie::isVolumic ( ) const
{
	if (0 == _cellsDimension)
		initialize ( );
	return 3 == _cellsDimension ? true : false;
}	// LimaQualifSerie::isVolumic


bool LimaQualifSerie::isThreadable ( ) const
{
	return true;
}	// LimaQualifSerie::isThreadable


void LimaQualifSerie::initialize ( ) const
{
	// Sur la dimension ...
	// Qualif l'utilise d'un point de vue spatial, notamment pour les polygones.
	// Dans certaines cas les calculs ne sont pas les mêmes (ex : jacobien d'un
	// triangle ou quadrangle 2D ou 3D).
	// Ici on n'utilise pas la dimension au mêmes fins, c'est juste pour
	// départager polygones/polyèdres.
	_cellsDimension	= 0 == _surface ? 0 : 2;
	if (0 == _cellsDimension)
	{
		if (0 != _volume)
			_cellsDimension	= 3;
		else if (0 != _mesh.get ( ))
		{
			if (0 != _mesh->nb_polyedres ( ))
				_cellsDimension	= 3;
			else if (0 != _mesh->nb_polygones ( ))
				_cellsDimension	= 2;
		}	// else if (0 != _mesh.get ( ))
	}	// if (0 == _cellsDimension)
}	// LimaQualifSerie::initialize

}	// namespace GQualif


