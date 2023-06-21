#include <vtkConfigure.h>	// VTK_MAJOR_VERSION ...

#include "GQVtk/VtkQualifSerie.h"
#include "GQualif/QualifHelper.h"

#include <TkUtil/Exception.h>
#include <TkUtil/File.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>

#include <vtkCellType.h>
#include <vtkUnstructuredGridReader.h>

/*#ifdef VTK_8
#include <vtkAutoInit.h>
#include <vtkIOExportModule.h>
VTK_MODULE_INIT(vtkIOExportModule)
#endif	// VTK_8*/

#include <iostream>
#include <assert.h>


USING_UTIL
USING_STD
using namespace Qualif;

static const Charset	charset ("àéèùô");

namespace GQualif
{

VtkQualifSerie::VtkQualifSerie (const string& fileName, const string& name)
	: AbstractQualifSerieAdapter (fileName, name, 3),
	  _mesh (0), _cell (0)
{
	try
	{
		File	file (getFileName ( ));
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
		 vtkUnstructuredGridReader*	reader	= vtkUnstructuredGridReader::New( );
		CHECK_NULL_PTR_ERROR (reader)
		reader->SetFileName (fileName.c_str ( ));
		if (0 == reader->IsFileUnstructuredGrid ( ))
		{
			UTF8String	message (charset);
			message << "Le fichier " << fileName
			        << " ne contient pas un maillage VTK de type supporté."
			        << "\n" << "Types supportés : "
			        << "vtkUnstructuredGrid";
			throw Exception (message);
		}	// if (0 == reader->IsFileUnstructuredGrid ( ))
		_mesh	= reader->GetOutput ( );
		CHECK_NULL_PTR_ERROR (_mesh)
		_mesh->Register (0);
#ifdef VTK_5
		_mesh->Update ( );	// On s'assure que le pipeline est exécuté
#endif	// VTK_5
		reader->Delete ( );		reader	= 0;

		_cell	= vtkGenericCell::New ( );
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
}	// VtkQualifSerie::VtkQualifSerie


VtkQualifSerie::VtkQualifSerie (
		vtkUnstructuredGrid& mesh, const string& name, const string& fileName)
	: AbstractQualifSerieAdapter (fileName, name, 3),
	  _mesh (&mesh), _cell (0)
{
	if (0 != _mesh)
	{
		_mesh->Register (0);
#ifdef VTK_5
		_mesh->Update ( );	// On s'assure que le pipeline est exécuté
#endif	// VTK_5
	}	// if (0 != _mesh)

	_cell	= vtkGenericCell::New ( );
}	// VtkQualifSerie::VtkQualifSerie


VtkQualifSerie::VtkQualifSerie (const VtkQualifSerie&)
	: AbstractQualifSerieAdapter (
						"Invalid file name", "Invalid name", (unsigned char)-1),
	  _mesh (0), _cell (0)
{
	assert (0 && "VtkQualifSerie copy constructor is not allowed.");
}	// VtkQualifSerie::VtkQualifSerie


VtkQualifSerie& VtkQualifSerie::operator = (const VtkQualifSerie&)
{
	assert (0 && "VtkQualifSerie assignment operator is not allowed.");
	return *this;
}	// VtkQualifSerie::operator =


VtkQualifSerie::~VtkQualifSerie ( )
{
	if (0 != _mesh)
		_mesh->UnRegister (0);
	_mesh	= 0;
	if (0 != _cell)
		_cell->Delete ( );
	_cell	= 0;
}	// VtkQualifSerie::~VtkQualifSerie


size_t VtkQualifSerie::getCellCount ( ) const
{
	if (0 == _mesh)
	{
		INTERNAL_ERROR (exc, "Absence de groupe de mailles.", "VtkQualifSerie::getCellCount")
		throw exc;
	}	// if (0 == _mesh)

	return _mesh->GetNumberOfCells ( );
}	// VtkQualifSerie::getCellCount


Qualif::Maille& VtkQualifSerie::getCell (size_t i) const
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
		CHECK_NULL_PTR_ERROR (_mesh)
		CHECK_NULL_PTR_ERROR (_cell)
		_mesh->GetCell (i, _cell);
//		vtkCell*	cell	= _mesh->GetCell (i);	// CP : Not thread safe
		CHECK_NULL_PTR_ERROR (_cell->GetPoints ( ))
#if ((VTK_MAJOR_VERSION > 5) || (VTK_MINOR_VERSION >= 10))
		VTKCellType	type	= (VTKCellType)_cell->GetCellType ( );
#else	// #if ((VTK_MAJOR_VERSION > 5) || (VTK_MINOR_VERSION >= 10))
		int	type	= (int)_cell->GetCellType ( );
#endif	// #if ((VTK_MAJOR_VERSION > 5) || (VTK_MINOR_VERSION >= 10))

		if (VTK_POLYGON == type)
		{
			switch (_cell->GetPoints ( )->GetNumberOfPoints ( ))
			{
				case	3	: type	= VTK_TRIANGLE;	break;
				case	4	: type	= VTK_QUAD;		break;
			}	// switch (_cell->GetPoints ( )->GetNumberOfPoints ( ))
		}	// if (VTK_POLYGON == type)

		double	coords [3];
		switch (type)
		{
			case VTK_TRIANGLE		:
			case VTK_TRIANGLE_STRIP	:
				for (size_t s = 0; s < 3; s++)
				{
					_cell->GetPoints ( )->GetPoint (s, coords);
					_triangle.Modifier_Sommet (
							s, coords [0], coords [1], coords [2]);
				}	// for (size_t s = 0; s < 3; s++)
				return _triangle;
			case VTK_QUAD			:
				for (size_t s = 0; s < 4; s++)
				{
					_cell->GetPoints ( )->GetPoint (s, coords);
					_quadrangle.Modifier_Sommet (
							s, coords [0], coords [1], coords [2]);
				}	// for (size_t s = 0; s < 4; s++)
				return _quadrangle;
			case VTK_TETRA			:
				for (size_t s = 0; s < 4; s++)
				{
					_cell->GetPoints ( )->GetPoint (s, coords);
					_tetraedron.Modifier_Sommet (
							s, coords [0], coords [1], coords [2]);
				}	// for (size_t s = 0; s < 3; s++)
				return _tetraedron;
			case VTK_PYRAMID		:
				for (size_t s = 0; s < 5; s++)
				{
					_cell->GetPoints ( )->GetPoint (s, coords);
					_pyramid.Modifier_Sommet (
							s, coords [0], coords [1], coords [2]);
				}	// for (size_t s = 0; s < 5; s++)
				return _pyramid;
			case VTK_WEDGE			:
				for (size_t s = 0; s < 6; s++)
				{
					_cell->GetPoints ( )->GetPoint (s, coords);
					_prism.Modifier_Sommet (
							s, coords [0], coords [1], coords [2]);
				}	// for (size_t s = 0; s < 6; s++)
				return _prism;
			case VTK_HEXAHEDRON		:
				for (size_t s = 0; s < 8; s++)
				{
					_cell->GetPoints ( )->GetPoint (s, coords);
					_hexaedron.Modifier_Sommet (
							s, coords [0], coords [1], coords [2]);
				}	// for (size_t s = 0; s < 8; s++)
				return _hexaedron;
			default		:
			{
				UTF8String	error (charset);
				error << "Type de maille non supporté ("
				      << (unsigned long)_cell->GetCellType ( ) << ").";
				INTERNAL_ERROR (exc, error, "VtkQualifSerie::getCell")
				throw exc;
			}
		}	// switch (type)
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
}	// VtkQualifSerie::getCell


size_t VtkQualifSerie::getCellType (size_t i) const
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

