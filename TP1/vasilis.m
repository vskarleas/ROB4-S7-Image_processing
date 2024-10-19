%% Copyright Vasileios Filippos Skarleas

%% TP1
close all;

%%%%%%%%%%%%%%%%%%%% PARTIE 1 - RESTAURATION DE L'IMAGE %%%%%%%%%%%%%%%%%%%%
% Restauration de l'image

I = imread("chromosomes.tif");

% Checking if the image is already in "Niveau de grey"
[n, m, l] = size(I);
if l == 1
    "Niveau de grey"
else
    "RGB"
end

% Verifying that image has been loaded succesfully on Matlab
figure(1);  
imshow(I);
title('Figure 1 : Image originale en niveaux de gris');

% We observe that the image is already in niveau de grey, so we proceed
% with the filtering of the noise and binarisation / segmentation by choosing a seuil.

%% Filtering out the noise (bruit)
% In order to filter out the noise we are going to use a Median filter that
% is applied below. We use this filter because according to the theory: "ce 
% filtre donne de très bon résultats de part son principe sur le bruit 
% impulsionnel (poivre et sel)"

J = medfilt2(I); % Updating the results directly
figure(2);
imshowpair(I, J, 'montage');
title('Figure 2 : Before and after filtering with Median filter');

%% Segmentation
% In order to choose the seuil for the segmentation we can print the
% histogramme of the image, and then we can observe what is more powerful
% on the image. On the histogramme, we can see that there are many bright
% and white elements and so it's logic to choose a value that is exaclty
% before this area. So we obserev that this can be 233-235 and this is the
% reason why we chose this value below for the binarisation.
figure(3);
imhist(J);
title('Figure 3: The histogram of the filtered image');

seuil = 233;

% ANother idea is to use the Otsy's threshold method that computes a global
% threshold T from a grayscale image. Otsu's method chooses a threshold 
% that minimizes the intraclass variance of the thresholded black and 
% white pixels.

% There are two methods to proceed with binarisation, either with boucles or
% the direct function of Matlab for binarisaton.

% Method 1
I_segmentation_1 = double(J);
for i=1:n
    for j=1:m
        if I_segmentation_1(i,j) <= seuil
            I_segmentation_1(i,j) = 1;
        else
            I_segmentation_1(i,j) = 0;
        end
    end
end

% Method 2
level = graythresh(J);
I_segmentation_2 = imbinarize(J, level);

% Keep in mind that this method provides teh complement image from what we
% need for the analysis. In that case we simply need to receive the
% complement of every image.

%I_segmentation_2 = imcomplement(I_segmentation_2);

% Checking the segmentation result for both methods
figure(4);
imshowpair(I_segmentation_1, I_segmentation_2, 'montage');
title('Figure 4: Method 1 (right), and Method 2 (left) for segmentation');

%% Amelioration de l'image
% On the image I_segmentation_1 we can see that there is some noise that 
% isn't cleared from the filter in the first step. So, we proceed with 
% the Opening method. Meaning that we do an erosion and then a dilatation

mask = strel("disk", 2);
I_erosion = imerode(I_segmentation_1, mask);

% We have choosen a great circle of erosion (5 times) in order to remove
% teh greatest amount if reaming noise. Then we proceed with dilatation in
% order to restore the broke pixels after erosion.

% Now we proceed with dilatation. So we are opening hte image (Methode
% d'ouverture). In general, this is used to remove small objects (noise) 
% without significantly affecting larger objects, while closing method is 
% used to fill in holes or gaps in objects.
I_dilatation = imdilate(I_erosion, mask);

% We continue with the image I_segmentation_1 from now and on
figure(5);
imshow(I_dilatation);
title('Figure 5: Dilated image I-Segmentation-1');

%%%%%%%%%%%%%%%%%%%% PARTIE 2 - DÉTECTION ET ÉTIQUETAGE DES CHROMOSOMES %%%%%%%%%%%%%%%%%%%%
% Interested in takign the contours of the chromosomes. In that case a
% methode that is used is to take the difference between I_segmentation and
% I_erosion. The second method that we are trying below is the integrated
% function of matlab that is called imcontour.

I_contour = I_segmentation_1 - I_erosion;
figure(6);
imshow(I_contour);
title('Figure 6: Contours of the image');

% In order to keep only the chromosomes, we can be based on the etiquettage
% method and the area covered from every chromosome.

% Method 1
[I_etiquettage, n] = bwlabeln(I_dilatation);
figure(7);
imagesc(I_etiquettage);
title('Figure 7: Etiquettage of the image');
n;

stats = regionprops(I_etiquettage, 'Area', 'PixelIdxList');
area_min = 150;
area_max = 4300;
for i=1:n
    if (stats(i).Area <= area_min || stats(i).Area >= area_max)
        area = stats(i).Area % An information to the area
        I_etiquettage(stats(i).PixelIdxList) = 0;
    end
end

figure(8);
imagesc(I_etiquettage);
title('Figure 8: Finalised image without tache');

% Using this method we can have a much more clear idea by beeing cable to
% remove the tache, however, we observe that some cromosoms that in the
% real image are different objects, here they are considered as the same
% object and thus around the tâche, we can see that there were some
% chromosomes that are being removed since after the etiquettage method
% they are considered as the same object. This is an essential problem.

n

% Method 2
% Inverser l'image binaire pour que les chromosomes soient en blanc et le fond en noir
I_inverted = imcomplement(I_segmentation_2);

% Définir une plage pour les tailles des objets à conserver (chromosomes)
taille_min = 150;  % Taille minimale supposée pour un chromosome
taille_max = 3000;  % Taille maximale supposée pour un chromosome

% Utiliser bwareafilt pour conserver uniquement les objets dont la taille est comprise dans cette plage
I_clean = bwareafilt(I_inverted, [taille_min, taille_max]);  % Conserver uniquement les objets dans la plage de taille

% Inverser à nouveau l'image pour revenir à l'image originale (chromosomes en blanc)

figure(9);
imshowpair(I_segmentation_1, I_clean, 'montage');
title('Figure 9: Image avant (gauche) et après (droite) suppression des taches basées sur la taille');

[L, num] = bwlabel(I_clean);
num

% Method 3
seuil_2 = 111;


[n, m, l] = size(J);

I_segmentation_3 = double(J);
for i=1:n
    for j=1:m
        if I_segmentation_3(i,j) <= seuil_2
            I_segmentation_3(i,j) = 1;
        else
            I_segmentation_3(i,j) = 0;
        end
    end
end

figure(10);
imshow(I_segmentation_3);
title('Figure 10: New seuil');

I_closed = imclose(I_segmentation_3, strel("disk", 7));
figure(11);
imshow(I_closed);
title('Figure 11: Only tache');




I_opened = imopen(I_closed, strel("disk", 9));
figure(12);
imshow(I_opened);
title('Figure 12: Clearing new noise');

I_new = I_segmentation_1 - I_closed;
figure(13);
imshow(I_new);
title('Figure 13: No tache in the image by default');
