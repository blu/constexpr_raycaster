#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <png.h>

#ifndef Z_BEST_COMPRESSION
#define Z_BEST_COMPRESSION 9
#endif

#include "scoped.hpp"
#include "stream.hpp"

// verify iostream-free status
#if _GLIBCXX_IOSTREAM
#error rogue iostream acquired
#endif

namespace stream {

// deferred initialization by main()
in cin;
out cout;
out cerr;

} // namespace stream

namespace testbed {

template < typename T >
class generic_free {
public:
	void operator()(T* arg) {
		assert(0 != arg);
		free(arg);
	}
};

template <>
class scoped_functor< FILE > {
public:
	void operator()(FILE* arg) {
		assert(0 != arg);
		fclose(arg);
	}
};

} // namespace testbed

static
bool get_file_size(
	const char* const filename,
	size_t& size)
{
	assert(0 != filename);
	struct stat filestat;

	if (-1 == stat(filename, &filestat)) {
		stream::cerr << __FUNCTION__ << " cannot stat file '" << filename << "'\n";
		return false;
	}

	if (!S_ISREG(filestat.st_mode)) {
		stream::cerr << __FUNCTION__ << " encountered a non-regular file '" << filename << "'\n";
		return false;
	}

	size = filestat.st_size;
	return true;
}

static
char* get_buffer_from_file(
	const char* const filename,
	size_t& size,
	const size_t roundToIntegralMultiple)
{
	using testbed::scoped_ptr;
	using testbed::scoped_functor;
	using testbed::generic_free;

	assert(0 != filename);
	assert(0 == (roundToIntegralMultiple & roundToIntegralMultiple - 1));

	if (!get_file_size(filename, size)) {
		stream::cerr << __FUNCTION__ << " cannot get size of file '" << filename << "'\n";
		return 0;
	}

	const scoped_ptr< FILE, scoped_functor > file(fopen(filename, "rb"));

	if (0 == file()) {
		stream::cerr << __FUNCTION__ << " cannot open file '" << filename << "'\n";
		return 0;
	}

	const size_t roundTo = roundToIntegralMultiple - 1;
	scoped_ptr< char, generic_free > source(
		reinterpret_cast< char* >(malloc((size + roundTo) & ~roundTo)));

	if (0 == source()) {
		stream::cerr << __FUNCTION__ << " cannot allocate memory for file '" << filename << "'\n";
		return 0;
	}

	if (1 != fread(source(), size, 1, file())) {
		stream::cerr << __FUNCTION__ << " cannot read from file '" << filename << "'\n";
		return 0;
	}

	char* const ret = source();
	source.reset();

	return ret;
}

static bool
write_png(
	const bool grayscale,
	const unsigned w,
	const unsigned h,
	void* const bits,
	FILE* const fp)
{
	using testbed::scoped_ptr;
	using testbed::generic_free;

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		return false;

	png_infop info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return false;
	}

	// declare any RAII before the longjump, lest no destruction at longjump
	const scoped_ptr< png_bytep, generic_free > row((png_bytepp) malloc(sizeof(png_bytep) * h));

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}

	size_t pixel_size = sizeof(png_byte[3]);
	int color_type = PNG_COLOR_TYPE_RGB;

	if (grayscale) {
		pixel_size = sizeof(png_byte);
		color_type = PNG_COLOR_TYPE_GRAY;
	}

	for (size_t i = 0; i < h; ++i)
		row()[i] = (png_bytep) bits + w * (h - 1 - i) * pixel_size;

	png_init_io(png_ptr, fp);
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
	png_set_IHDR(png_ptr, info_ptr, w, h, 8, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_set_rows(png_ptr, info_ptr, row());
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	return true;
}

int main(int, char**)
{
	stream::cin.open(stdin);
	stream::cout.open(stdout);
	stream::cerr.open(stderr);

	using testbed::scoped_ptr;
	using testbed::scoped_functor;
	using testbed::generic_free;

	const char inputName[] = "image.bin";
	size_t inputLength = 0;
	const scoped_ptr< void, generic_free > input(get_buffer_from_file(inputName, inputLength, 16));

	if (0 == input()) {
		stream::cerr << "failure opening input file\n";
		return -1;
	}

	const size_t image_w = reinterpret_cast< uint16_t* >(input())[0];
	const size_t image_h = reinterpret_cast< uint16_t* >(input())[1];

	if (image_w * image_h + sizeof(uint16_t[2]) != inputLength) {
		stream::cerr << "input file dimensions mismatch; not an image or corrupt?\n";
		return -1;
	}

	const char outName[] = "image.png";
	const scoped_ptr< FILE, scoped_functor > file(fopen(outName, "wb"));

	if (0 == file()) {
		stream::cerr << "failure opening output file '" << outName << "'\n";
		return -1;
	}

	if (!write_png(true, image_w, image_h, reinterpret_cast< uint16_t* >(input()) + 2, file())) {
		stream::cerr << "failure writing output file '" << outName << "'\n";
		return -1;
	}

	return 0;
}
