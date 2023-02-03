#include "QtQualif/QtQualifWidget.h"
#include "GQualif/QualifHelper.h"
#include "GQualif/QualifAnalysisTask.h"
#include "GQualif/QCalQualThread.h"

#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/ThreadManager.h>
#include <TkUtil/UTF8String.h>

#include <QLayout>
#include <QHBoxLayout>
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
//                         Classe QtQualifWidget
// ===========================================================================


QtQualifWidget::QtQualifWidget (QWidget* parent, const string& appTitle)
	: QWidget (parent),
	  _histogramPanel (0), _appTitle (appTitle), _series ( ),
	  _theoreticalRadioButton (0), _computedRadioButton (0),
	  _userDefinedRadioButton (0), _minTextField (0), _maxTextField (0),
	  _strictDomainUsageCheckBox (0),
	  _barNumTextField (0), _heightTextField (0), _automaticHeightCheckBox (0),
	  _criterionComboBox (0), _dataTypesList (0),
	  _coordinatesCheckBox (0), _releaseDataCheckBox (0),
	  _buttonsLayout (0), _applyButton (0),
	  _criterion ((Qualif::Critere)-1)
{
	QVBoxLayout*	mainLayout	= new QVBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	// Zone supérieure : l'histogramme :
	_histogramPanel	= new QwtHistogramChartPanel (this);
	mainLayout->addWidget (_histogramPanel, 10);
	_histogramPanel->setScaleLabelFormat ('c');

	// Zone inférieure : paramétrage de l'analyse :
	QHBoxLayout*	hboxLayout	= new QHBoxLayout ( );

	// 1ère ligne : mode de définition du domaine representé en abscisses
	mainLayout->addLayout (hboxLayout);
	_theoreticalRadioButton = new QRadioButton (QSTR ("Théorique"), this);
	_theoreticalRadioButton->setChecked (true);
	_theoreticalRadioButton->setFixedSize (
									_theoreticalRadioButton->sizeHint ( ));
	hboxLayout->addWidget (_theoreticalRadioButton);
	_computedRadioButton	= new QRadioButton (QSTR ("Calculé"), this);
	_computedRadioButton->setFixedSize (_computedRadioButton->sizeHint ( ));
	hboxLayout->addWidget (_computedRadioButton);
	_userDefinedRadioButton	= new QRadioButton (QSTR ("Manuel"), this);
	_userDefinedRadioButton->setFixedSize (
									_userDefinedRadioButton->sizeHint ( ));
	hboxLayout->addWidget (_userDefinedRadioButton);
	hboxLayout->addStretch (10.);
	connect (_theoreticalRadioButton, SIGNAL(clicked ( )), this,
	         SLOT (updateDomainCallback ( )));
	connect (_computedRadioButton, SIGNAL(clicked ( )), this,
	         SLOT (updateDomainCallback ( )));
	connect (_userDefinedRadioButton, SIGNAL(clicked ( )), this,
	         SLOT (updateDomainCallback ( )));

	// 2ème ligne : min + max.
	hboxLayout	= new QHBoxLayout ( );
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
	_minTextField->setWhatsThis (
			QSTR ("Champ de saisie de la valeur minimale à représenter."));
	_minTextField->setToolTip (
			QSTR ("Champ de saisie de la valeur minimale à représenter."));
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
	_maxTextField->setWhatsThis (
			QSTR ("Champ de saisie de la valeur maximale à représenter."));
	_maxTextField->setToolTip (
			QSTR ("Champ de saisie de la valeur maximale à représenter."));
	_strictDomainUsageCheckBox = new QCheckBox (QSTR ("Restreindre au domaine"), this);
	_strictDomainUsageCheckBox->setChecked (true);
	hboxLayout->addWidget (_strictDomainUsageCheckBox);	
	_strictDomainUsageCheckBox->setFixedSize (
								_strictDomainUsageCheckBox->sizeHint ( ));
	_strictDomainUsageCheckBox->setWhatsThis (
		QSTR ("Coché les mailles dont le critère sort du domaine ne sont pas prises en compte dans les classes."));
	_strictDomainUsageCheckBox->setToolTip (
		QSTR ("Coché les mailles dont le critère sort du domaine ne sont pas prises en compte dans les classes."));
	hboxLayout->addStretch (10.);
	
	// 3-ème ligne : nombre de classes :
	hboxLayout	= new QHBoxLayout ( );
	mainLayout->addLayout (hboxLayout);
	label	= new QLabel (QSTR (" Nombre de classes :"), this);
	hboxLayout->addWidget (label);
	_barNumTextField	= new QtTextField ("10", this);
	hboxLayout->addWidget (_barNumTextField);
	_barNumTextField->setVisibleColumns (3);
	QIntValidator*	barValidator	=
							new QIntValidator (0, 100, _barNumTextField);
	_barNumTextField->setValidator (barValidator);
	_barNumTextField->setFixedSize (_barNumTextField->sizeHint ( ));
	_barNumTextField->setToolTip (
			QSTR ("Champ de saisie du nombre de classes de l'histogramme."));
	_barNumTextField->setWhatsThis (
			QSTR ("Champ de saisie du nombre de classes de l'histogramme."));
	hboxLayout->addStretch (10.);

	// 4-ème ligne : hauteur maximale
	hboxLayout	= new QHBoxLayout ( );
	mainLayout->addLayout (hboxLayout);
	label	 = new QLabel (QSTR (" Hauteur      :"), this);
	hboxLayout->addWidget (label);
	_heightTextField	= new QtTextField (this);
	hboxLayout->addWidget (_heightTextField);
	QIntValidator*	heightValidator	=
							new QIntValidator (1, MAXINT, _heightTextField);
	_heightTextField->setValidator (heightValidator);
	_heightTextField->setVisibleColumns (10);
	_heightTextField->setFixedSize (_heightTextField->sizeHint ( ));
	_heightTextField->setToolTip (
			QSTR ("Champ de saisie de la valeur maximale à afficher."));
	_heightTextField->setWhatsThis (
			QSTR ("Champ de saisie de la valeur maximale à afficher."));
	_automaticHeightCheckBox	= new QCheckBox (QSTR ("Automatique"), this);
	hboxLayout->addWidget (_automaticHeightCheckBox);	
	_automaticHeightCheckBox->setFixedSize (
								_automaticHeightCheckBox->sizeHint ( ));
	double	min	= 0., max = 0.;
	bool	autoScale	= true;
	_histogramPanel->getOrdinateScale (min, max, autoScale);
	_automaticHeightCheckBox->setChecked (autoScale);
	_heightTextField->setEnabled (false);
	_automaticHeightCheckBox->setToolTip (
		QSTR ("Activé, l'histogramme est totalement affiché. Masqué, les séries sont tronquées à la hauteur définie."));
	_automaticHeightCheckBox->setWhatsThis (
		QSTR ("Activé, l'histogramme est totalement affiché. Masqué, les séries sont tronquées à la hauteur définie."));
	connect (_automaticHeightCheckBox, SIGNAL(clicked ( )), this,
	         SLOT (automaticHeightCallback ( )));
	hboxLayout->addStretch (10.);

	// 5-ième ligne : critère :
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
	_criterionComboBox->setWhatsThis (QSTR ("Critère de qualité à appliquer."));
	connect (_criterionComboBox, SIGNAL(activated (int)), this,
	         SLOT (updateDomainCallback ( )));
	connect (_criterionComboBox, SIGNAL(activated (int)), this,
	         SLOT (updateSelectableTypesCallback ( )));
	hboxLayout->addStretch (10.);

	// 6-ième ligne : sélection :
	hboxLayout	= new QHBoxLayout ( );
	mainLayout->addLayout (hboxLayout);
	_dataTypesList	= new QListWidget (this);
	_dataTypesList->setSelectionMode (QListWidget::ExtendedSelection);
	_dataTypesList->setSortingEnabled (false);
	hboxLayout->addWidget (_dataTypesList);
	QAbstractItemModel*	dataTypeModel	= _dataTypesList->model ( );
	dataTypeModel->setHeaderData (
						0, Qt::Horizontal, QVariant(QSTR ("Types de données")));
	connect (_dataTypesList, SIGNAL(itemSelectionChanged ( )), this,
	         SLOT (updateDomainCallback ( )));
	_dataTypesList->setToolTip (
			QSTR ("Liste des types de mailles à prendre en compte lors du traitement."));
	_dataTypesList->setWhatsThis (
			QSTR ("Liste des types de mailles à prendre en compte lors du traitement."));
	QVBoxLayout*	vlayout	= new QVBoxLayout ( );
	hboxLayout->addLayout (vlayout);
	_coordinatesCheckBox	= new QCheckBox (QSTR ("Validité des coordonnées"), this);
	vlayout->addWidget (_coordinatesCheckBox);	
	_coordinatesCheckBox->setFixedSize (_coordinatesCheckBox->sizeHint ( ));
	_coordinatesCheckBox->setChecked (true);
	_coordinatesCheckBox->setToolTip (
		QSTR ("Activé, effectue une analyse préalable des coordonnées des noeuds des mailles."));
	_coordinatesCheckBox->setWhatsThis (
		QSTR ("Activé, effectue une analyse préalable des coordonnées des noeuds des mailles."));
	_releaseDataCheckBox	= new QCheckBox (QSTR ("Libérer la mémoire après chaque critère"), this);
	vlayout->addWidget (_releaseDataCheckBox);	
	_releaseDataCheckBox->setFixedSize (_releaseDataCheckBox->sizeHint ( ));
	_releaseDataCheckBox->setChecked (true);
	_releaseDataCheckBox->setToolTip (
		QSTR ("Activé, libère la mémoire utilisée en cache à chaque changement de critère."));
	_releaseDataCheckBox->setWhatsThis (
		QSTR ("Activé, libère la mémoire utilisée en cache à chaque changement de critère."));
	hboxLayout->addStretch (10.);
	vlayout->addStretch (10.);

	// 7-ième ligne : boutons :
	_buttonsLayout	= new QHBoxLayout ( );
	mainLayout->addLayout (_buttonsLayout);
	_applyButton	= new QPushButton (QSTR ("Calculer"), this);
	_buttonsLayout->addWidget (_applyButton);
	_applyButton->setFixedSize (_applyButton->sizeHint ( ));
	connect (_applyButton, SIGNAL(clicked ( )), this,
	         SLOT(computeCallback ( )));
	_applyButton->setToolTip (QSTR ("Provoque la mise à jour de l'histogramme."));
	_applyButton->setWhatsThis (QSTR ("Provoque la mise à jour de l'histogramme."));
	_buttonsLayout->addStretch (10.);
	setFocusPolicy (Qt::StrongFocus);

	layout ( );
}	// QtQualifWidget::QtQualifWidget


