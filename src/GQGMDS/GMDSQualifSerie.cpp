

#include "GQGMDS/GMDSQualifSerie.h"

#include "GQualif/QualifHelper.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>

#include <assert.h>



USING_UTIL
USING_STD
using namespace gmds;
using namespace GQualif;

static const Charset	charset ("àéèùô");


GMDSQualifSerie::GMDSQualifSerie (
		gmds::Mesh& mesh,
		unsigned char dimension,
		const std::string& name, const std::string& fileName)
: AbstractQualifSerieAdapter (fileName, name, dimension),
  _faces (), _regions ()
{
	// Méthodes getL* de Mesh : retournent la maille d'ID local i.
	// La numérotation commence souvent à 0, mais peut commencer à 1 (voire
	// autre chose), et ne pas être contigue.
	// => on utilise  mesh.getAllFaces et  mesh.getAllRegions (GMDS v 0.2.33)
	// qui stockent dans un vecteur les mailles (inconvénient) mais permettent
	// d'avoir toutes les mailles du maillage indépendamment de leur id, et
	// sans itérateur (avantage).
	try
	{
	switch (dimension)
	{
		case	2	:
			if (0 != name.size ( ))
				fillFaces(mesh, name);
			else
				mesh.getAll<Face> (_faces);
			break;
		case	3	:
			if (0 != name.size ( ))
				fillRegions(mesh, name);
			else
				mesh.getAll<Region> (_regions);
			break;
		default		:
		{
			TkUtil::UTF8String	error (charset);
			error << "Dimension non supportée (" << (unsigned long)dimension
			      << ") pour l'adaptateur GMDS/Qualif du groupe " << name
			      << " du maillage GMDS issu du fichier " << fileName << ".";
			throw TkUtil::Exception (error);
		}
	}	// switch (dimension)

	}
	catch (const gmds::GMDSException& gexc)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur GMDS lors de la construction de l'adaptateur "
		      << "GMDS/Qualif du groupe " << name
		      << " du maillage GMDS issu du fichier " << fileName << " :\n"
		      << gexc.what ( );
		throw TkUtil::Exception (error);
	}
	catch (const TkUtil::Exception& exc)
	{
		throw;
	}
	catch (const std::exception& e)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur lors de la construction de l'adaptateur "
		      << "GMDS/Qualif du groupe " << name
		      << " du maillage GMDS issu du fichier " << fileName << " :\n"
		      << e.what ( );
		throw TkUtil::Exception (error);
	}
	catch (...)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur non documentée lors de la construction de "
		      << "l'adaptateur GMDS/Qualif du groupe " << name
		      << " du maillage GMDS issu du fichier " << fileName << ".";
		throw TkUtil::Exception (error);
	}
}	// GMDSQualifSerie::GMDSQualifSerie


GMDSQualifSerie::GMDSQualifSerie (
	const std::vector<gmds::Face>& s, const std::string& name,
	const std::string& fileName)
	: AbstractQualifSerieAdapter (fileName, name, 2),
	  _faces (s), _regions ()
{
}	// GMDSQualifSerie::GMDSQualifSerie


GMDSQualifSerie::GMDSQualifSerie (
	const std::vector<gmds::Region>& v, const std::string& name, 
	const std::string& fileName)
	: AbstractQualifSerieAdapter (fileName, name, 3),
	  _faces (), _regions (v)
{
}	// GMDSQualifSerie::GMDSQualifSerie


GMDSQualifSerie::GMDSQualifSerie (const GMDSQualifSerie&)
	: AbstractQualifSerieAdapter ("Invalid file name", "Invalid serie name", 3),
	  _faces (), _regions ()
{
	assert (0 && "GMDSQualifSerie copy constructor is not allowed.");
}	// GMDSQualifSerie::GMDSQualifSerie


GMDSQualifSerie& GMDSQualifSerie::operator = (
												const GMDSQualifSerie&)
{
	assert (0 && "GMDSQualifSerie assignment operator is not allowed.");
	return *this;
}	// GMDSQualifSerie::operator =


GMDSQualifSerie::~GMDSQualifSerie ( )
{
}	// GMDSQualifSerie::~GMDSQualifSerie


size_t  GMDSQualifSerie::getCellCount ( ) const
{
	switch (getDimension ( ))
	{
		case	2	:
			return _faces.size ( );
		case	3	:
			return _regions.size ( );
	}	// switch (getDimension ( ))

	INTERNAL_ERROR (exc, "Absence de groupe de mailles et de maillage.", "GMDSQualifSerie::getCellCount")
	throw exc;
}	// GMDSQualifSerie::getCellCount


