#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <string>

class Grid
{
private:
	int pix_width, stripes;
public:
	Grid(int pix_width, int stripes):pix_width(pix_width), stripes(stripes) {}
	int operator[](size_t i)const
	{
		return i/pix_width%stripes;
	}
};

SDL_Surface *Merge(const std::vector<SDL_Surface*>& images, Grid grid)
{
	SDL_Surface *result = SDL_CreateRGBSurfaceWithFormat(0, images[0]->w, images[0]->h, 32, SDL_PIXELFORMAT_RGBA8888);
	for (int y = 0; y < result->h; ++y)
	{
		for (int x = 0; x < result->w; ++x)
		{
			((uint32_t*)result->pixels)[y * result->w + x] = ((uint32_t*)images[grid[x]]->pixels)[y * result->w + x];
		}
	}
	return result;
}

SDL_Surface *GridImage(int w, int h, Grid grid)
{
	SDL_Surface *result = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA8888);
	for (int y = 0; y < result->h; ++y)
	{
		for (int x = 0; x < result->w; ++x)
		{
			((uint32_t*)result->pixels)[y * result->w + x] = SDL_MapRGBA(result->format, grid[x]==0?255:0, grid[x]==0?255:0, grid[x]==0?255:0, grid[x]==0?0:255);
		}
	}
	return result;
}

std::vector<SDL_Surface*> GetSurfacesFromFiles(char **files, int count)
{
	std::vector<SDL_Surface*> surfaces(count);
	for (int i = 0; i < count; ++i)
	{
		auto img = IMG_Load(files[i]);
		auto fmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
		surfaces[i] = SDL_ConvertSurface(img, fmt, 0);
		SDL_FreeFormat(fmt);
		SDL_FreeSurface(img);
	}
	return std::move(surfaces);
}

int main(int argc, char *argv[])
{
	if (argc < 2) return 1;

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP | IMG_INIT_JXL | IMG_INIT_AVIF);

	int count = argc - 1, stripe_width = 0;
	std::cout << "Enter stripe width (px): ";
	std::cin >> stripe_width;

	auto sources = GetSurfacesFromFiles(&argv[1], count);
	Grid grid(stripe_width, count);

	auto result = Merge(sources, grid);
	IMG_SavePNG(result, "scanimation.png");
	IMG_SavePNG(GridImage(result->w, result->h, grid), "stripes.png");

	std::cout << "Result files scanimation.png and stripes.png can be found in this folder\n";
	return 0;
}
