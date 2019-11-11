#include <stdio.h>

#include <cstdint>
#include <cfloat>
#include <cmath>

// type float3 provides basic arithmetics over cartesian vectors
struct float3
{
	float x;
	float y;
	float z;

	float3(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
	{}

	float3(float same)
	: x(same)
	, y(same)
	, z(same)
	{}

	float3 operator -() const
	{
		return float3(
			-x,
			-y,
			-z);
	}

	float3 rcp() const
	{
		// division by zero is underfined at compile time -- emulate runtime behaviour
		const float rcp_x = x != 0 ? 1.f / x : INFINITY;
		const float rcp_y = y != 0 ? 1.f / y : INFINITY;
		const float rcp_z = z != 0 ? 1.f / z : INFINITY;
		return float3(
			rcp_x,
			rcp_y,
			rcp_z);
	}

	float3 operator +(const float3& rhs) const
	{
		return float3(
			x + rhs.x,
			y + rhs.y,
			z + rhs.z);
	}

	float3 operator -(const float3& rhs) const
	{
		return *this + -rhs;
	}

	float3 operator *(const float3& rhs) const
	{
		return float3(
			x * rhs.x,
			y * rhs.y,
			z * rhs.z);
	}

	float3 operator /(const float3& rhs) const
	{
		return *this * rhs.rcp();
	}
};

// type float4 provides functionality needed by 4x4 matrices
struct float4
{
	float m[4];

	float4(float e0, float e1, float e2, float e3)
	: m{ e0, e1, e2, e3 }
	{}

	float4(float same)
	: m{ same, same, same, same }
	{}

	float4 operator -() const
	{
		return float4(
			-m[0],
			-m[1],
			-m[2],
			-m[3]);
	}

	float4 operator +(const float4& rhs) const
	{
		return float4(
			m[0] + rhs[0],
			m[1] + rhs[1],
			m[2] + rhs[2],
			m[3] + rhs[3]);
	}

	float4 operator -(const float4& rhs) const
	{
		return *this + -rhs;
	}

	float4 operator *(const float4& rhs) const
	{
		return float4(
			m[0] * rhs[0],
			m[1] * rhs[1],
			m[2] * rhs[2],
			m[3] * rhs[3]);
	}

	float operator [](size_t index) const
	{
		return m[index];
	}
};

struct matx4
{
	float4 m[4];

	matx4(
		const float c00, const float c01, const float c02, const float c03,
		const float c10, const float c11, const float c12, const float c13,
		const float c20, const float c21, const float c22, const float c23,
		const float c30, const float c31, const float c32, const float c33)
	: m{ float4(c00, c01, c02, c03),
         float4(c10, c11, c12, c13),
         float4(c20, c21, c22, c23),
         float4(c30, c31, c32, c33) }
	{}

	matx4(
		const float4& row0,
		const float4& row1,
		const float4& row2,
		const float4& row3)
	: m{ row0, row1, row2, row3 }
	{}

	matx4(const float4& same)
	: m{ same, same, same, same }
	{}

	float4 operator [](size_t index) const
	{
		return m[index];
	}

	matx4 transpose() const
	{
		return matx4(
			m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3]);
	}
};

__attribute__ ((always_inline)) float3 operator *(
	const float3& v,
	const matx4& m)
{
	const float4 r =
		m[0] * float4(v.x) +
		m[1] * float4(v.y) +
		m[2] * float4(v.z) +
		m[3];

	return float3(
		r[0],
		r[1],
		r[2]);
}

__attribute__ ((always_inline)) matx4 operator *(
	const matx4& a,
	const matx4& b)
{
	const float4 r0 =
		float4(a[0][0]) * b[0] +
		float4(a[0][1]) * b[1] +
		float4(a[0][2]) * b[2] +
		float4(a[0][3]) * b[3];

	const float4 r1 =
		float4(a[1][0]) * b[0] +
		float4(a[1][1]) * b[1] +
		float4(a[1][2]) * b[2] +
		float4(a[1][3]) * b[3];

	const float4 r2 =
		float4(a[2][0]) * b[0] +
		float4(a[2][1]) * b[1] +
		float4(a[2][2]) * b[2] +
		float4(a[2][3]) * b[3];

	const float4 r3 =
		float4(a[3][0]) * b[0] +
		float4(a[3][1]) * b[1] +
		float4(a[3][2]) * b[2] +
		float4(a[3][3]) * b[3];

	return matx4(r0, r1, r2, r3);
}

struct matx4_rotate : matx4
{
	matx4_rotate(
		float sin_a,
		float cos_a,
		float x,
		float y,
		float z)
	: matx4{ float4(x * x + cos_a * (1 - x * x),         x * y - cos_a * (x * y) + sin_a * z, x * z - cos_a * (x * z) - sin_a * y, 0.f),
             float4(y * x - cos_a * (y * x) - sin_a * z, y * y + cos_a * (1 - y * y),         y * z - cos_a * (y * z) + sin_a * x, 0.f),
             float4(z * x - cos_a * (z * x) + sin_a * y, z * y - cos_a * (z * y) - sin_a * x, z * z + cos_a * (1 - z * z),         0.f),
             float4(0.f, 0.f, 0.f, 1.f) }
	{}
};