QtQualifWidget::QtQualifWidget (const QtQualifWidget&)
	: QWidget (0),
	  _histogramPanel (0), _appTitle ("Invalid application"), _series ( ),
	  _theoreticalRadioButton (0), _computedRadioButton (0),
	  _userDefinedRadioButton (0), _minTextField (0), _maxTextField (0),
	  _strictDomainUsageCheckBox (0),
	  _barNumTextField (0), _heightTextField (0), _automaticHeightCheckBox (0),
	  _criterionComboBox (0), _dataTypesList (0),
	  _coordinatesCheckBox (0), _releaseDataCheckBox (0),
	  _buttonsLayout (0), _applyButton (0),
	  _criterion ((Qualif::Critere)-1)
{
	assert (0 && "QtQualifWidget copy constructor is not allowed.");
}	// QtQualifWidget::QtQualifWidget


QtQualifWidget& QtQualifWidget::operator = (const QtQualifWidget&)
{
	assert (0 && "QtQualifWidget assignment operator is not allowed.");
	return *this;
}	// QtQualifWidget::operator =


QtQualifWidget::~QtQualifWidget ( )
{
	removeSeries ( );
}	// QtQualifWidget::~QtQualifWidget


QwtHistogramChartPanel& QtQualifWidget::getHistogramPanel ( )
{
	CHECK_NULL_PTR_ERROR (_histogramPanel)
	return *_histogramPanel;
}	//  QtQualifWidget::getHistogramPanel


