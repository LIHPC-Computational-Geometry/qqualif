#include "QtQualif/QtQualityDividerWidget.h"
#include "GQualif/QualifHelper.h"
#include "GQualif/QualifAnalysisTask.h"
#include "GQualif/QCalQualThread.h"

#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/ThreadManager.h>
#include <TkUtil/UTF8String.h>

#include <QLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>

#include <assert.h>
#include <iostream>
#include <memory>


USING_UTIL
USING_STD
using namespace Qualif;

static const Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


#define BEGIN_TRY_CATCH_BLOCK                                              \
    bool            hasError    = false;                                   \
    try {

#define COMPLETE_TRY_CATCH_BLOCK                                           \
    }                                                                      \
    catch (const IN_UTIL Exception& exc)                                   \
    {                                                                      \
        displayErrorMessage (exc.getFullMessage ( ));                      \
        hasError    = true;                                                \
    }                                                                      \
    catch (const IN_STD exception& stdExc) /* IN_STD : pour OSF */         \
    {                                                                      \
        displayErrorMessage (stdExc.what ( ));                             \
        hasError    = true;                                                \
    }                                                                      \
    catch (...)                                                            \
    {                                                                      \
        displayErrorMessage ("Erreur interne non documentée.");            \
        hasError    = true;                                                \
    }


namespace GQualif
{

// ===========================================================================
//                         Classe QtQualifDataTypeItem
// ===========================================================================

class QtQualifDataTypeItem : public QListWidgetItem
{
	public :

	QtQualifDataTypeItem (QListWidget* parent, size_t type, size_t dimension)
		: QListWidgetItem (parent), _type (type), _dimension (dimension)
	{
		setText (UTF8TOQSTRING (QualifHelper::dataTypeToName (_type)));
	}

	virtual size_t getDataDimension ( ) const
	{ return _dimension; }

	virtual size_t getDataType ( ) const
	{ return _type; }


	private :

	/** La dimension de la donnée (0 : noeud, 1 : arête, 2 : polygone,
	 *  3 : polyèdre, ... */
	size_t			_dimension;

