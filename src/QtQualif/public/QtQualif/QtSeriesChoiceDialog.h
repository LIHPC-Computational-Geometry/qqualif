#ifndef QT_SERIES_CHOICE_DIALOG_H
#define QT_SERIES_CHOICE_DIALOG_H

#include <TkUtil/util_config.h>

#include <QDialog>
#include <QListWidget>

#include <string>
#include <vector>
#include <utility>		// pair


namespace GQualif
{

/**
 * <P>Classe de boite de dialogue <I>Qt</I> permettant de sélectionner des
 * groupes de mailles à analyser.
 * </P>
 */
class QtSeriesChoiceDialog : public QDialog
{
	public :

	/**
	 * Constructeur.
	 * @param		Widget parent.
	 * @param		Titre de l'application (pour les messages).
	 * @param		couples nom de groupe de mailles/dimension
	 */
	QtSeriesChoiceDialog (
			QWidget* parent, const IN_STD string& appTitle,
			const IN_STD vector <IN_STD pair <IN_STD string, unsigned char> >& groups);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSeriesChoiceDialog ( );

	/**
	 * @return		La liste des couples nom de groupe de mailles/dimension
	 *				retenus.
	 */
	virtual IN_STD vector <IN_STD pair <IN_STD string, unsigned char> >
															getGroups ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSeriesChoiceDialog (const QtSeriesChoiceDialog&);
	QtSeriesChoiceDialog& operator = (const QtSeriesChoiceDialog&);

	/** La liste des couples nom de groupe de mailles/dimension. */
	QListWidget*					_groupsList;
};	// class QtSeriesChoiceDialog

}	// namespace GQualif

#endif	// QT_SERIES_CHOICE_DIALOG_H