const QwtHistogramChartPanel& QtQualifWidget::getHistogramPanel( ) const
{
	CHECK_NULL_PTR_ERROR (_histogramPanel)
	return *_histogramPanel;
}	//  QtQualifWidget::getHistogramPanel


void QtQualifWidget::addSerie (AbstractQualifSerie* serie)
{
	assert (0 != _dataTypesList && "QtQualifWidget::addSerie : null data list widget.");
	if (0 == serie)
	{
		INTERNAL_ERROR (exc, UTF8String ("Série nulle.", charset), "QtQualifWidget::addSerie")
		throw exc;
	}	// if (0 == serie)

	_series.push_back (serie);

	_dataTypesList->clear ( );
	map<size_t, size_t>	types	= getDataTypes ( );
	for (map<size_t, size_t>::iterator itt = types.begin ( );
	     types.end ( ) != itt; itt++)
	{
		QtQualifDataTypeItem*	item	=
			new QtQualifDataTypeItem (_dataTypesList, (*itt).first, (*itt).second);
		bool	selectable	= QualifHelper::isTypeSupported ((*itt).first);
		item->setFlags (true == selectable ?
					Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::NoItemFlags);
		if ((true == selectable) && (1 == types.size ( )))
			item->setSelected (true);
	}	// for (map<size_t, size_t>::iterator itt ...
	_dataTypesList->setFixedSize (_dataTypesList->sizeHint ( ));
}	// QtQualifWidget::addSerie