Qualif::Maille& GMDSQualifSerie::getCell (size_t i) const
{
	if (i >= getCellCount ( ))
	{
		TkUtil::UTF8String	error (charset);
		error << "Le groupe de données ne contient que " << getCellCount ( )
		      << " mailles. Accès à la " << i << "-ème maille impossible.";
		throw TkUtil::Exception (error);
	}	// if (i >= getCellCount ( ))

	try
	{

		int							dimension	= getDimension ( );
		size_t						s	= 0;
		std::vector<gmds::Node>	nodes;
		switch (dimension)
		{
			case	2	:
			{
/*				if ((0 == _surface) && (0 == _mesh))
				{
					INTERNAL_ERROR (exc, "Surface nulle pour une dimension 2",
								" GMDSQualifSerie::getCell")
					throw exc;
				}	// if ((0 == _surface) && (0 == _mesh))
				gmds::Face*	face	=
					0 == _surface ? _faces [i] : (*_surface) [i];
				CHECK_NULL_PTR_ERROR (face) */
				gmds::Face	face	= getGMDSFace (i);
				face.getAll<Node>(nodes);

				switch (face.type())
				{
					case gmds::GMDS_TRIANGLE	:
						for (s = 0; s < 3; s++)
						{
							gmds::Node	node	= nodes [s];
							_triangle.Modifier_Sommet (s,
										node.X(),node.Y(), node.Z());
						}	// for (s = 0; s < 3; s++)
						return _triangle;
					case gmds::GMDS_QUAD	:
						for (s = 0; s < 4; s++)
						{
							gmds::Node	node	= nodes [s];

							_quadrangle.Modifier_Sommet (s,
									node.X(),node.Y(), node.Z());
						}	// for (s = 0; s < 4; s++)
						return _quadrangle;
					default	:
					{
						TkUtil::UTF8String	error (charset);
						error << "Type de maille GMDS invalide ("
						      << (unsigned long)face.type() << " pour la "
						      << i << "-ème maille de type polygone.";
						throw TkUtil::Exception (error);
					}	// default
				}	// switch (face.type())
			}	// case 2
			break;
			case 3	:
			{
/*				if ((0 == _volume) && (0 == _mesh))
				{
					INTERNAL_ERROR (exc, "Volume nul pour une dimension 3",
								" GMDSQualifSerie::getCell")
					throw exc;
				}	// if ((0 == _volume) && (0 == _mesh))

				gmds::Region*	cell	= 
					0 == _volume ? _regions [i] : (*_volume) [i];
				CHECK_NULL_PTR_ERROR (cell)
*/
				gmds::Region	cell	= getGMDSRegion (i);
				cell.getAll<Node>(nodes);

				switch (cell.type())
				{
					case gmds::GMDS_TETRA	:
						for (s = 0; s < 4; s++)
						{
							gmds::Node	node	= nodes [s];

							_tetraedron.Modifier_Sommet (s,
									node.X(),node.Y(), node.Z());
						}	// for (s = 0; s < 4; s++)
						return _tetraedron;
					case gmds::GMDS_PYRAMID	:
						for (s = 0; s < 5; s++)
						{
							gmds::Node	node	= nodes [s];

							_pyramid.Modifier_Sommet (s,
									node.X(),node.Y(), node.Z());
						}	// for (s = 0; s < 5; s++)
						return _pyramid;
					case gmds::GMDS_PRISM3	:
						for (s = 0; s < 6; s++)
						{
							gmds::Node	node	= nodes [s];

							_prism.Modifier_Sommet (s,
									node.X(),node.Y(), node.Z());
						}	// for (s = 0; s < 6; s++)
						return _prism;
					case gmds::GMDS_HEX	:
						for (s = 0; s < 8; s++)
						{
							gmds::Node	node	= nodes [s];

							_hexaedron.Modifier_Sommet (s,
									node.X(),node.Y(), node.Z());
						}	// for (s = 0; s < 8; s++)
						return _hexaedron;
					default	:
					{
						TkUtil::UTF8String	error (charset);
						error << "Type de maille GMDS invalide ("
						      << (unsigned long)cell.type() << " pour la "
						      << i << "-ème maille de type polyèdre.";
						throw TkUtil::Exception (error);
					}	// default
				}	// switch (cell.getType ( ))
			}
			break;
			default	:
			{
				TkUtil::UTF8String	error (charset);
				error << "Dimension non supportée ("
				      << (unsigned long)getDimension ( ) << ").";
				INTERNAL_ERROR (exc, error, "GMDSQualifSerie::getCell")
				throw exc;
			}	// default (switch (dimension))
		}	// switch (dimension)
	}
	catch (const TkUtil::Exception& exc)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << exc.getFullMessage ( );
		throw TkUtil::Exception (error);
	}	// catch (const TkUtil::Exception& exc)
	catch (const gmds::GMDSException& gexc)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur GMDS lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << gexc.what ( );
		throw TkUtil::Exception (error);
	}	// catch (const gmds::GMDSException& gexc)
	catch (const std::exception& e)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << e.what ( );
		throw TkUtil::Exception (error);
	}	// catch (const std::exception& e)
	catch (...)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur non documentée lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << ".";
		throw TkUtil::Exception (error);
	}	// catch (...)

	INTERNAL_ERROR (exc, "Cas non traité.", "GMDSQualifSerie::getCell")
	throw exc;
}	// GMDSQualifSerie::getCell