__attribute__ ((always_inline)) float3 fmin(const float3& a, const float3& b)
{
	return float3(
		fminf(a.x, b.x),
		fminf(a.y, b.y),
		fminf(a.z, b.z));
}

__attribute__ ((always_inline)) float3 fmax(const float3& a, const float3& b)
{
	return float3(
		fmaxf(a.x, b.x),
		fmaxf(a.y, b.y),
		fmaxf(a.z, b.z));
}

__attribute__ ((always_inline)) float3 clamp(const float3& x, const float3& min, const float3& max)
{
	return fmax(fmin(x, max), min);
}

__attribute__ ((always_inline)) bool isless(float a, float b)
{
	return a < b;
}

__attribute__ ((always_inline)) float select(float arg_else, float arg_then, bool pred)
{
	return pred ? arg_then : arg_else;
}

struct BBox
{
	float3 min;
	float3 max;

	BBox(const float3& min, const float3& max)
	: min(min)
	, max(max)
	{}
};

typedef BBox Voxel;

struct Ray
{
	float3 origin;
	float3 rcpdir;

	Ray(const float3& origin, const float3& rcpdir)
	: origin(origin)
	, rcpdir(rcpdir)
	{}
};

__attribute__ ((always_inline)) float intersect(
	const BBox& bbox,
	const Ray& ray)
{
	const float3 t0 = (bbox.min - ray.origin) * ray.rcpdir;
	const float3 t1 = (bbox.max - ray.origin) * ray.rcpdir;

	const float3 axial_min = fmin(t0, t1);
	const float3 axial_max = fmax(t0, t1);

	const float min = fmaxf(fmaxf(axial_min.x, axial_min.y), axial_min.z);
	const float max = fminf(fminf(axial_max.x, axial_max.y), axial_max.z);

	return select(INFINITY, min, isless(0.f, min) && isless(min, max));
}

typedef uint8_t Pixel;

__attribute__ ((always_inline)) Pixel shootRay(
	int global_idx,
	int image_w,
	int image_h,
	const float3 (&cam)[4],
	const Voxel* scene,
	size_t size)
{
	const int idy = global_idx / image_w;
	const int idx = global_idx % image_w;

	const float3 ray_direction =
		cam[0] * ((idx * 2 - image_w) * (1.f / image_w)) +
		cam[1] * ((idy * 2 - image_h) * (1.f / image_h)) +
		cam[2];

	const Ray ray{ cam[3], clamp(ray_direction.rcp(), -MAXFLOAT / 2, MAXFLOAT / 2) };
	float closest = INFINITY;

	for (size_t i = 0; i < size; ++i)
		closest = fminf(closest, intersect(scene[i], ray));

	return INFINITY != closest ? Pixel(closest / 4.f * 255.f) : 0;
}

__attribute__ ((always_inline)) BBox computeSceneBBox(const Voxel* scene, size_t size)
{
	float3 bbox_min{ +INFINITY, +INFINITY, +INFINITY };
	float3 bbox_max{ -INFINITY, -INFINITY, -INFINITY };

	for (size_t i = 0; i < size; ++i) {
		bbox_min = fmin(bbox_min, scene[i].min);
		bbox_max = fmax(bbox_max, scene[i].max);
	}

	return BBox(bbox_min, bbox_max);
}