void QtQualifWidget::removeSeries (bool clearHistogram)
{
	assert (0 != _dataTypesList && "QtQualifWidget::removeSerie : null data list widget.");
	const size_t	seriesNum	= _series.size ( );
	if (0 == seriesNum)
		return;	// Surtout ne rien faire, Qwt a horreur du vide.

	for (size_t i = 0; i < seriesNum; i++)
		delete _series [i];
	_series.clear ( );
	_dataTypesList->clear ( );

	if (false == clearHistogram)
		return;

	// ========================================================================
	// Ici on a donc true == clearHistogram.
	// Le widget ne doit pas être en cours de destruction sinon plantage
	// prochain dans Qwt.
	// ========================================================================

	// On donne du grain à moudre à Qwt, qui, comme dit ci-dessus, à horreur
	// du vide. Si absence de courbe, se lance alors dans des calculs pas
	// possibles avec des valeurs infinies dans les calculs des axes. Le
	// programme se termine là alors, avec un plantage du à un assert, ou
	// éventuellement une exception type bad_alloc dans le calcul des ticks.
	QList<QwtText>					seriesNames;
	QVector<double>					classValues, serie0;
	QVector< QVector <double> >		seriesValues;
	seriesNames.append (QwtText (""));
	classValues.push_back (0.);
	serie0.push_back (0.);
	seriesValues.push_back (serie0);
	getHistogramPanel ( ).setSeries (seriesNames, classValues, seriesValues);
}	// QtQualifWidget::removeSeries


static void computeXDomain (
			double& min, double& max, double& interval, double& cnRatio,
			bool& autoScale, unsigned char classNum)
{	// Si min et ou max invalide => problèmes, et Qwt aura du mal.
	// Si min = -inf et max = +inf alors max - min est hors domaine ...
	if (false == NumericServices::isValidAndNotMax (min))
	{
		min			= -NumericServices::doubleMachMax ( ) / 10.;
		autoScale	= false;
	}
	if (false == NumericServices::isValidAndNotMax (max))
	{
		max			= NumericServices::doubleMachMax ( ) / 10.;
		autoScale	= false;
	}
	if ((fabs (interval) <= NumericServices::doubleMachEpsilon ( )) ||
	    (false == NumericServices::isValid (interval)))
	{
		interval	= max / classNum - min / classNum;
		autoScale	= false;
	}
	cnRatio	= max / classNum - min / classNum;
}	// computeXDomain


void QtQualifWidget::updateHistogram ( )
{
	assert (0 != _applyButton && "QtQualifWidget::updateHistogram : null apply button.");
	assert (0 != _userDefinedRadioButton && "QtQualifWidget::updateHistogram : null user defined button.");
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
				message << "Erreur avec la série " << serie->getName ( ) << " :"
				        << "\n" << exc.getFullMessage ( );
				throw (Exception (message));
			}
		}	// if (true == coordinatesEvaluation ( ))
	}	// if (true == coordinatesEvaluation ( ))

	// Récupération des paramètres de présentation, application à l'histogramme,
	// et on refait les classes.
	QwtHistogramChartPanel&	histogramPanel		= getHistogramPanel ( );
	const Critere					criterion	= getCriterion ( );
	const unsigned char				classNum	= getClassesNum ( );
	const size_t					cellTypes	= getQualifiedTypes ( );
	const size_t					maxHeight	= false==isAutomaticHeight ( ) ?
												  getMaxHeight ( ) : 1;
	QList<QwtText>					seriesNames;
	double							min	= 0., max	= 1., interval	= max - min;

	if (0 == cellTypes)
		throw Exception (UTF8String ("Impossibilité de recalculer l'histogramme : absence de types de mailles sélectionné.", charset));

	// Domaine d'application :
	// Si domaine calculé, on prend le domaine théorique, puis on récupère
	// min et max calculés et on refait le classement.
	if (true == _computedRadioButton->isChecked ( ))
		getDataRange (min, max);
	else if (true == _theoreticalRadioButton->isChecked ( ))
		QualifHelper::getDomain (criterion, cellTypes, min, max);
	else
	{
		min	= getDomainMinValue ( );
		max	= getDomainMaxValue ( );
	}
