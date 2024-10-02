% Programme pour l'identification d'une plaque d'immatriculation

%% 1) Reading the image and printing the result in matlab
I = imread("image3.jpeg");
[n, m, L] = size(I); % Si L == 3 -> c'est RGB. 
% À noter que n est le nombre des points suivant x, et m est le nombre des points suivants y. 
% Si L == 1 -> c'est Ndg

figure(1) % Used to afficher l'image
% imagesc(I);

% Another way to do it is:
imshow(I)

%% 2) To gray space we do
%%% Option 1
Igrey = rgb2gray(I);

figure(2)
imagesc(Igrey)

%%% Option 2 (methode moyenne)
R = I(:,:,1);
G = I(:,:,2);
B = I(:,:,3);

Igrey2 = (R + G + B)/3;
figure(3)
imagesc(Igrey2)

% Steps: 1) Image Couleur -> 2) On pase au niveau du gris -> 3) On passe en binaire
% (methode de segmentation [par seullage) -> 4) Apres on fait une erosion
% -> 5) Dilatation -> 6) Etiquettage -> 7) On va requperer les proprietes
% de chaqe'une des regions

%% 3) Segmentation
% On pose un seuil. Pour tous les pixels de l;image via une fouble boucle,
% si ;e pixel en question est < seuil on le pose à 1, sinon on le pose à 0.

seuil = 100; % seuild choisi
I_segmentation = double(Igrey); % Il faut etre sur que ca soit en double, comme ca on peut effectuer de operations mathematiques sans beaucoup perte d'information
for i=1:n
    for j=1:m
        if (I_segmentation(i,j) <= seuil)
            I_segmentation(i,j) = 1; % black === noir
        else
            I_segmentation(i,j) = 0; % white === blanche
        end
    end
end

figure(4)
imshow(I_segmentation)

%% 4) On va faire une erosion
% On fait l'erosion pour

masque = strel('rectangle', [4 4]);
masque2 = strel("disk", 2);
I_erosion = imerode(I_segmentation, masque);
I_erosion2 = imerode(I_segmentation, masque2);
I_diff = I_segmentation - I_erosion; %% SOS: Une maniere de calculer les contours dans limage

figure(5)
imshow(I_diff)

%% 5) Dilatation
% On fait la dilatation pour

I_dilatation = imdilate(I_erosion, masque);
I_dilatation2 = imdilate(I_erosion2, masque2);

figure(6)
imshow(I_dilatation)

figure(7)
imshow(I_dilatation2)

%% 6) Edictage / Etiquettage
% L'objectif est de faire un groupment de differents areas of the image
% pour faire une choix apres.

[I_etiquettage, n] = bwlabeln(I_dilatation);
figure(8)
imagesc(I_etiquettage)
n

%% 7) Proprietes de la region

properties = regionprops(I_etiquettage, 'Area', 'PixelIdxList', 'BoundingBox'); % The regionprops function measures properties such as area

area_min = 140;
area_max = 400;
for i=1:n
    if (properties(i).Area <= area_min || properties(i).Area >= area_max)
        area = properties(i).Area % Pour avoir une information sur l'area
        I_etiquettage(properties(i).PixelIdxList) = 0;
        
        figure(10)
        imagesc(I_etiquettage)
    end
end

for i=1:n
    largeur = properties(i).BoundingBox(3) - properties(i).BoundingBox(1);
    hauteur = properties(i).BoundingBox(4) - properties(i).BoundingBox(2);
    ratio = hauteur / largeur;
    if (ratio <= 0.73 || ratio >= 1.2)
        I_etiquettage(properties(i).PixelIdxList) = 0;
        
        figure(10)
        imagesc(I_etiquettage)
    end
end