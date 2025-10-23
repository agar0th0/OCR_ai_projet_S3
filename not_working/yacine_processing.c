#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION


#include "stb_image.h"//pour charger une image
#include "stb_image_write.h"//pour sauvegarder une image


// tu n'inclus pas stdio.h et stdlib bro 
// tes malloc sont mal faits 


int main(int argc, char **argv)
{
	
    	if (argc < 3) 
    	{
        	return 1;
    	}



	int w;
	int h;
	int channels;

	unsigned char *img = stbi_load(argv[1], &w, &h, &channels, 0);
	if (!img) 
	{
	       	// Si l'image n'a pas pu etre chargée
	       	printf("Error #1");
	       	return 1;
	}



	unsigned char *new_img = malloc(w * h);

	if (!new_img)
	{
        	printf("Error #2");
        	stbi_image_free(img); // on libère l'image source avant de quitter
        	return 1;
	}

	// parcours de chaque pixel de l'image 
    	for (int y = 0; y < h; y++) 
    	{
        	for (int x = 0; x < w; x++) 
		{
           
			// idx = position du pixel dans le tableau (selon largeur et nb de canaux)
            		int idx = (y * w + x) * channels;

            		// On récupère les composantes rouge, verte et bleue du pixel
            		int r = img[idx];
            		int g = (channels > 1) ? img[idx + 1] : r; 
			int b = (channels > 2) ? img[idx + 2] : r;
			unsigned char gval = (unsigned char)(0.299*r + 0.587*g + 0.114*b); // Conversion en gris selon la formule standard de luminance 

           
            		new_img[y * w + x] = gval;
        	}
    	}

	stbi_write_png(argv[2], w, h, 1, new_img, w);

	stbi_image_free(img);

    	return 0;
}
































