//	interval	= (max - min) / classNum;	// Si min = -inf et max = +inf ...
	interval		= max / classNum - min / classNum;
	double	cnRatio	= 1.;
	bool	autoScale	= _theoreticalRadioButton->isChecked ( );
	computeXDomain (min, max, interval, cnRatio, autoScale, classNum);
	// Rem CP : attention, on peut avoir des problèmes d'arrondis ci-dessous
	// type setText (min + epsilon) => le passage en mode manuel peut réserver
	// des surprises.
	_minTextField->setText (QString::number (min));
	_maxTextField->setText (QString::number (max));
	if ((false == autoScale) && (true == _theoreticalRadioButton->isChecked( )))
	{
		_theoreticalRadioButton->setChecked (false);
		_userDefinedRadioButton->setChecked (true);
		_minTextField->setEnabled (true);
		_maxTextField->setEnabled (true);
		updateHistogram ( );
		return;
	}
	for (i = 0; i < seriesNum; i++)
	{
		AbstractQualifSerie*	serie	= _series [i];
		CHECK_NULL_PTR_ERROR (serie)
		seriesNames.append (QwtText (serie->getName ( ).c_str ( )));
	}	// for (i = 0; i < seriesNum; i++)

	// Version 0.20.0 : mode strict possible, on ne considère que les mailles
	// donc le critère est dans le domaine.
	const bool	strictMode	=
		(true == _userDefinedRadioButton->isChecked ( )) &&
		(true == _strictDomainUsageCheckBox->isChecked ( )) ? true : false;
	// Version 0.20.0 : exécution dans une tâche Qualif :
	unique_ptr<QualifAnalysisTask>	task (createAnalysisTask (
				cellTypes, criterion, classNum, min, max, strictMode, _series));
	task->execute ( );

	// Les séries de l'histogramme :
	QVector<double>					classValues;
	QVector< QVector <double> >		seriesValues;
	seriesValues.resize (classNum);
    const vector< vector <size_t> >&	values	= task->getSeriesValues ( );
	for (i = 0; i < classNum; i++)
	{
		const double	classValue	= min + interval / 2. + i * interval;
//		classValues.push_back (min + i * interval);
		classValues.push_back (classValue);	// v 1.3.0
		seriesValues [i].resize (seriesNum);
		for (size_t j = 0; j < seriesNum; j++)
			seriesValues [i][j]	= (double)values [i][j];
	}
	// Cas particulier : VALIDITY, le critère vaut 0 ou 1.
	if ((VALIDITY == criterion) && (2 == classNum))
	{
		classValues [0]	= 0.;
		classValues [1]	= 1.;
	}	// if ((VALIDITY == criterion) && (2 == classNum))
	getHistogramPanel ( ).setSeries (seriesNames, classValues, seriesValues);

	// Redéfinition des axes : après setSeries of course :
	double	xTickStep	= histogramPanel.getAbscissaStep ( );
	double	yTickStep	= histogramPanel.getOrdinateStep ( );

	xTickStep	= interval;
	histogramPanel.setAbscissaScale (min, max, autoScale, xTickStep);
	histogramPanel.setOrdinateScale (
						 0, maxHeight, isAutomaticHeight ( ), yTickStep);
	histogramPanel.dataModified ( );

	COMPLETE_TRY_CATCH_BLOCK

	_applyButton->setEnabled (true);
}	// QtQualifWidget::updateHistogram


Qualif::Critere QtQualifWidget::getCriterion ( ) const
{
	assert ((0 != _criterionComboBox) && "QtQualifWidget::getCriterion : combo box nulle.");
	return (Critere)_criterionComboBox->currentIndex ( );
}	// QtQualifWidget::getCriterion


double QtQualifWidget::getDomainMinValue ( ) const
{
	assert ((0 != _minTextField) && "QtQualifWidget::getDomainMinValue : null textfield.");
	if (true == _minTextField->text ( ).isEmpty ( ))
		throw Exception (UTF8String ("Absence de valeur minimale saisie pour le domaine.", charset));

	bool	ok	= true;
	return _minTextField->text ( ).toDouble (&ok);
}   // QtQualifWidget::getDomainMinValue


double QtQualifWidget::getDomainMaxValue ( ) const
{
	assert ((0 != _maxTextField) && "QtQualifWidget::getDomainMaxValue : null textfield.");
	if (true == _maxTextField->text ( ).isEmpty ( ))
		throw Exception (UTF8String ("Absence de valeur maximale saisie pour le domaine.", charset));

	bool	ok	= true;
	return _maxTextField->text ( ).toDouble (&ok);
}   // QtQualifWidget::getDomainMaxValue


