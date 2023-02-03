#include "QtCalQualMainWindow.h"
#include "QtQualif/QtSeriesChoiceDialog.h"
#include "GQLima/LimaQualifSerie.h"
#include "GQVtk/VtkQualifSerie.h"
#ifdef USE_GMDS
#include "GQGMDS/GMDSQualifSerie.h"
#endif	// USE_GMDS
#include <QwtCharts/QwtChartsManager.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <TkUtil/Exception.h>
#include <TkUtil/File.h>
#include <TkUtil/InternalError.h>

#include <QApplication>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMenuBar>
#include <QLayout>

#include <Lima/enum.h>
#include <LimaP/reader.h>
#ifdef USE_GMDS
// TODO F.Ledoux#include <GMDSIO/CubitFacetReader.h>
#include <GMDS/IO/MeditReader.h>
#include <GMDS/IO/VTKReader.h>
#endif	// USE_GMDS

#include <memory>

using namespace std;
using namespace TkUtil;
using namespace GQualif;
using namespace Lima;
#ifdef USE_GMDS
using namespace gmds;
#endif	// USE_GMDS

static const Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION


namespace GQualif
{

// ============================================================================
//                            FONCTIONS STATIQUES
// ============================================================================



// ============================================================================
//                       LA CLASSE QtMeshAnalysisView
// ============================================================================

#ifdef USE_GMDS
const int QtCalQualMainWindow::gmdsMask	=
				DIM3|N|E|F|R|E2N|F2N|R2N|F2E|R2F|F2R|N2F|N2E|E2F|N2R|E2R|R2E; 
const int readersMask	= N|F|R;

// Instanciations explicites :
//template class GMDSQualifSerie< QtCalQualMainWindow::gmdsMask >;
//template class QtGMDSMeshAnalysisView< QtCalQualMainWindow::gmdsMask >;

#endif	// USE_GMDS

QtMeshAnalysisView::QtMeshAnalysisView (QMdiArea* parent, const string& appTitle)
	: QMdiSubWindow (parent), _analysisWidget (0), _contextMenu (0)
{
	_analysisWidget	= new QtQualifWidget (this, appTitle);
	_analysisWidget->setDomainType (QtQualifWidget::COMPUTED_DOMAIN);
	layout ( )->addWidget (_analysisWidget);
}	// QtMeshAnalysisView::QtMeshAnalysisView


QtMeshAnalysisView::QtMeshAnalysisView (const QtMeshAnalysisView&)
{
	assert (0 && "QtMeshAnalysisView copy constructor is not allowed.");
}	// QtMeshAnalysisView::QtMeshAnalysisView


QtMeshAnalysisView& QtMeshAnalysisView::operator = (const QtMeshAnalysisView&)
{
	assert (0 && "QtMeshAnalysisView assignment operator is not allowed.");
	return *this;
}	// QtMeshAnalysisView::operator =


QtMeshAnalysisView::~QtMeshAnalysisView ( )
{
}	// QtMeshAnalysisView::~QtMeshAnalysisView


void QtMeshAnalysisView::setContextMenu (QMenu* menu)
{
	_contextMenu	= menu;
}	// QtMeshAnalysisView::setContextMenu


void QtMeshAnalysisView::contextMenuEvent (QContextMenuEvent* event)
{
	if (0 != _contextMenu)
		_contextMenu->popup (event->globalPos ( ));
}	// QtMeshAnalysisView::contextMenuEvent


// ============================================================================
//                       LA CLASSE QtLimaMeshAnalysisView
// ============================================================================

QtLimaMeshAnalysisView::QtLimaMeshAnalysisView (
		QMdiArea* parent, const string& appTitle, const string& fileName)
	: QtMeshAnalysisView (parent, appTitle), _mesh ( )
{
	try
	{
		_mesh.reset (new _MaillageInterne ( ));
		_mesh->lire (fileName);
		vector < pair <string, unsigned char> >	groups;
		size_t	g	= 0;
		for (g = 0; g < _mesh->nb_volumes ( ); g++)
			 groups.push_back (
					pair<string, unsigned char>(_mesh->volume (g)->nom( ), 3));
		for (g = 0; g < _mesh->nb_surfaces ( ); g++)
			 groups.push_back (
					pair<string, unsigned char>(_mesh->surface (g)->nom( ), 2));
		if (0 != groups.size ( ))
		{
			QtSeriesChoiceDialog	dialog (this, appTitle, groups);
			dialog.exec ( );
			if (QDialog::Accepted != dialog.result ( ))
				throw Exception (UTF8String ("Opération annulée par l'utilisateur.", charset));

			groups	= dialog.getGroups ( );
		}
		else if ((0 != _mesh->nb_polygones( )) && (0 == _mesh->nb_polyedres( )))
			throw Exception (UTF8String ("Absence de mailles de dimension 2 et 3.", charset));

		LimaQualifSerie*	serie	= 0;
		size_t				types	= 0;
		if (0 != groups.size ( ))
		{
			for (vector < pair <string, unsigned char> >::const_iterator it =
					groups.begin ( ); groups.end ( ) != it; it++)
			{
				switch ((*it).second)
				{
					case	2	:
					{
						_SurfaceInterne*	s	= _mesh->surface ((*it).first);
						serie	= new LimaQualifSerie (
											*s, _mesh->dimension ( ), fileName);
					}	// case    2
					break;
					case	3	:
					{
						_VolumeInterne*	v	= _mesh->volume ((*it).first);
						serie	= new LimaQualifSerie (*v, fileName);
					}	// case    3
					break;
					default		:
					{
						UTF8String	message (charset);
						message << "Dimension inattendue ("
						        << (unsigned long)(*it).second
						        << " pour le groupe de mailles "
						        << (*it).first << " du maillage du fichier "
						        << fileName << ".";
						INTERNAL_ERROR (exc, message,
							"QtLimaMeshAnalysisView::QtLimaMeshAnalysisView")
						throw exc;
					}
				}	// switch ((*it).second)

				types	|= serie->getDataTypes ( );
				getAnalysisPanel ( ).addSerie (serie);
			}	// for (vector < pair <string, unsigned char> >::const_iterator
		}	// if (0 != groups.size ( ))
		else	// => On prend les mailles 2D ou 3D du maillage
		{
			unsigned char	dimension	= 0;
			if (0 != _mesh->nb_polyedres ( ))
				dimension	= 3;
			else if (0 != _mesh->nb_polygones ( ))
				dimension	= 2;
			if (0 == dimension)
				throw Exception (
							UTF8String ("Absence de mailles 2D ou 3D dans le maillage.", charset));
			serie	= new LimaQualifSerie (
							*(_mesh.get ( )), false, dimension, "", fileName);
			types	|= serie->getDataTypes ( );

			getAnalysisPanel ( ).addSerie (serie);
		}	// else if (0 != groups.size ( ))

		getAnalysisPanel ( ).selectQualifiedTypes (types);
//									getAnalysisPanel ( ).getQualifiedTypes ( ));
	}
	catch (const Exception& exc)
	{
		UTF8String	message (charset);
		message << "Impossibilité de charger le maillage Lima contenu dans le "
		        << "fichier " << fileName << " :"
		        << "\n" << exc.getFullMessage ( );
		throw Exception (message);
	}
	catch (const erreur& err)
	{
		UTF8String	message (charset);
		message << "Impossibilité de charger le maillage Lima contenu dans le "
		        << "fichier " << fileName << " :"
		        << "\n" << err.what ( );
		throw Exception (message);
	}
	catch (const exception& exc)
	{
		UTF8String	message (charset);
		message << "Impossibilité de charger le maillage Lima contenu dans le "
		        << "fichier " << fileName << " :"
		        << "\n" << exc.what ( );
		throw Exception (message);
	}
	catch (...)
	{
		UTF8String	message (charset);
		message << "Impossibilité de charger le maillage Lima contenu dans le "
		        << "fichier " << fileName << " : erreur non documentée.";
		throw Exception (message);
	}
}	// QtLimaMeshAnalysisView::QtLimaMeshAnalysisView


QtLimaMeshAnalysisView::QtLimaMeshAnalysisView (const QtLimaMeshAnalysisView&)
	: QtMeshAnalysisView (0, "Invalid application"), _mesh ( )
{
	assert (0 && "QtLimaMeshAnalysisView copy constructor is not allowed.");
}	// QtLimaMeshAnalysisView::QtMeshAnalysisView


QtLimaMeshAnalysisView& QtLimaMeshAnalysisView::operator = (
												const QtLimaMeshAnalysisView&)
{
	assert (0 && "QtLimaMeshAnalysisView assignment operator is not allowed.");
	return *this;
}	// QtLimaMeshAnalysisView::operator =


QtLimaMeshAnalysisView::~QtLimaMeshAnalysisView ( )
{
}	// QtLimaMeshAnalysisView::~QtLimaMeshAnalysisView


#ifdef USE_GMDS

// ============================================================================
//                       LA CLASSE QtGMDSMeshAnalysisView
// ============================================================================

void loadMesh (	gmds::IGMesh& mesh, const string& fileName)
{
	TkUtil::File	file (fileName);
	if ((0 == strcasecmp (file.getExtension ( ).c_str ( ), "mesh")) ||
	    (0 == strcasecmp (file.getExtension ( ).c_str ( ), "med")))
	{
		gmds::MeditReader<gmds::IGMesh>	reader (mesh);
		reader.read (fileName, readersMask);
	}	// mesh med
	else if ((0 == strcasecmp (file.getExtension ( ).c_str ( ), "vtu")) ||
	         (0 == strcasecmp (file.getExtension ( ).c_str ( ), "vtp")))
	{
		gmds::VTKReader<gmds::IGMesh>	reader (mesh);
		reader.read (fileName);
	}	// vtu vtp
	else if (0 == strcasecmp (file.getExtension ( ).c_str ( ), "fac"))
	{
// TODO ADD F. LEDOUX
//		gmds::CubitFacetReader<gmds::IGMesh>	reader (mesh);
//		reader.read (fileName);
		UTF8String	error (charset);
		error << "Le fichier " << fileName << " n'est pas lisible par GMDS.";
		throw Exception (error);
	}	// fac
	else
	{
		UTF8String	error (charset);
		error << "Le fichier " << fileName << " n'est pas lisible par GMDS.";
		throw Exception (error);
	}
}	// loadMesh


QtGMDSMeshAnalysisView::QtGMDSMeshAnalysisView (
		QMdiArea* parent, const string& appTitle, const string& fileName)
	: QtMeshAnalysisView (parent, appTitle), _mesh ( )
{
	try
	{
		GMDSQualifSerie*	serie	= 0;

		_mesh.reset (new gmds::IGMesh(QtCalQualMainWindow::gmdsMask));
		loadMesh (*_mesh.get ( ), fileName);
		vector < pair <string, unsigned char> >	groups;
		for (gmds::IGMesh::surfaces_iterator its =
			_mesh->surfaces_begin ( ); _mesh->surfaces_end ( ) != its; its++)
			 groups.push_back (pair<string, unsigned char>((*its).name ( ), 2));
		for (gmds::IGMesh::volumes_iterator itv =
			_mesh->volumes_begin ( ); _mesh->volumes_end ( ) != itv; itv++)
			 groups.push_back (pair<string, unsigned char>((*itv).name ( ), 3));
		if (0 != groups.size ( ))
		{
			QtSeriesChoiceDialog	dialog (this, appTitle, groups);
			dialog.exec ( );
			if (QDialog::Accepted != dialog.result ( ))
				throw Exception (UTF8String ("Opération annulée par l'utilisateur.", charset));

			groups	= dialog.getGroups ( );
		}	// if (0 != groups.size ( ))
		else if ((0 == _mesh->getNbRegions ( )) && (0 == _mesh->getNbFaces ( )))
			throw Exception (UTF8String ("Absence de mailles de dimension 2 et 3.", charset));

		if (0 != groups.size ( ))
		{
			for (vector < pair <string, unsigned char> >::const_iterator it =
					groups.begin ( ); groups.end ( ) != it; it++)
			{
				switch ((*it).second)
				{
					case	2	:
					{
						IGMesh::surface& s = _mesh->getSurface ((*it).first);
						serie	= new GMDSQualifSerie (s, (*it).first, fileName);
					}	// case    2
					break;
					case	3	:
					{
						IGMesh::volume&	v = _mesh->getVolume ((*it).first);
						serie	= new GMDSQualifSerie(v, (*it).first, fileName);
					}	// case    3
					break;
					default		:
					{
						UTF8String	message (charset);
						message << "Dimension inattendue ("
						        << (unsigned long)(*it).second
						        << " pour le groupe de mailles "
						        << (*it).first << " du maillage du fichier "
						        << fileName << ".";
						INTERNAL_ERROR (exc, message,
							"QtGMDSMeshAnalysisView::QtGMDSMeshAnalysisView")
						throw exc;
					}
				}	// switch ((*it).second)

				getAnalysisPanel ( ).addSerie (serie);
			}	// for (vector < pair <string, unsigned char> >:: ...
		}	// if (0 != groups.size ( ))
		else	// => On prend les mailles 2D ou 3D du maillage
		{
			unsigned char	dimension	= 0;
			if (0 != _mesh->getNbRegions ( ))
			{
				dimension	= 3;
			}	// if (0 != _mesh->getNbRegions ( ))
			else if (0 != _mesh->getNbFaces ( ))
			{
				dimension	= 2;
			}	// else if (0 != _mesh->getNbFaces ( ))
			if (0 == dimension)
				throw Exception (
							UTF8String ("Absence de mailles 2D ou 3D dans le maillage.", charset));
			serie	= new GMDSQualifSerie(
							*(_mesh.get ( )), false, dimension, "", fileName);
			getAnalysisPanel ( ).addSerie (serie);
		}	// else if (0 != groups.size ( ))

		getAnalysisPanel ( ).selectQualifiedTypes (
									getAnalysisPanel ( ).getQualifiedTypes ( ));
	}
	catch (const Exception& exc)
	{
		UTF8String	message (charset);
		message << "Impossibilité de charger le maillage GMDS contenu dans le "
		        << "fichier " << fileName << " :"
		        << "\n" << exc.getFullMessage ( );
		throw Exception (message);
	}
	catch (const gmds::GMDSException& gexc)
	{
		UTF8String	message (charset);
		message << "Impossibilité de charger le maillage GMDS contenu dans le "
		        << "fichier " << fileName << " :"
		        << "\n" << gexc.what ( );
		throw Exception (message);
	}
	catch (const exception& exc)
	{
		UTF8String	message (charset);
		message << "Impossibilité de charger le maillage GMDS contenu dans le "
		        << "fichier " << fileName << " :"
		        << "\n" << exc.what ( );
		throw Exception (message);
	}
	catch (...)
	{
		UTF8String	message (charset);
		message << "Impossibilité de charger le maillage GMDS contenu dans le "
		        << "fichier " << fileName << " : erreur non documentée.";
		throw Exception (message);
	}
}	// QtGMDSMeshAnalysisView::QtGMDSMeshAnalysisView


QtGMDSMeshAnalysisView::QtGMDSMeshAnalysisView (
										const QtGMDSMeshAnalysisView&)
	: QtMeshAnalysisView (0, "Invalid application"), _mesh ( )
{
	assert (0 && "QtGMDSMeshAnalysisView copy constructor is not allowed.");
}	// QtGMDSMeshAnalysisView::QtMeshAnalysisView


QtGMDSMeshAnalysisView&
QtGMDSMeshAnalysisView::operator = (const QtGMDSMeshAnalysisView&)
{
	assert (0 && "QtGMDSMeshAnalysisView assignment operator is not allowed.");
	return *this;
}	// QtGMDSMeshAnalysisView::operator =


QtGMDSMeshAnalysisView::~QtGMDSMeshAnalysisView ( )
{
}	// QtGMDSMeshAnalysisView::~QtGMDSMeshAnalysisView

#endif	// USE_GMDS


// ============================================================================
//                       LA CLASSE QtCalQualMainWindow
// ============================================================================

QtCalQualMainWindow::QtCalQualMainWindow (
									QWidget* parent, const string& appTitle)
	: QMainWindow (parent), _mdiArea (0), _windowMenu (0)
{
	setWindowTitle (appTitle.c_str ( ));
	_mdiArea	= new QMdiArea (this);
	// Les panneaux graphiques ne reÃ§oivent pas l'évÃ¨nement focusInEvent
	// car c'est un ancÃªtre (QtMeshAnalysisView) qui le recevra => s'informer du
	// changement de fenÃªtre active afin d'en faire part au gestionnaire
	// de graphiques :
	connect (_mdiArea, SIGNAL (subWindowActivated (QMdiSubWindow*)), this,
	         SLOT (subWindowActivated (QMdiSubWindow*)));
	setCentralWidget (_mdiArea);

	// Création du menu :
	QAction*	action	= 0;
	QMenuBar*	bar	= menuBar ( );
	QMenu*		menu	= bar->addMenu ("Application");
	QAction*	analysisAction	=
		new QAction (QIcon (":/images/open.png"), "Analyser un maillage ...",
		             this);
	connect (analysisAction, SIGNAL (triggered ( )), this,
	         SLOT (analyseMeshCallback ( )));
	QwtChartsManager::getToolBar ( ).addAction (analysisAction);
	menu->addAction (analysisAction);
	menu->addSeparator ( );
	menu->addAction (QLatin1String ("Quitter"), this, SLOT (exitCallback ( )));
	_windowMenu	= bar->addMenu (QLatin1String ("Fenêtre"));
	_windowMenu->addAction (QLatin1String ("Cascade"), this, SLOT (cascadeCallback ( )));
	_windowMenu->addAction (QLatin1String ("Tuiles"), this, SLOT (tileCallback ( )));
	_windowMenu->addSeparator ( );
	_windowMenu->addAction (QwtChartsManager::getPrintAction ( ));
	_windowMenu->addSeparator ( );

	// Utilisation de la barre d'outils par défaut de QwtChartsManager :
	addToolBar (Qt::LeftToolBarArea, &QwtChartsManager::getToolBar ( ));
}	// QtCalQualMainWindow::QtCalQualMainWindow


QtCalQualMainWindow::QtCalQualMainWindow (const QtCalQualMainWindow&)
{
	assert (0 && "QtCalQualMainWindow copy constructor is not allowed.");
}	// QtCalQualMainWindow::QtCalQualMainWindow


QtCalQualMainWindow& QtCalQualMainWindow::operator = (
													const QtCalQualMainWindow&)
{	
	assert (0 && "QtCalQualMainWindow assignment operator is not allowed.");
	return *this;
}	// QtCalQualMainWindow::operator =


QtCalQualMainWindow::~QtCalQualMainWindow ( )
{
}	// QtCalQualMainWindow::~QtCalQualMainWindow


void QtCalQualMainWindow::analyseMeshCallback ( )
{
	UTF8String	error (charset);

	try
	{
		UTF8String	extensions (charset);
		extensions << "Lima++ (";
		const char**	ext		= Lima::liste_format_lecture ( );
		size_t			count	= 0;
		while (NULL != *ext)
		{
			string				extension (*ext);
			string::size_type	comma	= extension.find (',');
			if (string::npos != comma)
				extension	= extension.substr (0, comma);
			if (0 == count)
				extensions << "*." << extension;
			else
				extensions << " *." << extension;
			ext++;
			count++;
		}	// while (NULL != *ext)
		extensions << ")";
		extensions << ";;VTK(*.vtk);;GMDS(*.mesh *.med *.vtu *.vtp *.fac)";

		static QString	lastFile, lastFilter;
		QFileInfo		path (lastFile);
		QFileDialog		dialog (
					this, windowTitle ( ), path.absoluteFilePath ( ), 
					UTF8TOQSTRING (extensions));
		dialog.setFileMode (QFileDialog::ExistingFile);
		dialog.setAcceptMode (QFileDialog::AcceptOpen);
		dialog.selectNameFilter (lastFilter);

		if (QDialog::Accepted != dialog.exec ( ))
			return;

		QStringList	files	= dialog.selectedFiles ( );
		lastFile			= files [0];
		lastFilter			= dialog.selectedNameFilter ( );
		error << "Erreur rencontrée lors du chargement du maillage du fichier "
		      << lastFile.toStdString ( ) << " :" << "\n";

		QtMeshAnalysisView*	analysisWidget	= createAnalysisView (
						lastFilter.toStdString ( ), lastFile.toStdString ( ));
		analysisWidget->show ( );

		return;
	}
	catch (const Exception& exc)
	{
		error << exc.getFullMessage ( );
	}
	catch (const exception& exc)
	{
		 error << exc.what ( );
	}
	catch (...)
	{
		error << "Erreur non documentée.";
	}

	QtMessageBox::displayErrorMessage (
								this, windowTitle ( ).toStdString ( ), error);
}	// QtCalQualMainWindow::analyseMeshCallback



void QtCalQualMainWindow::exitCallback ( )
{
	if (1 == QMessageBox::warning (this, windowTitle ( ),
				QSTR ("Souhaitez-vous réellement quitter cette application ?"),
				"Oui", "Non", QString ( ), 0, -1))
		return;

	QApplication::exit ( );
}	// QtCalQualMainWindow::exitCallback


void QtCalQualMainWindow::tileCallback ( )
{
	_mdiArea->tileSubWindows ( );
}	// QtCalQualMainWindow::tileCallback


void QtCalQualMainWindow::cascadeCallback ( )
{
	_mdiArea->cascadeSubWindows ( );
}	// QtCalQualMainWindow::cascadeCallback


QtMeshAnalysisView* QtCalQualMainWindow::createAnalysisView (
								const string& filter, const string& fileName)
{
	QtMeshAnalysisView*	view	= 0;
	TkUtil::File		file (fileName);

	// Est-ce un fichier Lima ?
	if (0 == filter.compare (0, 6, "Lima++", 0, 6))
		view	= new QtLimaMeshAnalysisView (
						_mdiArea, windowTitle ( ).toStdString ( ), fileName);
	else if (0 == filter.compare (0, 3, "VTK", 0, 3))
	{
		view	= new QtMeshAnalysisView (
								_mdiArea, windowTitle ( ).toStdString ( ));
		VtkQualifSerie*	serie	=
					new VtkQualifSerie (fileName, file.getFileName ( ));
		view->getAnalysisPanel ( ).addSerie (serie);
	}	// else if (file.getExtension ( ) == string ("vtk"))
#ifdef USE_GMDS
	else if (0 == filter.compare (0, 4, "GMDS", 0, 4))
	{
		view	= new QtGMDSMeshAnalysisView(
						_mdiArea, windowTitle ( ).toStdString ( ), fileName);
	}	// else if (file.getExtension ( ) == string ("med"))
#endif	// USE_GMDS
	else
	{
		UTF8String	message (charset);
		message << "Format de maillage non supporté pour le maillage contenu "
		        << "dans le fichier " << fileName << ".";
		throw Exception (message);
	}	// else if (Lima::SUFFIXE != ...

//	_mdiArea->addSubWindow (view);
	view->resize (1200, 800);
	view->show ( );
	QMenu*	menu	= new QMenu (view);
	menu->addMenu (_windowMenu);
	menu->setToolTipsVisible (true);
	QMenu*	graphicMenu	= QwtChartsManager::createMenu (0, "Graphique ...");
	menu->addMenu (graphicMenu);	
	view->setContextMenu (menu);

//	view->getAnalysisPanel ( ).updateHistogram ( );

	return view;
}	// QtCalQualMainWindow::createHistogramView


void QtCalQualMainWindow::subWindowActivated (QMdiSubWindow* window)
{
	QtMeshAnalysisView*		view	= dynamic_cast<QtMeshAnalysisView*>(window);
	if (0 != view)
		QwtChartsManager::getManager ( ).focusChanged (
							&view->getAnalysisPanel ( ).getHistogramPanel ( ));
}	// QtCalQualMainWindow::subWindowActivated


}	// namespace GQualif