	/** Le type de la donnée : Node::NODE, ..., Cell3D::HEXAEDRON, ...
	 */
	size_t			_type;
};	// class QtQualifDataTypeItem


// ===========================================================================
//                         Classe QtQualityDividerWidget
// ===========================================================================


QtQualityDividerWidget::QtQualityDividerWidget (QWidget* parent, const string& appTitle)
	: QWidget (parent),
	  _appTitle (appTitle), _series ( ),
	  _minTextField (0), _maxTextField (0), _domainLabel (0),
	  _criterionComboBox (0), _dataTypesList (0), _seriesExtractionsTableWidget (),
	  _coordinatesCheckBox (0), _releaseDataCheckBox (0),
	  _buttonsLayout (0), _optionsLayout (0), _applyButton (0),
	  _criterion ((Qualif::Critere)-1)
{
	QVBoxLayout*	mainLayout	= new QVBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	// Paramétrage de l'analyse :
	// 1ère ligne : min + max.
	QHBoxLayout*	hboxLayout	= new QHBoxLayout ( );
	mainLayout->addLayout (hboxLayout);
	QLabel*	label	= new QLabel ("Min : ", this);
	hboxLayout->addWidget (label);
	hboxLayout->setStretchFactor (label, 1);
	_minTextField	= new QtTextField ("1.", this);
	hboxLayout->addWidget (_minTextField);
	_minTextField->setValidator (new QDoubleValidator (_minTextField));
	_minTextField->setVisibleColumns (6);
	_minTextField->setFixedSize (_minTextField->sizeHint ( ));
	hboxLayout->setStretchFactor (_minTextField, 1);
	_minTextField->setToolTip (QSTR ("Champ de saisie de la valeur minimale à représenter."));
	label	= new QLabel (QSTR ("Max : "), this);
	hboxLayout->addWidget (label);
	label->setFixedSize (label->sizeHint ( ));
	hboxLayout->setStretchFactor (label, 1);
	_maxTextField	= new QtTextField ("50.", this);
	hboxLayout->addWidget (_maxTextField);
	_maxTextField->setValidator (new QDoubleValidator (_maxTextField));
	_maxTextField->setVisibleColumns (6);
	_maxTextField->setFixedSize (_maxTextField->sizeHint ( ));
	hboxLayout->setStretchFactor (_maxTextField, 1);
	_maxTextField->setToolTip (QSTR ("Champ de saisie de la valeur maximale à représenter."));
	_domainLabel	= new QLabel ("Domaine : [,]", this);
	_domainLabel->setAlignment (Qt::AlignLeft | Qt::AlignVCenter);
	hboxLayout->addWidget (_domainLabel);
//	hboxLayout->addStretch (10.);
	
	// 2-ième ligne : critère :
	hboxLayout	= new QHBoxLayout ( );
	mainLayout->addLayout (hboxLayout);
	label	= new QLabel (QSTR ("Critère :"), this);
	hboxLayout->addWidget (label);
	_criterionComboBox	= new QComboBox (this);
	hboxLayout->addWidget (_criterionComboBox);
	for (int criterion = 0; criterion < (int)FIN; criterion++)
		_criterionComboBox->addItem (CRITERESTR [criterion].c_str( ),criterion);
	_criterionComboBox->setCurrentIndex (Qualif::SCALEDJACOBIAN);
	_criterionComboBox->setToolTip (QSTR ("Critère de qualité à appliquer."));
	connect (_criterionComboBox, SIGNAL(activated (int)), this, SLOT (updateSelectableTypesCallback ( )));
	connect (_criterionComboBox, SIGNAL(activated (int)), this, SLOT (updateDomainCallback ( )));
	hboxLayout->addStretch (10.);

	// 3-ème ligne : les extractions de séries
	_seriesExtractionsTableWidget	= new QTableWidget (this);
	mainLayout->addWidget (_seriesExtractionsTableWidget);
	if (0 != _seriesExtractionsTableWidget->verticalHeader ( ))
		_seriesExtractionsTableWidget->verticalHeader ( )->hide ( );
	_seriesExtractionsTableWidget->setColumnCount (2);
	QStringList	labels;
	labels.push_back ("Groupes de données");
	labels.push_back ("Nombre de mailles correspondantes");
	_seriesExtractionsTableWidget->setHorizontalHeaderLabels (labels);
	connect (_seriesExtractionsTableWidget, SIGNAL(cellClicked(int,int)), this, SLOT (displaySerieExtractionCallback (int, int)));
	_seriesExtractionsTableWidget->setSelectionBehavior (QTableWidget::SelectRows);
	if (0 != _seriesExtractionsTableWidget->horizontalHeader ( ))
	{
		_seriesExtractionsTableWidget->horizontalHeader ( )->setStretchLastSection (true);
		_seriesExtractionsTableWidget->horizontalHeader ( )->setSectionResizeMode (QHeaderView::ResizeToContents);
	}

	// 4-ième ligne : sélection :
	hboxLayout	= new QHBoxLayout ( );
	mainLayout->addLayout (hboxLayout);
	_dataTypesList	= new QListWidget (this);
	_dataTypesList->setSelectionMode (QListWidget::ExtendedSelection);
	_dataTypesList->setSortingEnabled (false);
	connect (_dataTypesList, SIGNAL(itemSelectionChanged ( )), this, SLOT (updateDomainCallback ( )));
	hboxLayout->addWidget (_dataTypesList);
	QAbstractItemModel*	dataTypeModel	= _dataTypesList->model ( );
	dataTypeModel->setHeaderData (0, Qt::Horizontal, QVariant(QSTR ("Types de données")));
	_dataTypesList->setToolTip (QSTR ("Liste des types de mailles à prendre en compte lors du traitement."));
	_optionsLayout	= new QVBoxLayout ( );
	hboxLayout->addLayout (_optionsLayout);
	_coordinatesCheckBox	= new QCheckBox (QSTR ("Validité des coordonnées"), this);
	_optionsLayout->addWidget (_coordinatesCheckBox);	
	_coordinatesCheckBox->setFixedSize (_coordinatesCheckBox->sizeHint ( ));
	_coordinatesCheckBox->setChecked (true);
	_coordinatesCheckBox->setToolTip (QSTR ("Activé, effectue une analyse préalable des coordonnées des noeuds des mailles."));
	_releaseDataCheckBox	= new QCheckBox (QSTR ("Libérer la mémoire après chaque critère"), this);
	_optionsLayout->addWidget (_releaseDataCheckBox);	
	_releaseDataCheckBox->setFixedSize (_releaseDataCheckBox->sizeHint ( ));
	_releaseDataCheckBox->setChecked (true);
	_releaseDataCheckBox->setToolTip (QSTR ("Activé, libère la mémoire utilisée en cache à chaque changement de critère."));
	hboxLayout->addStretch (10.);
	_optionsLayout->addStretch (10.);

	// 4-ième ligne : boutons :
	_buttonsLayout	= new QHBoxLayout ( );
	mainLayout->addLayout (_buttonsLayout);
	_applyButton	= new QPushButton (QSTR ("Calculer"), this);
	_buttonsLayout->addWidget (_applyButton);
	_applyButton->setFixedSize (_applyButton->sizeHint ( ));
	connect (_applyButton, SIGNAL(clicked ( )), this, SLOT(computeCallback ( )));
	_applyButton->setToolTip (QSTR ("Provoque la mise à jour des extractions."));
	_buttonsLayout->addStretch (10.);
	setFocusPolicy (Qt::StrongFocus);

	layout ( );
	
	// Affichage d'un domaine possible :
	updateDomainCallback ( );
}	// QtQualityDividerWidget::QtQualityDividerWidget


QtQualityDividerWidget::QtQualityDividerWidget (const QtQualityDividerWidget&)
	: QWidget (0),
	  _appTitle ("Invalid application"), _series ( ),
	  _minTextField (0), _maxTextField (0), _domainLabel (0),
	  _criterionComboBox (0), _dataTypesList (0), _seriesExtractionsTableWidget (0),
	  _coordinatesCheckBox (0), _releaseDataCheckBox (0),
	  _buttonsLayout (0), _optionsLayout (0), _applyButton (0),
	  _criterion ((Qualif::Critere)-1)
{
	assert (0 && "QtQualityDividerWidget copy constructor is not allowed.");
}	// QtQualityDividerWidget::QtQualityDividerWidget


QtQualityDividerWidget& QtQualityDividerWidget::operator = (const QtQualityDividerWidget&)
{
	assert (0 && "QtQualityDividerWidget assignment operator is not allowed.");
	return *this;
}	// QtQualityDividerWidget::operator =


QtQualityDividerWidget::~QtQualityDividerWidget ( )
{
	removeSeries ( );
}	// QtQualityDividerWidget::~QtQualityDividerWidget


void QtQualityDividerWidget::addSerie (AbstractQualifSerie* serie)
{
	assert (0 != _dataTypesList && "QtQualityDividerWidget::addSerie : null data list widget.");
	if (0 == serie)
	{
		INTERNAL_ERROR (exc, UTF8String ("Série nulle.", charset), "QtQualityDividerWidget::addSerie")
		throw exc;
	}	// if (0 == serie)

	_series.push_back (serie);

	_dataTypesList->clear ( );
	map<size_t, size_t>	types	= getDataTypes ( );
	for (map<size_t, size_t>::iterator itt = types.begin ( ); types.end ( ) != itt; itt++)
	{
		QtQualifDataTypeItem*	item	= new QtQualifDataTypeItem (_dataTypesList, (*itt).first, (*itt).second);
		bool	selectable	= QualifHelper::isTypeSupported ((*itt).first);
		item->setFlags (true == selectable ? Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::NoItemFlags);
		if ((true == selectable) && (1 == types.size ( )))
			item->setSelected (true);
	}	// for (map<size_t, size_t>::iterator itt ...
	_dataTypesList->setFixedSize (_dataTypesList->sizeHint ( ));
}	// QtQualityDividerWidget::addSerie


void QtQualityDividerWidget::removeSeries (bool clear)
{
	assert (0 != _dataTypesList && "QtQualityDividerWidget::removeSerie : null data list widget.");
	const size_t	seriesNum	= _series.size ( );
	if (0 == seriesNum)
		return;	// Surtout ne rien faire, Qwt a horreur du vide.

	for (size_t i = 0; i < seriesNum; i++)
		delete _series [i];
	_series.clear ( );
	_dataTypesList->clear ( );
}	// QtQualityDividerWidget::removeSeries


void QtQualityDividerWidget::compute ( )
{
	assert (0 != _seriesExtractionsTableWidget);
	assert (0 != _applyButton && "QtQualityDividerWidget::compute : null apply button.");
	_applyButton->setEnabled (false);

	BEGIN_TRY_CATCH_BLOCK

	QtAutoWaitingCursor				cursor (true);

	// Faut il préalablement évaluer les coordonnées des noeuds ?
	size_t			i	= 0;
	const size_t	seriesNum	= _series.size ( );
	if (true == coordinatesEvaluation ( ))
	{
		for (i = 0; i < seriesNum; i++)
		{
			AbstractQualifSerie*	serie	= _series [i];
			CHECK_NULL_PTR_ERROR (serie)
			try
			{
				serie->validateCoordinates ( );
			}
			catch (const Exception& exc)
			{
				UTF8String	message (charset);
				message << "Erreur avec la série " << serie->getName ( ) << " :\n" << exc.getFullMessage ( );
				throw (Exception (message));
			}
		}	// for (i = 0; i < seriesNum; i++)
	}	// if (true == coordinatesEvaluation ( ))

	const Critere					criterion	= getCriterion ( );
	const size_t					cellTypes	= getQualifiedTypes ( );

	if (0 == cellTypes)
		throw Exception (UTF8String ("Impossibilité de recalculer les extractions : absence de types de mailles sélectionné.", charset));

	// Domaine d'application :
	const double	min	= getDomainMinValue ( );
	const double	max	= getDomainMaxValue ( );
	const double	interval		= max - min;

	// Exécution dans une tâche Qualif :
	unique_ptr<QualifAnalysisTask>	task (createAnalysisTask (cellTypes, criterion, min, max, _series));
	task->execute ( );

	// Les séries :
    const vector< vector <size_t> >&	values	= task->getSeriesValues ( );
	_seriesExtractionsTableWidget->setRowCount (0);
	_seriesExtractionsTableWidget->setRowCount (seriesNum);
	_seriesExtractionsTableWidget->setColumnCount (2);
	for (int j = 0; j < seriesNum; j++)
	{
		AbstractQualifSerie*	serie	= _series [j];
		CHECK_NULL_PTR_ERROR (serie)
		QTableWidgetItem*	item	= new QTableWidgetItem (serie->getName ( ).c_str ( ));
		item->setCheckState (Qt::Unchecked);
		_seriesExtractionsTableWidget->setItem (j, 0, item);
		item	= new QTableWidgetItem (QString::number (values [0][j]));
		_seriesExtractionsTableWidget->setItem (j, 1, item);
	}	// for (int j = 0; j < seriesNum; j++)

	COMPLETE_TRY_CATCH_BLOCK

	_applyButton->setEnabled (true);
}	// QtQualityDividerWidget::compute


Qualif::Critere QtQualityDividerWidget::getCriterion ( ) const
{
	assert ((0 != _criterionComboBox) && "QtQualityDividerWidget::getCriterion : combo box nulle.");
	return (Critere)_criterionComboBox->currentIndex ( );
}	// QtQualityDividerWidget::getCriterion


double QtQualityDividerWidget::getDomainMinValue ( ) const
{
	assert ((0 != _minTextField) && "QtQualityDividerWidget::getDomainMinValue : null textfield.");
	if (true == _minTextField->text ( ).isEmpty ( ))
		throw Exception (UTF8String ("Absence de valeur minimale saisie pour le domaine.", charset));

	bool	ok	= true;
	return _minTextField->text ( ).toDouble (&ok);
}   // QtQualityDividerWidget::getDomainMinValue


double QtQualityDividerWidget::getDomainMaxValue ( ) const
{
	assert ((0 != _maxTextField) && "QtQualityDividerWidget::getDomainMaxValue : null textfield.");
	if (true == _maxTextField->text ( ).isEmpty ( ))
		throw Exception (UTF8String ("Absence de valeur maximale saisie pour le domaine.", charset));

	bool	ok	= true;
	return _maxTextField->text ( ).toDouble (&ok);
}   // QtQualityDividerWidget::getDomainMaxValue


unsigned char QtQualityDividerWidget::getSeriesNum ( ) const
{
	return _series.size ( );
}	// QtQualityDividerWidget::getSeriesNum


const AbstractQualifSerie& QtQualityDividerWidget::getSerie (unsigned char i) const
{
	if (i >= _series.size ( ))
	{
		UTF8String	error (charset);
		error << "Impossibilité d'obtenir la " << (unsigned long)i << "-ème série : ce panneau n'en gère que " << (_series.size ( ) + 1) << ".";
		throw Exception (error);
	}	// if (i >= _series.size ( ))

	return *(_series [i]);
}	// QtQualityDividerWidget::getSerie


AbstractQualifSerie& QtQualityDividerWidget::getSerie (unsigned char i)
{
	if (i >= _series.size ( ))
	{
		UTF8String	error (charset);
		error << "Impossibilité d'obtenir la " << (unsigned long)i << "-ème série : ce panneau n'en gère que " << (_series.size ( ) + 1) << ".";
		throw Exception (error);
	}	// if (i >= _series.size ( ))

	return *(_series [i]);
}	// QtQualityDividerWidget::getSerie


vector<unsigned char> QtQualityDividerWidget::getSelectedSeries ( ) const
{
	assert (0 != _seriesExtractionsTableWidget);
	vector<unsigned char>	series;
	
	for (unsigned char i = 0; i < _seriesExtractionsTableWidget->rowCount ( ); i++)
	{
		QTableWidgetItem*	item	= _seriesExtractionsTableWidget->item (i, 0);
		if ((0 != item) && (true == item->isSelected ( )))
			series.push_back (i);
	}	// for (unsigned char i = 0; i < _seriesExtractionsTableWidget->rowCount ( ); i++)
	
	return series;
}	// QtQualityDividerWidget::getSelectedSeries


size_t QtQualityDividerWidget::getQualifiedTypes ( ) const
{
	assert ((0 != _dataTypesList) && "QtQualityDividerWidget::getQualifiedTypes : liste des types de données nulle.");
	size_t	types	= 0;

	const int	itemCount	= _dataTypesList->count ( );
	for (int i = 0; i < itemCount; i++)
	{
		QListWidgetItem*	item	= _dataTypesList->item (i);
		CHECK_NULL_PTR_ERROR (item)
		if (true == item->isSelected ( ))
		{
			QtQualifDataTypeItem*	dataItem	= dynamic_cast<QtQualifDataTypeItem*>(item);
			if (0 == dataItem)
			{
				INTERNAL_ERROR (exc, "Invalid item type", "QtQualityDividerWidget::getQualifiedTypes")
				throw exc;
			}	// if (0 == dataItem)

			types	|= dataItem->getDataType ( );
		}	// if (true == item->isSelected ( ))
	}	// for (int i = 0; i < itemCount; i++)

	return types;
}	// QtQualityDividerWidget::getQualifiedTypes


void QtQualityDividerWidget::selectQualifiedTypes (size_t types)
{
	assert ((0 != _dataTypesList) && "QtQualityDividerWidget::selectQualifiedTypes : liste des types de données nulle.");
	const int	itemCount	= _dataTypesList->count ( );
	for (int i = 0; i < itemCount; i++)
	{
		QtQualifDataTypeItem*	item	= dynamic_cast<QtQualifDataTypeItem*>(_dataTypesList->item (i));
		CHECK_NULL_PTR_ERROR (item)
		bool					select	= 0 == (types & item->getDataType ( )) ? false : true;
		item->setSelected (select);
	}	// for (int i = 0; i < itemCount; i++)
}	// QtQualityDividerWidget::selectQualifiedTypes


bool QtQualityDividerWidget::coordinatesEvaluation ( ) const
{
	assert ((0 != _coordinatesCheckBox) && "QtQualityDividerWidget::coordinatesEvaluation : null coordinates evaluation check box.");
	return _coordinatesCheckBox->isChecked ( );
}	// QtQualityDividerWidget::coordinatesEvaluation


bool QtQualityDividerWidget::shouldReleaseData ( ) const
{
	assert ((0 != _releaseDataCheckBox) && "QtQualityDividerWidget::shouldReleaseData : null release data check box.");
	return _releaseDataCheckBox->isChecked ( );
}	// QtQualityDividerWidget::shouldReleaseData


static void addToMap (map<size_t, size_t>& dataTypes, size_t type, size_t dim)
{
	for (map<size_t, size_t>::const_iterator it = dataTypes.begin ( );
	     dataTypes.end ( ) != it; it++)
		if (type == (*it).first)
			return;

	dataTypes.insert (pair<size_t, size_t>(type, dim));
}	// addToMap


map<size_t, size_t> QtQualityDividerWidget::getDataTypes ( )
{
	map<size_t, size_t>	dataTypes;

	const size_t	seriesNum	= _series.size ( );
	for (size_t j = 0; j < seriesNum; j++)
	{
		AbstractQualifSerie*	serie	= _series [j];
		CHECK_NULL_PTR_ERROR (serie)

		if (2 > serie->getDimension ( ))
			continue;
		size_t	serieTypes	= serie->getDataTypes ( );
		if (0 != (serieTypes & QualifHelper::TRIANGLE))
			addToMap (dataTypes, QualifHelper::TRIANGLE, 2);
		if (0 != (serieTypes & QualifHelper::QUADRANGLE))
			addToMap (dataTypes, QualifHelper::QUADRANGLE, 2);
		if (0 != (serieTypes & QualifHelper::TETRAEDRON))
			addToMap (dataTypes, QualifHelper::TETRAEDRON, 3);
		if (0 != (serieTypes & QualifHelper::PYRAMID))
			addToMap (dataTypes, QualifHelper::PYRAMID, 3);
		if (0 != (serieTypes & QualifHelper::HEXAEDRON))
			addToMap (dataTypes, QualifHelper::HEXAEDRON, 3);
		if (0 != (serieTypes & QualifHelper::TRIANGULAR_PRISM))
			addToMap (dataTypes, QualifHelper::TRIANGULAR_PRISM, 3);
	}	// for (size_t j = 0; j < seriesNum; j++)

	return dataTypes;
}	// QtQualityDividerWidget::getDataTypes


QHBoxLayout& QtQualityDividerWidget::getButtonsLayout ( )
{
	assert (0 != _buttonsLayout && "QtQualityDividerWidget::getButtonsLayout : null layout.");
	return *_buttonsLayout;
}	// QtQualityDividerWidget::getButtonsLayout


QVBoxLayout& QtQualityDividerWidget::getOptionsLayout ( )
{
	assert (0 != _optionsLayout && "QtQualityDividerWidget::getOptionsLayout : null layout.");
	return *_optionsLayout;
}	// QtQualityDividerWidget::getOptionsLayout


void QtQualityDividerWidget::displayExtraction (size_t i, bool display)
{
	throw Exception ("QtQualityDividerWidget::displayExtraction should be overloaded");
}	// 	QtQualityDividerWidget::displayExtraction


static bool isChecked (QTableWidgetItem& item)
{
	return Qt::Unchecked == item.checkState ( ) ? false : true;
}	// isChecked


void QtQualityDividerWidget::displaySelectedExtractions (bool display)
{
	assert (0 != _seriesExtractionsTableWidget);
	const vector<unsigned char>	selection	= getSelectedSeries ( );
	
	for (vector<unsigned char>::const_iterator it = selection.begin ( ); selection.end ( ) != it; it++)
	{
		QTableWidgetItem*	item	= _seriesExtractionsTableWidget->item (*it, 0);
		if ((0 != item) && (true == item->isSelected ( )) && (display != isChecked (*item)))
		{
			item->setCheckState (true == display ? Qt::Checked : Qt::Unchecked);
			displayExtraction (*it, display);
		}	// if ((0 != item) && (display != item->isSelected ( )))
	}	// for (vector<unsigned char>::const_iterator it = selection.begin ( ); selection.end ( ) != it; it++)
}	// QtQualityDividerWidget::displaySelectedExtractions

	
string QtQualityDividerWidget::getAppTitle ( ) const
{
	return _appTitle;
}	// QtQualityDividerWidget::getAppTitle


void QtQualityDividerWidget::displayErrorMessage (const UTF8String& msg)
{
	QtMessageBox::displayErrorMessage (this, getAppTitle ( ), msg);
}	// QtQualityDividerWidget::displayErrorMessage


QualifRangeTask* QtQualityDividerWidget::createRangeTask (size_t types, Critere criterion, const vector<AbstractQualifSerie*>& series)
{
	return new QualifRangeTask (types, criterion, series);
}	// QtQualityDividerWidget::createRangeTask


QualifAnalysisTask* QtQualityDividerWidget::createAnalysisTask (size_t types, Critere criterion, double min, double max, const vector<AbstractQualifSerie*>& series)
{
	return new QualifAnalysisTask (types, criterion, 1, min, max, true, series);
}	// QtQualityDividerWidget::createAnalysisTask


void QtQualityDividerWidget::updateDomainCallback ( )
{
	assert (0 != _minTextField);
	assert (0 != _maxTextField);
	assert (0 != _domainLabel);
	try
	{
		QtAutoWaitingCursor cursor (true);

		const double	userMin	= getDomainMinValue ( ), userMax	= getDomainMaxValue ( );
		double	min	= 0., max = 1.;
		QualifHelper::getDomain (getCriterion ( ), getQualifiedTypes ( ), min, max);
		UTF8String	domain ("Domaine : ", charset);
		domain << "[" << NumericConversions::shortestRepresentation (min, 2) << ", " << NumericConversions::shortestRepresentation (max, 2) << "]";
		_domainLabel->setText (domain.utf8 ( ).c_str ( ));
		if ((userMin < min) || (userMin >= max))
			_minTextField->setText (QString::number (min));
		if ((userMax > max) || (userMax <= min))
			_maxTextField->setText (QString::number (max));
	}
	catch (...)
	{
	}
}	// QtQualityDividerWidget::updateDomainCallback


void QtQualityDividerWidget::updateSelectableTypesCallback ( )
{
	assert ((0 != _criterionComboBox) && "QtQualityDividerWidget::updateSelectableTypesCallback : null criteria combo box.");
	assert ((0 != _dataTypesList) && "QtQualityDividerWidget::updateSelectableTypesCallback : null types list.");
	assert (0 != _domainLabel);

	BEGIN_TRY_CATCH_BLOCK

	_domainLabel->setText ("Domaine : [, ]");
	Critere		criterion	= getCriterion ( );
	const int	itemCount	= _dataTypesList->count ( );
	for (int i = 0; i < itemCount; i++)
	{
		QListWidgetItem*	item			= _dataTypesList->item (i);
		QtQualifDataTypeItem*	dataItem	= dynamic_cast<QtQualifDataTypeItem*>(item);
		if (0 == dataItem)
		{
			INTERNAL_ERROR (exc, "Invalid type of item", "QtQualityDividerWidget::updateSelectableTypesCallback")
			throw exc;
		}	// if (0 == dataItem)
		bool	selectable	= QualifHelper::isTypeSupported (dataItem->getDataType( ), criterion);
		if ((true == item->isSelected( )) && (false == selectable))
			item->setSelected (false);
		item->setFlags (true == selectable ? Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::NoItemFlags);
	}	// for (int i = 0; i < itemCount; i++)

	COMPLETE_TRY_CATCH_BLOCK
}	// QtQualityDividerWidget::updateSelectableTypesCallback


void QtQualityDividerWidget::displaySerieExtractionCallback (int row, int col)
{
	assert (0 != _seriesExtractionsTableWidget);
	
	BEGIN_TRY_CATCH_BLOCK

	if (0 != col)
		return;
	if (row >= getSeriesNum ( ))
	{
		UTF8String	error (charset);
		error << "Erreur interne. Série numéro " << (unsigned long)(row+1) << " demandée alors qu'il n'y a que " << (unsigned long)getSeriesNum ( ) << " séries.";
		throw Exception (error);
	}	// if (row >= getSeriesNum ( ))

	QTableWidgetItem*	item	= _seriesExtractionsTableWidget->item (row, col);
	CHECK_NULL_PTR_ERROR (item)
	
	const bool	display	= Qt::Checked == item->checkState ( ) ? true : false;
	displayExtraction (row, display);
	
	COMPLETE_TRY_CATCH_BLOCK
}	// QtQualityDividerWidget::displaySerieExtractionCallback


void QtQualityDividerWidget::computeCallback ( )
{
	BEGIN_TRY_CATCH_BLOCK

	if ((getCriterion ( ) != _criterion) && (true == shouldReleaseData ( )))
	{
		const size_t	seriesNum	= _series.size ( );
		for (size_t j = 0; j < seriesNum; j++)
		{
			AbstractQualifSerie*	serie	= _series [j];
			CHECK_NULL_PTR_ERROR (serie)
			serie->releaseStoredData ( );
		}	// for (size_t j = 0; j < seriesNum; j++)
	}	// if ((getCriterion ( ) != _criterion) && ...
	_criterion	= getCriterion ( );

	compute ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// QtQualityDividerWidget::computeCallback


}	// namespace GQualif