void QtQualifWidget::getDataRange (double& min, double& max)
{
	const size_t			cellTypes	= getQualifiedTypes ( );
	const Qualif::Critere		criterion	= getCriterion ( );
	unique_ptr<QualifRangeTask>	task (createRangeTask (cellTypes, criterion, _series));
	task->execute ( );
	task->getRange (min, max);
}	// QtQualifWidget::getDataRange


bool QtQualifWidget::useStrictDomain ( ) const
{
	assert ((0 != _strictDomainUsageCheckBox) && "QtQualifWidget::useStrictDomain : null strict domain checkbox.");
	return _strictDomainUsageCheckBox->isChecked ( );
}	// QtQualifWidget::useStrictDomain


unsigned char QtQualifWidget::getClassesNum ( ) const
{
	assert ((0 != _barNumTextField) && "QtQualifWidget::getClassesNum : null textfield.");
	if (true == _barNumTextField->text ( ).isEmpty ( ))
		throw Exception (UTF8String ("Absence de nombre de classes saisi.", charset));

	bool	ok	= true;
	return (unsigned char)(_barNumTextField->text ( ).toULong (&ok));
}	// QtQualifWidget::getClassesNum


unsigned char QtQualifWidget::getSeriesNum ( ) const
{
	return _series.size ( );
}	// QtQualifWidget::getSeriesNum


const AbstractQualifSerie& QtQualifWidget::getSerie (unsigned char i) const
{
	if (i >= _series.size ( ))
	{
		UTF8String	error (charset);
		error << "Impossibilité d'obtenir la " << (unsigned long)i
		      << "-ème série : "
		      << "ce panneau n'en gère que " << (_series.size ( ) + 1)
		      << ".";
		throw Exception (error);
	}	// if (i >= _series.size ( ))

	return *(_series [i]);
}	// QtQualifWidget::getSerie


AbstractQualifSerie& QtQualifWidget::getSerie (unsigned char i)
{
	if (i >= _series.size ( ))
	{
		UTF8String	error (charset);
		error << "Impossibilité d'obtenir la " << (unsigned long)i
		      << "-ème série : "
		      << "ce panneau n'en gère que " << (_series.size ( ) + 1)
		      << ".";
		throw Exception (error);
	}	// if (i >= _series.size ( ))

	return *(_series [i]);
}	// QtQualifWidget::getSerie


bool QtQualifWidget::isAutomaticHeight ( ) const
{
	assert ((0 != _automaticHeightCheckBox) && "QtQualifWidget::isAutomaticHeight : null automatic height check box.");
	return _automaticHeightCheckBox->isChecked ( );
}	// QtQualifWidget::isAutomaticHeight


size_t QtQualifWidget::getMaxHeight ( ) const
{
	assert ((0 != _heightTextField) && "QtQualifWidget::getMaxHeight : null height textfield.");
	if (true == _heightTextField->text ( ).isEmpty ( ))
		throw Exception (UTF8String ("Absence de hauteur maximale saisie.", charset));

	bool	ok	= true;
	return (size_t)(_heightTextField->text ( ).toULong (&ok));
}	// QtQualifWidget::getMaxHeight


size_t QtQualifWidget::getQualifiedTypes ( ) const
{
	assert ((0 != _dataTypesList) && "QtQualifWidget::getQualifiedTypes : liste des types de données nulle.");
	size_t	types	= 0;

	const int	itemCount	= _dataTypesList->count ( );
	for (int i = 0; i < itemCount; i++)
	{
		QListWidgetItem*	item	= _dataTypesList->item (i);
		CHECK_NULL_PTR_ERROR (item)
		if (true == item->isSelected ( ))
		{
			QtQualifDataTypeItem*	dataItem	=
									dynamic_cast<QtQualifDataTypeItem*>(item);
			if (0 == dataItem)
			{
				INTERNAL_ERROR (exc, "Invalid item type", "QtQualifWidget::getQualifiedTypes")
				throw exc;
			}	// if (0 == dataItem)

			types	|= dataItem->getDataType ( );
		}	// if (true == item->isSelected ( ))
	}	// for (int i = 0; i < itemCount; i++)

	return types;
}	// QtQualifWidget::getQualifiedTypes


