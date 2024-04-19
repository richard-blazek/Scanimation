#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface *load_image(const char *path)
{
	return SDL_ConvertSurface(IMG_Load(path), SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0);
}

SDL_Surface *create_merged(SDL_Surface** images, int count, int stripe_width)
{
	SDL_Surface *result = SDL_CreateRGBSurfaceWithFormat(0, images[0]->w, images[0]->h, 32, SDL_PIXELFORMAT_RGBA8888);
	for (int y = 0; y < result->h; ++y)
	{
		for (int x = 0; x < result->w; ++x)
		{
			SDL_Surface *src = images[x / stripe_width % count];
			((uint32_t*)result->pixels)[y * result->w + x] = ((uint32_t*)src->pixels)[y * result->w + x];
		}
	}
	return result;
}

SDL_Surface *create_stripes(int w, int h, int count, int stripe_width)
{
	SDL_Surface *result = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA8888);
	for (int y = 0; y < result->h; ++y)
	{
		for (int x = 0; x < result->w; ++x)
		{
			int blank = x / stripe_width % count == 0;
			((uint32_t*)result->pixels)[y * result->w + x] = SDL_MapRGBA(result->format, blank?255:0, blank?255:0, blank?255:0, blank?0:255);
		}
	}
	return result;
}

int main(int argc, char *argv[])
{
	if (argc < 5) return 1;

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP | IMG_INIT_JXL | IMG_INIT_AVIF);

	int count = argc - 4, stripe_width = 0;
	sscanf(argv[3], "%d", &stripe_width);

	SDL_Surface *sources[count];
	for (int i = 0; i < count; ++i)
	{
		sources[i] = load_image(argv[i + 4]);
	}

	SDL_Surface *result = create_merged(sources, count, stripe_width);
	IMG_SavePNG(result, argv[1]);
	IMG_SavePNG(create_stripes(result->w, result->h, count, stripe_width), argv[2]);
	return 0;
}
