#include "QtQualif/QtSeriesChoiceDialog.h"

#include <TkUtil/InternalError.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

#include <iostream>
#include <assert.h>


USING_UTIL
USING_STD

static const Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


namespace GQualif
{


// ===========================================================================
//                         Classe QtGroupItem
// ===========================================================================


class QtGroupItem : public QListWidgetItem
{
	public :

	QtGroupItem (const QString& text, QListWidget* parent, unsigned char dim)
		: QListWidgetItem (text, parent),
		  _name (text.toStdString ( )), _dimension (dim)
	{
		UTF8String	str (charset);
		str << _name << " (dimension " << (unsigned long)_dimension << ")";
		setText (UTF8TOQSTRING (str));
	}

	virtual ~QtGroupItem ( )
	{ }

	virtual string getName ( ) const
	{ return _name; }

	virtual unsigned char getDimension ( ) const
	{ return _dimension; }


	private :

	QtGroupItem (const QtGroupItem&)
		: QListWidgetItem (0)
	{
		assert (0 && "QtGroupItem copy constructor is not allowed.");
	}

	QtGroupItem& operator = (const QtGroupItem&)
	{
		assert (0 && "QtGroupItem assignment operator is not allowed.");
		return *this;
	}

	string						_name;
	unsigned char 				_dimension;
};	// class QtGroupItem


// ===========================================================================
//                         Classe QtSeriesChoiceDialog
// ===========================================================================


QtSeriesChoiceDialog::QtSeriesChoiceDialog (
					QWidget* parent, const string& appTitle,
					const vector <pair <string, unsigned char> >& groups)
	: QDialog (parent), _groupsList (0)
{
	QVBoxLayout*	mainLayout	= new QVBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	QLabel*	label	= new QLabel (
					QSTR ("Sélectionnez les groupes de mailles à analyser :"), this);
	mainLayout->addWidget (label);
	_groupsList	= new QListWidget (this);
	_groupsList->setSelectionMode (QListWidget::ExtendedSelection);
	_groupsList->setSortingEnabled (false);
	mainLayout->addWidget (_groupsList);
	QAbstractItemModel*	dataTypeModel	= _groupsList->model ( );
	dataTypeModel->setHeaderData (
						0, Qt::Horizontal, QVariant(QSTR ("Groupes de mailles")));
	for (vector <pair <string, unsigned char> >::const_iterator it =
			groups.begin ( ); groups.end ( ) != it; it++)
		_groupsList->addItem (
			new QtGroupItem ((*it).first.c_str ( ), _groupsList, (*it).second));
	QFrame*	separator	= new QFrame (this);
	separator->setFrameStyle (QFrame::HLine);
	mainLayout->addWidget (separator);
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	mainLayout->addLayout (hlayout);
	hlayout->addStretch (10.);
	QPushButton*	closeButton	= new QPushButton (QSTR ("Fermer"), this);
	hlayout->addWidget (closeButton);
	connect (closeButton, SIGNAL(clicked ( )), this, SLOT(accept ( )));
	QPushButton*	cancelButton	= new QPushButton (QSTR ("Annuler"), this);
	hlayout->addWidget (cancelButton);
	connect (cancelButton, SIGNAL(clicked ( )), this, SLOT(reject ( )));
	hlayout->addStretch (10.);
}	// QtSeriesChoiceDialog::QtSeriesChoiceDialog


QtSeriesChoiceDialog::QtSeriesChoiceDialog (const QtSeriesChoiceDialog&)
	: QDialog (0), _groupsList ( )
{
	assert (0 && "QtSeriesChoiceDialog copy constructor is not allowed.");
}	// QtSeriesChoiceDialog::QtSeriesChoiceDialog


QtSeriesChoiceDialog& QtSeriesChoiceDialog::operator = (
												const QtSeriesChoiceDialog&)
{
	assert (0 && "QtSeriesChoiceDialog assignment operator is not allowed.");
	return *this;
}	// QtSeriesChoiceDialog::operator =


QtSeriesChoiceDialog::~QtSeriesChoiceDialog ( )
{
}	// QtSeriesChoiceDialog::~QtSeriesChoiceDialog


vector <pair <string, unsigned char> > QtSeriesChoiceDialog::getGroups ( ) const
{
	assert (0 != _groupsList && "QtSeriesChoiceDialog::getGroups : null groups list.");
	vector <pair <string, unsigned char> >	groups;

	QList<QListWidgetItem*>	selection	= _groupsList->selectedItems ( );
	for (size_t s = 0; s < selection.size ( ); s++)
	{
		QtGroupItem*	item	= dynamic_cast<QtGroupItem*>(selection [s]);
		if (0 == item)
		{
			INTERNAL_ERROR (exc, "Unexpected item type.", "QtSeriesChoiceDialog::getGroups")
			throw exc;
		}	// if (0 == item)

		groups.push_back (pair<string, unsigned char>(
									item->getName ( ), item->getDimension ( )));
	}	// for (size_t s = 0; s < selection.size ( ); s++)

	return groups;
}	//  QtSeriesChoiceDialog::getGroups


}	// namespace GQualif