void QtQualifWidget::selectQualifiedTypes (size_t types)
{
	assert ((0 != _dataTypesList) && "QtQualifWidget::selectQualifiedTypes : liste des types de données nulle.");
	const int	itemCount	= _dataTypesList->count ( );
	for (int i = 0; i < itemCount; i++)
	{
		QtQualifDataTypeItem*	item	=
				dynamic_cast<QtQualifDataTypeItem*>(_dataTypesList->item (i));
		CHECK_NULL_PTR_ERROR (item)
		bool					select	= 0 == (types & item->getDataType ( )) ?
									  false : true;
		item->setSelected (select);
	}	// for (int i = 0; i < itemCount; i++)
}	// QtQualifWidget::selectQualifiedTypes


bool QtQualifWidget::coordinatesEvaluation ( ) const
{
	assert ((0 != _coordinatesCheckBox) && "QtQualifWidget::coordinatesEvaluation : null coordinates evaluation check box.");
	return _coordinatesCheckBox->isChecked ( );
}	// QtQualifWidget::coordinatesEvaluation


bool QtQualifWidget::shouldReleaseData ( ) const
{
	assert ((0 != _releaseDataCheckBox) && "QtQualifWidget::shouldReleaseData : null release data check box.");
	return _releaseDataCheckBox->isChecked ( );
}	// QtQualifWidget::shouldReleaseData


QtQualifWidget::DOMAIN_TYPE QtQualifWidget::getDomainType ( ) const
{
	assert (0 != _theoreticalRadioButton);
	assert (0 != _computedRadioButton);
	assert (0 != _userDefinedRadioButton);

	if (_theoreticalRadioButton->isChecked ( ))
		return QtQualifWidget::THEORETICAL_DOMAIN;
	else if (true == _computedRadioButton->isChecked ( ))
		return QtQualifWidget::COMPUTED_DOMAIN;

	return QtQualifWidget::USER_DEFINED_DOMAIN;
}	// QtQualifWidget::getDomainType


void QtQualifWidget::setDomainType (QtQualifWidget::DOMAIN_TYPE type)
{
	assert (0 != _theoreticalRadioButton);
	assert (0 != _computedRadioButton);
	assert (0 != _userDefinedRadioButton);

	switch (type)
	{
		case QtQualifWidget::THEORETICAL_DOMAIN	:
			_theoreticalRadioButton->setChecked (true);	break;
		case QtQualifWidget::COMPUTED_DOMAIN	:
			_computedRadioButton->setChecked (true);	break;
		default	:
			_userDefinedRadioButton->setChecked (true);	break;
	}	// switch (type)
}	// QtQualifWidget::setDomainType


static void addToMap (map<size_t, size_t>& dataTypes, size_t type, size_t dim)
{
	for (map<size_t, size_t>::const_iterator it = dataTypes.begin ( );
	     dataTypes.end ( ) != it; it++)
		if (type == (*it).first)
			return;

	dataTypes.insert (pair<size_t, size_t>(type, dim));
}	// addToMap


map<size_t, size_t> QtQualifWidget::getDataTypes ( )
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
}	// QtQualifWidget::getDataTypes


QHBoxLayout& QtQualifWidget::getButtonsLayout ( )
{
	assert (0 != _buttonsLayout && "QtQualifWidget::getButtonsLayout : null layout.");
	return *_buttonsLayout;
}	// QtQualifWidget::getButtonsLayout


string QtQualifWidget::getAppTitle ( ) const
{
	return _appTitle;
}	// QtQualifWidget::getAppTitle


void QtQualifWidget::displayErrorMessage (const UTF8String& msg)
{
	QtMessageBox::displayErrorMessage (this, getAppTitle ( ), msg);
}	// QtQualifWidget::displayErrorMessage


QualifRangeTask* QtQualifWidget::createRangeTask (
	size_t types, Critere criterion, const vector<AbstractQualifSerie*>& series)
{
	return new QualifRangeTask (types, criterion, series);
}	// QtQualifWidget::createAnalysisTask


QualifAnalysisTask* QtQualifWidget::createAnalysisTask (
	size_t types, Critere criterion, size_t classNum, double min, double max,
	bool strict, const vector<AbstractQualifSerie*>& series)
{
	return new QualifAnalysisTask (
						types, criterion, classNum, min, max, strict, series);
}	// QtQualifWidget::createAnalysisTask