Face GMDSQualifSerie::getGMDSFace (size_t i) const
{
	return _faces[i];
}	//  GMDSQualifSerie::getGMDSFace


Region GMDSQualifSerie::getGMDSRegion (size_t i) const
{
	return _regions[i];
}	//  GMDSQualifSerie::getGMDSRegion


size_t GMDSQualifSerie::getCellType (size_t i) const
{
	if (i >= getCellCount ( ))
	{
		TkUtil::UTF8String	error (charset);
		error << "Le groupe de données ne contient que " << getCellCount ( )
		      << " mailles. Accès à la " << i << "-ème maille impossible.";
		throw TkUtil::Exception (error);
	}	// if (i >= getCellCount ( ))

	try
	{

		int							dimension	= getDimension ( );
		switch (dimension)
		{
			case	2	:
			{
				gmds::Face face = _faces[i];

				switch (face.type())
				{
					case gmds::GMDS_TRIANGLE	: return QualifHelper::TRIANGLE;
					case gmds::GMDS_QUAD		: return QualifHelper::QUADRANGLE;
					default	:
					{
						TkUtil::UTF8String	error (charset);
						error << "Type de maille GMDS invalide ("
						      << (unsigned long)face.type() << " pour la "
						      << i << "-ème maille de type polygone.";
						throw TkUtil::Exception (error);
					}	// default
				}	// switch (face->getType ( ))
			}	// case 2
			break;
			case 3	:
			{	gmds::Region cell =  _regions[i];

				switch (cell.type())
				{
					case gmds::GMDS_TETRA	: return QualifHelper::TETRAEDRON;
					case gmds::GMDS_PYRAMID	: return QualifHelper::PYRAMID;
					case gmds::GMDS_PRISM3	: return QualifHelper::TRIANGULAR_PRISM;
					case gmds::GMDS_HEX		: return QualifHelper::HEXAEDRON;
					default	:
					{
						TkUtil::UTF8String	error (charset);
						error << "Type de maille GMDS invalide ("
						      << (unsigned long)cell.type() << " pour la "
						      << i << "-ème maille de type polyèdre.";
						throw TkUtil::Exception (error);
					}	// default
				}	// switch (cell->getType ( ))
			}
			break;
			default	:
			{
				TkUtil::UTF8String	error (charset);
				error << "Dimension non supportée ("
				      << (unsigned long)getDimension ( ) << ").";
				INTERNAL_ERROR (exc, error, "GMDSQualifSerie::getCellType")
				throw exc;
			}	// default (switch (dimension))
		}	// switch (dimension)
	}
	catch (const TkUtil::Exception& exc)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << exc.getFullMessage ( );
		throw TkUtil::Exception (error);
	}	// catch (const TkUtil::Exception& exc)
	catch (const gmds::GMDSException& gexc)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur GMDS lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << gexc.what ( );
		throw TkUtil::Exception (error);
	}	// catch (const gmds::GMDSException& gexc)
	catch (const std::exception& e)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << " : "
		      << e.what ( );
		throw TkUtil::Exception (error);
	}	// catch (const std::exception& e)
	catch (...)
	{
		TkUtil::UTF8String	error (charset);
		error << "Erreur non documentée lors de la récupération de la " << i
		      <<"-ème maille du groupe " << getName ( )
		      << " du maillage du fichier " << getFileName ( ) << ".";
		throw TkUtil::Exception (error);
	}	// catch (...)

	INTERNAL_ERROR (exc, "Cas non traité.", "GMDSQualifSerie::getCellType")
	throw exc;
}	// GMDSQualifSerie::getCellType


bool GMDSQualifSerie::isVolumic ( ) const
{
	return (0 != _regions.size());
}	// GMDSQualifSerie::isVolumic


bool GMDSQualifSerie::isThreadable ( ) const
{
	return true;
}	// GMDSQualifSerie::isThreadable ( )

void GMDSQualifSerie::fillFaces(gmds::Mesh& mesh, const std::string& name)
{
	auto group = mesh.getGroup<gmds::Face>(name);
	_faces.clear();
	_faces.resize(group->size());
	for (auto i = 0 ; i < group->size(); i++) {
		_faces[i] = mesh.get<gmds::Face>((*group)[i]);
	}
}

void GMDSQualifSerie::fillRegions(gmds::Mesh& mesh, const std::string& name)
{
	auto group = mesh.getGroup<gmds::Region>(name);
	_regions.clear();
	_regions.resize(group->size());
	for (auto i = 0; i < group->size() ; i++) {
		_regions[i] = mesh.get<gmds::Region>((*group)[i]);
	}
}