int main(int, char**)
{
	// scene content in world space
	const Voxel scene[] = {
		Voxel(float3(-.75f, -.75f, -.75f), float3(.25f, .25f, .25f)),
		Voxel(float3(-.25f, -.25f, -.25f), float3(.75f, .75f, .75f)),
	};
	// scene meta
	const size_t scene_size = sizeof(scene) / sizeof(scene[0]);
	const BBox bbox = computeSceneBBox(scene, scene_size);
	const float3 centre = (bbox.max + bbox.min) * float3(.5f);
	const float3 extent = (bbox.max - bbox.min) * float3(.5f);
	const float max_extent = fmaxf(extent.x, fmaxf(extent.y, extent.z));

	// camera settings in world space
	const float sce_roll = M_PI_2 * .25f;
	const float sce_azim = 0;
	const float sce_decl = 0;
	const float3 cam_pos{ 0, 0, 2.125f };

	// view transform
	const float sin_roll = sin(sce_roll);
	const float cos_roll = cos(sce_roll);
	const float sin_azim = sin(sce_azim);
	const float cos_azim = cos(sce_azim);
	const float sin_decl = sin(sce_decl);
	const float cos_decl = cos(sce_decl);

	const matx4 rot =
		matx4_rotate(sin_roll, cos_roll, 0.f, 1.f, 0.f) *
		matx4_rotate(sin_azim, cos_azim, 0.f, 0.f, 1.f) *
		matx4_rotate(sin_decl, cos_decl, 1.f, 0.f, 0.f);

	const matx4 eye{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		cam_pos.x,
		cam_pos.y,
		cam_pos.z, 1.f};

	const matx4 zoom_n_pan(
		max_extent, 0.f, 0.f, 0.f,
		0.f, max_extent, 0.f, 0.f,
		0.f, 0.f, max_extent, 0.f,
		centre.x,
		centre.y,
		centre.z, 1.f);

	// forward: pan * zoom * rot * eyep
	// inverse: (eyep)-1 * rotT * (zoom)-1 * (pan)-1

	const matx4 mv_inv = eye * rot.transpose() * zoom_n_pan;

	// view transform as expected by the image integrator (4x float3)
	const int image_w = 256;
	const int image_h = 256;
	// warning: updates to the image dimensions require updates to the injection macros below

	const float3 cam[] = {
		float3(mv_inv[0][0], mv_inv[0][1], mv_inv[0][2]),
		float3(mv_inv[1][0], mv_inv[1][1], mv_inv[1][2]) * float3(float(image_h) / image_w),
		float3(mv_inv[2][0], mv_inv[2][1], mv_inv[2][2]) * float3(-1),
		float3(mv_inv[3][0], mv_inv[3][1], mv_inv[3][2])
	};

	// image-array-element injector macros -- log2 expansion
	#define INJECT_ELEMENTS_0(n) shootRay(n, image_w, image_h, cam, scene, scene_size),
	#define INJECT_ELEMENTS_1(n) INJECT_ELEMENTS_0(n) INJECT_ELEMENTS_0(n +     1)
	#define INJECT_ELEMENTS_2(n) INJECT_ELEMENTS_1(n) INJECT_ELEMENTS_1(n +     2)
	#define INJECT_ELEMENTS_3(n) INJECT_ELEMENTS_2(n) INJECT_ELEMENTS_2(n +     4)
	#define INJECT_ELEMENTS_4(n) INJECT_ELEMENTS_3(n) INJECT_ELEMENTS_3(n +     8)
	#define INJECT_ELEMENTS_5(n) INJECT_ELEMENTS_4(n) INJECT_ELEMENTS_4(n +    16)
	#define INJECT_ELEMENTS_6(n) INJECT_ELEMENTS_5(n) INJECT_ELEMENTS_5(n +    32)
	#define INJECT_ELEMENTS_7(n) INJECT_ELEMENTS_6(n) INJECT_ELEMENTS_6(n +    64)
	#define INJECT_ELEMENTS_8(n) INJECT_ELEMENTS_7(n) INJECT_ELEMENTS_7(n +   128)
	#define INJECT_ELEMENTS_9(n) INJECT_ELEMENTS_8(n) INJECT_ELEMENTS_8(n +   256)
	#define INJECT_ELEMENTS_A(n) INJECT_ELEMENTS_9(n) INJECT_ELEMENTS_9(n +   512)
	#define INJECT_ELEMENTS_B(n) INJECT_ELEMENTS_A(n) INJECT_ELEMENTS_A(n +  1024)
	#define INJECT_ELEMENTS_C(n) INJECT_ELEMENTS_B(n) INJECT_ELEMENTS_B(n +  2048)
	#define INJECT_ELEMENTS_D(n) INJECT_ELEMENTS_C(n) INJECT_ELEMENTS_C(n +  4096)
	#define INJECT_ELEMENTS_E(n) INJECT_ELEMENTS_D(n) INJECT_ELEMENTS_D(n +  8192)
	#define INJECT_ELEMENTS_F(n) INJECT_ELEMENTS_E(n) INJECT_ELEMENTS_E(n + 16384)
	#define INJECT_ELEMENTS_G(n) INJECT_ELEMENTS_F(n) INJECT_ELEMENTS_F(n + 32768)

	const Pixel image[] = {
		INJECT_ELEMENTS_G(0) // global element index starts from 0
	};

#if 0
	for (auto i : scene)
		fprintf(stdout, "voxel( min(%f, %f, %f), max(%f, %f, %f) )\n",
			i.min.x,
			i.min.y,
			i.min.z,
			i.max.x,
			i.max.y,
			i.max.z);

#endif
	if (FILE* f = fopen("image.bin", "wb")) {
		const size_t image_size = image_w * image_h;
		uint16_t dim[] = { image_w, image_h };

		if (2 != fwrite(dim, sizeof(dim[0]), 2, f) || image_size != fwrite(image, sizeof(image[0]), image_size, f))
			fprintf(stderr, "error: failure writing to file\n");

		fclose(f);
	}

	return 0;
}
