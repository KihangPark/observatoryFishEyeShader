#ifndef OBSERVATORY_FISHEYE_H
#define OBSERVATORY_FISHEYE_H

#include "shader.h"

struct observatoryFishEyeShader {
	    miColor outside_color;
		miBoolean circle_type;
		miInteger upside_degree;
		miInteger wide_degree;
		miBoolean guide_line;
	    miColor guide_color;
		miBoolean env_override;
        miInteger env_override_distance;
		miBoolean reflection_override;
        miInteger reflection_override_distance;
		miBoolean refraction_override;
        miInteger refraction_override_distance;
		miBoolean offset;
		miScalar offset_degree;
		miScalar offset_distance;
		miBoolean mask;
		miTag mask_texture;
		miBoolean background;
		miTag background_texture;
		miBoolean projection;
		miScalar projection_left;
		miScalar projection_right;
		miScalar projection_up;
		miScalar projection_down;
		miTag projection_texture;
		miBoolean projection_alpha;
		miBoolean debug;
		miBoolean ray_limit;
		miScalar ray_limit_degree;
};

miC_LINKAGE DLLEXPORT void observatoryFishEyeShader_init(miState*, struct observatoryFishEyeShader*, miBoolean*);
miC_LINKAGE DLLEXPORT miBoolean observatoryFishEyeShader(miColor*, miState*, struct observatoryFishEyeShader*);
miC_LINKAGE DLLEXPORT miInteger observatoryFishEyeShader_version();
miC_LINKAGE DLLEXPORT void observatoryFishEyeShader_exit(miState*, struct observatoryFishEyeShader*);


#endif
