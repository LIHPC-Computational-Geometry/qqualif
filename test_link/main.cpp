#include "QtQualif/QtQualifServices.h"
#include <TkUtil/Exception.h>
#include <TkUtil/ThreadManager.h>
#include <QApplication>


USING_UTIL
USING_STD
using namespace GQualif;


static const string	appTitle ("QCalQual");


int main (int argc, char* argv[])
{
	try
	{
		cout << "Version " << QtQualifServices::getVersion ( ).getVersion ( )
		     << " de " << appTitle << "." << endl;

		QApplication	application (argc, argv);
		// GQualif::initialize ( ) après QApplication car initialise des
		// pixmaps.
		QtQualifServices::initialize ( );
		ThreadManager::initialize ( );	// => Nombre de procs de la station

		return 0;
	}
	catch (const Exception& exc)
	{
		cerr << "Exception levée dans " << appTitle << " : " << endl
		     << exc.getFullMessage ( ) << endl;
	}
	catch (const exception& e)
	{
		cerr << "Exception standard levée dans " << appTitle << " : " << endl
		     << e.what ( ) << endl;
	}
	catch (...)
	{
		cerr << "Erreur non documentée dans " << appTitle << "." << endl;
	}
}	// main
