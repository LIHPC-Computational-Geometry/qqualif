//
// Petit programme de génération d'hexaèdres +- beaux
//

#include <Lima/maillage.h>
#include <Lima/maillage.h>
#include <Lima/noeud.h>
#include <Lima/volume.h>
#include <Lima/polyedre.h>
#include <Lima/erreur.h>

#include <iostream>
#include <string>
#include <string.h>


using namespace Lima;
using namespace std;


static string	meshPath ("hexas.mli");

static int parseArgs (int argc, char* argv []);
static int syntax (const string& pgm);


int main (int argc, char* argv[])
{
	if (0 != parseArgs (argc, argv))
		return syntax (argv [0]);

	try
	{

	// Création du maillage :
	Maillage	mesh ("CELLS");
	Volume		line1 ("L1");
	Volume		line2 ("L2");
	Volume		line3 ("L3");
	Volume		line4 ("L4");
	Volume		line5 ("L5");
	mesh.dimension (D3);
	mesh.ajouter (line1);
	mesh.ajouter (line2);
	mesh.ajouter (line3);
	mesh.ajouter (line4);
	mesh.ajouter (line5);

	// Les noeuds :
	Noeud	n1, n2, n3, n4, n5, n6, n7, n8;
	double	xHalf	= 0.5, yHalf	= 0.5,	zHalf	= 0.5;
	double	xCenter = xHalf, yCenter = yHalf, zCenter = zHalf;

	// La maille idéale ?
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	Polyedre	hexa (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line1.ajouter (hexa);
	xCenter	+= 6 * xHalf;
	xHalf	= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line1.ajouter (hexa);
	xCenter	+= 2 * xHalf + 0.5;
	yHalf	/= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line1.ajouter (hexa);

	// Des mailles pathologiques :
	// Inversion de 2 noeuds
	yCenter	+= 6 * yHalf;
	xCenter	= xHalf;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n7, n6, n8);
	mesh.ajouter (hexa);
	line2.ajouter (hexa);
	// Rotation autour du centre :
	xCenter	+= 2 * xHalf + 0.5;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n6, n7, n8, n5);
	mesh.ajouter (hexa);
	line2.ajouter (hexa);
	// Maille croisée :
	xCenter	+= 2 * xHalf + 0.5;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n7, n6, n4, n3, n2, n8, n5);
	mesh.ajouter (hexa);
	line2.ajouter (hexa);
	// Noeuds alignés :
	yCenter	+= 6 * yHalf;
	xCenter	= xHalf;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter, yCenter + yHalf, zCenter);
	n7	= Noeud (xCenter, yCenter - yHalf, zCenter);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line3.ajouter (hexa);
	// Dégénerescence :
	xCenter	+= 2 * xHalf + 0.5;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter, zCenter);
	n3	= Noeud (xCenter + xHalf, yCenter, zCenter);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter, zCenter);
	n7	= Noeud (xCenter + xHalf, yCenter, zCenter);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n7, n6, n8);
	mesh.ajouter (hexa);
	line3.ajouter (hexa);

	// Des mailles plus ou moins applaties :
	double	tmpy	= yHalf;
	yCenter	+= 6 * yHalf;
	xCenter	= xHalf;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line4.ajouter (hexa);
	xCenter	+= 2 * xHalf + 0.5;
	yHalf	/= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line4.ajouter (hexa);
	xCenter	+= 2 * xHalf + 0.5;
	yHalf	/= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line4.ajouter (hexa);
	xCenter	+= 2 * xHalf + 0.5;
	yHalf	/= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line4.ajouter (hexa);
	xCenter	+= 2 * xHalf + 0.5;
	yHalf	/= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line4.ajouter (hexa);

	// Des mailles plus ou moins pointues :
	yHalf	= tmpy;
	yCenter	+= 6 * yHalf;
	xCenter	= xHalf;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line5.ajouter (hexa);
	xCenter	+= 2 * xHalf + 0.5;
	yHalf	/= 2.;	zHalf	/= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line5.ajouter (hexa);
	xCenter	+= 2 * xHalf + 0.5;
	yHalf	/= 2.;	zHalf	/= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line5.ajouter (hexa);
	xCenter	+= 2 * xHalf + 0.5;
	yHalf	/= 2.;	zHalf	/= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line5.ajouter (hexa);
	xCenter	+= 2 * xHalf + 0.5;
	yHalf	/= 2.;	zHalf	/= 2.;
	n1	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter + zHalf);
	n2	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter + zHalf);
	n3	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter + zHalf);
	n4	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter + zHalf);
	n5	= Noeud (xCenter - xHalf, yCenter + yHalf, zCenter - zHalf);
	n6	= Noeud (xCenter + xHalf, yCenter + yHalf, zCenter - zHalf);
	n7	= Noeud (xCenter + xHalf, yCenter - yHalf, zCenter - zHalf);
	n8	= Noeud (xCenter - xHalf, yCenter - yHalf, zCenter - zHalf);
	mesh.ajouter (n1);		mesh.ajouter (n2);		mesh.ajouter (n3);
	mesh.ajouter (n4);		mesh.ajouter (n5);		mesh.ajouter (n6);
	mesh.ajouter (n7);		mesh.ajouter (n8);
	hexa	= Polyedre (n1, n2, n3, n4, n5, n6, n7, n8);
	mesh.ajouter (hexa);
	line5.ajouter (hexa);

	// Sauvegarde :
	mesh.ecrire (meshPath);
	cout << "Maillage enregistré dans le fichier " << meshPath << "." << endl;

	}
	catch (const erreur& err)
	{
		cerr << "Erreur lima : " << err.what ( ) << endl;
		return -2;
	}
	catch (const exception& exc)
	{
		cerr << "Erreur : " << exc.what ( ) << endl;
		return -4;
	}
	catch (...)
	{
		cerr << "Erreur non documentée." << endl;
		return -5;
	}
	
	return 0;
}	// int main (int, char*[])


static int parseArgs (int argc, char* argv [])
{
	for (int i = 0; i < argc; i++)
	{
		if (i < argc - 1)
		{
			if (0 == strcmp (argv [i], "-mesh"))
			{
				meshPath	= argv [++i];
				continue;
			}	// if (0 == strcmp (argv [i], "-mesh"))
		}	// if (i < argc - 1)
	}	// for (int i = 0; i < argc; i++)

	return 0;
}	// parseArgs


static int syntax (const string& pgm)
{
	cout << "Syntaxe : " << pgm << " -mesh filename" << endl;

	return -1;
}	// syntax

