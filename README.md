# OCR_ai_projet_S3
Projet ocr


lun. 22 sept. 2025 14:25:49 CEST

Coeur du projet le réseaux de neurones :
	Termes clés :
		- apprentissage supervisé
		- perceptron multicouche
		- rétropropagation
		- descente de gradient
		- AdaBoost

Autre grosse partie algorithme de résolution solver

Contraintes :
	- codé en C
	- compilé sans erreur avec -Wall -Wextra
	- tout les commentaires devront etre ecrit en anglais 
	- LE DEPOT GIT NE DEVRA PAS CONTENIR QUE:
		LES FICHIERS EXECUTABLES
		DE CODE SOURCE EXTERIEUR
		TOUT AUTRE TYPE DE FICHIER NON NECESSAIRE AU PROJET


SEULS LES FICHIERS PRESENTS SUR LE DEPOT AU MOMENT DU CLONAGE SERONT DISPONIBLES PENDANT LA SOUTENANCE

Niveaux d'images:

	Niveau 1 (image sans défaut)
	Niveau 2 (image avec de légers défauts)
	Niveau 3 (image avec d'autres éléments que la grille et la liste de mots)


Rapport de soutenance :
• Le rôle et la mission de chaque membre (~1 page) ;
• Les objectifs, la planification des tâches, l’état d’avancement attendu et réel du projet (~2 pages) ;
• Pour chaque objectif, les difficultés éventuelles, les choix technologiques et algorithmiques, la
justification de ces choix (~6 pages) ;
• Une bibliographie (~1 page)


GIT REPO
	rapport.pdf à mettre sur le git 
	il devra aussi contenir un Makefile (minimum all and clean rules)
	un README qui expliquera comment utiliser l'appli
	un AUTHORS avec les login de chaque membre du groupe
	quelques images pour faire une demo à la soutenance





PREMIERE SOUTENANCE :


• Chargement d’une image et suppression des couleurs ;
• Rotation manuelle de l’image ;
• Détection de la position :
◦ De la grille ;
◦ De la liste de mots ;
◦ Des lettres dans la grille ;
◦ Des mots de la liste ;
◦ Des lettres dans les mots de la liste.
• Découpage de l’image (sauvegarde de chaque lettre sous la forme d’une image) ;
• Implémentation de l’algorithme de résolution d’une grille de mots cachés. Vous devrez implémenter
cet algorithme dans le programme en ligne de commande solver (cf. III.2.5 Résolution d’une grille de
mots cachés) ;
• Une preuve de concept de votre réseau de neurones. Pour cette preuve, vous réaliserez un mini réseau
capable d’apprendre la fonction logique A.B + A.B (A et B étant des variables booléennes n’acceptant
que les valeurs 0 et 1).




DEUXIEME SOUTENANCE :


• Le prétraitement complet (avec la rotation automatique) ;
• Le réseau de neurones complet et fonctionnel
◦ Apprentissage ;
◦ Reconnaissance des lettres de la grille et de la liste de mots.
• La reconstruction de la grille ;
• La reconstruction de la liste de mots ;
• La résolution de la grille ;
• L’affichage de la grille ;
• La sauvegarde du résultat ;
• Une interface graphique permettant d’utiliser tous ces éléments.




BONUS (évalués uniquement si la partie obligatoire est faite) :

• Générateur d’une grille de mots cachés à partir d’une liste de mots ;
• Site internet présentant votre projet ;
• Autres.


outil pour éviter des problèmes avec le C : valgrind  /   (option gcc) -fsanitize=address 




