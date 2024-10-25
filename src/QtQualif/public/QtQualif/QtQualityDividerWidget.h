#ifndef QT_QUALITY_DIVIDER_WIDGET_H
#define QT_QUALITY_DIVIDER_WIDGET_H

#include "GQualif/QualifAnalysisTask.h"
#include "GQualif/QualifRangeTask.h"

#include <QtUtil/QtGroupBox.h>
#include <QtUtil/QtTextField.h>

#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTableWidget>

namespace GQualif
{

/**
 * <P>Classe de Widget <I>Qt</I> permettant de rechercher dans un maillage les mailles correspondant à un certain profil de qualité
 * (domaine min-max d'un critère de qualité), l'objectif étant de rechercher les mailles invalides dans une botte de foin !
 * L'accès aux données est effectué à l'aide d'une instance de la classe <I>AbstractQualifSerie</I>, indépendante du type de maillage analysé.
  * </P>
 *
 * <P>Cette classe est à surcharger afin de récupérer et afficher le résultat de l'extraction dans un outil dédié.</P>
 *
 * <P>Les méthodes de cette classe sont susceptibles de lever des exceptions de type <I>TkUtil::Exception</I>.</P>
 *
 * <P>En environnement multithread cette classe exécute si possible les analyses de chaque série dans des threads différents. <U>Requiert dans ce cas que
 * <I>ThreadManager::initialize</I> ait été préalablement appelé.
 * </P>
 *
 * @since	4.4.0
 * @see		AbstractQualifSerie
 */
class QtQualityDividerWidget : public QWidget
{
	Q_OBJECT

	public :

	/**
	 * Constructeur.
	 * @param		Widget parent.
	 * @param		Titre de l'application (pour les messages).
	 */
	QtQualityDividerWidget (QWidget* parent, const IN_STD string& appTitle);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtQualityDividerWidget ( );

	/**
	 * Recense la série transmise en argument.
	 * Cette série est adoptée et donc détruite à la destruction du widget.
	 * @see			removeSeries
	 */
	virtual void addSerie (AbstractQualifSerie* serie);

	/**
 	 * Détruit (operateur <I>delete</I>) toutes les séries.
 	 * @see			addSerie
 	 */
	virtual void removeSeries (bool clear = false);

	/**
	 * Actualise les extractions conformément aux paramètres renseignés dans l'IHM.
	 */
	virtual void compute ( );

	/**
	 * @return		Le critère <I>Qualif</I> appliqué pour l'analyse.
	 */
	virtual Qualif::Critere getCriterion ( ) const;

	/**
	 * @return		La valeur minimale (saisie) du domaine d'échantillonnage.
	 * @see			getDomainMinValue
	 */
	virtual double getDomainMinValue ( ) const;

	/**
	 * @return		La valeur maximale (saisie) du domaine d'échantillonnage.
	 * @see			getDomainMinValue
	 */
	virtual double getDomainMaxValue ( ) const;

	/**
	 * @return		Le nombre de séries analysées.
	 * @see			getSerie
	 */
	virtual unsigned char getSeriesNum ( ) const;

	/**
	 * @return		La <I>i-ème</I> série prise en charge.
	 * @see			getSeriesNum
	 */
	virtual const AbstractQualifSerie& getSerie (unsigned char i) const;
	virtual AbstractQualifSerie& getSerie (unsigned char i);
	
	/**
	 * @return		Les indices des séries sélectionnées.
	 */
	virtual IN_STD vector<unsigned char> getSelectedSeries ( ) const;

	/**
	 * @return		Les types de données à soumettre au traitement.
	 *				La valeur retournée est issue d'un ou exclusif entre les types unitaires (QualifHelper::TRIANGLE, ...) sélectionnés par l'utilisateur.
	 * @see			selectQualifiedTypes (size_t types);
	 */
    virtual size_t getQualifiedTypes ( ) const;

	/**
	 * @param		Types de données à sélectionner.
	 * @see			getQualifiedTypes
	 */
	virtual void selectQualifiedTypes (size_t types);

	/**
	 * @return		une table contenant les associations type de maille - dimension des mailles.
	 */
	virtual IN_STD map<size_t, size_t> getDataTypes ( );

	/**
	 * @return		<I>true</I> si les coordonnées des noeuds doivent être préalblement évaluées, <I>false</I> dans le cas contraire.
	 */
	virtual bool coordinatesEvaluation ( ) const;

	/**
	 * @return		<I>true</I> si les données en cache doivent être libérées à chaque changement de critère, <I>false</I> s'il faut les
	 * 				conserver (en vue d'un retour à un critère déjà exploré).
	 */
	virtual bool shouldReleaseData ( ) const;