void QtQualifWidget::updateDomainCallback ( )
{
	assert (0 != _theoreticalRadioButton);
	assert (0 != _computedRadioButton);
	assert (0 != _userDefinedRadioButton);
	assert (0 != _minTextField);
	assert (0 != _maxTextField);
	assert (0 != _strictDomainUsageCheckBox);
	try
	{
		QtAutoWaitingCursor cursor (true);

		if (true == _userDefinedRadioButton->isChecked ( ))
		{
			_minTextField->setEnabled (true);
			_maxTextField->setEnabled (true);
			_strictDomainUsageCheckBox->setEnabled (true);
			return;
		}	// if (true == _userDefinedRadioButton->isChecked ( ))
		_minTextField->setEnabled (false);
		_maxTextField->setEnabled (false);
		_strictDomainUsageCheckBox->setEnabled (false);

		double	min	= 0., max = 1.;
		if (true == _theoreticalRadioButton->isChecked ( ))
		{
			QualifHelper::getDomain (
							getCriterion ( ), getQualifiedTypes ( ), min, max);
			_minTextField->setText (QString::number (min));
			_maxTextField->setText (QString::number (max));
		}
		else	// => calcule
		{
			bool	autoScale	= true;
			QwtHistogramChartPanel&	histogramPanel	= getHistogramPanel ( );
			histogramPanel.getAbscissaScale (min, max, autoScale);
			_minTextField->setText (QString::number (min));
			_maxTextField->setText (QString::number (max));
		}
	}
	catch (...)
	{
		// Rem : theorique peut etre coche alors qu'aucun type de maille
		// n'est encore defini.
	}
}	// QtQualifWidget::updateDomainCallback


void QtQualifWidget::automaticHeightCallback ( )
{
	assert ((0 != _automaticHeightCheckBox) && "QtQualifWidget::automaticHeightCallback : null automatic height check box.");
	assert ((0 != _heightTextField) && "QtQualifWidget::automaticHeightCallback : null height textfield.");
	assert ((0 != _histogramPanel) && "QtQualifWidget::automaticHeightCallback : null histogram panel.");

	BEGIN_TRY_CATCH_BLOCK

	bool	autoScale	= true;
	double	min	= 0., max = 0.;
	_histogramPanel->getOrdinateScale (min, max, autoScale);
	_heightTextField->setEnabled (!_automaticHeightCheckBox->isChecked ( ));
	if (false == _automaticHeightCheckBox->isChecked ( ))
		_heightTextField->setText (QString ( ).setNum (max));
	autoScale	= _automaticHeightCheckBox->isChecked ( );
	if (false == autoScale)
		max	= getMaxHeight ( );
	_histogramPanel->setOrdinateScale (
					min, max, autoScale, _histogramPanel->getOrdinateStep ( ));

	COMPLETE_TRY_CATCH_BLOCK
}	// QtQualifWidget::automaticHeightCallback


void QtQualifWidget::updateSelectableTypesCallback ( )
{
	assert ((0 != _criterionComboBox) && "QtQualifWidget::updateSelectableTypesCallback : null criteria combo box.");
	assert ((0 != _dataTypesList) && "QtQualifWidget::updateSelectableTypesCallback : null types list.");
	assert ((0 != _barNumTextField) && "QtQualifWidget::updateSelectableTypesCallback : null bar num textfield.");
	assert ((0 != _theoreticalRadioButton) && "QtQualifWidget::updateSelectableTypesCallback : null bar num textfield.");

	BEGIN_TRY_CATCH_BLOCK

	Critere		criterion	= getCriterion ( );
	const int	itemCount	= _dataTypesList->count ( );
	for (int i = 0; i < itemCount; i++)
	{
		QListWidgetItem*	item		= _dataTypesList->item (i);
		QtQualifDataTypeItem*	dataItem=
									dynamic_cast<QtQualifDataTypeItem*>(item);
		if (0 == dataItem)
		{
			INTERNAL_ERROR (exc, "Invalid type of item", "QtQualifWidget::updateSelectableTypesCallback")
			throw exc;
		}	// if (0 == dataItem)
		bool	selectable	=
			QualifHelper::isTypeSupported (dataItem->getDataType( ), criterion);
		if ((true == item->isSelected( )) && (false == selectable))
			item->setSelected (false);
		item->setFlags (true == selectable ?
					Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::NoItemFlags);
	}	// for (int i = 0; i < itemCount; i++)

	if (VALIDITY == criterion)
	{	// Cas binaire, 0 ou 1, on propose le défaut suivant :
		_barNumTextField->setText (QString ("2"));
		_theoreticalRadioButton->setChecked (true);
		updateDomainCallback ( );
	}	// if (VALIDITY == criterion)

	COMPLETE_TRY_CATCH_BLOCK
}	// QtQualifWidget::updateSelectableTypesCallback


void QtQualifWidget::computeCallback ( )
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

	updateHistogram ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// QtQualifWidget::computeCallback


}	// namespace GQualif

