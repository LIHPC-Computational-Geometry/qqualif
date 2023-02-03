#ifndef QT_QUALIF_WIDGET_H
#define QT_QUALIF_WIDGET_H

#include "GQualif/QualifAnalysisTask.h"
#include "GQualif/QualifRangeTask.h"

#include <QwtCharts/QwtHistogramChartPanel.h>
#include <QtUtil/QtGroupBox.h>
#include <QtUtil/QtTextField.h>

#include <QPushButton>
#include <QListWidget>
#include <QRadioButton>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>

namespace GQualif
{

/**
 * <P>Classe de Widget <I>Qt</I> permettant d'évaluer la qualité d'un
 * maillage à l'aide de la bibliothèque <I>Qualif</I>. L'accès aux données
 * est effectué à l'aide d'une instance de la classe <I>AbstractQualifSerie</I>,
 * indépendante du type de maillage analysé.
 * </P>
 *
 * <P>L'affichage des résultats est effectué à l'aide d'histogrammes de la
 * bibliothèques
 * <A HREF="../../QwtCharts-2.5.0/QwtCharts/index.html">QwtCharts</A>.
 * </P>
 *
 * <P>Les méthodes de cette classe sont susceptibles de lever des exceptions
 * de type <I>TkUtil::Exception</I>.
 * </P>
 *
 * <P>En environnement multithread cette classe exécute si possible les analyses
 * de chaque série dans des threads différents. <U>Requiert dans ce cas que
 * <I>ThreadManager::initialize</I> ait été préalablement appelé.
 * </P>
 *
 * @see		AbstractQualifSerie
 */
class QtQualifWidget : public QWidget
{
	Q_OBJECT

	public :

	/**
	 * Le type de domaine utilisé : théorique, calculé, manuel.
	 */
	enum DOMAIN_TYPE { THEORETICAL_DOMAIN,COMPUTED_DOMAIN,USER_DEFINED_DOMAIN };

	/**
	 * Constructeur.
	 * @param		Widget parent.
	 * @param		Titre de l'application (pour les messages).
	 */
	QtQualifWidget (QWidget* parent, const IN_STD string& appTitle);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtQualifWidget ( );

	/**
	 * @return		Le widget contenant l'histogramme.
	 */
	virtual QwtHistogramChartPanel& getHistogramPanel ( );
	virtual const QwtHistogramChartPanel& getHistogramPanel ( ) const;

	/**
	 * Ajoute la série transmise en argument à l'histogramme.
	 * Cette série est adoptée et donc détruite à la destruction du widget.
	 * @see			removeSeries
	 */
	virtual void addSerie (AbstractQualifSerie* serie);

	/**
 	 * Détruit (operateur <I>delete</I>) toutes les séries de l'histogramme.
 	 * @param		Si clearHistogram vaut <I>true</I> l'histogramme sera
 	 * 				raffraichit (avec rien dedans). Ne pas mettre <I>true</I> en
 	 * 				cas de destruction du widget (susceptible de provoquer un
 	 * 				plantage dans <I>Qwt</I>).
 	 * @see			addSerie
 	 * @warning		Si appelé lors de la destruction du panneau alors
 	 * 				transmettre false pour clearHistogram.
 	 */
	virtual void removeSeries (bool clearHistogram = false);

	/**
	 * Actualise l'histogramme conformément aux paramètres renseignés dans
	 * l'IHM.
	 */
	virtual void updateHistogram ( );

	/**
	 * @return		Le critère <I>Qualif</I> appliqué pour l'analyse.
	 */
	virtual Qualif::Critere getCriterion ( ) const;

	/**
	 * @return		La valeur minimale (saisie) du domaine d'échantillonnage.
	 * @see			getDomainMinValue
	 * @see			getDataRange
	 * @see			useStrictDomain
	 */
	virtual double getDomainMinValue ( ) const;

	/**
	 * @return		La valeur maximale (saisie) du domaine d'échantillonnage.
	 * @see			getDomainMinValue
	 * @see			getDataRange
	 * @see			useStrictDomain
	 */
	virtual double getDomainMaxValue ( ) const;

