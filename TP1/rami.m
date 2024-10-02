close all, clear all;

I = imread('chromosomes.tif');
ind = 1; %indice d'image
figure(ind), ind = ind+1, imshow(I), title('image original');


% enlever le bruit (=> plusieur filtre possible on choisi median)

img_filtered = medfilt2(I);

figure(ind), ind=ind+1, imshow(img_filtered), title('image sans bruit')

% histogramme pour trouver le seuil
imhist(img_filtered);
figure(ind), ind=ind+1, imshow(img_filtered), title('image sans bruit')

% image binarise
seuil = 200
Ib = ((img_filtered<=seuil)==1)
figure(ind), ind=ind+1, imshow(Ib), title('Image binaire')

%% Ameliorer la binarisation
% Interprétation visuelle des résultats :
% Ouverture : L'opération d'ouverture va permettre de supprimer les petites taches de bruit sans changer significativement la forme des chromosomes.
% Fermeture : L'opération de fermeture va combler les petits trous à l'intérieur des chromosomes, améliorant ainsi leur continuité et permettant un meilleur comptage.

% Appliquer une ouverture (érosion suivie d'une dilatation) 
% morphologie mathematique
se = strel('cube',8);
Ir = imerode(Ib, se);
Id = imdilate(Ir, se);
figure(ind), ind=ind+1, imshow(Id), title('Image apres une erosion puis dilatation (ouverture)')

% Appliquer une fermeture (dilatation suivie d'une érosion) :
se = strel('cube',1);
Id = imdilate(Id, se);
Ir = imerode(Id, se);
figure(ind), ind=ind+1, imshow(Ir), title('Image apres une dilatation puis une erosion (fermeture)')

%% Contour on choisi Laplacien
laplacian_img = fspecial('laplacian', 0.2); %creer le filtre
edges_laplacian = imfilter(Ir, laplacian_img); %convolution
figure(ind), ind=ind+1, imshow(edges_laplacian);

%% Supprimer la tache 2 methode
% Première methode labeler l'image et enlever les grands contour
% Inconvenient c'est que les chromosomes a cote vont disparaitre
[labeled_img, num_objects] = bwlabel(Ir);
properties = regionprops(labeled_img, 'Area', 'PixelIdxList');
figure(ind), ind=ind+1, imagesc(labeled_img);

min_area = 100
max_area = 4000
for i = 1:length(properties)
    if (properties(i).Area <= min_area) || (properties(i).Area > max_area)
        area = properties(i).Area;
        labeled_img(properties(i).PixelIdxList) = 0;
    end
end
final1 = labeled_img
figure(ind), ind=ind+1, imshow(final1);
[labeled_img1, num_objects1] = bwlabel(final1);
% num_objects1 chromosome avec cette methode

% Deuxieme methode c'est de créer une grosse ouverture pour faire
% disparaitre tout les chromosomes sur une nouvelle image, puis soustraire
% les deux images

se = strel('cube',35);
IrTache = imerode(Ib, se);
Tache = imdilate(IrTache, se);
figure(ind), ind=ind+1, imshow(Tache);

final2 = Ir-Tache
figure(ind), ind=ind+1, imshow(final2);

se = strel('cube',8);
Ir = imerode(final2, se);
final2 = imdilate(Ir, se);
figure(ind), ind=ind+1, imshow(final2);

[labeled_img2, num_objects2] = bwlabel(final2);
% num_objects2 chromosome avec cette methode