

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


template<typename TCellType, unsigned char Dim>
GMDSQualifSerie<TCellType, Dim>::GMDSQualifSerie (
		gmds::Mesh& mesh,
		const std::string& name, const std::string& fileName)
: AbstractQualifSerieAdapter (fileName, name, Dim),
  _gmdsCells()
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
		if (Dim == 2 || Dim == 3)
		{
			if (0 != name.size ( ))
				fill(mesh, name);
			else
				mesh.getAll<TCellType>(_gmdsCells);
		}
		else
		{
			TkUtil::UTF8String	error (charset);
			error << "Dimension non supportée (" << (unsigned long)Dim
			      << ") pour l'adaptateur GMDS/Qualif du groupe " << name
			      << " du maillage GMDS issu du fichier " << fileName << ".";
			throw TkUtil::Exception (error);
		}
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
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::GMDSQualifSerie


template<typename TCellType, unsigned char Dim>
GMDSQualifSerie<TCellType, Dim>::GMDSQualifSerie (
	const std::vector<TCellType>& v, const std::string& name,
	const std::string& fileName)
	: AbstractQualifSerieAdapter (fileName, name, Dim),
	  _gmdsCells(v)
{
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::GMDSQualifSerie

template<typename TCellType, unsigned char Dim>
GMDSQualifSerie<TCellType, Dim>::GMDSQualifSerie (const GMDSQualifSerie&)
	: AbstractQualifSerieAdapter ("Invalid file name", "Invalid serie name", 3),
	  _gmdsCells()
{
	assert (0 && "GMDSQualifSerie copy constructor is not allowed.");
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::GMDSQualifSerie

template<typename TCellType, unsigned char Dim>
GMDSQualifSerie<TCellType, Dim>& GMDSQualifSerie<TCellType, Dim>::operator = (const GMDSQualifSerie&)
{
	assert (0 && "GMDSQualifSerie assignment operator is not allowed.");
	return *this;
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::operator =


template<typename TCellType, unsigned char Dim>
GMDSQualifSerie<TCellType, Dim>::~GMDSQualifSerie ( )
{
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::~GMDSQualifSerie


template<typename TCellType, unsigned char Dim>
size_t GMDSQualifSerie<TCellType, Dim>::getCellCount ( ) const
{
	return _gmdsCells.size();
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::getCellCount


template<typename TCellType, unsigned char Dim>
Qualif::Maille& GMDSQualifSerie<TCellType, Dim>::getCell (size_t i) const
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
		size_t s = 0;
		std::vector<gmds::Node>	nodes;

		TCellType cell = getGMDSCell(i);
		gmds::ECellType t = cell.type();
		cell.getAll(nodes);

		switch (t)
		{
			case gmds::GMDS_TRIANGLE	:
				for (s = 0; s < 3; s++)
				{
					gmds::Node node = nodes [s];
					_triangle.Modifier_Sommet (s, node.X(),node.Y(), node.Z());
				}
				return _triangle;
			case gmds::GMDS_QUAD	:
				for (s = 0; s < 4; s++)
				{
					gmds::Node node = nodes [s];
					_quadrangle.Modifier_Sommet (s, node.X(),node.Y(), node.Z());
				}
				return _quadrangle;
			case gmds::GMDS_TETRA	:
				for (s = 0; s < 4; s++)
				{
					gmds::Node node = nodes [s];
					_tetraedron.Modifier_Sommet (s, node.X(),node.Y(), node.Z());
				}
				return _tetraedron;
			case gmds::GMDS_PYRAMID	:
				for (s = 0; s < 5; s++)
				{
					gmds::Node node = nodes [s];
					_pyramid.Modifier_Sommet (s, node.X(),node.Y(), node.Z());
				}
				return _pyramid;
			case gmds::GMDS_PRISM3	:
				for (s = 0; s < 6; s++)
				{
					gmds::Node node = nodes [s];
					_prism.Modifier_Sommet (s, node.X(),node.Y(), node.Z());
				}
				return _prism;
			case gmds::GMDS_HEX	:
				for (s = 0; s < 8; s++)
				{
					gmds::Node node = nodes [s];
					_hexaedron.Modifier_Sommet (s, node.X(),node.Y(), node.Z());
				}
				return _hexaedron;

			default	:
			{
				TkUtil::UTF8String	error (charset);
				error << "Type de maille GMDS invalide ("
						<< (unsigned long)t << " pour la "
						<< i << "-ème maille de type polygone/polyèdre.";
				throw TkUtil::Exception (error);
			}	// default
		}	// switch (t)
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

	INTERNAL_ERROR (exc, "Cas non traité.", "template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::getCell")
	throw exc;
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::getCell


template<typename TCellType, unsigned char Dim>
TCellType GMDSQualifSerie<TCellType, Dim>::getGMDSCell (size_t i) const
{
	return _gmdsCells[i];
}	//  template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::getGMDSFace

template<typename TCellType, unsigned char Dim>
size_t GMDSQualifSerie<TCellType, Dim>::getCellType (size_t i) const
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

		TCellType cell = _gmdsCells[i];
		gmds::ECellType t = cell.type();
		switch (t)
		{
			case gmds::GMDS_TRIANGLE	: return QualifHelper::TRIANGLE;
			case gmds::GMDS_QUAD		: return QualifHelper::QUADRANGLE;
			case gmds::GMDS_TETRA		: return QualifHelper::TETRAEDRON;
			case gmds::GMDS_PYRAMID		: return QualifHelper::PYRAMID;
			case gmds::GMDS_PRISM3		: return QualifHelper::TRIANGULAR_PRISM;
			case gmds::GMDS_HEX			: return QualifHelper::HEXAEDRON;
			default	:
			{
				TkUtil::UTF8String	error (charset);
				error << "Type de maille GMDS invalide ("
					<< (unsigned long)t << " pour la "
					<< i << "-ème maille de type polygone/polyèdre.";
				throw TkUtil::Exception (error);
			}	// default
		}
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

	INTERNAL_ERROR (exc, "Cas non traité.", "template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::getCellType")
	throw exc;
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::getCellType


template<typename TCellType, unsigned char Dim>
bool GMDSQualifSerie<TCellType, Dim>::isVolumic ( ) const
{
	return (Dim > 2);
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::isVolumic


template<typename TCellType, unsigned char Dim>
bool GMDSQualifSerie<TCellType, Dim>::isThreadable ( ) const
{
	return true;
}	// template<typename TCellType, int Dim> GMDSQualifSerie<TCellType>::isThreadable ( )


template<typename TCellType, unsigned char Dim>
void GMDSQualifSerie<TCellType, Dim>::fill(gmds::Mesh& mesh, const std::string& name)
{
	auto group = mesh.getGroup<TCellType>(name);
	_gmdsCells.clear();
	_gmdsCells.resize(group->size());
	for (auto i = 0 ; i < group->size(); i++) {
		_gmdsCells[i] = mesh.get<TCellType>((*group)[i]);
	}
}

template class GMDSQualifSerie<gmds::Face, 2>;  // Explicit instantiation
template class GMDSQualifSerie<gmds::Region, 3>;  // Explicit instantiation