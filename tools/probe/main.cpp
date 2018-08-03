//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math/algebra/vector.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// Convert latlong images into environment probes

using namespace std;
using namespace rev::math;

struct Params {
	std::string in;
	std::string out;

#ifdef _WIN32
	static constexpr size_t arg0 = 1;
#else
	static constexpr size_t arg0 = 0;
#endif

	bool parseArguments(int _argc, const char** _argv) {
		for (int i = arg0; i < _argc; ++i) {
			const char* arg = _argv[i];
			if (0 == strcmp(arg, "-i")) { // Input file
				++i;
				if (i == _argc) {
					cout << "Error: missing input filename after -i\n";
					return false;
				}
				in = _argv[i];
			}
			else if (0 == strcmp(arg, "-o")) { // Output file
				++i;
				if (i == _argc) {
					cout << "Error: missing output filename after -o\n";
					return false;
				}
				out = _argv[i];
			}
		} // for
		return checkParamsAreValid();
	}

private:
	bool checkParamsAreValid() {
		if (in.empty())
			return false;
		return true;
	}
};

auto extension(const std::string_view& s)
{
	auto dot = s.find_last_of(".");
	if (dot == std::string_view::npos)
		return std::string_view("");
	else
		return s.substr(dot+1);
}

// clamp t into [a,b]
float clamp(float t, float a, float b)
{
	return max(a, min(b, t));
}

struct Image
{
	Image(int sx, int sy)
		: nx(sx), ny(sy)
	{
		m = new Vec3f[nx*ny];
	}

	~Image() { delete[] m; }

	int nPixels() const { return nx*ny; }

	Vec3f& at(int x, int y)
	{
		return m[x+y*nx];
	}

	Vec3f& at(int i)
	{
		return m[i];
	}

	const Vec3f& at(int x, int y) const
	{
		return m[x+y*nx];
	}

	const Vec3f& at(int i) const
	{
		return m[i];
	}

	void saveHDR(const std::string& fileName) const
	{
		stbi_write_hdr(fileName.c_str(), nx, ny, 3, reinterpret_cast<const float*>(m));
	}

	void save2sRGB(const std::string& fileName) const
	{
		if(extension(fileName) != "png")
		{
			cout << "Only png is supported for output images\n";
		}
		auto nBytes = 3*nx*ny;
		std::vector<uint8_t> raw(nBytes);
		auto src = reinterpret_cast<const float*>(m);

		// Linear to sRGB conversion following the formal specification of sRGB
		for(int i = 0; i < nBytes; ++i)
		{
			auto linear = src[i];
			float srgb;
			if(linear <= 0.0031308f)
			{
				srgb = uint8_t(clamp(linear * 3294.6, 0.f, 255.f)); 
			}
			else
			{
				srgb = 269.025f * pow(linear,1/2.4f) - 0.055f;
			}
			raw[i] = (uint8_t)clamp(srgb, 0.f, 255.f);
		}
		auto bytesPerRow = 3*nx;
		stbi_write_png(fileName.c_str(), nx, ny, 3, raw.data(), bytesPerRow);
	}

	Image* reduce2x() const
	{
		if(nx & 1 || ny & 1) // Odd sizes not supported
			return nullptr;

		auto reduced = new Image(nx/2, ny/2);
		for(int i = 0; i < ny/2; ++i)
		{
			auto y = 2*i;
			for(int j = 0; j < nx/2; ++j)
			{
				auto x = 2*j;
				reduced->at(j,i) = 0.25f * (at(x,y)+at(x+1,y)+at(x,y+1)+at(x+1,y+1));
			}
		}

		return reduced;
	}

	std::vector<Image*> generateMipMaps()
	{
		std::vector<Image*> mips;
		auto mip = this;
		while(mip)
		{
			mips.push_back(mip);
			mip = mip->reduce2x();
		}
		return mips;
	}

	Vec3f* m;
	int nx, ny;
};

//----------------------------------------------------------------------------------------------------------------------
Image* loadImage(string& fileName)
{
	// Load source data
	int nx, ny, nc;

	auto rawData = stbi_loadf(fileName.c_str(), &nx, &ny, &nc, 3);
	if(!rawData)
		return nullptr;
	
	auto img = new Image(nx, ny);
	for(int i = 0; i < img->nPixels(); ++i)
		img->at(i) = reinterpret_cast<Vec3f&>(rawData[3*i]);

	stbi_image_free(rawData);

	return img;
}

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Parse arguments
	Params params;
	if (!params.parseArguments(_argc, _argv))
		return -1;

	// Load source data
	auto srcImg = loadImage(params.in);

	if(!srcImg)
	{
		cout << "Error: Unable to load input image\n";
		return -1;
	}

	auto mips = srcImg->generateMipMaps();

	return 0;
}