	/**
	 * Personnalisation de l'IHM : ajouter un bouton. Le parent du bouton doit être a priori cette instance, il convient ensuite de l'ajouter au
	 * gestionnaire de mise en forme du bandeau horizontal de boutons. Ex :<BR>
	 * <CODE>
	 * QtQualityDividerWidget*	qualifWidget = new QtQualityDividerWidget (...);
	 * QPushButton*		button	= new QPushButton ("Afficher", qualifWidget);
	 * qualifWidget->getButtonsLayout ( ).addWidget (button);
	 * connect (button, ...);
	 * </CODE>
	 * @return		Une référence sur le gestionnaire de mise en forme du bandeau horizontal de boutons.
	 */
	virtual QHBoxLayout& getButtonsLayout ( );
	
	/**
	 * Personnalisation de l'IHM : ajouter un case à cocher "option". Le parent du bouton doit être a priori cette instance, il convient ensuite
	 * de l'ajouter au gestionnaire de mise en forme du bandeau vertical de cases à cocher. Ex :<BR>
	 * <CODE>
	 * QtQualityDividerWidget*	qualifWidget = new QtQualityDividerWidget (...);
	 * QCheckBox*		wireCheckBox	= new QCheckBox ("Mode filaire", qualifWidget);
	 * qualifWidget->getOptionsLayout ( ).addWidget (wireCheckBox);
	 * connect (wireCheckBox, ...);
	 * </CODE>
	 * @return		Une référence sur le gestionnaire de mise en forme du bandeau vertical de cases à cocher.
	 */
	virtual QVBoxLayout& getOptionsLayout ( );



	protected :

	/**
	 * Affiche/masque l'extraction dont l'indice est transmis en argument. Ne fait rien par défaut, méthode à surcharger.
	 * @param		Indice de l'extraction à afficher.
	 * @param		<I>true</I> s'il faut l'afficher, <I>false</I> s'il faut la masquer.
	 */
	virtual void displayExtraction (size_t i, bool display);
	
	/**
	 * Affiche/masque les extractions sélectionnées. Invoque displayExtraction pour chaque extraction sélectionnée.
	 * @param		true si il faut les afficher, false dans le cas contraire.
	 * @see			displayExtraction
	 */
	virtual void displaySelectedExtractions (bool display);

	/**
	 * \@return		Le titre de l'application.
	 */
	virtual IN_STD string getAppTitle ( ) const;

	/**
	 * Affiche le message d'erreur transmis en argument.
	 */
	virtual void displayErrorMessage (const IN_UTIL UTF8String& msg);

	/**
	 * Créé une tâche <I>Qualif</I> d'analyse de mailles. Cette tâche est à détruire par la fonction appelante.
	 * @param		Les types de mailles au sens <I>QualifHelper</I>, définis par un ou exclusif sur les types élémentaires (TRIANGLE, ...).
	 * @param		Le critère <I>Qualif</I> appliqué à la tâche.
	 * @param		La valeur minimale du domaine d'échantillonnage.
	 * @param		La valeur maximale du domaine d'échantillonnage.
	 * @param		Les séries soumises à l'analyse <I>Qualif</I>.
	 */
	virtual GQualif::QualifAnalysisTask* createAnalysisTask (size_t types, Qualif::Critere criterion, double min, double max, const std::vector<AbstractQualifSerie*>& series);


	protected slots :

	/**
	 * Actualise le domaine (min-max) selon le critère de qualité et les types de mailles sélectionnés.
	 */
	virtual void updateDomainCallback ( );
	
	/**
	 * <P>Appelé lorsque le critère de qualité est modifié. Met à jour le caractère "sélectionnable" des types de données à soumettre au traitement.</P>
	 */
	virtual void updateSelectableTypesCallback ( );
	
	/**
	 * <P>Appelé lorsque l'utilisateur coche une extraction en vue d'en visualiser/masquer le résultat.
	 * invoque <I>displayExtraction</I>.
	 */
	virtual void displaySerieExtractionCallback (int row, int col);
	
    /**
	 * Effectue les extractions selon les paramètres définis par l'utilisateur.
	 * @see		compute
	 */
	virtual void computeCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtQualityDividerWidget (const QtQualityDividerWidget&);
	QtQualityDividerWidget& operator = (const QtQualityDividerWidget&);

	/** Le titre de l'application. */
	IN_STD string							_appTitle;

	/** Les séries prises en charge par le widget. */
	IN_STD vector<AbstractQualifSerie*>		_series;

	QtTextField*							_minTextField;
	QtTextField*							_maxTextField;
	QComboBox*								_criterionComboBox;
	QListWidget*							_dataTypesList;
	QCheckBox*								_coordinatesCheckBox;
	QCheckBox*								_releaseDataCheckBox;
	// Les boutons :
	QHBoxLayout*							_buttonsLayout;
	QVBoxLayout*							_optionsLayout;
	QPushButton*							_applyButton;

	// Gestion des données en cache :
	Qualif::Critere							_criterion;
	
	// Les sous-ensembles extraits
	QTableWidget*							_seriesExtractionsTableWidget;
};	// class QtQualityDividerWidget

}	// namespace GQualif

#endif	// QT_QUALITY_DIVIDER_WIDGET_H