	/**
	 * @return		Les valeurs extremales (calculées à partir des séries)
	 * 				du domaine d'échantillonnage.
	 * @see			getDomainMinValue
	 * @see			getDomainMaxValue
	 * @see			useStrictDomain
	 */
	virtual void getDataRange (double& min, double& max);

	/**
	 * @return		<I>true</I> si en mode "manuel" il ne faut pas prendre en
	 *				compte les mailles dont le critère est hors-domaine,
	 *				<I>false</I> dans le cas contraire.
	 * @see			getDomainMinValue
	 * @see			getDomainMaxValue
	 * @see			getDataRange
	 */
	virtual bool useStrictDomain ( ) const;

	/**
	 * @return		Le nombre d'échantillons à créer.
	 */
	virtual unsigned char getClassesNum ( ) const;

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
	 * @return		true si la hauteur maximale est automatique, ou false
	 * 				si elle est définie par l'utilisateur.
	 */
	virtual bool isAutomaticHeight ( ) const;

	/**
	 * @return		La hauteur maximale à afficher.
	 */
	virtual size_t getMaxHeight ( ) const;

	/**
	 * @return		Les types de données à soumettre au traitement.
	 *				La valeur retournée est issue d'un ou exclusif entre
	 *				les types unitaires (QualifHelper::TRIANGLE, ...)
	 *				sélectionnés par l'utilisateur.
	 * @see			selectQualifiedTypes (size_t types);
	 */
    virtual size_t getQualifiedTypes ( ) const;

	/**
	 * @param		Types de données à sélectionner.
	 * @see			getQualifiedTypes
	 */
	virtual void selectQualifiedTypes (size_t types);

	/**
	 * @return		une table contenant les associations type de maille -
	 * 				dimension des mailles.
	 */
	virtual IN_STD map<size_t, size_t> getDataTypes ( );

	/**
	 * @return		<I>true</I> si les coordonnées des noeuds doivent être
	 *				préalblement évaluées, <I>false</I> dans le cas contraire.
	 */
	virtual bool coordinatesEvaluation ( ) const;

	/**
	 * @return		<I>true</I> si les données en cache doivent être libérées à
	 * 				chaque changement de critère, <I>false</I> s'il faut les
	 * 				conserver (en vue d'un retour à un critère déjà exploré).
	 */
	virtual bool shouldReleaseData ( ) const;

	/**
	 * @return	Le type de domaine utilisé (pour le prochain calcul).
	 */
	virtual DOMAIN_TYPE getDomainType ( ) const;

	/**
	 * @param	Le nouveau type de domaine utilisé (pour le prochain calcul).
	 */
	virtual void setDomainType (DOMAIN_TYPE type);

	/**
	 * Personnalisation de l'IHM : ajouter un bouton. Le parent du bouton doit
	 * être a priori cette instance, il convient ensuite de l'ajouter au
	 * gestionnaire de mise en forme du bandeau horizontal de boutons. Ex :<BR>
	 * <CODE>
	 * QtQualifWidget*	qualifWidget = new QtQualifWidget (...);
	 * QPushButton*		button	= new QPushButton ("Afficher", qualifWidget);
	 * qualifWidget->getButtonsLayout ( ).addWidget (button);
	 * connect (button, ...);
	 * </CODE>
	 * @return		Une référence sur le gestionnaire de mise en forme du
	 * 				bandeau horizontal de boutons.
	 */
	virtual QHBoxLayout& getButtonsLayout ( );


	protected :

	/**
	 * \@return		Le titre de l'application.
	 */
	virtual IN_STD string getAppTitle ( ) const;

	/**
	 * Affiche le message d'erreur transmis en argument.
	 */
	virtual void displayErrorMessage (const IN_UTIL UTF8String& msg);

