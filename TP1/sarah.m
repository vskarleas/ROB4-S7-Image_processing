%partir 1 
I = imread("chromosomes.tif");

figure(1);
imshow(I);

% Get the size of the image
[n, m, L] = size(I);

%on utilise un filtre median car ça réduit les bruits impulsionnel sans
%flouter
%effet poivre sel

filtre_median = medfilt2(I);


figure(2);
imshowpair(I,filtre_median, 'montage');

%on afiiche l'histogramme pour trouver le seuil

figure(3);
imhist(I)

%on choisit le seuil à l'aide de l'histogramme on fait des tests à la main
%pour faire le meilleur choix et voir qu'est-ce que je veux supprimer
s=233;
I_seg=double(filtre_median);

for i=1:n
    for j=1:m
        if I_seg(i,j)<=s
            I_seg(i,j)=1;
        else
            I_seg(i,j)=0;
 
        end

    end
end
%Ib = ((I<s==1);

figure(4);
imshow(I_seg);
title('image binaire');

% Amélioration 
se = strel('disk', 2);
Ir = imerode(filtre_median, se);
figure(5);
imshow(I_seg);
title('image érodé');
Id = imdilate(Ir, se);

figure(6);
imshow(I_seg);
title('image dilaté');

I_diff=filtre_median-Ir;

figure(7);
imshow(I_diff);
title('Image avec contours');

%etiquetage

[labels,nb]=bwlabeln(Id);
% c'est le nb de régions
nb

figure(8);
imagesc(labels);
title('image étiquetée');
%recuperer les proprietes des chacunes des régions

stats=regionprops(labels, 'Area','PixelIdxList');
%BoundingBox nous donne le rectangle leplus petit qui englobe l'image
%h=ymax_ymin
%L=xmax-xmin
%ration=h/l
%tout ce qui est inférieur à 500 on le récupère 
area_min=100;
area_max=4300;
for i=1:length(stats)
    if (stats(i).Area<= area_min || stats(i).Area>= area_max)
        area=stats(i).Area;
        labels (stats(i).PixelIdxList)=0;
        figure(9);
        imagesc(labels);
    end
end