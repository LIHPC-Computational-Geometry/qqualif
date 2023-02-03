#ifndef QT_CAL_QUAL_MAIN_WINDOW_H
#define QT_CAL_QUAL_MAIN_WINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QMdiArea>
#include <QMdiSubWindow>

#include <QtQualif/QtQualifWidget.h>

#include <LimaP/maillage_it.h>
#ifdef USE_GMDS
#include <GMDS/IG/IGMesh.h>
#endif	// USE_GMDS

#include <memory>


namespace GQualif
{

// ============================================================================
//                       LA CLASSE QtMeshAnalysisView
// ============================================================================

/**
 * Classe MDI affichant une analyse <I>Qualif</I> d'un maillage.
 */
class QtMeshAnalysisView : public QMdiSubWindow
{
	Q_OBJECT

	public :

	QtMeshAnalysisView (QMdiArea* parent, const IN_STD string& appTitle);
	virtual ~QtMeshAnalysisView ( );

	/**
	 * @param		Menu affich� lorsque l'utilisateur clique du bouton droit
	 *				de la souris.
	 */
	virtual void setContextMenu (QMenu* menu);

	/**
	 * @return		Une r�f�rence sur la fen�tre d'analyse repr�sent�e.
	 */
	GQualif::QtQualifWidget& getAnalysisPanel ( )
	{ return *_analysisWidget; }
	const GQualif::QtQualifWidget& getAnalysisPanel ( ) const
	{ return *_analysisWidget; }


	protected :

	/**
	 * Affiche le menu popup associ� � l'emplacement du curseur de la souris.
	 */
	virtual void contextMenuEvent (QContextMenuEvent* event);


	private :

	QtMeshAnalysisView (const QtMeshAnalysisView&);
	QtMeshAnalysisView& operator = (const QtMeshAnalysisView&);

	GQualif::QtQualifWidget*	_analysisWidget;
	QMenu*						_contextMenu;
};	// class QtMeshAnalysisView


// ============================================================================
//                   LA CLASSE QtLimaMeshAnalysisView
// ============================================================================

/**
 * Classe prenant en charge un maillage <I>Lima</I> dont elle propose �
 * l'utilisateur de s�lectionner les s�ries � analyser au sein d'un m�me
 * histogramme.
 */
class QtLimaMeshAnalysisView : public QtMeshAnalysisView
{
	public :

	/**
	 * Charge le maillage contenu dans le fichier <I>fileName</I> et propose
	 * � l'utilisateur de s�lectionner les groupes de donn�es � analyser.
	 */
	QtLimaMeshAnalysisView (QMdiArea* parent, const IN_STD string& appTitle,
	                        const IN_STD string&fileName);

	virtual ~QtLimaMeshAnalysisView ( );


	private :

	QtLimaMeshAnalysisView (const QtLimaMeshAnalysisView&);
	QtLimaMeshAnalysisView& operator = (const QtLimaMeshAnalysisView&);

	IN_STD unique_ptr <Lima::_MaillageInterne>	_mesh;
};	// class QtLimaMeshAnalysisView


#ifdef USE_GMDS

// ============================================================================
//                   LA CLASSE QtGMDSMeshAnalysisView
// ============================================================================

/**
 * Classe prenant en charge un maillage <I>GMDS</I> dont elle propose �
 * l'utilisateur de s�lectionner les s�ries � analyser au sein d'un m�me
 * histogramme.
 */
class QtGMDSMeshAnalysisView : public QtMeshAnalysisView
{
	public :

	/**
	 * Charge le maillage contenu dans le fichier <I>fileName</I> et propose
	 * � l'utilisateur de s�lectionner les groupes de donn�es � analyser.
	 */
	QtGMDSMeshAnalysisView (QMdiArea* parent, const IN_STD string& appTitle,
	                        const IN_STD string&fileName);

	virtual ~QtGMDSMeshAnalysisView ( );


	private :

	QtGMDSMeshAnalysisView (const QtGMDSMeshAnalysisView&);
	QtGMDSMeshAnalysisView& operator = (const QtGMDSMeshAnalysisView&);

	IN_STD unique_ptr<gmds::IGMesh>	_mesh;
};	// class QtGMDSMeshAnalysisView

#endif // USE_GMDS


// ============================================================================
//                       LA CLASSE QtCalQualMainWindow
// ============================================================================

/**
 * Fen�tre principale de l'application permettant d'afficher plusieurs analyses
 * dans un support MDI.
 */
class QtCalQualMainWindow : public QMainWindow
{
	Q_OBJECT


	public :

	QtCalQualMainWindow (QWidget* parent, const IN_STD string& appTitle);
	virtual ~QtCalQualMainWindow ( );

#ifdef USE_GMDS
	/** Le masque des maillages GMDS. */
	static const int	gmdsMask;
#endif	// USE_GMDS

	protected slots :

	virtual void analyseMeshCallback ( );
	virtual void exitCallback ( );
	virtual void cascadeCallback ( );
	virtual void tileCallback ( );
	virtual void subWindowActivated (QMdiSubWindow*);


	protected :

	virtual QtMeshAnalysisView* createAnalysisView (
					const IN_STD string& filter, const IN_STD string& fileName);


	private :

	QtCalQualMainWindow (const QtCalQualMainWindow&);
	QtCalQualMainWindow& operator = (const QtCalQualMainWindow&);

	QMdiArea			*_mdiArea;
	QMenu				*_windowMenu;
};	// class QtCalQualMainWindow


}	// namespace GQualif


#endif	// QT_CAL_QUAL_MAIN_WINDOW_H