	/**
	 * Créé une tâche <I>Qualif</I> qui recueille les extrema pris par les
	 * mailles d'analyse de mailles de séries soumises à un critère de qualité.
	 * Cette tâche est à détruire par la fonction appelante.
	 * @param		Les types de mailles au sens <I>QualifHelper</I>, définis
	 * 				par un ou exclusif sur les types élémentaires
	 * 				(TRIANGLE, ...).
	 * @param		Le critère <I>Qualif</I> appliqué à la tâche.
	 * @param		Les séries soumises à l'analyse <I>Qualif</I>.
	 */
	virtual GQualif::QualifRangeTask* createRangeTask (
				size_t types, Qualif::Critere criterion, 
				const std::vector<AbstractQualifSerie*>& series);

	/**
	 * Créé une tâche <I>Qualif</I> d'analyse de mailles. Cette tâche est à
	 * détruire par la fonction appelante.
	 * @param		Les types de mailles au sens <I>QualifHelper</I>, définis
	 * 				par un ou exclusif sur les types élémentaires
	 * 				(TRIANGLE, ...).
	 * @param		Le critère <I>Qualif</I> appliqué à la tâche.
	 * @param		Le nombre de classes pour la répartition des mailles selon
	 *				le domaine d'échantillonnage (!= 0).
	 * @param		La valeur minimale du domaine d'échantillonnage.
	 * @param		La valeur maximale du domaine d'échantillonnage.
	 * @param		<I>true</I> s'il ne faut pas prendre en compte les mailles
	 * 				dont le critère est hors-domaine, <I>false</I> dans le cas
	 * 				contraire.
	 * @param		Les séries soumises à l'analyse <I>Qualif</I>.
	 */
	virtual GQualif::QualifAnalysisTask* createAnalysisTask (
				size_t types, Qualif::Critere criterion, size_t classNum,
				double min, double max, bool strict,
				const std::vector<AbstractQualifSerie*>& series);


	protected slots :

	/**
	 * Recalcule les valeurs du domaine d'évaluation si nécessaire.
	 * Appelé lorsque l'utilisateur modifie le mode de définition du
	 * domaine évalué, le critère ou les types de données soumis
	 * au traitement.
	 */
    virtual void updateDomainCallback ( );

	/**
	 * Appelé lorsque l'état de la case à cocher "hauteur automatique"
	 * est modifié. Rend (in)actif le champ de texte associé.
	 */
	virtual void automaticHeightCallback ( );

	/**
	 * <P>Appelé lorsque le critère de qualité est modifié. Met à
	 * jour le caractère "sélectionnable" des types de données à
	 * soumettre au traitement.</P>
	 * <P>Paramétrage spécifique proposé pour le critère <P>VALIDITY</P> dont
	 * les valeurs possibles sont binaires (0/1).</P>
	 */
	virtual void updateSelectableTypesCallback ( );

	/**
	 * Recalcule l'histogramme selon les paramètres définis par l'utilisateur.
	 */
	virtual void computeCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtQualifWidget (const QtQualifWidget&);
	QtQualifWidget& operator = (const QtQualifWidget&);

	/** Le widget affichant l'histogramme. */
	QwtHistogramChartPanel*					_histogramPanel;

	/** Le titre de l'application. */
	IN_STD string							_appTitle;

	/** Les séries prises en charge par le widget. */
	IN_STD vector<AbstractQualifSerie*>		_series;

	QRadioButton*							_theoreticalRadioButton;
	QRadioButton*							_computedRadioButton;
	QRadioButton*							_userDefinedRadioButton;
	QtTextField*							_minTextField;
	QtTextField*							_maxTextField;
	QCheckBox*								_strictDomainUsageCheckBox;
	QtTextField*							_barNumTextField;
	QtTextField*							_heightTextField;
	QCheckBox*								_automaticHeightCheckBox;
	QComboBox*								_criterionComboBox;
	QListWidget*							_dataTypesList;
	QCheckBox*								_coordinatesCheckBox;
	QCheckBox*								_releaseDataCheckBox;
	// Les boutons :
	QHBoxLayout*							_buttonsLayout;
	QPushButton*							_applyButton;

	// Gestion des données en cache :
	Qualif::Critere							_criterion;
};	// class QtQualifWidget

}	// namespace GQualif

#endif	// QT_QUALIF_WIDGET_H
