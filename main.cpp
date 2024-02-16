#include "myframework/gui.h"

class Grid
{
private:
	uint32 pix_width, stripes;
public:
	Grid()=default;
	Grid(uint32 pix_width, uint32 stripes):pix_width(pix_width), stripes(stripes) {}
	uint32 operator[](size_t i)const
	{
		return i/pix_width%stripes;
	}
};

SDL::Surface Merge(const std::vector<SDL::Surface>& images, Grid grid)
try
{
	SDL::Surface result(images[0].Size(), images[0].Format());
	for(int y=0; y<result.Size().y; ++y)
	{
		for(int x=0; x<result.Size().x; ++x)
		{
			result.Draw(SDL::Point(x,y), images[grid[x]][SDL::Point(x,y)]);
		}
	}
	return func::Move(result);
}
catch(SDL::Error& err)
{
	throw std::runtime_error("Error: the size of images has to be the same");
}

SDL::Surface GridImage(SDL::Point size, Grid grid)
{
	SDL::Surface result(size, SDL::Pixel::Format::RGBA8888);
	for(int y=0; y<result.Size().y; ++y)
	{
		for(int x=0; x<result.Size().x; ++x)
		{
			result.Draw(SDL::Point(x,y), grid[x]==0?SDL::Color(255,255,255,0):SDL::Color(0,0,0));
		}
	}
	return func::Move(result);
}

std::vector<SDL::Surface> GetSurfacesFromFiles(const std::vector<std::string>& files)
try
{
	std::vector<SDL::Surface> surfaces;
	for(auto& filename:files)
	{
		surfaces.push_back(SDL::Surface::LoadImg(filename));
	}
	return func::Move(surfaces);
}
catch(SDL::Error& err)
{
	throw std::runtime_error("Error: Invalid file path");
}

int IntDialog(gui::Init& _i, std::string label_name, std::string confirm)
{
	return stoi(gui::Dialog(_i, "Scanimation", {{label_name, false}}, confirm, 120)[0]);
}

int main()try
{
	gui::Init _i("font.ttf", 12);

	SDL::MessageBox::Show("Scanimation", "This is a program for creating scanimation (barrier-grid animation)");

	uint16 count=IntDialog(_i, "Image count", "Continue");
	auto sources=GetSurfacesFromFiles(gui::Dialog(_i, "Scanimation", std::vector<gui::InputDescription>(count, {"File path", false}), "Continue"));
	uint32 sirka=IntDialog(_i, "Stripe width (px)", "Confirm");

	Grid grid(sirka, sources.size());

	SDL::Surface result=Merge(sources, grid);
	result.SaveAsPNG("scanimation.png");

	GridImage(result.Size(), grid).SaveAsPNG("stripes.png");

	SDL::MessageBox::Show("Scanimation", "Result files scanimation.png and stripes.png are located in the folder of this program");
	return 0;
}
catch(std::exception& exc)
{
	SDL::MessageBox::Show("Scanimation", exc.what());
}