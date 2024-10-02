%%%%%%%%%%%%%%%%%%%% PARTIE 1 - RESTAURATION DE L'IMAGE %%%%%%%%%%%%%%%%%%%%

%% Ouverture de l'image
% Importation de l'image des chromosomes dans l'environnement Matlab.
I = imread('chromosomes.tif');

% Vérifier si l'image est en couleur et la convertir en niveaux de gris si nécessaire.
if size(I, 3) == 3
    I = rgb2gray(I);  % Conversion en niveaux de gris si l'image est en couleur.
end

figure(1);  
imshow(I);
title('Figure 1 : Image originale en niveaux de gris');

%% Filtrage médian pour suppression du bruit
% Application d'un filtre médian pour supprimer le bruit sans flouter les contours des chromosomes.
J = medfilt2(I, [5 5]);

figure(2);  
imshowpair(I, J, 'montage');
title('Figure 2 : Image originale (gauche) et filtrée par médiane (droite)');

%% Binarisation de l'image
% Utilisation de la méthode de seuillage automatique d'Otsu pour transformer l'image en niveaux de gris
% en une image binaire (noir et blanc), ce qui permet de distinguer les chromosomes du fond.
tresh = graythresh(J);  % Calcul automatique du seuil avec la méthode d'Otsu.
I_segmentation = imbinarize(J, tresh);  % Binarisation de l'image.

figure(3);  
imshowpair(J, I_segmentation, 'montage');
title('Figure 3 : Image filtrée (gauche) et binarisée (droite)');

%% Amélioration de l'image binaire par opérations morphologiques
% Utilisation de l'ouverture morphologique pour supprimer les petites taches blanches
% tout en préservant la forme des chromosomes.
elemstrct = strel('disk', 2);  % Élément structurant circulaire de rayon 2 pixels.
I_opened = imopen(I_segmentation, elemstrct);  % Ouverture morphologique (érosion suivie de dilatation).

figure(4);  
imshowpair(I_segmentation, I_opened, 'montage');
title('Figure 4 : Image binarisée (gauche) et après ouverture morphologique (droite)');

%% Fermeture morphologique pour combler les petits trous
% Application d'une fermeture morphologique (dilatation suivie d'érosion) pour combler les petits trous
% à l'intérieur des chromosomes et les rendre plus compacts.
I_closed = imclose(I_segmentation, elemstrct);

figure(5);  
imshowpair(I_segmentation, I_closed, 'montage');
title('Figure 5 : Image binarisée (gauche) et après fermeture morphologique (droite)');






%%%%%%%%%%%%%%%%%%%% PARTIE 2 - DÉTECTION ET ÉTIQUETAGE DES CHROMOSOMES %%%%%%%%%%%%%%%%%%%%

%% Étape 1 : Détection des contours en soustrayant l'image érodée de l'image segmentée
% Nous allons réaliser une érosion sur l'image fermée puis soustraire cette image érodée
% de l'image segmentée afin de détecter les contours des chromosomes.

% Appliquer l'érosion sur l'image fermée
I_erosion = imerode(I_closed, elemstrct);  % Érosion de l'image fermée avec le même élément structurant

% Soustraction de l'image érodée de l'image fermée pour obtenir les contours
I_contour = I_closed - I_erosion;

% Affichage de l'image des contours
figure;
imshow(I_contour);
title('Contours des chromosomes');

%% Étape 2 :  suppression de la  tache en filtrant par taille

% Inverser l'image binaire pour que les chromosomes soient en blanc et le fond en noir
I_inverted = imcomplement(I_segmentation);

% Définir une plage pour les tailles des objets à conserver (chromosomes)
taille_min = 150;  % Taille minimale supposée pour un chromosome
taille_max = 3000;  % Taille maximale supposée pour un chromosome

% Utiliser bwareafilt pour conserver uniquement les objets dont la taille est comprise dans cette plage
I_clean = bwareafilt(I_inverted, [taille_min, taille_max]);  % Conserver uniquement les objets dans la plage de taille

% Inverser à nouveau l'image pour revenir à l'image originale (chromosomes en blanc)
I_clean = imcomplement(I_clean);

% Affichage de l'image avant et après suppression des taches
figure;
imshowpair(I_segmentation, I_clean, 'montage');
title('Image avant (gauche) et après (droite) suppression des taches basées sur la taille');

%% Étape 3 : Suppression des pixels résiduels avec ouverture et retour à un fond noir
% Élément structurant circulaire de rayon 1 pixel
elemstruct_small = strel('disk', 1);

% Appliquer l'ouverture morphologique pour supprimer les petits résidus
I_clean_final = imopen(I_clean, elemstruct_small);

% Assurer que le fond est noir et les chromosomes sont en blanc 
I_clean_final = imcomplement(I_clean_final);  

% Affichage de l'image avant et après suppression des pixels résiduels 
figure;
imshowpair(I_clean, I_clean_final, 'montage');
title('Image avant (gauche) et après (droite) suppression des pixels résiduels ');

%% Étape 4 : Étiquetage des chromosomes et comptage
% Nous utilisons bwlabel pour étiqueter chaque composante connexe 
% et déterminer le nombre total de chromosomes détectés.

% Étiqueter les composantes connexes (chromosomes) dans l'image
[L, num] = bwlabel(I_clean_final);

% Afficher le nombre de chromosomes détectés
disp(['Nombre de chromosomes détectés : ', num2str(num)]);

% Conversion des étiquettes en couleurs pour visualiser chaque chromosome
L_rgb = label2rgb(L, 'jet', 'w', 'shuffle');  % 'jet' pour la colormap, 'k' pour fond noir

% Affichage de l'image avec les chromosomes étiquetés
figure;
imshow(L_rgb);
title(['Étiquetage des chromosomes (Nombre détecté = ', num2str(num), ')']);