		CHECK_NULL_PTR_ERROR (_cell)
		CHECK_NULL_PTR_ERROR (_mesh)
		_mesh->GetCell (i, _cell);
//		vtkCell*	cell	= _mesh->GetCell (i);	// CP : Not thread safe
		CHECK_NULL_PTR_ERROR (_cell->GetPoints ( ))
#if ((VTK_MAJOR_VERSION > 5) || (VTK_MINOR_VERSION >= 10))
		VTKCellType	type	= (VTKCellType)_cell->GetCellType ( );
#else	// #if ((VTK_MAJOR_VERSION > 5) || (VTK_MINOR_VERSION >= 10))
		int	type	= (int)_cell->GetCellType ( );
#endif	// #if ((VTK_MAJOR_VERSION > 5) || (VTK_MINOR_VERSION >= 10))

		if (VTK_POLYGON == type)
		{
			switch (_cell->GetPoints ( )->GetNumberOfPoints ( ))
			{
				case	3	: type	= VTK_TRIANGLE;	break;
				case	4	: type	= VTK_QUAD;		break;
			}	// switch (_cell->GetPoints ( )->GetNumberOfPoints ( ))
		}	// if (VTK_POLYGON == type)

		switch (type)
		{
			case VTK_TRIANGLE		:
			case VTK_TRIANGLE_STRIP	: return QualifHelper::TRIANGLE;
			case VTK_QUAD			: return QualifHelper::QUADRANGLE;
			case VTK_TETRA			: return QualifHelper::TETRAEDRON;
			case VTK_PYRAMID		: return QualifHelper::PYRAMID;
			case VTK_WEDGE			: return QualifHelper::TRIANGULAR_PRISM;
			case VTK_HEXAHEDRON		: return QualifHelper::HEXAEDRON;
			default		:
			{
				UTF8String	error (charset);
				error << "Type de maille non supporté ("
				      << (unsigned long)_cell->GetCellType ( ) << ").";
				INTERNAL_ERROR (exc, error, "VtkQualifSerie::getCellType")
				throw exc;
			}
		}	// switch (type)
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
}	// VtkQualifSerie::getCellType


bool VtkQualifSerie::isVolumic ( ) const
{
	try
	{
		const size_t	cellType	= getCellType (0);
		if ((cellType == QualifHelper::TETRAEDRON)       ||
		    (cellType == QualifHelper::PYRAMID)          ||
		    (cellType == QualifHelper::TRIANGULAR_PRISM) ||
		    (cellType == QualifHelper::HEXAEDRON))
			return true;
	}
	catch (...)
	{
	}

	return false;
}	// VtkQualifSerie::isVolumic


bool VtkQualifSerie::isThreadable ( ) const
{
	return true;
}	// VtkQualifSerie::isThreadable ( )



}	// namespace GQualif


